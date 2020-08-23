#ifndef VULKAN_ENGINE_PIPELINE_H
#define VULKAN_ENGINE_PIPELINE_H

#include "vulkan.h"
#include <vector>

namespace Engine::Vulkan
{
/**
 * This class allow to configure then construct the pipeline.
 *
 * On its initial state, it doesn't own any pipeline.
 */
class Pipeline : public OnlyMovable
{
public:
    Pipeline();
    ~Pipeline() = default;
    Pipeline (Pipeline &&) noexcept = default;
    Pipeline &operator=(Pipeline &&) noexcept = default;

    void addShaderStage(VkPipelineShaderStageCreateInfo shaderStage);

private:
    std::vector<VkPipelineShaderStageCreateInfo> _shadersStages;
};
}

#endif //VULKAN_ENGINE_PIPELINE_H
