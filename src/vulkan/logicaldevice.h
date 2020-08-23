#ifndef VULKAN_ENGINE_LOGICALDEVICE_H
#define VULKAN_ENGINE_LOGICALDEVICE_H

# include "vulkan.h"
# include "physicaldevice.h"
# include "deviceallocator.h"

namespace Engine::Vulkan
{
/**
 * Represent a `VkDevice`.
 *
 * This class also become the ownership of a `PhysicalDevice` instance as the engine only support one logical device,
 * bonded to one physical device.
 */
class LogicalDevice : public OnlyMovable
{
public:
    struct Queues
    {
        not_null<VkQueue> graphics;
        not_null<VkQueue> present;
        not_null<VkQueue> transfer;
    };

    // Alias some types from `PhysicalDevice` to make the public-API cleaner.
    using SurfaceCapabilities = PhysicalDevice::SurfaceCapabilities;
    using QueueFamilies = PhysicalDevice::QueueFamilies;

    /**
     * `LogicalDevice` will take ownership of a `PhysicalDevice`.
     */
    [[nodiscard]] static LogicalDevice create(PhysicalDevice &&physicalDevice);

    ~LogicalDevice();
    LogicalDevice (LogicalDevice &&) noexcept = default;
    LogicalDevice &operator=(LogicalDevice &&) noexcept = default;
    operator VkDevice() const;

    [[nodiscard]] DeviceAllocator &allocator();

    [[nodiscard]] Queues queues() const;
    [[nodiscard]] QueueFamilies queueFamilies() const;
    [[nodiscard]] SurfaceCapabilities surfaceCapabilities() const;
    [[nodiscard]] VkPhysicalDeviceProperties properties() const;
    [[nodiscard]] VkPhysicalDeviceFeatures features() const;
    [[nodiscard]] VkPhysicalDeviceMemoryProperties memories() const;

private:
    LogicalDevice(not_null<VkDevice> device, PhysicalDevice &&physicalDevice, Queues queues);

    VkHandle<VkDevice> _device;
    PhysicalDevice _physicalDevice;
    std::unique_ptr<DeviceAllocator> _allocator;

    Queues _queues;
};
}

#endif //VULKAN_ENGINE_LOGICALDEVICE_H
