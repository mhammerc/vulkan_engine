#ifndef VULKAN_ENGINE_DEPTHSTENCILIMAGE_H
#define VULKAN_ENGINE_DEPTHSTENCILIMAGE_H

#include "vulkan.h"
#include "image.h"
#include "imageview.h"

namespace Engine::Vulkan
{
/**
 * This class just hold and auto-manage the regular Depth resources.
 */
class DepthStencilImage : public OnlyMovable
{
public:
    DepthStencilImage(DepthStencilImage &&) = default;
    DepthStencilImage &operator=(DepthStencilImage &&) = default;

    [[nodiscard]] static DepthStencilImage create(not_null<LogicalDevice*> device, VkExtent2D size);

    [[nodiscard]] not_null<Image*> image();
    [[nodiscard]] not_null<ImageView*> view();
    [[nodiscard]] VkFormat format() const;

private:
    DepthStencilImage(Image &&image, ImageView &&view);

    // Note: _image must be constructed before _view.
    //       _image must be destructed after _view.
    // Keep members in that exact order.
    Image _image;
    ImageView _view;
};
}

#endif //VULKAN_ENGINE_DEPTHSTENCILIMAGE_H
