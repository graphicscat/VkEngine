#include<Framework/Pipeline.h>
#include<Framework/Mesh.h>

bool GraphicsPipeline::loadShaderModule(const char* filePath , VkShaderModule* outShaderModule)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
        std::cout<<"cant find file";
		return false;
	}

	//find what the size of the file is by looking up the location of the cursor
	//because the cursor is at the end, it gives the size directly in bytes
	size_t fileSize = (size_t)file.tellg();

	//spirv expects the buffer to be on uint32, so make sure to reserve a int vector big enough for the entire file
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	//put file cursor at beggining
	file.seekg(0);

	//load the entire file into the buffer
	file.read((char*)buffer.data(), fileSize);

	//now that the file is loaded into the buffer, we can close it
	file.close();

	//create a new shader module, using the buffer we loaded
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;

	//codeSize has to be in bytes, so multply the ints in the buffer by size of int to know the real size of the buffer
	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode = buffer.data();

	//check that the creation goes well.
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_ctx->m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		return false;
	}
	*outShaderModule = shaderModule;
	return true;
}

GraphicsPipeline::GraphicsPipeline(VulkanContext* context)
{
    m_ctx = context;
}

void GraphicsPipeline::initRenderPass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = m_ctx->m_swapchain.m_swapchainImageFormat;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depth_attachment{};
    //TODO
	depth_attachment.format = VK_FORMAT_D32_SFLOAT;
	depth_attachment.flags = 0;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref{};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;
    
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency depth_dependency = {};
	depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	depth_dependency.dstSubpass = 0;
	depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depth_dependency.srcAccessMask = 0;
	depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::vector<VkAttachmentDescription> attachments = {color_attachment,depth_attachment};
	std::vector<VkSubpassDependency> dependencies = { dependency,depth_dependency};
    VkRenderPassCreateInfo renderpassInfo{};
    renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassInfo.attachmentCount = uint32_t(attachments.size());
    renderpassInfo.pAttachments = attachments.data();
    renderpassInfo.dependencyCount = uint32_t(dependencies.size());
    renderpassInfo.pDependencies = dependencies.data();
    renderpassInfo.subpassCount = 1;
    renderpassInfo.pSubpasses = &subpass;

    VK_CHECK(vkCreateRenderPass(m_ctx->m_device,&renderpassInfo,nullptr,&m_renderPass))
    LOG_TRACE("Init RenderPass Success");
}

void GraphicsPipeline::initFramebuffer()
{
    VkFramebufferCreateInfo fb_info = vkinit::framebufferCreateInfo(m_renderPass,{WIN_WIDTH,WIN_HEIGHT});

    const uint32_t swapchain_imageCount = uint32_t(m_ctx->m_swapchain.m_swapchainImageViews.size());
    m_framebuffers.resize(swapchain_imageCount);

    for( uint32_t i = 0;i<swapchain_imageCount;i++)
    {
		std::vector<VkImageView> attachments = {m_ctx->m_swapchain.m_swapchainImageViews[i],m_depthMap.view};
        fb_info.pAttachments = attachments.data();
		fb_info.attachmentCount = uint32_t(attachments.size());
        VK_CHECK(vkCreateFramebuffer(m_ctx->m_device,&fb_info,nullptr,&m_framebuffers[i]))
    }
    LOG_TRACE("Init Framebuffers Success");
}

void GraphicsPipeline::defaultGraphicsPipeline(const char* vertexShader,const char* fragShader,VkDescriptorSetLayout desLayout)
{
    m_ctx = VulkanContext::getInstance();
    //initRenderPass();
    //initDepthMap();
    //initFramebuffer();
    VkShaderModule m_vertexShader{};
	if (!loadShaderModule(vertexShader, &m_vertexShader))
	{
		LOG_ERROR("Error when building the vertex shader");
	}

    VkShaderModule m_fragShader{};
    if (!loadShaderModule(fragShader, &m_fragShader))
	{
		LOG_ERROR("Error when building the fragment shader");
	}

    m_shaderStages.push_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT,m_vertexShader));
    m_shaderStages.push_back(vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT,m_fragShader));

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = nullptr;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &desLayout;
    VK_CHECK(vkCreatePipelineLayout(m_ctx->m_device,&layoutInfo,nullptr,&m_pipelineLayout));
    LOG_TRACE("Init GraphicsPipelineLayout success");

    VertexInputDescription vertexDescription = Vertex::get_vertex_description();

    m_vertexInputInfo = vkinit::vertexInputStateCreateInfo();

    m_vertexInputInfo.vertexAttributeDescriptionCount = uint32_t(vertexDescription.attributes.size());
    m_vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
    m_vertexInputInfo.vertexBindingDescriptionCount = uint32_t(vertexDescription.bindings.size());
    m_vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();

    m_inputAssembly = vkinit::inputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    m_rasterizer = vkinit::rasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
    m_multisampling = vkinit::multisamplingStateCreateInfo();
    m_colorBlendAttachment = vkinit::colorBlendAttachmentState();
    m_depthStencil = vkinit::depthStencilCreateInfo(true,true,VK_COMPARE_OP_LESS_OR_EQUAL);

    m_viewport.minDepth = 0.0f;
    m_viewport.maxDepth = 1.0f;
    m_viewport.width = WIN_WIDTH;
    m_viewport.height = WIN_HEIGHT;
    m_viewport.x = 0.0;
    m_viewport.y = 0.0;

    m_scissor.extent = VkExtent2D{WIN_WIDTH,WIN_HEIGHT};
    m_scissor.offset = {0, 0};

    VkPipelineViewportStateCreateInfo viewStateInfo{};
    viewStateInfo.pViewports = &m_viewport;
    viewStateInfo.pScissors = &m_scissor;
    viewStateInfo.scissorCount = 1;
    viewStateInfo.viewportCount = 1;
    viewStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = attachmentCount;
	colorBlending.pAttachments = &m_colorBlendAttachment;

    VkFormat colorFormat = VK_FORMAT_R8G8B8_SRGB;
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.depthAttachmentFormat = depthFormat;
    renderingInfo.pColorAttachmentFormats = &colorFormat;
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

    LOG_INFO("RenderingInfo colorAttachment count {0}",renderingInfo.colorAttachmentCount);

    VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
    graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineInfo.pVertexInputState = &m_vertexInputInfo;
    graphicsPipelineInfo.pInputAssemblyState = &m_inputAssembly;
    graphicsPipelineInfo.pRasterizationState = &m_rasterizer;
    graphicsPipelineInfo.pColorBlendState = &colorBlending;
    graphicsPipelineInfo.stageCount = uint32_t(m_shaderStages.size());
    graphicsPipelineInfo.pStages = m_shaderStages.data();
    graphicsPipelineInfo.pViewportState = &viewStateInfo;
    graphicsPipelineInfo.pDepthStencilState = &m_depthStencil;
    graphicsPipelineInfo.layout = m_pipelineLayout;
    graphicsPipelineInfo.subpass = 0;
    graphicsPipelineInfo.renderPass = VulkanContext::getInstance()->m_renderpass;
    //graphicsPipelineInfo.pNext = &renderingInfo;

    VK_CHECK(vkCreateGraphicsPipelines(m_ctx->m_device,VK_NULL_HANDLE,1,&graphicsPipelineInfo,nullptr,&m_pipeline))
    LOG_TRACE("Init GraphicsPipeline Success");

    vkDestroyShaderModule(m_ctx->m_device,m_fragShader,nullptr);
    vkDestroyShaderModule(m_ctx->m_device,m_vertexShader,nullptr);

}

void GraphicsPipeline::quit()
{
    for(int i = 0;i<m_framebuffers.size();i++)
    {
        vkDestroyFramebuffer(m_ctx->m_device,m_framebuffers[i],nullptr);
    }
    vkDestroyRenderPass(m_ctx->m_device,m_renderPass,nullptr);
    vkDestroyPipelineLayout(m_ctx->m_device,m_pipelineLayout,nullptr);
    vkDestroyPipeline(m_ctx->m_device,m_pipeline,nullptr); 
    vkDestroyImageView(m_ctx->m_device,m_depthMap.view,nullptr);
    vkFreeMemory(m_ctx->m_device,m_depthMap.mem,nullptr);   
    vkDestroyImage(m_ctx->m_device,m_depthMap.image,nullptr);
}

void GraphicsPipeline::initDepthMap()
{
    VkFormat format= VK_FORMAT_D32_SFLOAT;
    m_depthMap.format = format;
    m_ctx->createImage(
		WIN_WIDTH,
		WIN_HEIGHT,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_depthMap.image,
		m_depthMap.mem
	);
	m_depthMap.view = m_ctx->createImageView(m_depthMap.image, format, VK_IMAGE_ASPECT_DEPTH_BIT);
    LOG_TRACE("Init DepthMap Success");
    
}