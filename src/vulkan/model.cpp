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

Model Model::createFromFile(std::string const &path)
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

Buffer Model::toBuffer(not_null<LogicalDevice*> device, CommandPool &commandPool)
{
    usize verticesSize = sizeof(decltype(_vertices)::value_type) * _vertices.size();
    usize indicesSize = sizeof(decltype(_indices)::value_type) * _indices.size();
    // Copy vertices then into the same buffer
    VkDeviceSize bufferSize = verticesSize + indicesSize;

    auto stagingBuffer = Buffer::create(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, bufferSize,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    {
        void *data = nullptr;
        stagingBuffer.map(&data);

        memcpy(data, _vertices.data(), verticesSize);
        memcpy(reinterpret_cast<uint8*>(data) + verticesSize, _indices.data(), indicesSize);

        stagingBuffer.unmap();
    }

    auto buffer = Buffer::create(device,
                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
                                 bufferSize, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    auto cmdBuffer = CommandBuffer::create(device, &commandPool);
    cmdBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    Buffer::cmdCopy(cmdBuffer, buffer, stagingBuffer, bufferSize);
    cmdBuffer.end();
    cmdBuffer.submit(true);

    return buffer;
}

bool Model::Vertex::operator==(const Vertex &other) const
{
    return pos == other.pos && texCoord == other.texCoord;
}
