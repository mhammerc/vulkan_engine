#include "descriptorset.h"
#include <array>

using namespace Engine::Vulkan;

DescriptorSet::~DescriptorSet()
{
//    if (_descriptorSet)
//    {
//        VkDescriptorSet descriptorSet = _descriptorSet;
//        vkFreeDescriptorSets(*_device, *_descriptorPool, 1, &descriptorSet);
//    }
}

DescriptorSet::DescriptorSet(VkHandle<VkDescriptorSet> descriptorSet, not_null<LogicalDevice *> device,
                             not_null<DescriptorPool *> pool) :
                             _descriptorSet(std::move(descriptorSet)),
                             _device(device),
                             _descriptorPool(pool)
{}

DescriptorSet::operator VkDescriptorSet() const
{
    return _descriptorSet;
}

std::vector<DescriptorSet> DescriptorSet::createManyFromBuffers(not_null<LogicalDevice*> device,
                                                                not_null<DescriptorPool*> pool,
                                                                not_null<VkDescriptorSetLayout> layout,
                                                                std::vector<Vulkan::UniformBuffer> &buffers,
                                                                Sampler &sampler, ImageView &imageView)
{
    usize count = buffers.size();

    std::vector<VkDescriptorSet> descriptorSets(count);

    std::vector<VkDescriptorSetLayout> layouts(count, layout);
    VkDescriptorSetAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = *pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
    allocInfo.pSetLayouts = layouts.data();

    ThrowError(vkAllocateDescriptorSets(*device, &allocInfo, descriptorSets.data()));

    for (usize i = 0; i < descriptorSets.size(); ++i)
    {
        VkDescriptorBufferInfo bufferInfo
        {
            .buffer = buffers[i].buffer(),
            .offset = 0,
            .range = sizeof(Vulkan::UniformBuffer::UniformBufferObject)
        };

        VkDescriptorImageInfo imageInfo
        {
            .sampler = sampler,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        // Each set have two bindings: 1 UBO and 1 texture
        std::array<VkWriteDescriptorSet, 2> writesInfos
        {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,
            },
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
            },
        };

        vkUpdateDescriptorSets(*device, static_cast<uint32>(writesInfos.size()), writesInfos.data(), 0, nullptr);
    }

    std::vector<DescriptorSet> result;
    result.reserve(count);

    for (auto &set : descriptorSets)
    {
        result.push_back(DescriptorSet(set, device, pool));
    }

    return result;
}

