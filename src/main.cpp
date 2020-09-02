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
    auto commandsBuffers = Vulkan::CommandBuffer::createMany(1, &device, &graphicsCommandPool);

    // Texture
    auto texture = Vulkan::Image::createFromFile("resources/textures/viking_room.png", &device,
    graphicsCommandPool, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_SAMPLED_BIT);
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
    while (!window.shouldClose())
    {
        glfwPollEvents();

    }

    vkDeviceWaitIdle(device);

    return 0;
}
