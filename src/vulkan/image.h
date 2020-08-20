#ifndef VULKAN_ENGINE_IMAGE_H
# define VULKAN_ENGINE_IMAGE_H

#include "vulkan.h"
#include "logicaldevice.h"

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

    ~Image();
    Image (Image &&) = default;
    Image &operator=(Image &&) = default;
    operator VkImage() const;

private:
    /**
     * This constructor doesn't take ownership of `VkImage`.
     * `VkImage` will not be destroyed on destruction.
     *
     * @param image The `VkImage` this instance reference.
     */
    Image(not_null<VkImage> image);

    VkHandle<VkImage> _image;

    // If logical device isn't set (std::nullopt), then Image have now ownership over VkImage
    std::optional<not_null<LogicalDevice*>> _device;
};
}

#endif //VULKAN_ENGINE_IMAGE_H
