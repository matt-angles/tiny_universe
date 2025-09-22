#pragma once
#include "logging.hpp"

#include <vulkan/vulkan.h>


class Device {
public:
    Device(VkInstance instance, uint32_t requiredVersion);
    ~Device();

    VkDevice get() { return VKDevice; }

private:
    VkInstance instance = VK_NULL_HANDLE;

    struct DeviceProfile {
        VkPhysicalDevice handle;
        VkPhysicalDeviceProperties2 properties;
    } profile;
    VkDevice VKDevice;

    Logger logger = Logger("vulkan");

private:
    struct DeviceProfile acquire_physical_device(uint32_t requiredVersion);
    void fill_profile(struct DeviceProfile& profile, VkPhysicalDevice device);
    bool check_profile(const struct DeviceProfile& profile, uint32_t requiredVersion);
    int  score_profile(const struct DeviceProfile& profile);
    void log_profile(const struct DeviceProfile& profile, uint32_t index);
};
