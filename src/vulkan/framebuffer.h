#ifndef VULKAN_ENGINE_FRAMEBUFFER_H
#define VULKAN_ENGINE_FRAMEBUFFER_H

#include <array>

#include "vulkan.h"
#include "logicaldevice.h"
#include "renderpass.h"
#include "imageview.h"

namespace Engine::Vulkan
{
class Framebuffer : public OnlyMovable
{
public:
    static Framebuffer create(VkExtent2D size, RenderPass const &renderPass, not_null<LogicalDevice*> device,
                              std::array<not_null<ImageView*>, 2> views);

    ~Framebuffer();
    Framebuffer (Framebuffer &&) = default;
    Framebuffer &operator=(Framebuffer &&) = default;
    operator VkFramebuffer() const;

private:
    Framebuffer(VkHandle<VkFramebuffer> framebuffer, not_null<LogicalDevice*> device);

    VkHandle<VkFramebuffer> _framebuffer;
    not_null<LogicalDevice*> _device;
};
}

#endif //VULKAN_ENGINE_FRAMEBUFFER_H
