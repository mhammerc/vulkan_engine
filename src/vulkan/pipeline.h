#ifndef VULKAN_ENGINE_PIPELINE_H
#define VULKAN_ENGINE_PIPELINE_H

#include "vulkan.h"
#include "logicaldevice.h"
#include "renderpass.h"

namespace Engine::Vulkan
{
/**
 * Add documentation like:
 * the pipeline own renderpass, etc
 * Add also to PipelineBuilder
 */
class Pipeline : public OnlyMovable
{
friend class PipelineBuilder;

public:
    ~Pipeline();
    Pipeline(Pipeline &&) noexcept = default;
    Pipeline &operator=(Pipeline &&) noexcept = default;

    [[nodiscard]] not_null<VkPipeline> pipeline() const;
    [[nodiscard]] not_null<VkPipelineLayout> layout() const;
    [[nodiscard]] not_null<RenderPass const *> renderPass() const;

private:
    Pipeline(not_null<VkPipeline> pipeline, not_null<VkPipelineLayout> layout,
             std::vector<VkHandle<VkDescriptorSetLayout>> &&descriptorSetsLayouts,
             RenderPass &&renderPass, not_null<LogicalDevice*> device);

    VkHandle<VkPipeline> _pipeline;
    VkHandle<VkPipelineLayout> _layout;
    std::vector<VkHandle<VkDescriptorSetLayout>> _descriptorSetsLayouts;
    RenderPass _renderPass;

    not_null<LogicalDevice*> _device;
};
}

#endif //VULKAN_ENGINE_PIPELINE_H
