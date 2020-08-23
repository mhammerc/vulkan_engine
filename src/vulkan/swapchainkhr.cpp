#include "swapchainkhr.h"

using namespace Engine::Vulkan;

SwapchainKHR::~SwapchainKHR()
{
    // Clear _imagesView first, which were create from _images.
    _imagesViews.clear();
    // Then, clear _images which were created by the swapchain.
    _images.clear();
    // Finally, delete the swapchain.
    if (_swapchain)
    {
        vkDestroySwapchainKHR(*_device, _swapchain, nullptr);
    }
}

SwapchainKHR::SwapchainKHR(not_null<VkSwapchainKHR> swapchain, not_null<LogicalDevice*> device,
std::vector<Image> &&images, std::vector<ImageView> &&imagesViews, VkSurfaceFormatKHR format,
PresentingAlgorithm presentingAlgorithm) :
_swapchain(swapchain),
_device(device),
_images(std::move(images)),
_imagesViews(std::move(imagesViews)),
_format(format),
_presentingAlgorithm(presentingAlgorithm)
{}

SwapchainKHR SwapchainKHR::create(not_null<LogicalDevice*> device, not_null<SurfaceKHR*> surface)
{
    auto surfaceCapabilities = device->surfaceCapabilities();
    auto queueFamilies = device->queueFamilies();

    VkSurfaceFormatKHR bestFormat = findBestFormat(surfaceCapabilities.formats);
    PresentingAlgorithm presentingAlgorithm = findBestPresentMode(surfaceCapabilities);

    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    createInfo.imageFormat = bestFormat.format;
    createInfo.imageColorSpace = bestFormat.colorSpace;
    createInfo.presentMode = presentingAlgorithm.mode;
    createInfo.minImageCount = presentingAlgorithm.bufferingSize;
    createInfo.surface = *surface;
    createInfo.imageExtent = surface->size();
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (*queueFamilies.graphics != *queueFamilies.present)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;

        std::array<uint32, 2> queueIndices {*queueFamilies.graphics, *queueFamilies.present};
        createInfo.pQueueFamilyIndices = queueIndices.data();
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = surfaceCapabilities.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    ThrowError(vkCreateSwapchainKHR(*device, &createInfo, nullptr, &swapchain), "create swapchain");

    uint32 imageCount = 0;
    ThrowError(vkGetSwapchainImagesKHR(*device, swapchain, &imageCount, nullptr));
    std::vector<VkImage> vkImages(imageCount);
    ThrowError(vkGetSwapchainImagesKHR(*device, swapchain, &imageCount, vkImages.data()));

    std::vector<Image> images {};
    std::vector<ImageView> imagesViews {};
    images.reserve(imageCount);
    imagesViews.reserve(imageCount);
    for (auto vkImage : vkImages)
    {
        auto image = Image::createFromExistingWithoutOwnership(vkImage);
        auto imageView = ImageView::createFromImage(&image, device, bestFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
        images.push_back(std::move(image));
        imagesViews.push_back(std::move(imageView));
    }

    return SwapchainKHR(swapchain, device, std::move(images), std::move(imagesViews), bestFormat, presentingAlgorithm);
}

VkSurfaceFormatKHR SwapchainKHR::findBestFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    // Simple, we want to work in SRGB color space.
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

SwapchainKHR::PresentingAlgorithm SwapchainKHR::findBestPresentMode(LogicalDevice::SurfaceCapabilities const &surfaceCapabilities)
{
    // For slower draw than refresh-rate, see https://developer.samsung.com/sdp/blog/en-us/2019/07/26/vulkan-mobile-best-practice-how-to-configure-your-vulkan-swapchain

    // Assuming faster draw than display refresh-rate.

    // Mailbox allow:
    // - V-Sync
    // - Perpetually draw and present new frames, even if draw loop is faster. (decreased latency as displayed image
    //   is as new as possible)

    // Fifo allow:
    // - V-Sync
    // - Block the draw loop as soon as presenting queue is full (increased latency but reduce power consumption)
    // FIFO is the only present mode that *must* be supported.

    // Immediate allow:
    // - No vsync
    // - Perpetually sent frames to display (faster than display is capable of)

    // Tripple-buffering allow to have 3 images on which we draw one after another.
    // It allow to not to wait for the first image to be sent to display:
    // - First image is being displayed (transfer to display) (takes time especially because v-sync)
    // - Second image is ready to be sent do display just after the first image
    // - Third image is being drawn (takes time)
    // Tripple-buffering effectively allow to parallelize presenting and drawing.

    // Double-buffering may involve pause in the draw-loop:
    // - First image is being displayed (transfer to display) (takes time)
    // - Second image is ready to be sent to display
    // We are now waiting.

    // Mailbox works best with tripple-buffering to reduce latency as we will never wait for the first image and
    // always work on second and third image, swapping them as we draw.

    // Fifo doesn't work good with tripple-buffering as it increase latency (any advantage for fifo tripple-buffering?)
    // as we have two images waiting to be displayed, the third image being displayed in a lot of time.

    // Immediate works best with double-buffering as presenting shouldn't be blocking, therefore we can swap
    // first and second image anytime.

    // Note that whatever choosen display-synchronisation (or lack of), draw-loop and update-loop should run on
    // different threads. Draw loop could even-be multithreaded.
    // It is especially important update-loop runs at constant rate.

    // display rate vs draw-loop speed -> how old the image displayed is:
    // - Mailbox double-buffering
    //     - 120hz display, drawing at 240hz -> image displayed max 8ms old
    //     - 60hz display, drawing at 240hz -> image displayed max 16ms old
    //
    // - Mailbox tripple-buffering
    //     - 120hz display, drawing at 240hz -> image displayed max 4ms old
    //     - 60hz display, drawing at 240hz -> image displayed max 4ms old
    //
    // - FIFO double-buffering
    //     - 120hz display, can draw as fast as 240hz -> image displayed max 12ms old
    //     - 60hz display, can draw as fast as 240hz -> image displayed max 20ms old
    //
    // - FIFO tripple-buffering
    //     - 120hz display, can draw as fast as 240hz -> image displayed max 16ms old
    //     - 60hz display, can draw as fast as 240hz -> image displayed max 33ms old
    //
    // - Immediate (with tearing)
    //     - 120hz display, can draw at 240hz -> should be less than mailbox but introduce tearing
    //     - 60hz display, can draw at 240hz -> should be less than mailbox but introduce tearing

    // We choose FIFO double-bufferd to stick to standard v-sync and v-sync will be our draw-synchronization-primitive.
    // Latency isn't a big deal to us, the lower gpu and cpu consumption is, the better.
    // FIFO is also the only present mode that is *required* to be supported.
    return
    {
        .mode = VK_PRESENT_MODE_FIFO_KHR,
        .bufferingSize = std::clamp(2u, surfaceCapabilities.capabilities.minImageCount, surfaceCapabilities.capabilities.maxImageCount)
    };
}

SwapchainKHR::operator VkSwapchainKHR() const
{
    return _swapchain;
}

VkSurfaceFormatKHR SwapchainKHR::format() const
{
    return _format;
}
