#ifndef VULKAN_ENGINE_VKHANDLE_H
#define VULKAN_ENGINE_VKHANDLE_H

#include <type_traits>
#include <vulkan/vulkan.h>

#include "../vulkan_engine.h"

namespace Engine::Vulkan
{
/**
 * This class is a simple wrapper which takes ownership over any Vulkan Handle.
 *
 * To enforce ownership, this class enforce many rules:
 *  - Can not be constructed to VK_NULL_HANDLE (default constructed)
 *  - Can be constructed with raw Vulkan handles.
 *  - Can not be copied or assigned.
 *  - If move-copied, handles are swapped between the two objects, effectively echanging ownership.
 *  - If move-constructed, the other is now VK_NULL_HANDLE. It then leave the other to un UNDEFINED state.
 *  - Can be implicitly converted to the handle type it wraps.
 * @tparam Handle Any Vulkan handle
 */
template <class Handle>
class VkHandle : public OnlyMovable
{
public:
    // Either a pointer or uint64 depending to platforms.
    static_assert(std::is_same<Handle, uint64_t>::value || std::is_assignable<Handle&, std::nullptr_t>::value, "Handle isn't a Vulkan handle.");

    template <typename = std::enable_if_t<!std::is_same<std::nullptr_t, Handle>::value>>
    VkHandle(Handle const &h) : _h(h) {};
    VkHandle &operator=(Handle const &) = delete;

    constexpr VkHandle(VkHandle<Handle> &&other) noexcept : _h(other._h)
    {
        other._h = VK_NULL_HANDLE;
    }

    constexpr VkHandle &operator=(VkHandle<Handle> &&other) noexcept
    {
        std::swap(_h, other._h);
        return *this;
    }

    VkHandle(VkHandle const &) = delete;
    VkHandle &operator=(VkHandle const &) = delete;

    constexpr Handle get() const
    {
        return _h;
    }

    constexpr operator Handle() const { return get(); }

    // Can only return false on undefined-states after a move.
    constexpr explicit operator bool() const { return _h != VK_NULL_HANDLE; }

    // prevents compilation from assigning a null handle
    VkHandle(std::nullptr_t) = delete;
    VkHandle& operator=(std::nullptr_t) = delete;

private:
    Handle _h;
};
}

#endif //VULKAN_ENGINE_VKHANDLE_H
