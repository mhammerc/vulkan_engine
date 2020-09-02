#include <stdexcept>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "frontend/glfw3.h"
#include "frontend/window.h"
#include "vulkan/commandbuffer.h"
#include "vulkan/commandpool.h"
#include "vulkan/depthstencilimage.h"
#include "vulkan/framebuffer.h"
#include "vulkan/instance.h"
#include "vulkan/logicaldevice.h"
#include "vulkan/model.h"
#include "vulkan/physicaldevice.h"
#include "vulkan/pipelinebuilder.h"
#include "vulkan/renderpass.h"
#include "vulkan/sampler.h"
#include "vulkan/shadermodule.h"
#include "vulkan/swapchainkhr.h"
#include "vulkan/uniformbuffer.h"
#include "vulkan/descriptorpool.h"
#include "vulkan/descriptorset.h"
#include "vulkan_engine.h"

using namespace Engine;

int main()
{
    auto console = spdlog::stdout_color_mt("log", spdlog::color_mode::always);
    spdlog::set_default_logger(console);
    spdlog::set_pattern("%^[%l]%$ (+%oms) %v");
    spdlog::set_level(spdlog::level::trace);

    if (!Frontend::init())
    {
        throw std::runtime_error("Could not init Frontend.");
    }
    if (!Frontend::isVulkanSupported())
    {
        throw std::runtime_error("Vulkan not found/supported by windowing system.");
    }

    Frontend::Window window(1280, 720, "Vulkan");

    auto requiredExtensions = Frontend::getRequiredInstanceExtensions();
    auto instance  = Vulkan::Instance::create({}, requiredExtensions);

    auto surface = Vulkan::SurfaceKHR::create(&instance, &window);

    auto physicalDevice = Vulkan::PhysicalDevice::findBest(&instance, &surface);
    if (!physicalDevice.has_value())
    {
        throw std::runtime_error("Could not find any suitable GPU.");
    }

    spdlog::debug("Using physical device {}. API Version: {}.{}.{}", physicalDevice->name(),
    VK_VERSION_MAJOR(physicalDevice->version()),
    VK_VERSION_MINOR(physicalDevice->version()),
    VK_VERSION_PATCH(physicalDevice->version()));

    auto device = Vulkan::LogicalDevice::create(std::move(*physicalDevice));
    physicalDevice.reset(); // `physicalDevice` is now in an unspecified state.

    auto swapchain = Vulkan::SwapchainKHR::create(&device, &surface);

    // Maybe make an image class which auto-handle the view
    Vulkan::DepthStencilImage depthImage = Vulkan::DepthStencilImage::create(&device, surface.size());

    // Mix somehow renderpass to framebuffer as they are linked together
    Vulkan::RenderPass renderPass = Vulkan::RenderPass::create(&device, swapchain, depthImage);
    std::vector<Vulkan::Framebuffer> framebuffers;
    for (auto swapchainView : swapchain.views())
    {
        auto f = Vulkan::Framebuffer::create(surface.size(), renderPass, &device,
        {swapchainView, depthImage.view()});
        framebuffers.push_back(std::move(f));
    }

    auto vert = Vulkan::ShaderModule::createFromSpirvFile(&device, "shaders/basic.vert.spv",
                                                          Vulkan::ShaderModule::Stage::Vertex);
    auto frag = Vulkan::ShaderModule::createFromSpirvFile(&device, "shaders/basic.frag.spv",
                                                          Vulkan::ShaderModule::Stage::Fragment);

    Vulkan::PipelineBuilder pipeline(std::move(renderPass), surface);
    pipeline.addShaderStage(vert.toPipeline());
    pipeline.addShaderStage(frag.toPipeline());
    pipeline.setVertexInputDescription<Vulkan::Model>();
    // Make the pipeline, descriptor sets, descriptor pools and descriptor layouts coherent. They actually are
    // independent objects
    pipeline.addDescriptorSetLayout(
    {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            }
    });
    auto pipelines = Vulkan::PipelineBuilder::build(&device, {&pipeline});

    auto graphicsCommandPool = Vulkan::CommandPool::create(&device);
    auto commandsBuffers = Vulkan::CommandBuffer::createMany(swapchain.views().size(), &device, &graphicsCommandPool);

    // Texture
    auto texture = Vulkan::Image::createFromFile("resources/textures/viking_room.png", &device,
    graphicsCommandPool, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT);
    auto textureView = Vulkan::ImageView::createFromImage(&texture, &device, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    auto sampler = Vulkan::Sampler::create(&device);

    // Model
    auto model = Vulkan::Model::createFromFile("resources/models/viking_room.obj");
    auto modelBuffer = model.toBuffer(&device, graphicsCommandPool);

    // UBO
    std::vector<Vulkan::UniformBuffer> ubos;
    for (usize i = 0; i < swapchain.views().size(); ++i)
    {
        ubos.push_back(std::move(Vulkan::UniformBuffer::create(&device)));
    }

    // Descriptors
    auto descriptorPool = Vulkan::DescriptorPool::create(&device);
    auto descriptorSets = Vulkan::DescriptorSet::createManyFromBuffers(&device,
                                                                       &descriptorPool,
                                                                       pipelines[0].descriptorSetsLayouts()[0],
                                                                        ubos, sampler, textureView);

    // Draw loop
    // todo: properly wrap and destroy those
    usize imageInFlightCount = swapchain.views().size();
    std::vector<VkSemaphore> imageAvailable(imageInFlightCount);
    std::vector<VkSemaphore> renderFinished(imageInFlightCount);
    std::vector<VkFence> inFlightFences(imageInFlightCount);
    std::vector<VkFence> imageInFlight(imageInFlightCount);

    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (usize i = 0; i < imageInFlightCount; ++i)
    {
        Vulkan::ThrowError(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailable[i]));
        Vulkan::ThrowError(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinished[i]));
        Vulkan::ThrowError(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]));
    }

    usize currentFrame = 0;
    while (!window.shouldClose())
    {
        glfwPollEvents();

        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        currentFrame = (currentFrame + 1) % imageInFlightCount;

        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_SUCCESS)
        {
            spdlog::info("new frame!");
        }
        else
        {
            spdlog::error("vkAcquireNextImageKHR didn't return VK_SUCCESS.");
        }

        if (imageInFlight[imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(device, 1, &imageInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }

        imageInFlight[imageIndex] = inFlightFences[currentFrame];

        // update UBO
        glm::quat qPitch = glm::angleAxis(glm::radians(0.f), glm::vec3(1, 0, 0));
        glm::quat qYaw = glm::angleAxis(glm::radians(-90.f), glm::vec3(0, 1, 0));
        glm::quat qRoll = glm::angleAxis(glm::radians(0.f), glm::vec3(0, 0, 1));
        glm::quat orientation = glm::normalize(qPitch * qYaw * qRoll);

        Vulkan::UniformBuffer::UniformBufferObject ubo {};
        ubo.model = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        ubo.view = glm::mat4_cast(orientation) * glm::translate(glm::mat4(1.f), {-1.f, -0.5f, 0.f});
        ubo.proj = glm::perspective(glm::radians(45.f), static_cast<float>(surface.size().width) / static_cast<float>(surface.size().height), 0.001f, 1000.f);

        // GLM was designed for OpenGL, where the Y coordinate of the clip is inverted. Compensate that.
        ubo.proj[1][1] *= -1;

        void *ptr = nullptr;
        ubos[imageIndex].buffer().map(&ptr);
        memcpy(ptr, &ubo, sizeof(ubo));
        ubos[imageIndex].buffer().unmap();

        // Create our command buffer now
        auto &commandBuffer = commandsBuffers[imageIndex];
        commandBuffer.reset();
        commandBuffer.begin(0);

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color = {0.f, 0.f, 0.f, 1.f};
        clearValues[1].depthStencil = {1.f, 0};

        VkRenderPassBeginInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = *pipelines[0].renderPass(),
            .framebuffer = framebuffers[imageIndex],
            .renderArea =
            {
                .offset = {.x = 0, .y = 0},
                .extent = surface.size(),
            },
            .clearValueCount = static_cast<uint32>(clearValues.size()),
            .pClearValues = clearValues.data(),
        };

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[0].pipeline());

        std::array<VkDeviceSize, 1> offsets {0};
        VkBuffer handle = modelBuffer.handle();
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &handle, offsets.data());
        vkCmdBindIndexBuffer(commandBuffer, handle, model.indexesOffset(), VK_INDEX_TYPE_UINT32);
        VkDescriptorSet set = descriptorSets[imageIndex];
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[0].layout(), 0, 1, &set, 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32>(model.indiceSize()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        commandBuffer.end();

        // Submit our command buffer
        VkCommandBuffer _commandBuffer = commandBuffer;

        VkSemaphore waitSemaphores[] = {imageAvailable[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signalSemaphores[] = {renderFinished[currentFrame]};
        VkSubmitInfo submitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &_commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores,
        };

        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        Vulkan::ThrowError(vkQueueSubmit(device.queues().graphics, 1, &submitInfo, inFlightFences[currentFrame]));

        VkSwapchainKHR swapchains[] = {swapchain};
        VkPresentInfoKHR presentInfo
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores,
            .swapchainCount = 1,
            .pSwapchains = swapchains,
            .pImageIndices = &imageIndex,
        };

        vkQueuePresentKHR(device.queues().present, &presentInfo);
    }

    vkDeviceWaitIdle(device);

    return 0;
}
