#include "physicaldevice.h"

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Engine::Vulkan;

std::optional<PhysicalDevice> PhysicalDevice::findBest(Instance &instance, SurfaceKHR &surface)
{
    uint32 deviceCount = 0;
    ThrowError(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));

    std::vector<VkPhysicalDevice> devices {deviceCount};

    ThrowError(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

    for (auto const &_device : devices)
    {
        PhysicalDevice device(instance, _device, surface);
        if (device.isSuitable())
        {
            return device;
        }
    }

    return std::nullopt;
}



PhysicalDevice::PhysicalDevice(Instance &instance, gsl::not_null<VkPhysicalDevice> physicalDevice, SurfaceKHR &surface) :
_physicalDevice(physicalDevice),
_instance(&instance),
_surface(&surface)
{
    discoverAndPopulateDeviceProperties();
    discoverAndPopulateQueueFamilies();
    discoverAndPopulateSurfaceProperties();
    discoverIfDeviceExtensionsAreSupported();
}

bool PhysicalDevice::isSuitable()
{
    // We need at least graphics, present and transfer capabilities.
    // Transfer capability is implied by graphics and compute. If transfer is missing, assign to it the graphics family.
    if (!_queueFamilies.graphics.has_value())
    {
        return false;
    }
    if (!_queueFamilies.present.has_value())
    {
        return false;
    }
    if (!_queueFamilies.transfer.has_value())
    {
        _queueFamilies.transfer = _queueFamilies.graphics;
    }

    if (!_areRequiredDeviceExtensionsSupported)
    {
        return false;
    }

    if (_surfaceCapabilities.formats.empty() || _surfaceCapabilities.presentModes.empty())
    {
        return false;
    }

    if (!_features.samplerAnisotropy)
    {
        return false;
    }


    return _properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
//    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

void PhysicalDevice::discoverAndPopulateQueueFamilies()
{
    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies {queueFamilyCount};
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (usize i = 0; i < queueFamilies.size(); ++i)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && !_queueFamilies.graphics.has_value())
        {
            _queueFamilies.graphics = i;
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT && !_queueFamilies.compute.has_value())
        {
            _queueFamilies.compute = i;
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT && !_queueFamilies.transfer.has_value())
        {
            _queueFamilies.transfer = i;
        }

        if (!_queueFamilies.present.has_value())
        {
            VkBool32 presentSupport = false;
            ThrowError(vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, *_surface, &presentSupport));
            if (presentSupport)
            {
                _queueFamilies.present = i;
            }
        }
    }
}

void PhysicalDevice::discoverIfDeviceExtensionsAreSupported()
{
    uint32_t extensionCount = 0;
    ThrowError(vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr));

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    ThrowError(vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, availableExtensions.data()));

    std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
    for (const auto &extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    _areRequiredDeviceExtensionsSupported = requiredExtensions.empty();
}

void PhysicalDevice::discoverAndPopulateSurfaceProperties()
{
    ThrowError(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, *_surface, &_surfaceCapabilities.capabilities));

    uint32_t formatCount;
    ThrowError(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, *_surface, &formatCount, nullptr));
    if (formatCount != 0)
    {
        _surfaceCapabilities.formats.resize(formatCount);
        ThrowError(vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, *_surface, &formatCount, _surfaceCapabilities.formats.data()));
    }

    uint32_t presentModeCount;
    ThrowError(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, *_surface, &presentModeCount, nullptr));
    if (presentModeCount != 0)
    {
        _surfaceCapabilities.presentModes.resize(presentModeCount);
        ThrowError(vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, *_surface, &presentModeCount, _surfaceCapabilities.presentModes.data()));
    }
}

void PhysicalDevice::discoverAndPopulateDeviceProperties()
{
    vkGetPhysicalDeviceProperties(_physicalDevice, &_properties);
    vkGetPhysicalDeviceFeatures(_physicalDevice, &_features);
}

std::string_view PhysicalDevice::name()
{
    return _properties.deviceName;
}

Engine::uint32 PhysicalDevice::version()
{
    return _properties.apiVersion;
}
