#ifndef VULKAN_ENGINE_PHYSICALDEVICE_H
# define VULKAN_ENGINE_PHYSICALDEVICE_H

# include <array>
# include <gsl/gsl>
# include <optional>

# include "vulkan.h"
# include "instance.h"
# include "surfacekhr.h"

namespace Engine::Vulkan
{
class PhysicalDevice
{
public:
    [[nodiscard]] static std::optional<PhysicalDevice> findBest(Instance &instance, SurfaceKHR &surface);

    std::string_view name();
    uint32 version();

    static constexpr std::array const requiredDeviceExtensions
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

private:
    PhysicalDevice(Instance &instance, not_null<VkPhysicalDevice> physicalDevice, SurfaceKHR &surface);

    not_null<VkPhysicalDevice> _physicalDevice;
    not_null<Instance *> _instance;
    not_null<SurfaceKHR *> _surface;

    struct
    {
        std::optional<uint32> graphics = std::nullopt;
        std::optional<uint32> compute = std::nullopt;
        std::optional<uint32> transfer = std::nullopt;
        std::optional<uint32> present = std::nullopt;
    } _queueFamilies {};

    struct
    {
        VkSurfaceCapabilitiesKHR capabilities {};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    } _surfaceCapabilities {};

    VkPhysicalDeviceProperties _properties {};
    VkPhysicalDeviceFeatures _features {};
    bool _areRequiredDeviceExtensionsSupported = false;

    /**
     * Should return a score instead of a boolean to choose wisely the right GPU.
     */
    [[nodiscard]] bool isSuitable();
    void discoverAndPopulateDeviceProperties();
    void discoverAndPopulateQueueFamilies();
    void discoverAndPopulateSurfaceProperties();
    void discoverIfDeviceExtensionsAreSupported();
};
}

#endif //VULKAN_ENGINE_PHYSICALDEVICE_H
