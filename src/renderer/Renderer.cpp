#include "Renderer.hpp"
#include "./vk/PipelineFactory.hpp"


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

    Pipeline defaultPipeline = PipelineFactory(swapchain, assets.get_shader("default.vert")).build(device);
}

Renderer::~Renderer()
{
    delete swapchain;
    delete command;
    delete device;
    delete instance;
}
