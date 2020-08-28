#ifndef VULKAN_ENGINE_DESCRIPTORPOOL_H
#define VULKAN_ENGINE_DESCRIPTORPOOL_H

#include <array>

#include "vulkan.h"
#include "logicaldevice.h"


namespace Engine::Vulkan
{
class DescriptorPool : public OnlyMovable
{
public:
    [[nodiscard]] static DescriptorPool create(not_null<LogicalDevice*> device);

    ~DescriptorPool();
    DescriptorPool(DescriptorPool &&) noexcept = default;
    DescriptorPool &operator=(DescriptorPool &&) noexcept = default;
    operator VkDescriptorPool() const;

private:
    DescriptorPool(VkHandle<VkDescriptorPool> pool, not_null<LogicalDevice*> device);

    VkHandle<VkDescriptorPool> _pool;
    not_null<LogicalDevice*> _device;
};
}

#endif //VULKAN_ENGINE_DESCRIPTORPOOL_H
