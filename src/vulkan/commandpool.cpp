#include "commandpool.h"

using namespace Engine::Vulkan;

CommandPool CommandPool::create(not_null<LogicalDevice*> device)
{
    uint32 queueFamily = device->queueFamilies().graphics.value();
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    ThrowError(vkCreateCommandPool(*device, &poolInfo, nullptr, &commandPool));

    return CommandPool(commandPool, queueFamily, device);
}

CommandPool::CommandPool(VkHandle<VkCommandPool> commandPool, uint32 queueFamily,
                         not_null<LogicalDevice *> device) :
                                         _commandPool(std::move(commandPool)),
                                         _queueFamily(queueFamily),
                                         _device(device)
{}

CommandPool::~CommandPool()
{
    if (_commandPool)
    {
        vkDestroyCommandPool(*_device, _commandPool, nullptr);
    }
}

uint32 CommandPool::queueFamily() const
{
    return _queueFamily;
}

CommandPool::operator VkCommandPool() const
{
    return _commandPool;
}
