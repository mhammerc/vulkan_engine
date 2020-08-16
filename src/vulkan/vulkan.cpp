#include "vulkan.h"
#include <stdexcept>
#include <string>

void Engine::Vulkan::ThrowError(VkResult code)
{
    std::string errorMessage {};

    switch (code)
    {
    case VK_SUCCESS:
        return;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        errorMessage = "Vulkan: out of host memory";
        break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        errorMessage = "Vulkan: out of device memory";
        break;
    case VK_ERROR_INITIALIZATION_FAILED:
        errorMessage = "Vulkan: initialization failed";
        break;
    case VK_ERROR_LAYER_NOT_PRESENT:
        errorMessage = "Vulkan: layer not present";
        break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        errorMessage = "Vulkan: extension not present";
        break;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        errorMessage = "Vulkan: incompatible driver";
        break;
    default:
        errorMessage = "Vulkan: unknown error";
        break;
    }

    throw std::runtime_error(errorMessage);
}
