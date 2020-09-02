#include "pipeline.h"

using namespace Engine::Vulkan;

Pipeline::~Pipeline()
{
    if (_pipeline)
    {
        vkDestroyPipeline(*_device, _pipeline, nullptr);
    }

    if (_layout)
    {
        vkDestroyPipelineLayout(*_device, _layout, nullptr);
    }

    for (auto const &layout : _descriptorSetsLayouts)
    {
        if (layout)
        {
            vkDestroyDescriptorSetLayout(*_device, layout, nullptr);
        }
    }
}

not_null<VkPipeline> Pipeline::pipeline() const
{
    return _pipeline;
}

not_null<VkPipelineLayout> Pipeline::layout() const
{
    return _layout;
}

not_null<RenderPass const *> Pipeline::renderPass() const
{
    return &_renderPass;
}

Pipeline::Pipeline(not_null<VkPipeline> pipeline, not_null<VkPipelineLayout> layout,
                   std::vector<VkHandle<VkDescriptorSetLayout>> &&descriptorSetsLayouts,
                   RenderPass &&renderPass, not_null<LogicalDevice *> device) :
                   _pipeline(pipeline),
                   _layout(layout),
                   _descriptorSetsLayouts(std::move(descriptorSetsLayouts)),
                   _renderPass(std::move(renderPass)),
                   _device(device)
{}

std::vector<not_null<VkDescriptorSetLayout>> Pipeline::descriptorSetsLayouts() const
{
    std::vector<not_null<VkDescriptorSetLayout>> result {};
    for (auto const &layout : _descriptorSetsLayouts)
    {
        result.push_back(layout.get());
    }
    return result;
}
