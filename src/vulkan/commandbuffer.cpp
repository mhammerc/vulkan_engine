#include "commandbuffer.h"

using namespace Vulkan;

std::vector<CommandBuffer> CommandBuffer::createMany(usize count, not_null<LogicalDevice*> device, not_null<CommandPool*> commandPool)
{

    VkCommandBufferAllocateInfo info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = *commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<uint32>(count),
    };

    std::vector<VkCommandBuffer> vkBuffers(count);
    ThrowError(vkAllocateCommandBuffers(*device, &info, vkBuffers.data()));
    // TODO: in case of error, should free command buffers which were successfully built.

    std::vector<Vulkan::CommandBuffer> buffers;
    buffers.reserve(count);

    for (usize i = 0; i < count; ++i)
    {
        buffers.push_back(std::move(CommandBuffer(vkBuffers[i], device, commandPool)));
    }

    return buffers;
}

CommandBuffer CommandBuffer::create(not_null<LogicalDevice *> device, not_null<CommandPool *> commandPool)
{
    auto buffers = createMany(1, device, commandPool);
    return std::move(buffers[0]);
}

CommandBuffer::~CommandBuffer()
{
    if (_commandBuffer)
    {
        VkCommandBuffer handle = _commandBuffer;
        vkFreeCommandBuffers(*_device, *_commandPool, 1, &handle);
    }
}

CommandBuffer::CommandBuffer(VkHandle<VkCommandBuffer> commandBuffer, not_null<LogicalDevice *> device,
                             not_null<CommandPool *> commandPool) :
                             _commandBuffer(std::move(commandBuffer)),
                             _device(device),
                             _commandPool(commandPool)
{}

void CommandBuffer::begin(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo beginInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = flags
    };

    ThrowError(vkBeginCommandBuffer(_commandBuffer, &beginInfo));
}

void CommandBuffer::end()
{
    ThrowError(vkEndCommandBuffer(_commandBuffer));
}

void CommandBuffer::submit(bool wait)
{
    VkCommandBuffer cmdBuffer = _commandBuffer;

    VkSubmitInfo submitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmdBuffer,
    };

    ThrowError(vkQueueSubmit(_device->queues().graphics, 1, &submitInfo, VK_NULL_HANDLE));

    if (wait)
    {
        ThrowError(vkQueueWaitIdle(_device->queues().graphics));
    }
}

CommandBuffer::operator VkCommandBuffer() const
{
    return _commandBuffer;
}

void CommandBuffer::reset()
{
    ThrowError(vkResetCommandBuffer(_commandBuffer, 0));
}
