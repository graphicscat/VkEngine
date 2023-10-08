#include<Framework/Engine.h>
#include<memory>
#include<Framework/Command.h>
void Engine::run()
{
    init();
    loop();
    quit();
}

void Engine::initSync()
{
    VkFenceCreateInfo fenceCreateInfo = vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

	m_fences.resize(2);
	for(int i = 0; i < 2; i++)
	{
		VK_CHECK(vkCreateFence(ctx->m_device, &fenceCreateInfo, nullptr, &m_fences[i]));
	}

    VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphoreCreateInfo();

    m_presentSemaphores.resize(2);
	m_renderSemaphores.resize(2);

	for(int i = 0;i < 2;i++)
	{
        VK_CHECK(vkCreateSemaphore(ctx->m_device, &semaphoreCreateInfo, nullptr, &m_presentSemaphores[i]));
        VK_CHECK(vkCreateSemaphore(ctx->m_device, &semaphoreCreateInfo, nullptr, &m_renderSemaphores[i]));
    }
    LOG_TRACE("Init SyncStruct Success");
}

void Engine::init()
{
    Logger::init();
    //std::unique_ptr<VulkanContext> ctx = std::make_unique<VulkanContext>();
    //ctx = new VulkanContext();
    //ctx->init();
    VulkanContext::init();
    ctx = VulkanContext::getInstance();
    ctx->initSwapchain();
    //defaultPipeline.m_ctx = VulkanContext::getInstance();
    blinn_phong.vertexShader = "../shader/default.vert.spv";
    blinn_phong.fragShader = "../shader/default.frag.spv";
    blinn_phong.init();

    //updateMesh();
    scene.init();

    //initBuffer();

    VkDescriptorSetLayout deslayout{};
   //defaultPipeline.defaultGraphicsPipeline("../shader/tri.vert.spv","../shader/tri.frag.spv",m_descriptor.m_desLayout);
    //defaultPipeline.defaultGraphicsPipeline("../shader/default.vert.spv","../shader/default.frag.spv",m_descriptor.m_desLayout);
    m_mainCmdBuffers.resize(2);

    for(int i = 0;i<m_mainCmdBuffers.size();i++)
    {
        Command::allocateCmdBuffer(m_mainCmdBuffers[i]);
    }

    initSync();


}

void Engine::loop()
{
    SDL_Event e;
    bool bQuit = false;
	
	float deltaTime = 0.0f;
	float lastTime = 0.0f;
	const int JOYSTICK_DEAD_ZONE = 8000;
    while(!bQuit)
    {
		float currentTime = static_cast<float>(SDL_GetTicks())/1000.f;
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
        while(SDL_PollEvent(&e) != 0)
        {
			//ImGui_ImplSDL2_ProcessEvent(&e);
            if(e.key.keysym.sym == SDLK_ESCAPE)
				bQuit = true;
            if(e.type == SDL_QUIT) bQuit = true;

        }
        updateBuffer();
        draw();
    }

}
void Engine::quit()
{
    vkDeviceWaitIdle(ctx->m_device);
    for(int i = 0;i<2;i++)
    {
        vkFreeCommandBuffers(ctx->m_device,ctx->m_commandPool,1,&m_mainCmdBuffers[i]);
        vkDestroyFence(ctx->m_device,m_fences[i],nullptr);
        vkDestroySemaphore(ctx->m_device,m_presentSemaphores[i],nullptr);
        vkDestroySemaphore(ctx->m_device,m_renderSemaphores[i],nullptr);
    }
    //m_buffer.clean();
    //mesh.clean();
    scene.quit();
    //m_descriptor.clean();
    //defaultPipeline.quit();
    blinn_phong.quit();
    VulkanContext::quit();
    //delete ctx;
}

void Engine::draw()
{
    uint32_t currentFrame = m_frameNumber % 2;
    uint32_t nextImage = 0;
	VK_CHECK(vkWaitForFences(ctx->m_device,1,&m_fences[currentFrame],VK_TRUE,UINT64_MAX));
	VK_CHECK(vkResetFences(ctx->m_device,1,&m_fences[currentFrame]));
    VK_CHECK(vkAcquireNextImageKHR(ctx->m_device,ctx->m_swapchain.m_swapchain,UINT64_MAX,m_presentSemaphores[currentFrame],VK_NULL_HANDLE,&nextImage))

	vkResetCommandBuffer(m_mainCmdBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::commandBufferBeginInfo();

	VkClearValue clearValues[2];
	VkViewport viewport{};
	VkRect2D scissor{};
    //float flash = std::abs(std::sin(_frameNumber / 120.f));
    clearValues[0].color = { { 0.5f, 0.5f, 0.5f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f , 0};
    
	VK_CHECK(vkBeginCommandBuffer(m_mainCmdBuffers[currentFrame], &cmdBeginInfo));

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = VulkanContext::getInstance()->m_renderpass;
    renderPassInfo.framebuffer = VulkanContext::getInstance()->m_framebuffers[nextImage];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {WIN_WIDTH,WIN_HEIGHT};
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;
    vkCmdBeginRenderPass(m_mainCmdBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(m_mainCmdBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, blinn_phong.defaultPipeline.m_pipeline);
    
    vkCmdBindDescriptorSets(m_mainCmdBuffers[currentFrame],VK_PIPELINE_BIND_POINT_GRAPHICS,blinn_phong.defaultPipeline.m_pipelineLayout,0,1,&blinn_phong.m_descriptor.m_set,0,nullptr);
    VkDeviceSize offset = 0;

    Mesh* mesh = scene.m_scene["monkey"];
	vkCmdBindVertexBuffers(m_mainCmdBuffers[currentFrame], 0, 1, &mesh->_vertexBuffer.buffer, &offset);
    vkCmdDraw(m_mainCmdBuffers[currentFrame], mesh->_vertices.size(), 1, 0, 0);

    vkCmdEndRenderPass(m_mainCmdBuffers[currentFrame]);
    VK_CHECK(vkEndCommandBuffer(m_mainCmdBuffers[currentFrame]));

	VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &m_mainCmdBuffers[currentFrame];
    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &m_presentSemaphores[currentFrame];
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &m_renderSemaphores[currentFrame];
    VK_CHECK(vkQueueSubmit(ctx->m_graphicsQueue,1,&submit,m_fences[currentFrame]))

    VkPresentInfoKHR present{};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pImageIndices = &nextImage;
    present.swapchainCount = 1;
    present.pSwapchains = &ctx->m_swapchain.m_swapchain;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = &m_renderSemaphores[currentFrame];
    VK_CHECK(vkQueuePresentKHR(ctx->m_graphicsQueue,&present))
	m_frameNumber ++;

}

// void Engine::updateMesh()
// {
//     mesh.load_from_obj("../assets/monkey_smooth.obj");
//     size_t vertexBufferSize = mesh._vertices.size()*sizeof(Vertex);

//     struct StagingBuffer
//     {
//         /* data */
//         VkBuffer buffer;
//         VkDeviceMemory memory;
//     }stagingBuffer{};

// 	if(mesh._vertices.empty())
// 	{
// 		std::cout<<"empty";
// 	}
//     ctx->createBuffer(vertexBufferSize,
//     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//     stagingBuffer.buffer,
//     stagingBuffer.memory,
//     mesh._vertices.data());

//     ctx->createBuffer(
//         vertexBufferSize,
//         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//         mesh._vertexBuffer.buffer,
//         mesh._vertexBuffer.memory
//     );

//     //ctx->copyBuffer(stagingBuffer.buffer,mesh._vertexBuffer.buffer,vertexBufferSize);
//     VkCommandBuffer cmd{};
//     Command::allocateCmdBuffer(cmd,true);
//     VkBufferCopy bufferCopy{};
//     bufferCopy.size = vertexBufferSize;
//     bufferCopy.dstOffset = 0;
//     bufferCopy.srcOffset = 0;
//     vkCmdCopyBuffer(cmd,stagingBuffer.buffer,mesh._vertexBuffer.buffer,1,&bufferCopy);
//     Command::flushCmdBuffer(cmd);

//     vkFreeMemory(ctx->m_device,stagingBuffer.memory,nullptr);
//     vkDestroyBuffer(ctx->m_device,stagingBuffer.buffer,nullptr);

//     LOG_TRACE("Init Obj Success");

// }

// void Engine::initBuffer()
// {
//     m_descriptor.init();
//     m_descriptor.bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT,0);
//     m_descriptor.updateDescriptorLayout();

//     uint32_t size = sizeof(m_mat);
//     m_buffer.size = size;
//     m_buffer.create();
//     m_buffer.upload(m_descriptor.m_set,0);
// }

void Engine::updateBuffer()
{
    glm::vec3 camPos = { 0.f,0.f,7.f };
    m_cam.Position = camPos;

	glm::mat4 view = m_cam.GetViewMatrix();;
	//camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1280.f / 720.f, 0.1f, 200.0f);
	projection[1][1] *= -1;
    ubo.proj = projection;
    ubo.lightPos = glm::vec4(1.0);
    ubo.view = view;
    ubo.camPos = glm::vec4(camPos,1.0);

    m_mat = projection*view;

    blinn_phong.m_buffer.update(&ubo);
}