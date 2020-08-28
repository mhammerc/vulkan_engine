#include "sampler.h"

using namespace Engine::Vulkan;

Sampler Sampler::create(not_null<LogicalDevice*> device)
{
    VkSamplerCreateInfo samplerInfo {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.f;
    samplerInfo.minLod = 0.f;
    samplerInfo.maxLod = 0.f;

    VkSampler sampler = VK_NULL_HANDLE;
    ThrowError(vkCreateSampler(*device, &samplerInfo, nullptr, &sampler));

    return Sampler(sampler, device);
}

Sampler::Sampler(VkHandle<VkSampler> sampler, not_null<LogicalDevice *> device) :
_sampler(std::move(sampler)),
_device(device)
{}

Sampler::~Sampler()
{
    if (_sampler)
    {
        vkDestroySampler(*_device, _sampler, nullptr);
    }
}

Sampler::operator VkSampler() const
{
    return _sampler;
}
