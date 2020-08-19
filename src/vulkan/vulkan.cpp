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
        errorMessage += "out of host memory (VK_ERROR_OUT_OF_HOST_MEMORY)";
        break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        errorMessage += "out of device memory (VK_ERROR_OUT_OF_DEVICE_MEMORY)";
        break;
    case VK_ERROR_INITIALIZATION_FAILED:
        errorMessage += "initialization failed (VK_ERROR_INITIALIZATION_FAILED)";
        break;
    case VK_ERROR_LAYER_NOT_PRESENT:
        errorMessage += "layer not present (VK_ERROR_LAYER_NOT_PRESENT)";
        break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        errorMessage += "extension not present (VK_ERROR_EXTENSION_NOT_PRESENT)";
        break;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        errorMessage += "incompatible driver (VK_ERROR_INCOMPATIBLE_DRIVER)";
        break;
    case VK_INCOMPLETE:
        errorMessage += "call may be success but incomplete results (VK_INCOMPLETE)";
        break;
    case VK_ERROR_SURFACE_LOST_KHR:
        errorMessage += "surface lost KHR (VK_ERROR_SURFACE_LOST_KHR)";
        break;
    case VK_ERROR_FEATURE_NOT_PRESENT:
        errorMessage += "unavailable feature (VK_ERROR_FEATURE_NOT_PRESENT)";
        break;
    case VK_ERROR_TOO_MANY_OBJECTS:
        errorMessage += "too many objects (VK_ERROR_TOO_MANY_OBJECTS)";
        break;
    case VK_ERROR_DEVICE_LOST:
        errorMessage += "device lost (VK_ERROR_DEVICE_LOST)";
        break;
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        errorMessage += "native window in use (VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)";
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
