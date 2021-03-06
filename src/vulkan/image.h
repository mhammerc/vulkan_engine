#ifndef VULKAN_ENGINE_IMAGE_H
#define VULKAN_ENGINE_IMAGE_H

#include <string>

#include "vulkan.h"
#include "logicaldevice.h"
#include "commandpool.h"
#include "commandbuffer.h"
#include "buffer.h"

namespace Engine::Vulkan
{
class Image : public OnlyMovable
{
public:
    /**
     * This constructor doesn't take ownership of `VkImage`.
     * `VkImage` will not be destroyed on destruction.
     *
     * @param image The `VkImage` this instance reference.
     */
    [[nodiscard]] static Image createFromExistingWithoutOwnership(not_null<VkImage> image);
    [[nodiscard]] static Image createEmpty(not_null<LogicalDevice*> device, VkExtent2D size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
    [[nodiscard]] static Image createFromFile(std::string const &path, not_null<LogicalDevice*> device, CommandPool &commandPool, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);

    ~Image();
    Image(Image &&) = default;
    Image &operator=(Image &&) = default;
    operator VkImage() const;

    void cmdTransition(CommandBuffer &commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyFromBuffer(CommandBuffer &commandBuffer, Buffer &buffer, VkExtent2D size);

private:
    /**
     * This constructor doesn't take ownership of `VkImage`.
     * `VkImage` will not be destroyed on destruction.
     *
     * @param image The `VkImage` this instance reference.
     */
    Image(not_null<VkImage> image);

    /**
     * This constructor does take ownership of VkImage and its suballocation.
     */
    Image(not_null<VkImage> image, not_null<LogicalDevice*> device, DeviceAllocator::ResourceMemory suballocation);

    VkHandle<VkImage> _image;

    // If logical device is set, then this instance have ownership of VkImage.
    // Otherwise, this instance doesn't have ownership of VkImage.
    std::optional<not_null<LogicalDevice*>> _device;
    std::optional<DeviceAllocator::ResourceMemory> _suballocation;
};
}

#endif //VULKAN_ENGINE_IMAGE_H
