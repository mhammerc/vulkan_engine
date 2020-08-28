#include "descriptorpool.h"

using namespace Engine::Vulkan;

DescriptorPool DescriptorPool::create(not_null<LogicalDevice*> device)
{
    // hardcoded for now

    std::array<VkDescriptorPoolSize, 2> poolSizes {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(2);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(2);

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(2);

    VkDescriptorPool pool = VK_NULL_HANDLE;
    ThrowError(vkCreateDescriptorPool(*device, &poolInfo, nullptr, &pool));

    return DescriptorPool(pool, device);
}

DescriptorPool::DescriptorPool(VkHandle<VkDescriptorPool> pool, not_null<LogicalDevice *> device) :
_pool(std::move(pool)),
_device(device)
{}

DescriptorPool::~DescriptorPool()
{
    if (_pool)
    {
        vkDestroyDescriptorPool(*_device, _pool, nullptr);
    }
}

DescriptorPool::operator VkDescriptorPool() const
{
    return _pool;
}
