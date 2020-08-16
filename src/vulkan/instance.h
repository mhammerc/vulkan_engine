#ifndef VULKAN_ENGINE_INSTANCE_H
# define VULKAN_ENGINE_INSTANCE_H

# include <array>
# include <span>

# include "vulkan.h"

namespace Engine::Vulkan
{
class Instance
{
public:
    /**
     * Create a Vulkan Instance.
     *
     * @param layers Add additional layers on top of `requiredValidationLayers`.
     * @param extensions Add additional layers on top of `requiredDeviceExtensions`
     */
    Instance(std::span<char const *> const layers = {}, std::span<char const *> const extensions = {});
    ~Instance();

    static constexpr std::array<char const *, 1> const requiredValidationLayers
    {
        "VK_LAYER_KHRONOS_validation",
    };

    static constexpr std::array<char const *, 1> const requiredDeviceExtensions
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

private:
    VkInstance instance = VK_NULL_HANDLE;
};
}


#endif //VULKAN_ENGINE_INSTANCE_H
