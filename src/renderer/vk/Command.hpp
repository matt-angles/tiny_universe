#pragma once
#include "./Device.hpp"
#include "logging.hpp"

#include <vulkan/vulkan.h>

class Command {
public:
    Command(Device* device);
    ~Command();

    void allocate_buffers(uint32_t nBuffers);

    VkCommandBuffer record(uint32_t index);
    void submit(uint32_t index);

private:
    Device* device = nullptr;

    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkCommandPool VKCommandPool = VK_NULL_HANDLE;
    uint32_t nCmdBufs = 0;
    VkCommandBuffer* cmdBufs = nullptr;

    Logger logger = Logger("vulkan");
};
