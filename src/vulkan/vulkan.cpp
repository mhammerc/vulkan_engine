#include "vulkan.h"
#include <stdexcept>
#include <string>

void Engine::Vulkan::ThrowError(VkResult code, char const *context)
{
    if (code == VK_SUCCESS)
        return;

    std::string errorMessage {"Vulkan: "};

    switch (code)
    {
    case VK_SUCCESS:
        return;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        errorMessage += "out of host memory";
        break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        errorMessage += "out of device memory";
        break;
    case VK_ERROR_INITIALIZATION_FAILED:
        errorMessage += "initialization failed";
        break;
    case VK_ERROR_LAYER_NOT_PRESENT:
        errorMessage += "layer not present";
        break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        errorMessage += "extension not present";
        break;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        errorMessage += "incompatible driver";
        break;
    case VK_INCOMPLETE:
        errorMessage += "call may be success but incomplete results";
        break;
    case VK_ERROR_SURFACE_LOST_KHR:
        errorMessage += "surface lost KHR";
        break;
    default:
        errorMessage += "unknown error";
        break;
    }

    if (context)
    {
        errorMessage += ". Context: ";
        errorMessage += context;
    }

    throw std::runtime_error(errorMessage);
}

bool Engine::Vulkan::areCStringEqual(char const *a, char const *b)
{
    return strcmp(a, b) == 0;
}
