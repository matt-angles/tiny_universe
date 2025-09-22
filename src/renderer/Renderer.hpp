#pragma once
#include "./vk/Instance.hpp"
#include "./vk/Device.hpp"
#include "./vk/Command.hpp"
#include "logging.hpp"

#include <GLFW/glfw3.h>


class Renderer {
public:
    Renderer();
    ~Renderer();

private:
    Instance* instance;
    Device* device;
    Command* command;

    Logger logger = Logger("vulkan");
};
