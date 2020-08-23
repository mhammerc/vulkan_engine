#include "image.h"
#include <stdexcept>

using namespace Engine::Vulkan;

Image Image::createFromExistingWithoutOwnership(not_null<VkImage> image)
{
    return Image(image);
}

Image Image::create(not_null<LogicalDevice*> device, VkExtent2D size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
{
    VkImageCreateInfo createInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {.width = size.width, .height = size.height, .depth = 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkImage image = VK_NULL_HANDLE;
    ThrowError(vkCreateImage(*device, &createInfo, nullptr, &image));

    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(*device, image, &requirements);

    DeviceAllocator::ResourceMemory mem = device->allocator().allocate(requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkBindImageMemory(*device, image, mem.memory, mem.offset);

    return Image(image, device, mem);
}

Image::Image(not_null<VkImage> image) :
_image(image)
{}

Image::Image(not_null<VkImage> image, not_null<LogicalDevice *> device, DeviceAllocator::ResourceMemory suballocation) :
_image(image),
_device(device),
_suballocation(suballocation)
{}

Image::~Image()
{
    if (_device.has_value())
    {
        // We are the owner of the _image resource
        if (_image)
        {
            vkDestroyImage(**_device, _image, nullptr);
            _device->get()->allocator().free(*_suballocation);
        }
    }
}

Image::operator VkImage() const
{
    return _image;
}
