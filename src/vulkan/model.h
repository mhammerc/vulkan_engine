#ifndef VULKAN_ENGINE_MODEL_H
#define VULKAN_ENGINE_MODEL_H

#include <array>
#include <vector>

#include "vulkan.h"
#include "buffer.h"
#include "logicaldevice.h"
#include "commandpool.h"
#include "commandbuffer.h"

namespace Engine::Vulkan
{
/**
 * Effectively represent a 3D model loaded into main memory.
 *
 * This instance doesn't own GPU resources.
 */
class Model : public OnlyMovable
{
public:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 texCoord;

        bool operator==(Vertex const &other) const;
    };
    static_assert(std::is_standard_layout_v<Vertex>);

    [[nodiscard]] static Model createFromFile(std::string const &path);

    ~Model() = default;
    Model(Model &&) noexcept = default;
    Model &operator=(Model &&) noexcept = default;

    [[nodiscard]] static VkVertexInputBindingDescription bindingDescription();
    [[nodiscard]] static std::vector<VkVertexInputAttributeDescription> attributesDescriptions();

    Buffer toBuffer(not_null<LogicalDevice*> device, CommandPool &commandPool);

private:
    Model(std::vector<Vertex> &&vertices, std::vector<uint32> &&indices);

    std::vector<Vertex> _vertices;
    std::vector<uint32> _indices;
};
}

#endif //VULKAN_ENGINE_MODEL_H
