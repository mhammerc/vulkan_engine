#include <stdexcept>

#include "frontend/glfw3.h"
#include "frontend/window.h"
#include "vulkan/instance.h"
#include "vulkan/physicaldevice.h"
#include "vulkan_engine.h"

using namespace Engine;

int main()
{
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

    auto physicalDevice = Vulkan::PhysicalDevice::findBest(instance, surface);
    if (!physicalDevice.has_value())
    {
        throw std::runtime_error("Could not find any suitable GPU.");
    }

    spdlog::debug("Using physical device {}. API Version: {}.{}.{}", physicalDevice->name(),
    VK_VERSION_MAJOR(physicalDevice->version()),
    VK_VERSION_MINOR(physicalDevice->version()),
    VK_VERSION_PATCH(physicalDevice->version()));

    // Logical Device
    // I think the logical device own the physical device
    // Should be straightforward as physical device already handle much

    return 0;
}
