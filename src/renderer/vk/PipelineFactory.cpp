#include "./PipelineFactory.hpp"


PipelineFactory::PipelineFactory(Swapchain* swapchain, const ShaderAsset* vertexShader)
  : dynamicStates({VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR})
{
    cfg = new VkGraphicsPipelineCreateInfo {};
    cfg->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    cfg->pNext = new VkPipelineRenderingCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = new VkFormat {swapchain->get_sformat()},
        .depthAttachmentFormat   = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED
    };

    add_shader(vertexShader, VK_SHADER_STAGE_VERTEX_BIT);

    cfg->pVertexInputState = new VkPipelineVertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions    = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions    = nullptr
    };

    cfg->pInputAssemblyState = new VkPipelineInputAssemblyStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    cfg->pTessellationState = VK_NULL_HANDLE;   // TODO: is this valid?

    cfg->pViewportState = new VkPipelineViewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports    = nullptr,   // viewport state is dynamic
        .scissorCount = 1,
        .pScissors    = nullptr,    // scissor state is dynamic
    };

    cfg->pRasterizationState = new VkPipelineRasterizationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable        = VK_FALSE,    // configurable value
        .rasterizerDiscardEnable = VK_FALSE,    // configurable value
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode  = VK_CULL_MODE_BACK_BIT,     // configurable value
        .frontFace = VK_FRONT_FACE_CLOCKWISE,   // configurable value
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth = 1.0f
    };

    cfg->pMultisampleState = new VkPipelineMultisampleStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,  // configurable value
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading    = 0,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE
    };

    cfg->pDepthStencilState = new VkPipelineDepthStencilStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable  = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthCompareOp   = VK_COMPARE_OP_ALWAYS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front = {},
        .back =  {},
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 0.0f
    };

    cfg->pColorBlendState = new VkPipelineColorBlendStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp       = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = new VkPipelineColorBlendAttachmentState {
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp        = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp        = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                              VK_COLOR_COMPONENT_A_BIT
        },
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
    };

    cfg->pDynamicState = new VkPipelineDynamicStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = (uint32_t) dynamicStates.size(),
        .pDynamicStates    = dynamicStates.data()
    };

    cfg->layout = VK_NULL_HANDLE;       // filled in by build()
    cfg->renderPass = VK_NULL_HANDLE;   // disabled for dynamic rendering
    cfg->subpass    = 0;                // disabled for dynamic rendering
    cfg->basePipelineHandle = VK_NULL_HANDLE;
    cfg->basePipelineIndex  = 0;


    lyt = new VkPipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts    = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };
}

PipelineFactory::PipelineFactory(Swapchain* swapchain, 
                                 const ShaderAsset* vertexShader, const ShaderAsset* fragmentShader)
  : PipelineFactory(swapchain, vertexShader)
{
    add_shader(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT);
}

Pipeline PipelineFactory::build(Device* device)
{
    logger.debug("building pipeline");
    return Pipeline(device, cfg, lyt);
}

PipelineFactory::~PipelineFactory()
{
    for (auto cfg : shaderCfgs)
        delete (VkShaderModuleCreateInfo*) cfg.pNext;

    delete lyt;
    delete cfg->pDynamicState;
    delete cfg->pColorBlendState->pAttachments;
    delete cfg->pColorBlendState;
    delete cfg->pDepthStencilState;
    delete cfg->pMultisampleState;
    delete cfg->pRasterizationState;
    delete cfg->pViewportState;
    delete cfg->pInputAssemblyState;
    delete cfg->pVertexInputState;
    vk_free_chain((void*) cfg->pNext);
    delete cfg;
}


void PipelineFactory::add_shader(const ShaderAsset* shader, VkShaderStageFlagBits type)
{
    VkPipelineShaderStageCreateInfo shaderInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage  = type,
        .module = VK_NULL_HANDLE,
        .pName  = "main",
        .pSpecializationInfo = nullptr
    };
    shaderInfo.pNext = new VkShaderModuleCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = shader->get_size(),
        .pCode    = (uint32_t*) shader->get()
    };
    shaderCfgs.push_back(shaderInfo);

    cfg->stageCount = shaderCfgs.size();
    cfg->pStages = shaderCfgs.data();
}

void PipelineFactory::set_topology(VkPrimitiveTopology type)
{
    // const discard: the sub struct was intentionally created as non-const for configuration
    // this is hackish but valid
    VkPipelineInputAssemblyStateCreateInfo* pSub = (VkPipelineInputAssemblyStateCreateInfo*) cfg->pInputAssemblyState;
    pSub->topology = type;
}

void PipelineFactory::set_polygon_mode(VkPolygonMode mode)
{
    VkPipelineRasterizationStateCreateInfo* pSub = (VkPipelineRasterizationStateCreateInfo*) cfg->pRasterizationState;
    pSub->polygonMode = mode;
}

void PipelineFactory::set_line_width(float width)
{
    VkPipelineRasterizationStateCreateInfo* pSub = (VkPipelineRasterizationStateCreateInfo*) cfg->pRasterizationState;
    pSub->lineWidth = width;
}
