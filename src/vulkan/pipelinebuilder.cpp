#include "pipelinebuilder.h"

using namespace Engine::Vulkan;

void PipelineBuilder::addShaderStage(VkPipelineShaderStageCreateInfo shaderStage)
{
    _shadersStages.push_back(shaderStage);
}

void PipelineBuilder::addDescriptorSetLayout(DescriptorSetLayout const &descriptorSetLayout)
{
    _descriptorSetsLayouts.push_back(descriptorSetLayout);
}

PipelineBuilder::PipelineBuilder(RenderPass &&renderPass, SurfaceKHR const &surface) :
_renderPass(std::move(renderPass))
{
    _inputAssemblyInfo = {};
    _inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    _inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    _inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    _viewport = {};
    _viewport.x = 0.f;
    _viewport.y = 0.f;
    _viewport.width = static_cast<float>(surface.size().width);
    _viewport.height = static_cast<float>(surface.size().height);
    _viewport.minDepth = 0.0f;
    _viewport.maxDepth = 1.0f;

    _scissor = {};
    _scissor.offset = {0, 0};
    _scissor.extent = surface.size();

    _viewportInfo = {};
    _viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    _viewportInfo.viewportCount = 1;
    _viewportInfo.pViewports = &_viewport;
    _viewportInfo.scissorCount = 1;
    _viewportInfo.pScissors = &_scissor;

    _rasterizerInfo = {};
    _rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    _rasterizerInfo.depthClampEnable = VK_FALSE;
    _rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
    _rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
    _rasterizerInfo.lineWidth = 1.f;
    _rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    _rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    _rasterizerInfo.depthBiasEnable = VK_FALSE;
    _rasterizerInfo.depthBiasConstantFactor = 0.0f;
    _rasterizerInfo.depthBiasClamp = 0.0f;
    _rasterizerInfo.depthBiasSlopeFactor = 0.0f;

    _multisampleInfo = {};
    _multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _multisampleInfo.sampleShadingEnable = VK_FALSE;
    _multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _multisampleInfo.minSampleShading = 1.0f;
    _multisampleInfo.pSampleMask = nullptr;
    _multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    _multisampleInfo.alphaToOneEnable = VK_FALSE;

    _colorBlendAttachment = {};
    _colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    _colorBlendAttachment.blendEnable = VK_FALSE;
    _colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    _colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    _colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    _colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    _colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    _colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    _colorBlendInfo = {};
    _colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _colorBlendInfo.logicOpEnable = VK_FALSE;
    _colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    _colorBlendInfo.attachmentCount = 1;
    _colorBlendInfo.pAttachments = &_colorBlendAttachment;
    _colorBlendInfo.blendConstants[0] = 0.f;
    _colorBlendInfo.blendConstants[1] = 0.f;
    _colorBlendInfo.blendConstants[2] = 0.f;
    _colorBlendInfo.blendConstants[3] = 0.f;

    _depthStencilInfo = {};
    _depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    _depthStencilInfo.depthTestEnable = VK_TRUE;
    _depthStencilInfo.depthWriteEnable = VK_TRUE;
    _depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    _depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    _depthStencilInfo.minDepthBounds = 0.f;
    _depthStencilInfo.maxDepthBounds = 1.f;
    _depthStencilInfo.stencilTestEnable = VK_FALSE;
}

std::vector<Pipeline> PipelineBuilder::build(not_null<LogicalDevice*> device, std::vector<not_null<PipelineBuilder*>> const &pipelinesBuilder)
{
    // Create each pipelines

    std::vector<VkGraphicsPipelineCreateInfo> pipelinesInfos;
    std::vector<std::vector<VkHandle<VkDescriptorSetLayout>>> pipelinesDescriptorSetsLayouts;

    for (auto const builder : pipelinesBuilder)
    {
        // First, create the vulkan descriptors sets layouts
        // Then, create the vulkan pipeline layout
        // Then, create the vulkan pipeline itself
        // Finally, create our own pipeline object which wrap them.

        if (!builder->_renderPass)
        {
            throw std::runtime_error("While building a Pipeline, renderpass must not be nullptr.");
        }

        // Create each descriptor sets layouts
        std::vector<VkDescriptorSetLayout> descriptorSetsLayouts;
        for (auto const &layouts : builder->_descriptorSetsLayouts)
        {
            std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
            for (auto const &bindingDescription : layouts)
            {
                VkDescriptorSetLayoutBinding binding
                {
                    .binding = bindingDescription.binding,
                    .descriptorType = bindingDescription.descriptorType,
                    .descriptorCount = 1,
                    .stageFlags = bindingDescription.stageFlags,
                    .pImmutableSamplers = nullptr
                };

                descriptorSetLayoutBindings.push_back(binding);
            }

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo {};
            descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutInfo.bindingCount = descriptorSetLayoutBindings.size();
            descriptorSetLayoutInfo.pBindings = descriptorSetLayoutBindings.data();

            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            ThrowError(vkCreateDescriptorSetLayout(*device, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout));
            descriptorSetsLayouts.push_back(descriptorSetLayout);
        }

        // Create the pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32>(descriptorSetsLayouts.size()),
            .pSetLayouts = descriptorSetsLayouts.data()
        };
        auto &ref = pipelinesDescriptorSetsLayouts.emplace_back();
        std::copy(descriptorSetsLayouts.begin(), descriptorSetsLayouts.end(), std::back_inserter(ref));

        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        ThrowError(vkCreatePipelineLayout(*device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

        // Create the pipeline itself
        VkGraphicsPipelineCreateInfo pipelineInfo
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = static_cast<uint32>(builder->_shadersStages.size()),
            .pStages = builder->_shadersStages.data(),
            .pVertexInputState = &builder->_vertexInputInfo,
            .pInputAssemblyState = &builder->_inputAssemblyInfo,
            .pTessellationState = nullptr,
            .pViewportState = &builder->_viewportInfo,
            .pRasterizationState = &builder->_rasterizerInfo,
            .pMultisampleState = &builder->_multisampleInfo,
            .pDepthStencilState = &builder->_depthStencilInfo,
            .pColorBlendState = &builder->_colorBlendInfo,
            .pDynamicState = nullptr,
            .layout = pipelineLayout,
            .renderPass = builder->_renderPass,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };
        pipelinesInfos.push_back(pipelineInfo);
    }

    // Instantiate every pipelines.
    std::vector<VkPipeline> vkPipelines(pipelinesInfos.size());
    ThrowError(vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, pipelinesInfos.size(), pipelinesInfos.data(), nullptr, vkPipelines.data()));

    // Create our own Pipeline object for each pipelines
    std::vector<Pipeline> pipelines;
    for (usize i = 0; i < vkPipelines.size(); ++i)
    {
        auto pipeline = Pipeline(vkPipelines[i], pipelinesInfos[i].layout, std::move(pipelinesDescriptorSetsLayouts[i]), std::move(pipelinesBuilder[i]->_renderPass), device);
        pipelines.push_back(std::move(pipeline));
    }

    return pipelines;
}
