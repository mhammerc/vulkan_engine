#include "surfacekhr.h"

using namespace Engine::Vulkan;

SurfaceKHR::SurfaceKHR(not_null<Instance *> instance, not_null<VkSurfaceKHR> surface) :
_surface(surface),
_instance(instance)
{}

SurfaceKHR::~SurfaceKHR()
{
    vkDestroySurfaceKHR(*_instance, _surface, nullptr);
}

SurfaceKHR SurfaceKHR::create(not_null<Instance *> instance, not_null<Engine::Frontend::Window *> window)
{
    VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
    ThrowError(glfwCreateWindowSurface(*instance, window->get(), nullptr, &surfaceHandle));

    return SurfaceKHR(instance, surfaceHandle);
}

VkSurfaceKHR SurfaceKHR::get()
{
    return _surface;
}

SurfaceKHR::operator VkSurfaceKHR() const
{
    return _surface;
}
