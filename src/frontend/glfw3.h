#ifndef VULKAN_ENGINE_GLFW3_H
# define VULKAN_ENGINE_GLFW3_H

# include <vulkan/vulkan.h>
# include <GLFW/glfw3.h>

# include <vector>

namespace Engine::Frontend
{
    bool init();
    bool isVulkanSupported();
    std::vector<char const *> getRequiredInstanceExtensions();

}

#endif //VULKAN_ENGINE_GLFW3_H
