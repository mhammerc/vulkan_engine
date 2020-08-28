#ifndef VULKAN_ENGINE_BUFFER_H
#define VULKAN_ENGINE_BUFFER_H

#include "vulkan.h"
#include "logicaldevice.h"
#include "commandbuffer.h"

namespace Engine::Vulkan
{
class Buffer : OnlyMovable
{
public:
    [[nodiscard]] static Buffer create(not_null<LogicalDevice*> device, VkBufferUsageFlags usage, VkDeviceSize size,
                                       VkMemoryPropertyFlags properties);

    static void cmdCopy(CommandBuffer &commandBuffer, Buffer &dst, Buffer &src, VkDeviceSize size);

    ~Buffer();
    Buffer(Buffer &&) = default;
    Buffer &operator=(Buffer &&) = default;
    operator VkBuffer() const;

    void map(void **ptr);
    void unmap();

private:
    Buffer(VkHandle<VkBuffer> buffer, DeviceAllocator::ResourceMemory allocation, not_null<LogicalDevice*> device);

    VkHandle<VkBuffer> _buffer;
    DeviceAllocator::ResourceMemory _allocation;

    not_null<LogicalDevice*> _device;
};
}

#endif //VULKAN_ENGINE_BUFFER_H
