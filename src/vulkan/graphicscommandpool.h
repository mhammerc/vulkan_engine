#ifndef VULKAN_ENGINE_GRAPHICSCOMMANDPOOL_H
#define VULKAN_ENGINE_GRAPHICSCOMMANDPOOL_H

#include "vulkan.h"
#include "logicaldevice.h"

namespace Engine::Vulkan
{
class GraphicsCommandPool : public OnlyMovable
{
public:
    [[nodiscard]] static GraphicsCommandPool create(not_null<LogicalDevice*> device);

    ~GraphicsCommandPool();
    GraphicsCommandPool(GraphicsCommandPool &&) noexcept = default;
    GraphicsCommandPool &operator=(GraphicsCommandPool &&) noexcept = default;

    [[nodiscard]] uint32 queueFamily() const;
private:
    GraphicsCommandPool(VkHandle<VkCommandPool> commandPool, uint32 queueFamily, not_null<LogicalDevice*> device);

    VkHandle<VkCommandPool> _commandPool;
    uint32 _queueFamily;
    not_null<LogicalDevice *> _device;
};
}

#endif //VULKAN_ENGINE_GRAPHICSCOMMANDPOOL_H
