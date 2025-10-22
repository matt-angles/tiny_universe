#include "./Buffer.hpp"


Buffer::Buffer(Device* device, VkBufferUsageFlags type, VkDeviceSize size)
  : device(device), VKDeviceSize(size)
{
    VkBufferCreateInfo bufferInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size  = size,
        .usage = type,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr
    };
    vk_try(vkCreateBuffer(device->get(), &bufferInfo, nullptr, &VKBuffer),
           "failed to create buffer");

    VkMemoryRequirements memoryReqs;
    vkGetBufferMemoryRequirements(device->get(), VKBuffer, &memoryReqs);
    VkMemoryAllocateInfo memoryInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memoryReqs.size,
        .memoryTypeIndex = device->get_memory_index(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                    memoryReqs.memoryTypeBits)
    };
    vk_try(vkAllocateMemory(device->get(), &memoryInfo, nullptr, &VKDeviceMemory),
           "failed to allocate memory for buffer");
    vk_try(vkBindBufferMemory(device->get(), VKBuffer, VKDeviceMemory, 0),
           "failed to bind memory for buffer");
}

Buffer::~Buffer()
{
    vkFreeMemory(device->get(), VKDeviceMemory, nullptr);
    vkDestroyBuffer(device->get(), VKBuffer, nullptr);
}


void* Buffer::map()
{
    void* ptr;
    vkMapMemory(device->get(), VKDeviceMemory, 0, VK_WHOLE_SIZE, 0, &ptr);
    return ptr;
}

void Buffer::unmap()
{
    vkUnmapMemory(device->get(), VKDeviceMemory);
}
