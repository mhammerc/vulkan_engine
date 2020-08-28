#include "buffer.h"

using namespace Engine::Vulkan;

Buffer Buffer::create(not_null<LogicalDevice *> device, VkBufferUsageFlags usage, VkDeviceSize size, VkMemoryPropertyFlags properties)
{
    VkBufferCreateInfo bufferInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VkBuffer buffer = VK_NULL_HANDLE;
    ThrowError(vkCreateBuffer(*device, &bufferInfo, nullptr, &buffer));

    VkMemoryRequirements requirements {};
    vkGetBufferMemoryRequirements(*device, buffer, &requirements);

    auto alloc = device->allocator().allocate(requirements, properties);

    ThrowError(vkBindBufferMemory(*device, buffer, alloc.memory, alloc.offset));

    return Buffer(buffer, alloc, device);
}

Buffer::Buffer(VkHandle<VkBuffer> buffer, DeviceAllocator::ResourceMemory allocation, not_null<LogicalDevice *> device) :
    _buffer(std::move(buffer)),
    _allocation(allocation),
    _device(device)
{}

Buffer::~Buffer()
{
    if (_buffer)
    {
        vkDestroyBuffer(*_device, _buffer, nullptr);
        _device->allocator().free(_allocation);
    }
}

Buffer::operator VkBuffer () const
{
    return _buffer;
}

void Buffer::map(void **ptr)
{
    ThrowError(vkMapMemory(*_device, _allocation.memory, _allocation.offset, _allocation.size, 0, ptr));
}

void Buffer::unmap()
{
    vkUnmapMemory(*_device, _allocation.memory);
}

void Buffer::cmdCopy(CommandBuffer &commandBuffer, Buffer &dst, Buffer &src, VkDeviceSize size)
{
    VkBufferCopy copyRegion {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
}
