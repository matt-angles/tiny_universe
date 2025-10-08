#pragma once
#include "./Pipeline.hpp"
#include "./Swapchain.hpp"
#include "AssetManager.hpp"
#include <vector>


class PipelineFactory {
public:
    PipelineFactory(Swapchain* swapchain, const ShaderAsset* vertexShader);
    PipelineFactory(Swapchain* swapchain, const ShaderAsset* vertexShader, const ShaderAsset* fragmentShader);
    ~PipelineFactory();

    Pipeline build(Device* device);

    void add_shader(const ShaderAsset* shader, VkShaderStageFlagBits type);
    void set_topology(VkPrimitiveTopology type);
    void set_polygon_mode(VkPolygonMode mode);
    void set_line_width(float width);

private:
    VkGraphicsPipelineCreateInfo* cfg = nullptr;
    VkPipelineLayoutCreateInfo* lyt = nullptr;

    std::vector<VkPipelineShaderStageCreateInfo> shaderCfgs;
    std::vector<VkDynamicState> dynamicStates;

    Logger logger = Logger("vulkan");
};
