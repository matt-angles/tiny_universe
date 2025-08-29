#include "Renderer.hpp"


Renderer::Renderer()
{
    std::set<std::string> requiredExtensions = {

    };
    std::set<std::string> optionalExtensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    instance = new Instance(VK_API_VERSION_1_0, VK_API_VERSION_1_4, requiredExtensions, optionalExtensions);
}

Renderer::~Renderer()
{
    delete instance;
}
