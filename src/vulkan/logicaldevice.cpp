#include "logicaldevice.h"
#include <set>
#include <utility>

using namespace Engine::Vulkan;

LogicalDevice LogicalDevice::create(PhysicalDevice &&physicalDevice)
{
    auto everyQueueFamilies = physicalDevice.queueFamilies();

    if (!everyQueueFamilies.graphics.has_value() || !everyQueueFamilies.present.has_value() || !everyQueueFamilies.transfer.has_value())
    {
        throw std::runtime_error("PhysicalDevice miss one or more of these queues families: graphic, present and transfer.");
    }

    // We will create 1 queue per required queue family.
    // All the queue families could be the same. Make all of them uniques.
    std::set<uint32> queueFamilies {*everyQueueFamilies.graphics, *everyQueueFamilies.present, *everyQueueFamilies.transfer};

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.reserve(queueFamilies.size());

    // Create one VkDeviceQueueCreateInfo per queue we need.
    float queuePriority = 1.f;
    for (auto const queueFamily : queueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32>(PhysicalDevice::requiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = PhysicalDevice::requiredDeviceExtensions.data();

    createInfo.enabledLayerCount = static_cast<uint32>(PhysicalDevice::requiredValidationLayers.size());
    createInfo.ppEnabledLayerNames = PhysicalDevice::requiredValidationLayers.data();

    VkDevice device = VK_NULL_HANDLE;
    ThrowError(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));

    // Our device and queues are created.
    // We created one queue per queue family, but we may be using many queue families.
    // Get each queue we require, all this queues may be the same queue (same handle).
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue transferQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    vkGetDeviceQueue(device, *everyQueueFamilies.graphics, 0, &graphicsQueue);
    vkGetDeviceQueue(device, *everyQueueFamilies.transfer, 0, &transferQueue);
    vkGetDeviceQueue(device, *everyQueueFamilies.present, 0, &presentQueue);

    Queues const queues
    {
        .graphics = graphicsQueue,
        .present = presentQueue,
        .transfer = transferQueue,
    };

    return LogicalDevice(device, std::move(physicalDevice), queues);
}

LogicalDevice::LogicalDevice(not_null<VkDevice> device, PhysicalDevice &&physicalDevice, Queues queues) :
_device(device),
_physicalDevice(std::move(physicalDevice)),
_queues(std::move(queues))
{}

LogicalDevice::~LogicalDevice()
{
    if (_device)
    {
        vkDestroyDevice(_device, nullptr);
    }
}

LogicalDevice::operator VkDevice() const
{
    return _device;
}

LogicalDevice::Queues LogicalDevice::queues() const
{
    return _queues;
}

LogicalDevice::SurfaceCapabilities LogicalDevice::surfaceCapabilities() const
{
    return _physicalDevice.surfaceCapabilities();
}

VkPhysicalDeviceProperties LogicalDevice::properties() const
{
    return _physicalDevice.properties();
}

VkPhysicalDeviceFeatures LogicalDevice::features() const
{
    return _physicalDevice.features();
}

LogicalDevice::QueueFamilies LogicalDevice::queueFamilies() const
{
    return _physicalDevice.queueFamilies();
}
