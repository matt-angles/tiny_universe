#pragma once
#include "logging.hpp"

#include <vulkan/vulkan.h>
#include <set>


class Device {
public:
    Device(VkInstance instance, uint32_t requiredVersion,
           const std::set<std::string>& requiredExtensions, const std::set<std::string>& optionalExtensions);
    ~Device();

    VkDevice get() const     { return VKDevice; }
    uint32_t version() const { return profile.properties.properties.apiVersion; }
    bool has_extension (const char* name) const;

    enum class QueueFamily { GRAPHICS };
    uint32_t get_queue_family(QueueFamily type) const 
    {
        if (type == QueueFamily::GRAPHICS) return profile.qFamilyGraphics;
        throw std::runtime_error("vulkan: invalid queue family type");
    }

private:
    VkInstance instance = VK_NULL_HANDLE;

    struct DeviceProfile {
        VkPhysicalDevice handle;
        VkPhysicalDeviceProperties2 properties;
        uint32_t qFamilyGraphics = UINT32_MAX;

        uint32_t* _nAvlExtensions;
        VkExtensionProperties* _avlExtensions;
    } profile;

    VkDevice VKDevice;
    uint32_t nExtensions = 0;
    char** extensions = nullptr;

    Logger logger = Logger("vulkan");

private:
    struct DeviceProfile acquire_physical_device(uint32_t requiredVersion, 
                                                 const std::set<std::string>& requiredExtensions, 
                                                 const std::set<std::string>& optionalExtensions);
    void fill_profile(struct DeviceProfile& profile, VkPhysicalDevice device);
    bool check_profile(const struct DeviceProfile& profile, uint32_t requiredVersion,
                       std::set<std::string> requiredExtensions);
    int  score_profile(const struct DeviceProfile& profile);
    void log_profile(const struct DeviceProfile& profile, uint32_t index);
};
