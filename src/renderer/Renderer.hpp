#pragma once
#include "./vk/Instance.hpp"
#include "logging.hpp"

#include <GLFW/glfw3.h>


class Renderer {
public:
    Renderer();
    ~Renderer();

private:
    Instance* instance;
    Logger logger = Logger("vulkan");
};
