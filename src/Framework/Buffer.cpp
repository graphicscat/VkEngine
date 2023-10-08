#include<Framework/Buffer.h>

Buffer::Buffer(uint32_t s):size(s)
{}

void Buffer::create()
{
	VulkanContext::getInstance()->createBuffer(size,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	m_buffer,
	m_mem);

    m_descriptor.buffer = m_buffer;
    m_descriptor.offset = 0;
    m_descriptor.range = size;

    VK_CHECK(vkMapMemory(VulkanContext::getInstance()->m_device,m_mem,0,size,0,&m_mapped))

}

void Buffer::upload(VkDescriptorSet set, uint32_t binding)
{
    VkWriteDescriptorSet writer{};
    writer.descriptorCount = 1;
    writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writer.dstBinding = binding;
    writer.dstSet = set;
    writer.pBufferInfo = &m_descriptor;
    writer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vkUpdateDescriptorSets(VulkanContext::getInstance()->m_device,1,&writer,0,nullptr);
}

void Buffer::update(const void * data)
{
    memcpy(m_mapped,data,size);
}

void Buffer::clean()
{
    auto* ctx = VulkanContext::getInstance();

    vkFreeMemory(ctx->m_device,m_mem,nullptr);
    vkDestroyBuffer(ctx->m_device,m_buffer,nullptr);
    
}

