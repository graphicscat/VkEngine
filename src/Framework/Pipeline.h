#pragma once 

#include<macros.h>
#include<Framework/VulkanContext.h>

class GraphicsPipeline
{
    public:
    VulkanContext* m_ctx;
    
    //VkDescriptorSetLayout m_descriptorSetLayout;

    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

	VkPipelineVertexInputStateCreateInfo m_vertexInputInfo;

	VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;

	VkViewport m_viewport;

	VkRect2D m_scissor;

	VkPipelineRasterizationStateCreateInfo m_rasterizer;

	VkPipelineColorBlendAttachmentState m_colorBlendAttachment;

	VkPipelineMultisampleStateCreateInfo m_multisampling;

	VkPipelineLayout m_pipelineLayout;

	VkPipelineDepthStencilStateCreateInfo m_depthStencil;

	uint32_t attachmentCount = 1;

    VkPipeline m_pipeline;

	VkRenderPass m_renderPass;
	
	std::vector<VkFramebuffer> m_framebuffers;

	struct {
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
		VkFormat format;
	} m_depthMap;

    bool loadShaderModule(const char* , VkShaderModule* outShaderModule);

    GraphicsPipeline() = default;
	GraphicsPipeline(VulkanContext* );
    void defaultGraphicsPipeline(const char* vertexShader,const char* fragShader,VkDescriptorSetLayout desLayout);
	
	void initRenderPass();

	void initFramebuffer();

	void quit();

	void initDepthMap();

};