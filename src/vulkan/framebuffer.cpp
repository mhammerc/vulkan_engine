#include "framebuffer.h"

using namespace Engine::Vulkan;

Framebuffer::~Framebuffer()
{
    if (_framebuffer)
    {
        vkDestroyFramebuffer(*_device, _framebuffer, nullptr);
    }
}

Framebuffer::operator VkFramebuffer() const
{
    return _framebuffer;
}

Framebuffer::Framebuffer(VkHandle<VkFramebuffer> framebuffer, not_null<LogicalDevice*> device) :
_framebuffer(std::move(framebuffer)),
_device(device)
{}

Framebuffer Framebuffer::create(VkExtent2D size, RenderPass const &renderPass, not_null<LogicalDevice*> device,
std::array<not_null<ImageView*>, 2> views)
{
    std::array<VkImageView, 2> attachments = {
        *views[0],
        *views[1]
    };

    VkFramebufferCreateInfo framebufferInfo
    {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = size.width,
        .height = size.height,
        .layers = 1,
    };


    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    ThrowError(vkCreateFramebuffer(*device, &framebufferInfo, nullptr, &framebuffer));

    return Framebuffer(framebuffer, device);
}
