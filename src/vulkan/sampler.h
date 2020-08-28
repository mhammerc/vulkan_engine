#ifndef VULKAN_ENGINE_SAMPLER_H
#define VULKAN_ENGINE_SAMPLER_H

#include "vulkan.h"
#include "logicaldevice.h"

namespace Engine::Vulkan
{
class Sampler : public OnlyMovable
{
public:
    [[nodiscard]] static Sampler create(not_null<LogicalDevice*> device);

    ~Sampler();
    Sampler(Sampler &&) = default;
    Sampler &operator=(Sampler &&) = default;
    operator VkSampler() const;
private:
    Sampler(VkHandle<VkSampler> sampler, not_null<LogicalDevice*> device);

    VkHandle<VkSampler> _sampler;
    not_null<LogicalDevice*> _device;

};
}

#endif //VULKAN_ENGINE_SAMPLER_H
