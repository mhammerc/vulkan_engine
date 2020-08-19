#include <stdexcept>

#include "frontend/glfw3.h"
#include "frontend/window.h"
#include "vulkan/instance.h"
#include "vulkan/physicaldevice.h"
#include "vulkan/logicaldevice.h"
#include "vulkan/swapchainkhr.h"
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
    physicalDevice.reset(); // `physicalDevice` is now in a unspecified state.

    auto swapchain = Vulkan::SwapchainKHR::create(&device, &surface);
//
//    VkImageView a = reinterpret_cast<VkImageView>(1);
//    VkImageView b = reinterpret_cast<VkImageView>(2);
//    VkImageView c = reinterpret_cast<VkImageView>(3);
//    uint64 d = 4;
//
//    Engine::Vulkan::VkHandle<VkImageView> o {a};
//    Engine::Vulkan::VkHandle<VkImageView> p {b};

//    o = p;
//
//Engine::Vulkan::VkHandle<VkImageView> z = VK_NULL_HANDLE;
//gsl::not_null<int*> j = VK_NULL_HANDLE;
//    o = std::move(p);
//
//    Engine::Vulkan::VkHandle<uint64> i {d};

//    if (o)
//    {
//        spdlog::info("ok");
//    }

    return 0;
}
