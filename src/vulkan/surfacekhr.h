#ifndef VULKAN_ENGINE_SURFACEKHR_H
# define VULKAN_ENGINE_SURFACEKHR_H

# include <gsl/gsl>

# include "vulkan.h"
# include "instance.h"
# include "../frontend/window.h"

namespace Engine::Vulkan
{
/**
 * This class represent a `VkSurfaceKHR` handle.
 * It own the handle.
 *
 * It also save a `VkInstance` handle but do not own it.
 */
class SurfaceKHR
{
public:
    static SurfaceKHR create(not_null<Instance *> instance, not_null<Engine::Frontend::Window *> window);
    ~SurfaceKHR();
    operator VkSurfaceKHR() const;

    VkSurfaceKHR get();

private:
    SurfaceKHR(not_null<Instance *> instance, not_null<VkSurfaceKHR> surface);

    not_null<VkSurfaceKHR> _surface;
    not_null<Instance *> _instance;
};
}

#endif //VULKAN_ENGINE_SURFACEKHR_H
