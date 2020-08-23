#include "pipeline.h"

using namespace Engine::Vulkan;

void Pipeline::addShaderStage(VkPipelineShaderStageCreateInfo shaderStage)
{
    _shadersStages.push_back(shaderStage);
}

Pipeline::Pipeline()
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
}
