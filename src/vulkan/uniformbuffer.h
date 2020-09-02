#ifndef VULKAN_ENGINE_UNIFORMBUFFER_H
#define VULKAN_ENGINE_UNIFORMBUFFER_H

#include "vulkan.h"
#include "buffer.h"

namespace Engine::Vulkan
{
/**
 * For now, it is not generic.
 * Full genericity and plug-and-play will come later.
 */
class UniformBuffer : public OnlyMovable
{
public:
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };
    static_assert(std::is_standard_layout_v<UniformBufferObject>);

    [[nodiscard]] static UniformBuffer create(not_null<LogicalDevice*> device);

    UniformBuffer(UniformBuffer &&) = default;
    UniformBuffer &operator=(UniformBuffer &&) = default;

    Buffer &buffer();

private:
    UniformBuffer(Buffer &&buffer);

    Buffer _buffer;

};
}

#endif //VULKAN_ENGINE_UNIFORMBUFFER_H
