#pragma once
#include "./vk/Instance.hpp"
#include "./vk/Device.hpp"
#include "./vk/Command.hpp"
#include "./vk/Swapchain.hpp"
#include "AssetManager.hpp"
#include "logging.hpp"

#include <GLFW/glfw3.h>


class Renderer {
public:
    Renderer(GLFWwindow* window, const AssetManager& assets);
    ~Renderer();

private:
    const AssetManager& assets;

    Instance* instance;
    Device* device;
    Command* command;
    Swapchain* swapchain;

    Logger logger = Logger("vulkan");
};
