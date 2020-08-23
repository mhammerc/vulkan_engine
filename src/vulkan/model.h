#ifndef VULKAN_ENGINE_MODEL_H
#define VULKAN_ENGINE_MODEL_H

#include "vulkan.h"

#include <array>
#include <vector>

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

    [[nodiscard]] static Model loadFromFile(std::string const &path);

    ~Model() = default;
    Model(Model &&) noexcept = default;
    Model &operator=(Model &&) noexcept = default;

    [[nodiscard]] VkVertexInputBindingDescription bindingDescription() const;
    [[nodiscard]] std::array<VkVertexInputAttributeDescription, 2> attributesDescriptions() const;

private:
    Model(std::vector<Vertex> &&vertices, std::vector<uint32> &&indices);

    std::vector<Vertex> _vertices;
    std::vector<uint32> _indices;
};
}

#endif //VULKAN_ENGINE_MODEL_H
