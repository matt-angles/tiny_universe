#pragma once
#include "./vk/Instance.hpp"
#include "./vk/Device.hpp"
#include "./vk/Command.hpp"
#include "./vk/Swapchain.hpp"
#include "./vk/PipelineFactory.hpp"
#include "./vk/Buffer.hpp"
#include "renderer/ViewObject.hpp"
#include "AssetManager.hpp"
#include "logging.hpp"

#include <GLFW/glfw3.h>
#include <vector>


class Renderer {
public:
    Renderer(GLFWwindow* window, const AssetManager& assets);
    ~Renderer();

    void add(ViewObject* obj);
    void present();

private:
    uint32_t draw_vertices();

    const AssetManager& assets;

    std::vector<ViewObject*> scene;
    Buffer* vertexBuffer;
    Buffer* indexBuffer;

    Instance* instance;
    Device* device;
    Command* command;
    Swapchain* swapchain;

    Pipeline* defaultPipeline;
    VkSemaphore sigImageAcquired;
    VkFence     sigImageRendered;
    VkSemaphore* sigImageAvailable;

    Logger logger = Logger("vulkan");
};
