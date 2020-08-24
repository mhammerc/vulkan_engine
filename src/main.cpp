#include <stdexcept>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "frontend/glfw3.h"
#include "frontend/window.h"
#include "vulkan/depthstencilimage.h"
#include "vulkan/deviceallocator.h"
#include "vulkan/instance.h"
#include "vulkan/logicaldevice.h"
#include "vulkan/model.h"
#include "vulkan/physicaldevice.h"
#include "vulkan/pipelinebuilder.h"
#include "vulkan/renderpass.h"
#include "vulkan/shadermodule.h"
#include "vulkan/swapchainkhr.h"
#include "vulkan_engine.h"

#include <thread>

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

    Vulkan::DepthStencilImage depthImage = Vulkan::DepthStencilImage::create(&device, surface.size());

    Vulkan::RenderPass renderPass = Vulkan::RenderPass::create(&device, swapchain, depthImage);

    auto vert = Vulkan::ShaderModule::createFromSpirvFile(&device, "shaders/basic.vert.spv",
                                                          Vulkan::ShaderModule::Stage::Vertex);
    auto frag = Vulkan::ShaderModule::createFromSpirvFile(&device, "shaders/basic.frag.spv",
                                                          Vulkan::ShaderModule::Stage::Fragment);

    Vulkan::PipelineBuilder pipeline(std::move(renderPass), surface);
    pipeline.addShaderStage(vert.toPipeline());
    pipeline.addShaderStage(frag.toPipeline());
    pipeline.setVertexInputDescription<Vulkan::Model>();
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

//    auto model = Vulkan::Model::loadFromFile("resources/models/viking_room.obj");

    return 0;
}
