#pragma once

#include<Framework/VulkanContext.h>
#include<macros.h>


class Descriptor
{
    public:
    Descriptor() = default;
    ~Descriptor() = default;
    void init();

    void bind(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding);

    std::vector<VkDescriptorSetLayoutBinding> m_bindings;

    VkDescriptorSetLayout m_desLayout;

    VkDescriptorPool m_pool;

    VkDescriptorSet m_set;

    void updateDescriptorLayout();

    void clean();

};