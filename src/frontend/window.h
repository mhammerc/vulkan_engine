#ifndef VULKAN_ENGINE_WINDOW_H
# define VULKAN_ENGINE_WINDOW_H

# include <string>
# include <gsl/gsl>

# include "../vulkan/vulkan.h"
# include "glfw3.h"

namespace Engine::Frontend
{
class Window
{
public:
    Window(int width, int height, std::string const &title);
    ~Window();

    [[nodiscard]] not_null<GLFWwindow *> get() const;
    [[nodiscard]] VkExtent2D size() const;

    [[nodiscard]] bool shouldClose();

private:
    GLFWwindow *_window = nullptr;
};
}

#endif //VULKAN_ENGINE_WINDOW_H
