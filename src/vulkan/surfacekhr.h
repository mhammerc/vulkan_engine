#ifndef VULKAN_ENGINE_SURFACEKHR_H
# define VULKAN_ENGINE_SURFACEKHR_H

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
class SurfaceKHR : public OnlyMovable
{
public:
    ~SurfaceKHR();
    operator VkSurfaceKHR() const;

    static SurfaceKHR create(not_null<Instance *> instance, not_null<Engine::Frontend::Window *> window);

    [[nodiscard]] VkSurfaceKHR get() const;
    [[nodiscard]] VkExtent2D size() const;

private:
    SurfaceKHR(not_null<VkSurfaceKHR> surface, not_null<Instance *> instance, not_null<Engine::Frontend::Window *> window);

    not_null<VkSurfaceKHR> _surface;
    not_null<Instance *> _instance;
    not_null<Engine::Frontend::Window*> _window;
};
}

#endif //VULKAN_ENGINE_SURFACEKHR_H
