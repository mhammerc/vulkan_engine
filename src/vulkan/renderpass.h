#ifndef VULKAN_ENGINE_RENDERPASS_H
#define VULKAN_ENGINE_RENDERPASS_H

#include "depthstencilimage.h"
#include "logicaldevice.h"
#include "swapchainkhr.h"
#include "vulkan.h"

namespace Engine::Vulkan
{
/**
 * Create a basic renderpass that isn't editable for now.
 * It render to the swapchain image and render a depth image.
 */
class RenderPass : public OnlyMovable
{
public:
    [[nodiscard]] static RenderPass create(not_null<LogicalDevice*> device, SwapchainKHR const &swapchain, DepthStencilImage const &depthImage);

    ~RenderPass();
    RenderPass(RenderPass &&) noexcept = default;
    RenderPass &operator=(RenderPass &&) noexcept = default;
    operator VkRenderPass() const;

private:
    RenderPass(not_null<VkRenderPass> renderPass, not_null<LogicalDevice*> device);

    VkHandle<VkRenderPass> _renderPass;
    not_null<LogicalDevice*> _device;
};
}

#endif //VULKAN_ENGINE_RENDERPASS_H
