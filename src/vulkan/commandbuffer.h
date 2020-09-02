#ifndef VULKAN_ENGINE_COMMANDBUFFER_H
#define VULKAN_ENGINE_COMMANDBUFFER_H

#include <vector>

#include "vulkan.h"
#include "logicaldevice.h"
#include "commandpool.h"

namespace Engine::Vulkan
{
class CommandBuffer : public OnlyMovable
{
public:
    [[nodiscard]] static std::vector<CommandBuffer> createMany(usize count, not_null<LogicalDevice*> device, not_null<CommandPool*> commandPool);
    [[nodiscard]] static CommandBuffer create(not_null<LogicalDevice*> device, not_null<CommandPool*> commandPool);

    ~CommandBuffer();
    CommandBuffer(CommandBuffer &&) noexcept = default;
    CommandBuffer &operator=(CommandBuffer &&) noexcept = default;
    operator VkCommandBuffer() const;


    void begin(VkCommandBufferUsageFlags flags);
    void end();
    // The wait argument have to go away.
    // Make that function static to submit multiples at once.
    // Make an efficient asynchronous queues model.
    void submit(bool wait = false);

    void reset();

private:
    CommandBuffer(VkHandle<VkCommandBuffer> commandBuffer, not_null<LogicalDevice*> device, not_null<CommandPool*> commandPool);

    VkHandle<VkCommandBuffer> _commandBuffer;
    not_null<LogicalDevice*> _device;
    not_null<CommandPool*> _commandPool;

};
}

#endif //VULKAN_ENGINE_COMMANDBUFFER_H
