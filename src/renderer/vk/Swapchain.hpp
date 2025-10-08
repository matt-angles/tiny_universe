#pragma once
#include "./Instance.hpp"
#include "./Device.hpp"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


class Swapchain {
public:
    Swapchain(GLFWwindow* window, Instance* instance, Device* device);
    ~Swapchain();

    uint32_t acquire_img(VkSemaphore sigPostAcquire);
    void present_img(uint32_t index, VkSemaphore sigPrePresent);
    void transition_img(VkCommandBuffer cmd, uint32_t index, VkImageLayout newLayout);

    VkFormat get_sformat() const { return curFormat; }
    VkExtent2D get_sextent() const { return curExtent; }
    uint32_t get_image_count() const { return nImages; }
    VkImageView get_image_view(uint32_t index) const { return imageViews[index]; }

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
    VkFormat curFormat;
    VkExtent2D curExtent;
    uint32_t nImages;
    VkImage* images;
    VkImageView* imageViews;

    Logger logger = Logger("vulkan");
    void log_surface();
    void log_swapchain(VkSwapchainCreateInfoKHR info);
};
