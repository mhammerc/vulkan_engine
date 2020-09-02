#include "uniformbuffer.h"

using namespace Engine::Vulkan;

UniformBuffer UniformBuffer::create(not_null<LogicalDevice*> device)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    auto buffer = Buffer::create(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferSize, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    return UniformBuffer(std::move(buffer));
}

UniformBuffer::UniformBuffer(Buffer &&buffer) :
_buffer(std::move(buffer))
{}

Buffer &UniformBuffer::buffer()
{
    return _buffer;
}
