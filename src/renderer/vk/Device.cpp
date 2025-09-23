#include "./Device.hpp"
#include "./utils.hpp"

#include <GLFW/glfw3.h>


Device::Device(VkInstance instance, uint32_t requiredVersion,
               const std::set<std::string>& requiredExtensions, const std::set<std::string>& optionalExtensions)
  : instance(instance)
{
    profile = acquire_physical_device(requiredVersion, requiredExtensions, optionalExtensions);

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo graphicsQueueInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = profile.qFamilyGraphics,
        .queueCount       = 1,
        .pQueuePriorities = &queuePriority
    };
    VkDeviceCreateInfo deviceInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos    = &graphicsQueueInfo,
        .enabledLayerCount   = 0,       // deprecated
        .ppEnabledLayerNames = nullptr, // deprecated
        .enabledExtensionCount   = nExtensions,
        .ppEnabledExtensionNames = extensions,
        .pEnabledFeatures = nullptr     // deprecated
    };
    vk_try(vkCreateDevice(profile.handle, &deviceInfo, nullptr, &VKDevice),
           "failed to create logical device");
    logger.info("created logical device");
    if (nExtensions)
    {
        logger.info("loaded {} extensions:", nExtensions);
        for (uint32_t i=0; i < nExtensions; i++)
            logger.info("\t{}", extensions[i]);
    }
}

Device::~Device()
{
    for (uint32_t i=0; i < nExtensions; i++)
        delete[] extensions[i];
    delete[] extensions;
    vkDestroyDevice(VKDevice, nullptr);
}


void Device::fill_profile(DeviceProfile& profile, VkPhysicalDevice device)
{
    profile.handle = device;

    profile.properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    profile.properties.pNext = nullptr;
    vkGetPhysicalDeviceProperties2(device, &profile.properties);

    uint32_t nQueueFamilies;
    vkGetPhysicalDeviceQueueFamilyProperties2(device, &nQueueFamilies, nullptr);
    VkQueueFamilyProperties2 queueFamilies[nQueueFamilies];
    for (uint32_t i=0; i < nQueueFamilies; i++)
        queueFamilies[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, queueFamilies[i].pNext = nullptr;
    vkGetPhysicalDeviceQueueFamilyProperties2(device, &nQueueFamilies, queueFamilies);
    for (uint32_t i=0; i < nQueueFamilies; i++)
    {
        auto& qFamilyFlags = queueFamilies[i].queueFamilyProperties.queueFlags;
        if (qFamilyFlags & VK_QUEUE_GRAPHICS_BIT) profile.qFamilyGraphics = i;
    }

    profile._nAvlExtensions = new uint32_t;
    vkEnumerateDeviceExtensionProperties(device, nullptr, profile._nAvlExtensions, nullptr);
    profile._avlExtensions = new VkExtensionProperties[*profile._nAvlExtensions];
    vk_try(vkEnumerateDeviceExtensionProperties(device, nullptr, profile._nAvlExtensions, profile._avlExtensions),
           VK_SUCCESS, "failed to retrieve device extensions");
}

bool Device::check_profile(const DeviceProfile& profile, uint32_t requiredVersion,
                           std::set<std::string> requiredExtensions)
{
    if (profile.properties.properties.apiVersion < requiredVersion)
        return false;

    if (profile.qFamilyGraphics == UINT32_MAX)
        return false;

    for (uint32_t i=0; i < *profile._nAvlExtensions; i++)
        requiredExtensions.erase(profile._avlExtensions[i].extensionName);
    if (!requiredExtensions.empty())
        return false;

    return true;
}

int Device::score_profile(const DeviceProfile& profile)
{
    int score = 1;
    if (profile.properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 25;
    return score;
}

void Device::log_profile(const DeviceProfile& profile, uint32_t index)
{
    auto& properties = profile.properties.properties;
    logger.info("GPU #{} - {} [{}]", index, properties.deviceName, string_VkPhysicalDeviceType2(properties.deviceType));

    logger.info("\tVulkan {}.{}.{}{} - Driver {}",
        VK_API_VERSION_MAJOR(properties.apiVersion), VK_API_VERSION_MINOR(properties.apiVersion),
        VK_API_VERSION_PATCH(properties.apiVersion), VK_API_VERSION_VARIANT(properties.apiVersion) ? " (variant!)": "",
        properties.driverVersion);
    logger.info("\tVendor: {:#x}", properties.vendorID);
}


// TODO: dynamically allocate Device::DeviceProfile and return a pointer here to avoid an unecessary copy
Device::DeviceProfile Device::acquire_physical_device(uint32_t requiredVersion,
                                                      const std::set<std::string>& requiredExtensions,
                                                      const std::set<std::string>& optionalExtensions)
{
    uint32_t nDevices;
    vkEnumeratePhysicalDevices(instance, &nDevices, nullptr);
    if (nDevices == 0)
        throw std::runtime_error("vulkan: no vulkan-compatible device found");
    VkPhysicalDevice devices[nDevices];
    vk_try(vkEnumeratePhysicalDevices(instance, &nDevices, devices),
           VK_SUCCESS, "could not retrieve physical devices");

    logger.info("{} available physical device", nDevices);
    DeviceProfile profiles[nDevices];
    int bestProfile = -1, bestScore = 0;
    for (uint32_t i=0; i < nDevices; i++)
    {
        fill_profile(profiles[i], devices[i]);
        log_profile(profiles[i], i);
        if (check_profile(profiles[i], requiredVersion, requiredExtensions))
        {
            int profileScore = score_profile(profiles[i]);
            if (profileScore > bestScore)
            {
                bestProfile = i; bestScore = profileScore;
            }
        }
    }
    if (bestProfile < 0)
        throw std::runtime_error("vulkan: no supported device found");

    auto missingExtensions = optionalExtensions;
    for (uint32_t i=0; i < *profiles[bestProfile]._nAvlExtensions; i++)
        missingExtensions.erase(profiles[bestProfile]._avlExtensions[i].extensionName);
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

    for (uint32_t i=0; i < nDevices; i++)
    {
        delete profiles[i]._nAvlExtensions;
        delete[] profiles[i]._avlExtensions;
    }
    logger.info("selected device {}", bestProfile);
    return profiles[bestProfile];
}


bool Device::has_extension(const char* name) const
{
    for (uint32_t i=0; i < nExtensions; i++)
        if (!strcmp(extensions[i], name))
            return true;
    return false;
}
