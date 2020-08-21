#ifndef VULKAN_ENGINE_INSTANCE_H
# define VULKAN_ENGINE_INSTANCE_H

# include <array>
# include <optional>
# include <span>
# include <string>

# include "vulkan.h"

namespace Engine::Vulkan
{
/**
 * Represent a Vulkan Instance.
 *
 * If debug mode is enabled, it will automatically setup validation layers and debug messengers.
 */
class Instance : public OnlyMovable
{
public:
    static constexpr std::array const requiredValidationLayers
    {
#ifndef NDEBUG
        "VK_LAYER_KHRONOS_validation",
#endif
    };

    static constexpr std::array const requiredInstanceExtensions
    {
#ifndef NDEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    };

    /**
     * Create a Vulkan Instance.
     *
     * @param layers Add additional layers on top of `requiredValidationLayers`.
     * @param extensions Add additional layers on top of `requiredDeviceExtensions`
     */
    ~Instance();
    Instance (Instance &&) noexcept = default;
    Instance &operator=(Instance &&) noexcept = default;
    operator VkInstance() const;

    [[nodiscard]] static Instance create(std::span<char const *> layers = {}, std::span<char const *> extensions = {});

    [[nodiscard]] not_null<VkInstance> get() const;

private:
    Instance(not_null<VkInstance> instance
#ifndef NDEBUG
, not_null<VkDebugUtilsMessengerEXT> debugMessenger
#endif
);

    VkHandle<VkInstance> _instance;

#ifndef NDEBUG
    VkHandle<VkDebugUtilsMessengerEXT> _debugMessenger;
#endif

    /**
     * Check the availability of every layers specified inside `layers`.
     * @param layers List of required layers.
     * @return `std::nullopt` if every layers are available. Otherwise, it return the first missing layer.
     */
    static std::optional<char const *> areLayersAvailable(std::vector<char const *> const &layers);

    /**
     * Check the availability of every extensions specified inside `extensions`.
     * @param extensions List of required extensions.
     * @return `std::nullopt` if every extensions are available. Otherwise, it return the first missing extension.
     */
    static std::optional<char const *> areExtensionsAvailable(std::vector<char const *> const &extensions);

#ifndef NDEBUG
    static not_null<VkDebugUtilsMessengerEXT> setupDebugMessenger(not_null<VkInstance> instance);
#endif
};
}


#endif //VULKAN_ENGINE_INSTANCE_H
