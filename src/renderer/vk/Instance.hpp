#pragma once
#include "logging.hpp"

#include <vulkan/vulkan.h>
#include <set>


class Instance {
public:
    Instance(uint32_t requiredVersion, uint32_t targetVersion,
             std::set<std::string>& requiredExtensions, const std::set<std::string>& optionalExtensions);
    ~Instance();

    VkInstance get()   const { return VKInstance; }
    uint32_t version() const { return versionNum; }
    bool has_extension (const char* name) const;

private:
    std::set<std::string> check_extensions(std::set<std::string> requiredExtensions, std::set<std::string> optionalExtensions);

    VkInstance VKInstance = nullptr;
    uint32_t versionNum;
    uint32_t nExtensions;
    char** extensions = nullptr;

    VkDebugUtilsMessengerEXT VKDebugMessenger = nullptr;
    Logger logger = Logger("vulkan");
};
