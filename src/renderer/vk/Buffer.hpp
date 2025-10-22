#pragma once
#include "./Device.hpp"
#include <vulkan/vulkan.h>


class Buffer {
public:
    Buffer(Device* device, VkBufferUsageFlags type, VkDeviceSize size);
    ~Buffer();

    VkBuffer get() const { return VKBuffer; }
    VkDeviceSize get_size() const { return VKDeviceSize; }
    void* map();
    void  unmap();

private:
    Device* device;

    VkBuffer VKBuffer;
    VkDeviceSize VKDeviceSize;
    VkDeviceMemory VKDeviceMemory;
};
