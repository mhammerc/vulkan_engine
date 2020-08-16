#ifndef VULKAN_ENGINE_VULKAN_H
# define VULKAN_ENGINE_VULKAN_H

#include <vulkan/vulkan.h>
#include <gsl/gsl>

#include "../vulkan_engine.h"

// This header alias namespace `Engine` to global namespace for ease of use.
using namespace Engine;

namespace Engine::Vulkan
{

/**
 * If the given `code` isn't VK_SUCCESS, throw an error.
 *
 * You can embed every Vulkan call which return VkResult to this function
 * eg. `ThrowError(vkFunction());`
 *
 * If it is expected to get an error, you should manually filter the error then call this function.
 *
 * @param code The `VkResult` code return by a Vulkan function call.
 * @param context An optional context information to print to error messages.
 */
void ThrowError(VkResult code, char const *context = nullptr);

bool areCStringEqual(char const *a, char const *b);

}

#endif //VULKAN_ENGINE_VULKAN_H
