#include "glfw3.h"



using namespace Engine;

bool Frontend::init()
{
    if (glfwInit() != GLFW_TRUE)
    {
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    return true;
}

bool Frontend::isVulkanSupported()
{
    return glfwVulkanSupported() == GLFW_TRUE;
}

std::vector<char const *> Frontend::getRequiredInstanceExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<char const *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}