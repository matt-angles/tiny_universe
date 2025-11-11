#include "Application.hpp"
#include "AssetManager.hpp"
#include "renderer/Renderer.hpp"

#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>
#include <cstdlib>


static void glfwcall_error(int errorCode, const char* errorMsg)
{
    spdlog::error("glfw: {} ({})", errorMsg, errorCode);
}

class Application {
public:
    Application();
    void run();
    ~Application();

private:
    AssetManager assets;

    GLFWwindow* window;
    Renderer* renderer;
};

// NOTE: assets is initialized before logging, which prevents it from logging properly
Application::Application()
  : assets(app::workDir)
{
    log_init(app::logMode, app::logLevel);


    glfwSetErrorCallback(glfwcall_error);
    glfwInitVulkanLoader(vkGetInstanceProcAddr);
    glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error("glfw: initialization failed");
    int maj, min, rev; glfwGetVersion(&maj, &min, &rev);
    spdlog::info("glfw: initialized GLFW {}.{}.{}", maj, min, rev);
    spdlog::debug("glfw: compiled {}", glfwGetVersionString());

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    if (app::fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* monitorMode = glfwGetVideoMode(monitor);
        int height = (16.0/9.0) * monitorMode->width; 
        window = glfwCreateWindow(monitorMode->width, height, app::windowTitle,
                                  monitor, nullptr);
    }
    else
    {
        window = glfwCreateWindow(app::windowWidth, app::windowHeight, app::windowTitle,
                                  nullptr, nullptr);
    }
    if (!window)
        throw std::runtime_error("glfw: failed to create window");
    spdlog::info("glfw: created window");

    ImageAsset windowIcon = assets.get_image("icon.png");
    GLFWimage windowIconInfo = {
        .width  = (int) windowIcon.get_width(),
        .height = (int) windowIcon.get_height(),
        .pixels = windowIcon.get()
    };
    if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND)   // ;(
        glfwSetWindowIcon(window, 1, &windowIconInfo);


    renderer = new Renderer(window, assets);
}

void Application::run()
{
    while (!glfwWindowShouldClose(window))
    {
        renderer->present();
        glfwPollEvents();
    }
}

Application::~Application()
{
    delete renderer;

    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}


int main()
{
    try
    {
        Application app = Application();
        app.run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "### An unknown error has occured!! ###" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
