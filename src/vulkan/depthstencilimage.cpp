#include "depthstencilimage.h"

using namespace Engine::Vulkan;

DepthStencilImage::DepthStencilImage(Image &&image, ImageView &&view) :
_image(std::move(image)),
_view(std::move(view))
{}

DepthStencilImage DepthStencilImage::create(not_null<LogicalDevice*> device, VkExtent2D size)
{
    Image image = Image::createEmpty(device, size,
                                     VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_TILING_OPTIMAL,
                                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

    ImageView view = ImageView::createFromImage(&image, device, VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_IMAGE_ASPECT_DEPTH_BIT);

    return DepthStencilImage(std::move(image), std::move(view));
}

not_null<Image *> DepthStencilImage::image()
{
    return &_image;
}

not_null<ImageView *> DepthStencilImage::view()
{
    return &_view;
}

VkFormat DepthStencilImage::format() const
{
    return VK_FORMAT_D32_SFLOAT_S8_UINT;
}
