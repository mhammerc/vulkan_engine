#ifndef VULKAN_ENGINE_VULKAN_ENGINE_H
#define VULKAN_ENGINE_VULKAN_ENGINE_H

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
}

#endif //VULKAN_ENGINE_VULKAN_ENGINE_H
