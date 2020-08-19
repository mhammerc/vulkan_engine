#include "surfacekhr.h"

using namespace Engine::Vulkan;

SurfaceKHR::SurfaceKHR(not_null<VkSurfaceKHR> surface, not_null<Instance *> instance, not_null<Engine::Frontend::Window *> window) :
_surface(surface),
_instance(instance),
_window(window)
{}

SurfaceKHR::~SurfaceKHR()
{
    vkDestroySurfaceKHR(*_instance, _surface, nullptr);
}

SurfaceKHR SurfaceKHR::create(not_null<Instance *> instance, not_null<Engine::Frontend::Window *> window)
{
    VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
    ThrowError(glfwCreateWindowSurface(*instance, window->get(), nullptr, &surfaceHandle));

    return SurfaceKHR(surfaceHandle, instance, window);
}

VkSurfaceKHR SurfaceKHR::get() const
{
    return _surface;
}

SurfaceKHR::operator VkSurfaceKHR() const
{
    return _surface;
}

VkExtent2D SurfaceKHR::size() const
{
    return _window->size();
}
