#include "imageview.h"

using namespace Engine::Vulkan;

ImageView ImageView::createFromImage(not_null<Image*> image, not_null<LogicalDevice*> device, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo createInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = *image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange =
        {
            .aspectMask = aspectFlags,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    VkImageView imageView = VK_NULL_HANDLE;
    ThrowError(vkCreateImageView(*device, &createInfo, nullptr, &imageView));

    return ImageView(imageView, device);
}

ImageView::ImageView(not_null<VkImageView> imageView, not_null<LogicalDevice *> device) :
_imageView(imageView),
_device(device)
{}

ImageView::~ImageView()
{
    if (_imageView != VK_NULL_HANDLE)
    {
        spdlog::debug("destroy");
        vkDestroyImageView(*_device, _imageView, nullptr);
    }
}

ImageView::operator VkImageView() const
{
    return _imageView;
}

//ImageView &ImageView::operator=(ImageView &&other) noexcept
//{
//    _imageView = other._imageView;
//    other._imageView = VK_NULL_HANDLE;
//
//    return *this;
//}
//
//ImageView::ImageView(ImageView &&other) noexcept :
//_imageView(std::move(other._imageView)),
//_device(std::move(other._device))
//{
//    other._imageView = VK_NULL_HANDLE;
//}
