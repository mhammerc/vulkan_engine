#include "model.h"

#include <tiny_obj_loader.h>

using namespace Engine::Vulkan;

namespace std
{
template<>
struct hash<Model::Vertex>
{
    size_t operator()(Model::Vertex const &vertex) const
    {
        return (hash<glm::vec3>()(vertex.pos) ^
            (hash<glm::vec2>()(vertex.texCoord) << 1));
    }
};
}

Model Model::loadFromFile(std::string const &path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices;
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
    for (const auto &shape : shapes)
    {
        for (const auto &index : shape.mesh.indices)
        {
            Vertex vertex {};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    spdlog::debug("Loaded model '{}'.", path);
    return Model(std::move(vertices), std::move(indices));
}

VkVertexInputBindingDescription Model::bindingDescription()
{
    VkVertexInputBindingDescription bindingDescription
    {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> Model::attributesDescriptions()
{
    std::vector<VkVertexInputAttributeDescription> attributesDescriptions {};

    attributesDescriptions.push_back(
    {
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, pos),
    });

    attributesDescriptions.push_back(
    {
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, texCoord),
    });

    return attributesDescriptions;
}

Model::Model(std::vector<Vertex> &&vertices, std::vector<uint32> &&indices) :
_vertices(std::move(vertices)),
_indices(std::move(indices))
{

}

bool Model::Vertex::operator==(const Vertex &other) const
{
    return pos == other.pos && texCoord == other.texCoord;
}
