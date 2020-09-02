#include "window.h"
#include <stdexcept>

using namespace Engine::Frontend;

Window::Window(int width, int height, std::string const &title)
{
    if (width <= 0 || height <= 0)
    {
        throw std::runtime_error("width or height must be positive integers.");
    }

    _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!_window)
    {
        throw std::runtime_error("Could not create window.");
    }
}

Window::~Window()
{
    if (_window != nullptr)
    {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }
}

gsl::not_null<GLFWwindow *> Window::get() const
{
    return _window;
}

VkExtent2D Window::size() const
{
    int width {};
    int height {};

    glfwGetFramebufferSize(_window, &width, &height);

    return {.width = static_cast<uint32_t>(width), .height = static_cast<uint32_t>(height)};
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(_window);
}