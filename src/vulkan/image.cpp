#include "image.h"
#include <stdexcept>

using namespace Engine::Vulkan;

Image Image::createFromExistingWithoutOwnership(not_null<VkImage> image)
{
    return Image(image);
}

Image::Image(not_null<VkImage> image) :
_image(image)
{}

Image::~Image()
{
    if (_device.has_value())
    {
        // We are the owner of the _image resource
        if (_image)
        {
            vkDestroyImage(**_device, _image, nullptr);
        }
    }
}

Image::operator VkImage() const
{
    return _image;
}
