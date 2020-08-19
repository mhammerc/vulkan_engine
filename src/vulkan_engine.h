#ifndef VULKAN_ENGINE_VULKAN_ENGINE_H
#define VULKAN_ENGINE_VULKAN_ENGINE_H

#include <cstdint>
#include <gsl/gsl>
#include <spdlog/spdlog.h>

namespace Engine
{
template <class T>
using not_null = ::gsl::not_null<T>;

using uint8 = uint8_t;
using byte = std::byte;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using usize = size_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

/**
 * Inherit this class to simply delete copy and assignment operators, effectively allowing move-only.
 */
class OnlyMovable
{
public:
    OnlyMovable() = default;
    ~OnlyMovable() = default;

    OnlyMovable(OnlyMovable &&) = default;
    OnlyMovable(OnlyMovable const &) = delete;

    OnlyMovable &operator=(OnlyMovable &&) = default;
    OnlyMovable &operator=(OnlyMovable const &) = delete;
};
}

#endif //VULKAN_ENGINE_VULKAN_ENGINE_H
