#ifndef VULKAN_ENGINE_DEVICEALLOCATOR_H
#define VULKAN_ENGINE_DEVICEALLOCATOR_H

#include <vector>
#include <map>
#include <tuple>
#include <optional>

#include "vulkan.h"

namespace Engine::Vulkan
{
class LogicalDevice;

/**
 * This class allow you to automatically allocate usable chunk memories.
 *
 * It manages big-allocation to create sub-allocations.
 *
 * Each suballocation size you request will be rounded to the nearest superior or equal power of two.
 */
class DeviceAllocator : public OnlyMovable
{
public:
    struct ResourceMemory
    {
        friend DeviceAllocator;
    public:
        not_null<VkDeviceMemory> memory;
        VkDeviceSize offset;
        VkDeviceSize size;

    private:
        ResourceMemory(not_null<VkDeviceMemory> memory, VkDeviceSize offset, VkDeviceSize size, usize heap);

        usize _heap;
    };

    [[nodiscard]] static DeviceAllocator create(not_null<LogicalDevice*> device);

    ~DeviceAllocator();
    DeviceAllocator(DeviceAllocator &&) = default;
    DeviceAllocator &operator=(DeviceAllocator &&) = default;

    [[nodiscard]] ResourceMemory allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties);
    void free(ResourceMemory const &resource);

    [[nodiscard]] usize totalDeviceAllocation() const;

private:
    struct Allocation
    {
        VkHandle<VkDeviceMemory> memory;
        VkDeviceSize size;
        VkDeviceSize subAllocatedSize = 0;

        // Keep track of all sub-allocations.
        // map<offset, size>
        std::map<VkDeviceSize, VkDeviceSize> resources;
    };

    struct Heap
    {
        VkDeviceSize size;
        VkDeviceSize allocatedSize = 0;

        std::vector<Allocation> allocations;
    };
    using MemoryType = uint32;
    using HeapIndex = uint32;
    using MemorySection = std::tuple<MemoryType, HeapIndex>;

    static constexpr const VkDeviceSize allocationSize = 268435456u; // 256MB

    explicit DeviceAllocator(not_null<LogicalDevice*> device);

    not_null<LogicalDevice*> _device;
    // _heaps represent available device heaps. They have same index as vulkan index.
    std::vector<Heap> _heaps;


    MemorySection findMemoryType(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties);
    void allocateMemory(MemorySection section);
    std::optional<ResourceMemory> suballocateMemory(MemorySection Section, VkMemoryRequirements requirements);
};
}

#endif //VULKAN_ENGINE_DEVICEALLOCATOR_H
