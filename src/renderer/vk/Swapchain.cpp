#include "./Swapchain.hpp"
#include "./utils.hpp"

#include <algorithm>


Swapchain::Swapchain(GLFWwindow* window, Instance* instance, Device* device)
  : instance(instance), device(device)
{
    vkGetDeviceQueue(device->get(), device->get_queue_family(Device::QueueFamily::PRESENT), 0, &presentQueue);
    logger.info("retrieved present queue from family {} [present]", device->get_queue_family(Device::QueueFamily::PRESENT));

    vk_try(glfwCreateWindowSurface(instance->get(), window, nullptr, &VKSurface),
           "failed to create window surface");
    logger.info("created window surface");

    device->get_surface_capabilities(VKSurface, &VKSurfaceCapabilities);

    device->get_surface_formats(VKSurface, &nFormats, nullptr);
    formats = new VkSurfaceFormatKHR[nFormats];
    device->get_surface_formats(VKSurface, &nFormats, formats);

    device->get_surface_modes(VKSurface, &nModes, nullptr);
    modes = new VkPresentModeKHR[nModes];
    device->get_surface_modes(VKSurface, &nModes, modes);
    log_surface();

    VkExtent2D swapchainExtent = VKSurfaceCapabilities.currentExtent;
    if (swapchainExtent.width == UINT32_MAX)
    {
        logger.info("surface extent hasn't been determined by the window manager");

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        swapchainExtent.width = std::clamp((uint32_t) width, VKSurfaceCapabilities.minImageExtent.width,
                                                             VKSurfaceCapabilities.maxImageExtent.width);
        swapchainExtent.height = std::clamp((uint32_t) height, VKSurfaceCapabilities.minImageExtent.height,
                                                               VKSurfaceCapabilities.maxImageExtent.height);
    }
    uint32_t swapchainQueueFamilies[] = {
        device->get_queue_family(Device::QueueFamily::GRAPHICS),
        device->get_queue_family(Device::QueueFamily::PRESENT)
    };
    VkSwapchainCreateInfoKHR swapchainInfo {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = VKSurface,
        .minImageCount = VKSurfaceCapabilities.maxImageCount > 0 && VKSurfaceCapabilities.minImageCount+1 > VKSurfaceCapabilities.maxImageCount
                       ? VKSurfaceCapabilities.maxImageCount : VKSurfaceCapabilities.minImageCount+1,
        .imageFormat     = request_sformat(VK_FORMAT_B8G8R8A8_SRGB,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR).format,
        .imageColorSpace = request_sformat(VK_FORMAT_B8G8R8A8_SRGB,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR).colorSpace,
        .imageExtent      = swapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = swapchainQueueFamilies[0] == swapchainQueueFamilies[1]
                          ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,                         // ignored when VK_SHARING_MODE_EXCLUSIVE
        .pQueueFamilyIndices   = swapchainQueueFamilies,    // ignored when VK_SHARING_MODE_EXCLUSIVE
        .preTransform   = VKSurfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = request_smode(VK_PRESENT_MODE_FIFO_KHR),
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };
    curFormat = swapchainInfo.imageFormat; curExtent = swapchainInfo.imageExtent;
    vk_try(vkCreateSwapchainKHR(device->get(), &swapchainInfo, nullptr, &VKSwapchain),
           "failed to create swapchain");
    logger.info("created swapchain");

    vkGetSwapchainImagesKHR(device->get(), VKSwapchain, &nImages, nullptr);
    images = new VkImage[nImages];
    vk_try(vkGetSwapchainImagesKHR(device->get(), VKSwapchain, &nImages, images),
           "failed to retrieve swapchain images");

    imageViews = new VkImageView[nImages];
    VkImageViewCreateInfo viewInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image    = VK_NULL_HANDLE,     // filled in the for loop
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = curFormat,
        .components = {VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,
                       VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount   = 1,
                             .baseArrayLayer = 0,
                             .layerCount     = 1}
    };
    for (uint32_t i=0; i < nImages; i++)
    {
        viewInfo.image = images[i];
        vk_try(vkCreateImageView(device->get(), &viewInfo, nullptr, imageViews+i),
               "failed to create swapchain views");
    }

    log_swapchain(swapchainInfo);
}

Swapchain::~Swapchain()
{
    for (uint32_t i=0; i < nImages; i++)
        vkDestroyImageView(device->get(), imageViews[i], nullptr);
    delete[] imageViews;
    delete[] images;
    vkDestroySwapchainKHR(device->get(), VKSwapchain, nullptr);

    delete[] modes;
    delete[] formats;
    vkDestroySurfaceKHR(instance->get(), VKSurface, nullptr);
}


uint32_t Swapchain::acquire_img(VkSemaphore sigPostAcquire)
{
    uint32_t index;
    vk_try(vkAcquireNextImageKHR(device->get(), VKSwapchain, UINT64_MAX, sigPostAcquire, VK_NULL_HANDLE, &index),
           "failed to acquire swapchain image");
    return index;
}

void Swapchain::present_img(uint32_t index, VkSemaphore sigPrePresent)
{
    VkPresentInfoKHR presentInfo {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &sigPrePresent,
        .swapchainCount = 1,
        .pSwapchains    = &VKSwapchain,
        .pImageIndices  = &index,
        .pResults = nullptr
    };
    vk_try(vkQueuePresentKHR(presentQueue, &presentInfo),
           "presentation failed");
}

void Swapchain::transition_img(VkCommandBuffer cmd, uint32_t index, VkImageLayout newLayout)
{
    VkPipelineStageFlags srcStageMask, dstStageMask;
    VkImageMemoryBarrier transitionInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = 0,                     //cond
        .dstAccessMask = 0,                     //cond
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED, //cond
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = images[index],
        .subresourceRange {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount   = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1
        }
    };
    if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        transitionInfo.srcAccessMask = VK_ACCESS_NONE;
        transitionInfo.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        transitionInfo.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    else if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        transitionInfo.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        transitionInfo.dstAccessMask = VK_ACCESS_NONE;
        transitionInfo.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    else
        throw std::runtime_error("vulkan: invalid/unimplemented image layout transition");

    vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, 0,
                         0, nullptr, 0, nullptr, 1, &transitionInfo);
}


VkSurfaceFormatKHR Swapchain::request_sformat(VkFormat rFormat, VkColorSpaceKHR rColorSpace)
{
    for (uint32_t i=0; i < nFormats; i++)
        if (formats[i].format == rFormat && formats[i].colorSpace == rColorSpace)
            return formats[i];

    logger.warn("requested format {},{} is unsupported by this surface",
                string_VkFormat(rFormat), string_VkColorSpaceKHR(rColorSpace));
    return formats[0];
}

VkPresentModeKHR Swapchain::request_smode(VkPresentModeKHR rMode)
{
    for (uint32_t i=0; i < nFormats; i++)
        if (modes[i] == rMode)
            return modes[i];

    logger.warn("requested mode {} is unsupported by this surface", 
                string_VkPresentModeKHR(rMode));
    return VK_PRESENT_MODE_FIFO_KHR;
}


void Swapchain::log_surface()
{
    logger.debug("available surface format/color combinations");
    for (uint32_t i=0; i < nFormats; i++)
        logger.debug("\t{}\t; {}", string_VkFormat(formats[i].format), string_VkColorSpaceKHR(formats[i].colorSpace));
    logger.debug("available surface presentation modes");
    for (uint32_t i=0; i < nModes; i++)
        logger.debug("\t{}", string_VkPresentModeKHR(modes[i]));
}

void Swapchain::log_swapchain(VkSwapchainCreateInfoKHR info)
{
    logger.info("\t{} images of extent {}x{}", nImages, info.imageExtent.width, info.imageExtent.height);
    logger.info("\tFormat: {}", string_VkFormat(info.imageFormat));
    logger.info("\tColor Space: {}", string_VkColorSpaceKHR(info.imageColorSpace));
    logger.info("\tPresent Mode: {}", string_VkPresentModeKHR(info.presentMode));
}
