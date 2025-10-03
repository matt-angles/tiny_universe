#include "./Pipeline.hpp"
#include "./utils.hpp"


Pipeline::Pipeline(Device* device, VkGraphicsPipelineCreateInfo* cfg, VkPipelineLayoutCreateInfo* lyt)
  : device(device)
{
    vk_try(vkCreatePipelineLayout(device->get(), lyt, nullptr, &VKPipelineLayout),
           "failed to create pipeline layout");
    cfg->layout = VKPipelineLayout;
    vk_try(vkCreateGraphicsPipelines(device->get(), VK_NULL_HANDLE, 1, cfg, nullptr, &VKPipeline),
           "failed to create pipeline");
    logger.info("compiled pipeline");
}

Pipeline::~Pipeline()
{
    vkDestroyPipelineLayout(device->get(), VKPipelineLayout, nullptr);
    vkDestroyPipeline(device->get(), VKPipeline, nullptr);
}
