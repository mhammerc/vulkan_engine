#ifndef VULKAN_ENGINE_VULKAN_H
# define VULKAN_ENGINE_VULKAN_H

#include <vulkan/vulkan.h>

#include "../vulkan_engine.h"

namespace Engine::Vulkan
{
void ThrowError(VkResult code);
}

#endif //VULKAN_ENGINE_VULKAN_H
