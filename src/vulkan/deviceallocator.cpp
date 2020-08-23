#include "deviceallocator.h"
#include "logicaldevice.h"

#include <algorithm>

using namespace Engine::Vulkan;

DeviceAllocator DeviceAllocator::create(not_null<LogicalDevice *> device)
{
    return DeviceAllocator(device);
}

DeviceAllocator::DeviceAllocator(not_null<LogicalDevice *> device) :
_device(device)
{
    VkPhysicalDeviceMemoryProperties const memories = _device->memories();

    // Populate _heaps object
    for (uint32 i = 0; i < memories.memoryHeapCount; ++i)
    {
        Heap heap
        {
            .size = memories.memoryHeaps[i].size,
        };
        _heaps.push_back(std::move(heap));
    }
}

DeviceAllocator::~DeviceAllocator()
{
    for (size_t heapIndex = 0; heapIndex < _heaps.size(); ++heapIndex)
    {
        auto &heap = _heaps[heapIndex];

        for (auto &allocation : heap.allocations)
        {
            if (!allocation.resources.empty())
            {
                spdlog::error("GPU allocation leak:");
                for (auto const &suballocation : allocation.resources)
                {
                    spdlog::error("    Heap: {}, Offset: {}, Size: {}", heapIndex, suballocation.first, suballocation.second);
                }
            }

            vkFreeMemory(*_device, allocation.memory, nullptr);
            heap.allocatedSize -= allocation.size;
            spdlog::debug("Freed {}MB (total: {}MB).", allocationSize / 1000000, totalDeviceAllocation() / 1000000);
        }
    }

    _heaps.clear();
}

void DeviceAllocator::free(ResourceMemory const &resource)
{
    auto allocation = std::find_if(_heaps[resource._heap].allocations.begin(), _heaps[resource._heap].allocations.end(), [&resource](Allocation &allocation)
    {
        return allocation.memory == resource.memory;
    });

    if (allocation == _heaps[resource._heap].allocations.end())
    {
        throw std::runtime_error("Tried to free unknown resource.");
    }

    auto rawResource = allocation->resources.find(resource.offset);
    if (rawResource == allocation->resources.end())
    {
        throw std::runtime_error("Tried to free unknown resource.");
    }
    allocation->resources.erase(rawResource);
    allocation->subAllocatedSize -= resource.size;

    spdlog::trace("Subfreed {}KB.", resource.size / 1000);
}

DeviceAllocator::ResourceMemory DeviceAllocator::allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties)
{
    if (requirements.size > allocationSize)
    {
        throw std::runtime_error("Requested allocation size is higher than maximum allocation size allowed.");
    }
    auto memoryType = findMemoryType(requirements, properties);

    auto suballocation = suballocateMemory(memoryType, requirements);

    if (!suballocation)
    {
        allocateMemory(memoryType);
        suballocation = suballocateMemory(memoryType, requirements);
    }

    if (!suballocation)
    {
        throw std::runtime_error("Could not allocate memory.");
    }

    spdlog::trace("Suballocated {}KB.", suballocation->size / 1000);

    return *suballocation;
}

DeviceAllocator::MemorySection DeviceAllocator::findMemoryType(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties const memories = _device->memories();

    for (uint32_t i = 0; i < memories.memoryTypeCount; ++i)
    {
        if ((requirements.memoryTypeBits & (1u << i)) && (memories.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return {i, memories.memoryTypes[i].heapIndex};
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void DeviceAllocator::allocateMemory(MemorySection section)
{
    VkMemoryAllocateInfo allocateInfo
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = allocationSize,
        .memoryTypeIndex = std::get<0>(section),
    };

    VkDeviceMemory memory = VK_NULL_HANDLE;
    ThrowError(vkAllocateMemory(*_device, &allocateInfo, nullptr, &memory));

    _heaps[std::get<1>(section)].allocatedSize += allocationSize;

    Allocation allocation
    {
        .memory = memory,
        .size = allocationSize,
    };
    _heaps[std::get<1>(section)].allocations.push_back(std::move(allocation));

    spdlog::debug("Allocated {}MB (total: {}MB).", allocationSize / 1000000, totalDeviceAllocation() / 1000000);
}

static VkDeviceSize nextPowerOfTwo(VkDeviceSize size)
{
    --size;
    size |= size >> 1u;
    size |= size >> 2u;
    size |= size >> 4u;
    size |= size >> 8u;
    size |= size >> 16u;
    size |= size >> 32u;
    ++size;

    return size;
}

std::optional<DeviceAllocator::ResourceMemory> DeviceAllocator::suballocateMemory(MemorySection section, VkMemoryRequirements requirements)
{
    HeapIndex heapIndex = std::get<1>(section);
    Heap &heap = _heaps[heapIndex];
    VkDeviceSize size = nextPowerOfTwo(requirements.size);

    for (auto &allocation : heap.allocations)
    {
        VkDeviceSize previousEnd = 0;

        for (auto const &resource : allocation.resources)
        {
            VkDeviceSize begin = resource.first;
            VkDeviceSize end = resource.first + resource.second;

            if (begin - previousEnd >= size)
            {
                ResourceMemory resourceMemory = {allocation.memory, previousEnd, size, heapIndex};
                allocation.resources.insert({previousEnd, size});
                allocation.subAllocatedSize += size;
                return resourceMemory;
            }

            previousEnd = end;
        }

        if (allocation.size - previousEnd >= size)
        {
            ResourceMemory resourceMemory = {allocation.memory, previousEnd, size, heapIndex};
            allocation.resources.insert({previousEnd, size});
            allocation.subAllocatedSize += size;
            return resourceMemory;
        }
    }

    return std::nullopt;
}

usize DeviceAllocator::totalDeviceAllocation() const
{
    usize total = 0;
    for (auto const &heap : _heaps)
    {
        total += heap.allocatedSize;
    }
    return total;
}

DeviceAllocator::ResourceMemory::ResourceMemory(not_null<VkDeviceMemory> memory, VkDeviceSize offset, VkDeviceSize size,
                                                usize heap) :
                                                memory(memory), offset(offset), size(size), _heap(heap)
{}
