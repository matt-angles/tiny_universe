#include "Renderer.hpp"


Renderer::Renderer()
{
    std::set<std::string> requiredInstanceExtensions = {

    };
    std::set<std::string> optionalInstanceExtensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    instance = new Instance(VK_API_VERSION_1_0, VK_API_VERSION_1_4, requiredInstanceExtensions, optionalInstanceExtensions);

    std::set<std::string> requiredDeviceExtensions = {

    };
    std::set<std::string> optionalDeviceExtensions = {

    };
    device = new Device(instance->get(), VK_API_VERSION_1_0, requiredDeviceExtensions, optionalDeviceExtensions);

    command = new Command(device);
    command->allocate_buffers(1);
}

Renderer::~Renderer()
{
    delete device;
    delete instance;
}
