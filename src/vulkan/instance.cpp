#include "instance.h"
#include <algorithm>

using namespace Engine::Vulkan;

Instance::~Instance()
{
#ifndef NDBUG
    if (_debugMessenger)
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(_instance,
                                  "vkDestroyDebugUtilsMessengerEXT"));
        if (!func)
        {
            ThrowError(VK_ERROR_EXTENSION_NOT_PRESENT, "loading vkDestroyDebugUtilsMessengerEXT");
        }

        func(_instance, _debugMessenger, nullptr);
    }
#endif

    if (_instance)
    {
        vkDestroyInstance(_instance, nullptr);
        spdlog::debug("Vulkan instance destroyed.");
    }
}

Instance::Instance(not_null<VkInstance> instance
#ifndef NDEBUG
, not_null<VkDebugUtilsMessengerEXT> debugMessenger
#endif
) :
_instance(instance),
_debugMessenger(debugMessenger)
{}

Instance Instance::create(std::span<const char *> _layers, std::span<const char *> _extensions)
{
    // Check layers availability
    // Copy each needed layers, then sort and delete duplicates.
    std::vector<char const *> layers {};
    layers.reserve(_layers.size() + requiredValidationLayers.size());

    std::copy(_layers.begin(), _layers.end(), std::back_inserter(layers));
    std::copy(requiredValidationLayers.begin(), requiredValidationLayers.end(), std::back_inserter(layers));

    std::sort(layers.begin(), layers.end(), strcmp);
    layers.erase(std::unique(layers.begin(), layers.end(), areCStringEqual), layers.end());

    if (auto missingLayer = areLayersAvailable(layers); missingLayer.has_value())
    {
        throw std::runtime_error(std::string("At least one required layer is missing: ") + *missingLayer);
    }

    // Check extensions availability
    std::vector<char const *> extensions {};
    extensions.reserve(_extensions.size() + requiredInstanceExtensions.size());

    std::copy(_extensions.begin(), _extensions.end(), std::back_inserter(extensions));
    std::copy(requiredInstanceExtensions.begin(), requiredInstanceExtensions.end(), std::back_inserter(extensions));

    std::sort(extensions.begin(), extensions.end(), strcmp);
    extensions.erase(std::unique(extensions.begin(), extensions.end(), areCStringEqual), extensions.end());

    if (auto missingExtension = areExtensionsAvailable(extensions); missingExtension.has_value())
    {
        throw std::runtime_error(std::string("At least one required instance extension is missing: ") + *missingExtension);
    }

    VkInstanceCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.flags = 0;
    info.pApplicationInfo = nullptr;

    info.enabledLayerCount = layers.size();
    info.ppEnabledLayerNames = layers.data();

    info.enabledExtensionCount = extensions.size();
    info.ppEnabledExtensionNames = extensions.data();

    VkInstance instance = VK_NULL_HANDLE;
    Vulkan::ThrowError(vkCreateInstance(&info, nullptr, &instance));

#ifndef NDEBUG
    not_null<VkDebugUtilsMessengerEXT> debugMessenger = setupDebugMessenger(instance);
#endif

    uint32 version {};
    Vulkan::ThrowError(vkEnumerateInstanceVersion(&version));

    uint32 major = VK_VERSION_MAJOR(version);
    uint32 minor = VK_VERSION_MINOR(version);
    uint32 patch = VK_VERSION_PATCH(version);

    spdlog::debug("Vulkan instance {}.{}.{} created.", major, minor, patch);

    return Instance(instance, debugMessenger);
}

std::optional<char const *> Instance::areLayersAvailable(const std::vector<const char *> &layers)
{
    uint32 layerCount;
    ThrowError(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

    std::vector<VkLayerProperties> availableLayers(layerCount);
    ThrowError(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

    // Find if a required layer is missing from available layers.
    auto missingLayer = std::find_if(layers.begin(), layers.end(), [&](auto const *requiredLayer)
    {
        auto found = std::find_if(availableLayers.begin(), availableLayers.end(), [&](auto const &availableLayer)
        {
            return strcmp(requiredLayer, availableLayer.layerName) == 0;
        });

        return found == availableLayers.end();
    });

    if (missingLayer != layers.end())
    {
        return *missingLayer;
    }

    return std::nullopt;
}

std::optional<char const *> Instance::areExtensionsAvailable(const std::vector<const char *> &extensions)
{
    uint32 extensionCount;
    ThrowError(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    ThrowError(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()));

    // Find if a required extension is missing from available extensions.
    auto missingExtension = std::find_if(extensions.begin(), extensions.end(), [&](auto const *requiredExtension)
    {
        auto found = std::find_if(availableExtensions.begin(), availableExtensions.end(), [&](auto const &availableExtension)
        {
            return strcmp(requiredExtension, availableExtension.extensionName) == 0;
        });

        return found == availableExtensions.end();
    });

    if (missingExtension != extensions.end())
    {
        return *missingExtension;
    }

    return std::nullopt;
}

#ifndef NDEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    spdlog::error("{}", pCallbackData->pMessage);

    return VK_FALSE;
}

not_null<VkDebugUtilsMessengerEXT> Instance::setupDebugMessenger(not_null<VkInstance> instance)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | */ VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (!func)
    {
        ThrowError(VK_ERROR_EXTENSION_NOT_PRESENT, "loading vkCreateDebugUtilsMessengerEXT");
    }

    VkDebugUtilsMessengerEXT debugMessenger;
    ThrowError(func(instance, &createInfo, nullptr, &debugMessenger));

    return debugMessenger;
}
#endif

not_null<VkInstance> Instance::get() const
{
    return _instance;
}

Instance::operator VkInstance() const
{
    return _instance;
}
