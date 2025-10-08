#include "./Command.hpp"
#include "./utils.hpp"


Command::Command(Device* device)
  : device(device)
{
    vkGetDeviceQueue(device->get(), device->get_queue_family(Device::QueueFamily::GRAPHICS),
                     0, &graphicsQueue);

    VkCommandPoolCreateInfo poolInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = device->get_queue_family(Device::QueueFamily::GRAPHICS)
    };
    vk_try(vkCreateCommandPool(device->get(), &poolInfo, nullptr, &VKCommandPool),
           "failed to create command pool [graphics]");
    logger.info("created command pool for family {} [graphics]", poolInfo.queueFamilyIndex);
}

Command::~Command()
{
    delete[] cmdBufs;
    vkDestroyCommandPool(device->get(), VKCommandPool, nullptr);
}


void Command::allocate_buffers(uint32_t nBuffers)
{
    if (nCmdBufs)
        vkFreeCommandBuffers(device->get(), VKCommandPool, nCmdBufs, cmdBufs);
    nCmdBufs = nBuffers;
    delete[] cmdBufs; cmdBufs = new VkCommandBuffer[nCmdBufs];

    const VkCommandBufferAllocateInfo bufferInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = VKCommandPool,
        .level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = nCmdBufs
    };
    vk_try(vkAllocateCommandBuffers(device->get(), &bufferInfo, cmdBufs),
           "failed to allocate command buffers [graphics]");
    logger.info("allocated {} command buffers [graphics]", nCmdBufs);
}

VkCommandBuffer Command::record(uint32_t index)
{
    if (index >= nCmdBufs)
        throw std::runtime_error("vulkan: invalid command buffer index [graphics]");
    vkResetCommandBuffer(cmdBufs[index], 0);

    const VkCommandBufferBeginInfo recordInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr
    };
    vk_try(vkBeginCommandBuffer(cmdBufs[index], &recordInfo),
           fmt::format("init failed for command buffer {} [graphics]", index).c_str());

    return cmdBufs[index];
}

// This function assumes the command buffer to be in the recording state (through Command::record()).
// This is ONLY enforced by the validation layer, so misuse will provoke undefined behaviour.
void Command::submit(uint32_t index, VkSemaphore sigPreSubmit, 
                     VkSemaphore semaphorePostSubmit, VkFence fencePostSubmit)
{
    if (index >= nCmdBufs)
        throw std::runtime_error("vulkan: invalid command buffer index [graphics]");
    vk_try(vkEndCommandBuffer(cmdBufs[index]),
           fmt::format("recording failed for command buffer {} [graphics]", index).c_str());

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &sigPreSubmit,
        .pWaitDstStageMask = &waitStage,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmdBufs[index],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &semaphorePostSubmit
    };
    vk_try(vkQueueSubmit(graphicsQueue, 1, &submitInfo, fencePostSubmit),
           fmt::format("submission failed for command buffer {} [graphics]", index).c_str());
}
