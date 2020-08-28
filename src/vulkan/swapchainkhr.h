#ifndef VULKAN_ENGINE_SWAPCHAINKHR_H
# define VULKAN_ENGINE_SWAPCHAINKHR_H

# include <vector>

# include "vulkan.h"
# include "logicaldevice.h"
# include "image.h"
# include "imageview.h"

namespace Engine::Vulkan
{
class SwapchainKHR : public OnlyMovable
{
public:
    [[nodiscard]] static SwapchainKHR create(not_null<LogicalDevice*> device, not_null<SurfaceKHR*> surface);

    ~SwapchainKHR();
    SwapchainKHR (SwapchainKHR &&) noexcept = default;
    SwapchainKHR &operator=(SwapchainKHR &&) noexcept = default;
    operator VkSwapchainKHR() const;

    [[nodiscard]] VkSurfaceFormatKHR format() const;
    [[nodiscard]] std::vector<not_null<ImageView*>> views();

private:
    // Describe what algorithm will be used to display image to screen.
    // See `findBestPresentMode` for more details.
    struct PresentingAlgorithm
    {
        VkPresentModeKHR mode;
        uint32 bufferingSize;
    };

    SwapchainKHR(not_null<VkSwapchainKHR> swapchain, not_null<LogicalDevice*> device, std::vector<Image> &&images,
    std::vector<ImageView> &&imagesViews, VkSurfaceFormatKHR format, PresentingAlgorithm presentingAlgorithm);

    VkHandle<VkSwapchainKHR> _swapchain;
    not_null<LogicalDevice*> _device;
    std::vector<Image> _images;
    std::vector<ImageView> _imagesViews;

    VkSurfaceFormatKHR _format;
    PresentingAlgorithm _presentingAlgorithm;

    [[nodiscard]] static VkSurfaceFormatKHR findBestFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats);
    [[nodiscard]] static PresentingAlgorithm findBestPresentMode(LogicalDevice::SurfaceCapabilities const &surfaceCapabilities);
};
}

#endif //VULKAN_ENGINE_SWAPCHAINKHR_H
