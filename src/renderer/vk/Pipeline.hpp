#pragma once
#include "./Device.hpp"
#include <vulkan/vulkan.h>


class Pipeline {
public:
    Pipeline(Device* device, VkGraphicsPipelineCreateInfo* cfg, VkPipelineLayoutCreateInfo* lyt);
    ~Pipeline();

private:
    Device* device;

    VkPipelineLayout VKPipelineLayout;
    VkPipeline VKPipeline;

    Logger logger = Logger("vulkan");
};
