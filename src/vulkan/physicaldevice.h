#ifndef VULKAN_ENGINE_PHYSICALDEVICE_H
# define VULKAN_ENGINE_PHYSICALDEVICE_H

# include <array>
# include <optional>

# include "vulkan.h"
# include "instance.h"
# include "surfacekhr.h"

namespace Engine::Vulkan
{
class PhysicalDevice : public OnlyMovable
{
public:
    static constexpr std::array const requiredDeviceExtensions
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    static constexpr std::array const requiredValidationLayers
    {
#ifndef NDEBUG
        "VK_LAYER_KHRONOS_validation",
#endif
    };

    struct QueueFamilies
    {
        std::optional<uint32> graphics = std::nullopt;
        std::optional<uint32> compute = std::nullopt;
        std::optional<uint32> transfer = std::nullopt;
        std::optional<uint32> present = std::nullopt;
    };

    struct SurfaceCapabilities
    {
        VkSurfaceCapabilitiesKHR capabilities {};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    /**
     * Find a suitable GPU to use.
     *
     * Requirements are:
     * - Graphics Queue
     * - Presenting capability queue (to the surface)
     * - Availability of device extensions
     * - Support a least one surface format
     * - Feature sampler anisotropy
     *
     * A `SurfaceKHR` instance is an abstraction layer to the native surface handle of the windowing system.
     * It is a required parameter because it allow us to determine which queue family, if any, allow presenting to
     * that particular surface.
     *
     * @param instance
     * @param surface
     * @return
     */
    [[nodiscard]] static std::optional<PhysicalDevice> findBest(not_null<Instance*> instance, not_null<SurfaceKHR*> surface);
    operator VkPhysicalDevice() const;

    [[nodiscard]] std::string_view name() const;
    [[nodiscard]] uint32 version() const;
    [[nodiscard]] QueueFamilies queueFamilies() const;
    [[nodiscard]] SurfaceCapabilities surfaceCapabilities() const;
    [[nodiscard]] VkPhysicalDeviceProperties properties() const;
    [[nodiscard]] VkPhysicalDeviceFeatures features() const;

private:
    PhysicalDevice(not_null<VkPhysicalDevice> physicalDevice, not_null<Instance*> instance, not_null<SurfaceKHR*> surface);

    not_null<VkPhysicalDevice> _physicalDevice;
    not_null<Instance *> _instance;
    not_null<SurfaceKHR *> _surface;

    QueueFamilies _queueFamilies {};
    SurfaceCapabilities _surfaceCapabilities {};
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
