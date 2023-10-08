#pragma once
#include<macros.h>
#include<Framework/VulkanContext.h>

class Buffer
{
    public:
    VkBuffer m_buffer;
    VkDeviceMemory m_mem;
    VkDescriptorBufferInfo m_descriptor{};
    void* m_mapped;
    uint32_t size = 0;
    
    Buffer() = default;
    ~Buffer() = default;

    Buffer(uint32_t s);

    void create();
    void upload(VkDescriptorSet set, uint32_t binding);
    void update(const void * data);

    void clean();
};