#pragma once
#include "./vk/Instance.hpp"
#include "./vk/Device.hpp"
#include "logging.hpp"

#include <GLFW/glfw3.h>


class Renderer {
public:
    Renderer();
    ~Renderer();

private:
    Instance* instance;
    Device* device;

    Logger logger = Logger("vulkan");
};
