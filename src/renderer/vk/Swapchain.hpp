#pragma once
#include "./Instance.hpp"
#include "./Device.hpp"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


class Swapchain {
public:
    Swapchain(GLFWwindow* window, Instance* instance, Device* device);
    ~Swapchain();

    uint32_t acquire_img();
    void present_img(uint32_t);

private:
    VkSurfaceFormatKHR request_sformat(VkFormat rFormat, VkColorSpaceKHR rColorSpace);
    VkPresentModeKHR request_smode(VkPresentModeKHR rMode);

    Instance* instance = nullptr;
    Device* device = nullptr;
    VkQueue presentQueue = VK_NULL_HANDLE;

    VkSurfaceKHR VKSurface = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR VKSurfaceCapabilities;
    uint32_t nFormats;
    VkSurfaceFormatKHR* formats;
    uint32_t nModes;
    VkPresentModeKHR* modes;

    VkSwapchainKHR VKSwapchain = VK_NULL_HANDLE;
    uint32_t nImages;
    VkImage* images;

    Logger logger = Logger("vulkan");
    void log_surface();
    void log_swapchain(VkSwapchainCreateInfoKHR info);
};
