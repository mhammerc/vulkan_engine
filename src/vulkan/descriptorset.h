#ifndef VULKAN_ENGINE_DESCRIPTORSET_H
#define VULKAN_ENGINE_DESCRIPTORSET_H

#include "vulkan.h"
#include "logicaldevice.h"
#include "descriptorpool.h"
#include "uniformbuffer.h"
#include "imageview.h"
#include "sampler.h"

namespace Engine::Vulkan
{
class DescriptorSet : OnlyMovable
{
public:
    [[nodiscard]] static std::vector<DescriptorSet> createManyFromBuffers(not_null<LogicalDevice*> device,
                                                                          not_null<DescriptorPool*> pool, not_null<VkDescriptorSetLayout> layout,
                                                                          std::vector<Vulkan::UniformBuffer> &buffers,
                                                                          Sampler &sampler, ImageView &imageView);

    ~DescriptorSet();
    DescriptorSet(DescriptorSet &&) = default;
    DescriptorSet &operator=(DescriptorSet &&) = default;
    operator VkDescriptorSet() const;

private:
    DescriptorSet(VkHandle<VkDescriptorSet> descriptorSet, not_null<LogicalDevice*> device, not_null<DescriptorPool*> pool);

    VkHandle<VkDescriptorSet> _descriptorSet;
    not_null<LogicalDevice*> _device;
    not_null<DescriptorPool*> _descriptorPool;

};
}

#endif //VULKAN_ENGINE_DESCRIPTORSET_H
