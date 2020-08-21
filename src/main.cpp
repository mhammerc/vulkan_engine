#include <stdexcept>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "frontend/glfw3.h"
#include "frontend/window.h"
#include "vulkan/instance.h"
#include "vulkan/logicaldevice.h"
#include "vulkan/physicaldevice.h"
#include "vulkan/shadermodule.h"
#include "vulkan/swapchainkhr.h"
#include "vulkan_engine.h"

using namespace Engine;

int main()
{
    auto console = spdlog::stdout_color_mt("log", spdlog::color_mode::always);
    spdlog::set_default_logger(console);
//    spdlog::set_pattern("[%l] %v (+%oms)");
    spdlog::set_level(spdlog::level::debug);

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

    auto vert = Vulkan::ShaderModule::createFromFile(&device, "shaders/basic.vert.spv", Vulkan::ShaderModule::Stage::Vertex);
    auto frag = Vulkan::ShaderModule::createFromFile(&device, "shaders/basic.frag.spv", Vulkan::ShaderModule::Stage::Fragment);

    return 0;
}
