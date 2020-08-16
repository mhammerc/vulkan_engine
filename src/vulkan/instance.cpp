#include "instance.h"

using namespace Engine;

Vulkan::Instance::~Instance()
{
    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
        spdlog::debug("Vulkan instance destroyed.");
    }
}

Vulkan::Instance::Instance(std::span<char const *> const _layers, std::span<char const *> const _extensions)
{
    VkInstanceCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    info.flags = 0;
    info.pApplicationInfo = nullptr;

    // reserve instead of resize then push_bask every layers and extensions
    // Then check their availablity
    // then create instance
    // also, check how to print validation layers messages
    std::vector<char const *> layers {_layers.size() + requiredValidationLayers.size()};
    std::vector<char const *> extensions {_extensions.size() + requiredDeviceExtensions.size()};

    info.enabledLayerCount = layers.size();
    info.ppEnabledLayerNames = layers.data();

    info.enabledExtensionCount = extensions.size();
    info.ppEnabledExtensionNames = extensions.data();

    Vulkan::ThrowError(vkCreateInstance(&info, nullptr, &instance));

    uint32 version {};
    Vulkan::ThrowError(vkEnumerateInstanceVersion(&version));

    uint32 major = VK_VERSION_MAJOR(version);
    uint32 minor = VK_VERSION_MINOR(version);
    uint32 patch = VK_VERSION_PATCH(version);

    spdlog::debug("Vulkan instance {}.{}.{} created.", major, minor, patch);
}
