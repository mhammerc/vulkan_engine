#ifndef VULKAN_ENGINE_COMMANDPOOL_H
#define VULKAN_ENGINE_COMMANDPOOL_H

#include "vulkan.h"
#include "logicaldevice.h"

namespace Engine::Vulkan
{
class CommandPool : public OnlyMovable
{
public:
    [[nodiscard]] static CommandPool create(not_null<LogicalDevice*> device);

    ~CommandPool();
    CommandPool(CommandPool &&) noexcept = default;
    CommandPool &operator=(CommandPool &&) noexcept = default;
    operator VkCommandPool() const;

    [[nodiscard]] uint32 queueFamily() const;
private:
    CommandPool(VkHandle<VkCommandPool> commandPool, uint32 queueFamily, not_null<LogicalDevice*> device);

    VkHandle<VkCommandPool> _commandPool;
    uint32 _queueFamily;
    not_null<LogicalDevice *> _device;
};
}

#endif //VULKAN_ENGINE_COMMANDPOOL_H
