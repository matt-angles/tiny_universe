#include "Instance.hpp"
#include "Application.hpp"
#include "utils.hpp"

#include <GLFW/glfw3.h>
#include <cstring>


static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
    VkDebugUtilsMessageTypeFlagsEXT             type,
    const VkDebugUtilsMessengerCallbackDataEXT* info,
    void*
)
{
    spdlog::log(spdlog_VkDebugMessageSeverity(severity), "{}: {}", string_VkDebugMessageType(type), info->pMessage);
    return true;
}


Instance::Instance(uint32_t requiredVersion, uint32_t targetVersion,
                   std::set<std::string>& requiredExtensions, const std::set<std::string>& optionalExtensions)
{
    versionNum = VK_API_VERSION_1_0;
    if (vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"))
        vkEnumerateInstanceVersion(&versionNum);
    if (VK_API_VERSION_VARIANT(versionNum) != 0)
        logger.warn("non-standard implementation detected: variant {} !", VK_API_VERSION_VARIANT(versionNum));
    if (requiredVersion > (versionNum & ((1 << 29) - 1)))   // variant is packed into bits 31-29 ; discard it here
    {
        logger.error("detected instance version {}.{} - app requires at least {}.{}",
                     VK_API_VERSION_MAJOR(versionNum), VK_API_VERSION_MINOR(versionNum),
                     VK_API_VERSION_MAJOR(requiredVersion), VK_API_VERSION_MINOR(requiredVersion));
        throw std::runtime_error("vulkan: unsupported version");
    }

    const char** glfwExtensions; uint32_t nGlfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&nGlfwExtensions);
    for (uint32_t i=0; i < nGlfwExtensions; i++)
        requiredExtensions.insert(glfwExtensions[i]);

    std::set<std::string> missingExtensions = check_extensions(requiredExtensions, optionalExtensions);
    nExtensions = requiredExtensions.size() + optionalExtensions.size() - missingExtensions.size();
    extensions = new char*[nExtensions]; uint32_t i=0;
    for (const std::string& extension : requiredExtensions)
    {
        extensions[i] = new char[extension.length()+1];
        strcpy(extensions[i++], extension.c_str());
    }
    for (const std::string& extension : optionalExtensions)
        if (!missingExtensions.count(extension))
        {
            extensions[i] = new char[extension.length()+1];
            strcpy(extensions[i++], extension.c_str());
        }

    const VkApplicationInfo appInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName   = app::name,
        .applicationVersion = (CMAKE_PROJECT_VERSION_MAJOR << 16) + CMAKE_PROJECT_VERSION_MINOR,
        .pEngineName   = nullptr,
        .engineVersion = 0,
        .apiVersion = targetVersion
    };
    VkDebugUtilsMessengerCreateInfoEXT debugInfo {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VkDebugMessageSeverity_spdlog(spdlog::get_level()),
        .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = vk_debug_callback,
        .pUserData = nullptr
    };
    VkInstanceCreateInfo instanceInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = has_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) ? &debugInfo : nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount   = 0,
        .ppEnabledLayerNames = extensions,
        .enabledExtensionCount   = nExtensions,
        .ppEnabledExtensionNames = extensions
    };
#ifndef NDEBUG
    const char* validationLayer = "VK_LAYER_KHRONOS_validation";
    instanceInfo.enabledLayerCount   = 1;
    instanceInfo.ppEnabledLayerNames = &validationLayer;
#endif
    vk_try(vkCreateInstance(&instanceInfo, nullptr, &VKInstance),
           "failed to create instance");
    logger.info("initialized Vulkan {}.{}.{}",
                VK_API_VERSION_MAJOR(versionNum), VK_API_VERSION_MINOR(versionNum), VK_API_VERSION_PATCH(versionNum));
    if (nExtensions)
    {
        logger.info("loaded {} extensions:", nExtensions);
        for (uint32_t i=0; i < nExtensions; i++)
            logger.info("\t{}", extensions[i]);
    }

    VkResult var;
    if (has_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        if ((var = vkCreateDebugUtilsMessengerEXT(VKInstance, &debugInfo, nullptr, &VKDebugMessenger)) != VK_SUCCESS)
            logger.warn("failed to create debug messenger {}", string_VkResult(var));
        else
            logger.info("created debug messenger");
    }
}

Instance::~Instance()
{
    if (VKDebugMessenger) vkDestroyDebugUtilsMessengerEXT(VKInstance, VKDebugMessenger, nullptr);
    if (VKInstance)       vkDestroyInstance(VKInstance, nullptr);

    for (uint32_t i=0; i < nExtensions; i++)
        delete extensions[i];
    delete extensions;
}


std::set<std::string> Instance::check_extensions(std::set<std::string> requiredExtensions, std::set<std::string> optionalExtensions)
{
    uint32_t nAvailableExtensions;
    vkEnumerateInstanceExtensionProperties(nullptr, &nAvailableExtensions, nullptr);
    VkExtensionProperties availableExtensions[nAvailableExtensions];
    vk_try(vkEnumerateInstanceExtensionProperties(nullptr, &nAvailableExtensions, availableExtensions),
           VK_SUCCESS, "failed to retrieve instance extensions");

    for (uint32_t i=0; i < nAvailableExtensions; i++)
    {
        requiredExtensions.erase(availableExtensions[i].extensionName);
        optionalExtensions.erase(availableExtensions[i].extensionName);
    }

    if (!requiredExtensions.empty())
    {
        logger.error("missing {} required instance extensions:", requiredExtensions.size());
        for (const std::string& missingExtension : requiredExtensions)
            logger.error("\t{}", missingExtension);
        throw std::runtime_error("vulkan: missing required instance extensions");
    }
    if (!optionalExtensions.empty())
    {
        logger.warn("missing {} optional instance extensions:", optionalExtensions.size());
        for (const std::string& missingExtension : optionalExtensions)
            logger.warn("\t{}", missingExtension);
    }
    return optionalExtensions;
}


bool Instance::has_extension(const char* name) const
{
    for (uint32_t i=0; i < nExtensions; i++)
        if (!strcmp(extensions[i], name))
            return true;
    return false;
}
