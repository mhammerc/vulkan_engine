#ifndef VULKAN_ENGINE_PIPELINEBUILDER_H
#define VULKAN_ENGINE_PIPELINEBUILDER_H

#include <vector>

#include "vulkan.h"
#include "swapchainkhr.h"
#include "renderpass.h"
#include "pipeline.h"

namespace Engine::Vulkan
{
/**
 * This class allow to configure then construct the pipeline layout and graphic pipeline.
 *
 * On its initial state, it doesn't own any pipeline. You must configure the pipeline through the provided functions,
 * then call `build()` which return the pipeline layout and the graphic pipeline.
 *
 * Build layout and graphic pipeline at the same time? Different things?
 */
class PipelineBuilder : public OnlyMovable
{
public:
    /**
     * This structure represent a descriptor set binding layout.
     * They have to be owned by a descriptor set layout.
     */
    struct DescriptorSetLayoutBinding
    {
        uint32 binding;
        VkDescriptorType descriptorType;
        VkShaderStageFlags stageFlags;
    };
    using DescriptorSetLayout = std::vector<DescriptorSetLayoutBinding>;

    explicit PipelineBuilder(RenderPass &&renderPass, SurfaceKHR const &surface);
    ~PipelineBuilder() = default;
    PipelineBuilder(PipelineBuilder &&) noexcept = default;
    PipelineBuilder &operator=(PipelineBuilder &&) noexcept = default;

    /**
     * The order descriptors sets are passed in the `std::vector` is important. It represent the `set` property
     * in your shader.
     */
    void addDescriptorSetLayout(DescriptorSetLayout const &descriptorSetLayout);
    void addShaderStage(VkPipelineShaderStageCreateInfo shaderStage);

    /**
     * Pass in your Vertex class which must be present conform methods for passing your vertex layout to the pipeline.
     */
    template <class VertexInput>
    void setVertexInputDescription()
    {
        _vertexBindingDescription = VertexInput::bindingDescription();
        _vertexAttributesDescriptions = VertexInput::attributesDescriptions();

        _vertexInputInfo = {};
        _vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        _vertexInputInfo.vertexBindingDescriptionCount = 1;
        _vertexInputInfo.pVertexBindingDescriptions = &_vertexBindingDescription;
        _vertexInputInfo.vertexAttributeDescriptionCount = _vertexAttributesDescriptions.size();
        _vertexInputInfo.pVertexAttributeDescriptions = _vertexAttributesDescriptions.data();
    }

    /**
     * Actually build create the Vulkan Pipelines objects.
     * You can pass multiple pipelines and build all of them at once.
     * Once built, you can discard your PipelineBuilder instances and your shaders modules.
     */
    [[nodiscard]] static std::vector<Pipeline> build(not_null<LogicalDevice*> device, std::vector<not_null<PipelineBuilder*>> const &pipelinesBuilder);


private:
    // Shaders, descriptors sets and render pass
    std::vector<VkPipelineShaderStageCreateInfo> _shadersStages;
    std::vector<DescriptorSetLayout> _descriptorSetsLayouts;
    RenderPass _renderPass;

    // Vertices
    VkVertexInputBindingDescription _vertexBindingDescription {};
    std::vector<VkVertexInputAttributeDescription> _vertexAttributesDescriptions;
    VkPipelineVertexInputStateCreateInfo _vertexInputInfo {};

    // Viewport
    VkViewport _viewport {};
    VkRect2D _scissor {};
    VkPipelineViewportStateCreateInfo _viewportInfo {};

    // Color Blend
    VkPipelineColorBlendAttachmentState _colorBlendAttachment {};
    VkPipelineColorBlendStateCreateInfo _colorBlendInfo {};

    // Others
    VkPipelineInputAssemblyStateCreateInfo _inputAssemblyInfo {};
    VkPipelineRasterizationStateCreateInfo _rasterizerInfo {};
    VkPipelineMultisampleStateCreateInfo _multisampleInfo {};
    VkPipelineDepthStencilStateCreateInfo _depthStencilInfo {};
};
}

#endif //VULKAN_ENGINE_PIPELINEBUILDER_H
