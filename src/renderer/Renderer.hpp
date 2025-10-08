#pragma once
#include "./vk/Instance.hpp"
#include "./vk/Device.hpp"
#include "./vk/Command.hpp"
#include "./vk/Swapchain.hpp"
#include "./vk/PipelineFactory.hpp"
#include "AssetManager.hpp"
#include "logging.hpp"

#include <GLFW/glfw3.h>


class Renderer {
public:
    Renderer(GLFWwindow* window, const AssetManager& assets);
    ~Renderer();

    void present();

private:
    const AssetManager& assets;

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
