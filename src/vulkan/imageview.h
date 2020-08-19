#ifndef VULKAN_ENGINE_IMAGEVIEW_H
#define VULKAN_ENGINE_IMAGEVIEW_H

#include "vulkan.h"
#include "logicaldevice.h"
#include "image.h"

namespace Engine::Vulkan
{
class ImageView : public OnlyMovable
{
public:
    [[nodiscard]] static ImageView createFromImage(not_null<Image*> image, not_null<LogicalDevice*> device, VkFormat format, VkImageAspectFlags aspectFlags);

    ~ImageView();
    ImageView (ImageView &&) noexcept = default;
    ImageView &operator=(ImageView &&) noexcept = default;
    operator VkImageView() const;

private:
    ImageView(not_null<VkImageView> imageView, not_null<LogicalDevice*> device);

    VkHandle<VkImageView> _imageView;
    not_null<LogicalDevice*> _device;

};
}

#endif //VULKAN_ENGINE_IMAGEVIEW_H
