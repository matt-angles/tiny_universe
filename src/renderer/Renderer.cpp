#include "Renderer.hpp"


Renderer::Renderer(GLFWwindow* window, const AssetManager& assets)
  : assets(assets)
{
    std::set<std::string> requiredInstanceExtensions = {

    };
    std::set<std::string> optionalInstanceExtensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    instance = new Instance(VK_API_VERSION_1_0, VK_API_VERSION_1_4, requiredInstanceExtensions, optionalInstanceExtensions);

    std::set<std::string> requiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    std::set<std::string> optionalDeviceExtensions = {

    };
    device = new Device(instance->get(), VK_API_VERSION_1_4, requiredDeviceExtensions, optionalDeviceExtensions);

    command = new Command(device);
    command->allocate_buffers(1);

    swapchain = new Swapchain(window, instance, device);

    ShaderAsset vertexShader   = assets.get_shader("default.vert");
    ShaderAsset fragmentShader = assets.get_shader("default.frag");
    PipelineFactory factoryDefaultPipeline = PipelineFactory(swapchain, &vertexShader, &fragmentShader);
    defaultPipeline = new Pipeline(factoryDefaultPipeline.build(device));

    sigImageAvailable = new VkSemaphore[swapchain->get_image_count()];
    VkSemaphoreCreateInfo semaphoreInfo {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};
    VkFenceCreateInfo fenceInfo {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT};
    vkCreateSemaphore(device->get(), &semaphoreInfo, nullptr, &sigImageAcquired);
    for (uint32_t i=0; i < swapchain->get_image_count(); i++)
        vkCreateSemaphore(device->get(), &semaphoreInfo, nullptr, sigImageAvailable+i);
    vkCreateFence(device->get(), &fenceInfo, nullptr, &sigImageRendered);
}

Renderer::~Renderer()
{
    vkDeviceWaitIdle(device->get());

    for (uint32_t i=0; i < swapchain->get_image_count(); i++)
        vkDestroySemaphore(device->get(), sigImageAvailable[i], nullptr);
    vkDestroySemaphore(device->get(), sigImageAcquired, nullptr);
    vkDestroyFence(device->get(), sigImageRendered, nullptr);
    delete defaultPipeline;

    delete swapchain;
    delete command;
    delete device;
    delete instance;
}


void Renderer::present()
{
    vkWaitForFences(device->get(), 1, &sigImageRendered, VK_TRUE, UINT64_MAX);
    vkResetFences(device->get(), 1, &sigImageRendered);
    auto cmd = command->record(0);

    uint32_t imageIndex = swapchain->acquire_img(sigImageAcquired);
    VkRenderingAttachmentInfo attachmentInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView   = swapchain->get_image_view(imageIndex),
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode        = VK_RESOLVE_MODE_NONE,
        .resolveImageView   = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp     = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp    = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = 0
    };
    VkRenderingInfo renderInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = {{0, 0}, swapchain->get_sextent()},
        .layerCount = 1,
        .viewMask   = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &attachmentInfo,
        .pDepthAttachment     = nullptr,
        .pStencilAttachment   = nullptr
    };
    swapchain->transition_img(cmd, imageIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkCmdBeginRendering(cmd, &renderInfo);

    VkViewport viewport {
        .x = 0.0f,
        .y = 0.0f,
        .width  = (float) swapchain->get_sextent().width,
        .height = (float) swapchain->get_sextent().height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    VkRect2D scissor {
        .offset = {0, 0},
        .extent = swapchain->get_sextent()
    };
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, defaultPipeline->get());
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRendering(cmd);
    swapchain->transition_img(cmd, imageIndex, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    command->submit(0, sigImageAcquired, sigImageAvailable[imageIndex], sigImageRendered);
    swapchain->present_img(imageIndex, sigImageAvailable[imageIndex]);
}
