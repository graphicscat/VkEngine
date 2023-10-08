#include<Framework/Descriptor.h>

void Descriptor::init()
{
    VulkanContext* ctx = VulkanContext::getInstance();
    std::vector<VkDescriptorPoolSize> sizes = 
	{
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,10}
	};

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.maxSets = 10;
	poolInfo.poolSizeCount = (uint32_t)sizes.size();
	poolInfo.pPoolSizes = sizes.data();

	VK_CHECK(vkCreateDescriptorPool(ctx->m_device,&poolInfo,nullptr,&m_pool))
    LOG_TRACE("Init DescriptorPool Success");

}

void Descriptor::bind(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
{
    VkDescriptorSetLayoutBinding bind = 
    vkinit::descriptorsetLayoutBinding(type,stageFlags,binding);
    m_bindings.push_back(bind);
}

void Descriptor::updateDescriptorLayout()
{
   VkDescriptorSetLayoutCreateInfo info = vkinit::descriptorSetLayoutLayoutCreateInfo(m_bindings);
   vkCreateDescriptorSetLayout(VulkanContext::getInstance()->m_device,&info,nullptr,&m_desLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_desLayout;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    VK_CHECK(vkAllocateDescriptorSets(VulkanContext::getInstance()->m_device,&allocInfo,&m_set))
}

void Descriptor::clean()
{
    vkDestroyDescriptorPool(VulkanContext::getInstance()->m_device,m_pool,nullptr);
    vkDestroyDescriptorSetLayout(VulkanContext::getInstance()->m_device,m_desLayout,nullptr);
}

