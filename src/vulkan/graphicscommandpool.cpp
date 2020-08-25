#include "graphicscommandpool.h"

using namespace Engine::Vulkan;

GraphicsCommandPool GraphicsCommandPool::create(not_null<LogicalDevice*> device)
{
    uint32 queueFamily = device->queueFamilies().graphics.value();
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamily;
    poolInfo.flags = 0;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    ThrowError(vkCreateCommandPool(*device, &poolInfo, nullptr, &commandPool));

    return GraphicsCommandPool(commandPool, queueFamily, device);
}

GraphicsCommandPool::GraphicsCommandPool(VkHandle<VkCommandPool> commandPool, uint32 queueFamily,
                                         not_null<LogicalDevice *> device) :
                                         _commandPool(std::move(commandPool)),
                                         _queueFamily(queueFamily),
                                         _device(device)
{}

GraphicsCommandPool::~GraphicsCommandPool()
{
    if (_commandPool)
    {
        vkDestroyCommandPool(*_device, _commandPool, nullptr);
    }
}

uint32 GraphicsCommandPool::queueFamily() const
{
    return _queueFamily;
}