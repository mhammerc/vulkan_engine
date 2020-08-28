#include <stdexcept>
#include <stb_image.h>

#include "image.h"

using namespace Engine::Vulkan;

Image Image::createFromExistingWithoutOwnership(not_null<VkImage> image)
{
    return Image(image);
}

Image Image::createEmpty(not_null<LogicalDevice*> device, VkExtent2D size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
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

Image Image::createFromFile(const std::string &path, not_null<LogicalDevice *> device, CommandPool &commandPool, VkFormat format,
                            VkImageTiling tiling, VkImageUsageFlags usage)
{
    spdlog::debug("Loading texture {}.", path);

    int width {};
    int height {};
    int channels;

    stbi_uc *pixels = stbi_load(path.c_str(), &width, &height, &channels,
    STBI_rgb_alpha);

    VkExtent2D size {.width = static_cast<uint32>(width), .height = static_cast<uint32>(height)};

    if (!pixels)
    {
        throw std::runtime_error("can not open texture " + path);
    }

    VkDeviceSize imageSize = size.width * size.height * 4;
    Buffer stagingBuffer = Buffer::create(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, imageSize,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    {
        void *data = nullptr;
        stagingBuffer.map(&data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        stagingBuffer.unmap();
    }

    stbi_image_free(pixels);

    // The staging buffer now hold our texture
    // We have to copy the staging buffer to our image texture

    auto image = Image::createEmpty(device, size, format, tiling, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    // We have to send three commands:
    // - Transition
    // - Copy
    // - Transition

    auto cmdBuffer = CommandBuffer::create(device, &commandPool);
    cmdBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    image.cmdTransition(cmdBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    image.copyFromBuffer(cmdBuffer, stagingBuffer, size);
    image.cmdTransition(cmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    cmdBuffer.end();
    cmdBuffer.submit(true);

    return image;
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

void Image::cmdTransition(CommandBuffer &commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = _image,
        .subresourceRange =
         {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    VkPipelineStageFlags sourceStage {};
    VkPipelineStageFlags destinationStage {};

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);
}

void Image::copyFromBuffer(CommandBuffer &commandBuffer, Buffer &buffer, VkExtent2D size)
{
    VkBufferImageCopy region
    {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource =
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {.x = 0, .y = 0, .z = 0},
        .imageExtent = {.width = size.width, .height = size.height, 1},
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}
