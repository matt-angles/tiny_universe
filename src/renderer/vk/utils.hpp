#pragma once
#include "logging.hpp"

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>


// Helper functions
inline void vk_try(VkResult result, const char* errorMsg)
{
    if (result < 0)
        throw std::runtime_error(spdlog::fmt_lib::format("vulkan: {} ({})", errorMsg, string_VkResult(result)));
}
inline void vk_try(VkResult result, VkResult expectedResult, const char* errorMsg)
{
    if (result != expectedResult)
        throw std::runtime_error(spdlog::fmt_lib::format("vulkan: {} ({})", errorMsg, string_VkResult(result)));
}

inline void* vk_get_chain(void* voidPtr, VkStructureType x)
{
    auto* ptr = (VkBaseOutStructure*) voidPtr;
    while (ptr && ptr->sType != x)
        ptr = ptr->pNext;
    return ptr;
}
inline void vk_free_chain(void* voidPtr)
{
    auto* ptr = (VkBaseOutStructure*) voidPtr;
    VkBaseOutStructure* oldPtr;
    while (ptr)
    {
        oldPtr = ptr;
        ptr = ptr->pNext;
        delete oldPtr;
    }
}


// Type conversions
inline VkDebugUtilsMessageSeverityFlagsEXT VkDebugMessageSeverity_spdlog(spdlog::level::level_enum x)
{
    VkDebugUtilsMessageSeverityFlagsEXT y = 0;
    if (x <= spdlog::level::trace)  y |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    if (x <= spdlog::level::debug)  y |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    if (x <= spdlog::level::warn)   y |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    if (x <= spdlog::level::err)    y |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    return y;
}
inline spdlog::level::level_enum spdlog_VkDebugMessageSeverity(VkDebugUtilsMessageSeverityFlagsEXT x)
{
    if (x & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)   return spdlog::level::err;
    if (x & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) return spdlog::level::warn;
    if (x & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)    return spdlog::level::debug;
    else                                                     return spdlog::level::trace;
}

inline const char* string_VkDebugMessageType(VkDebugUtilsMessageTypeFlagsEXT x)
{
    switch (x)
    {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:     return "vkLoader";
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:  return "vkValidator";
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: return "vkPerf";
        default:                                              return "vkUnknown";
    }
}

inline const char* string_VkPhysicalDeviceType2(VkPhysicalDeviceType x)
{
    switch (x)
    {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "dGPU";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "iGPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "virt";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "CPU";
        default:                                     return "???";
    }
}


// Extension loaders
inline VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
    auto foo = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (foo) return foo(instance, pCreateInfo, pAllocator, pMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

inline VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{
    auto foo = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (foo) foo(instance, messenger, pAllocator);
}
