#pragma once

#include<macros.h>
#include<Framework/VulkanContext.h>

namespace Command
{
    inline void allocateCmdBuffer(VkCommandBuffer& cmd,bool begin = false)
    {
        auto* ctx = VulkanContext::getInstance();
        VkCommandBufferAllocateInfo info = vkinit::commandBufferAllocateInfo(ctx->m_commandPool,1);
        VK_CHECK(vkAllocateCommandBuffers(ctx->m_device,&info,&cmd));

        if(begin)
        {
            VkCommandBufferBeginInfo commandBufferBI{};
			commandBufferBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			VK_CHECK(vkBeginCommandBuffer(cmd, &commandBufferBI));
        }
    }

    inline void flushCmdBuffer(VkCommandBuffer cmd)
    {
        auto* ctx = VulkanContext::getInstance();
        vkEndCommandBuffer(cmd);
        VkSubmitInfo submit{};
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkQueueSubmit(ctx->m_graphicsQueue,1,&submit,VK_NULL_HANDLE);
        vkDeviceWaitIdle(ctx->m_device);
        vkFreeCommandBuffers(ctx->m_device,ctx->m_commandPool,1,&cmd);
    }
}