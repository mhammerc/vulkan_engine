#ifndef VULKAN_ENGINE_LOGICALDEVICE_H
#define VULKAN_ENGINE_LOGICALDEVICE_H

# include "vulkan.h"
# include "physicaldevice.h"

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

    [[nodiscard]] LogicalDevice static create(PhysicalDevice &&physicalDevice);

    ~LogicalDevice();
    operator VkDevice() const;

    Queues queues();

private:
    LogicalDevice(not_null<VkDevice> device, PhysicalDevice &&physicalDevice, Queues queues);

    not_null<VkDevice> _device;
    PhysicalDevice _physicalDevice;

    Queues _queues;
};
}

#endif //VULKAN_ENGINE_LOGICALDEVICE_H
