#include<Scene/Scene.h>
#include<Framework/Command.h>

void Scene::add(const std::string& filename, const std::string& meshname)
{
    Mesh* mesh = new Mesh();
    mesh->load_from_obj(filename.c_str());

    updateMesh(mesh);

    m_scene[meshname] = mesh;
}

void Scene::init()
{
    add("../assets/monkey_smooth.obj","monkey");
}

void Scene::updateMesh(Mesh* mesh)
{
    size_t vertexBufferSize = mesh->_vertices.size()*sizeof(Vertex);

    struct StagingBuffer
    {
        /* data */
        VkBuffer buffer;
        VkDeviceMemory memory;
    }stagingBuffer{};

	if(mesh->_vertices.empty())
	{
		std::cout<<"empty";
	}
    VulkanContext::getInstance()->createBuffer(vertexBufferSize,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    stagingBuffer.buffer,
    stagingBuffer.memory,
    mesh->_vertices.data());

    VulkanContext::getInstance()->createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mesh->_vertexBuffer.buffer,
        mesh->_vertexBuffer.memory
    );

    //ctx->copyBuffer(stagingBuffer.buffer,mesh._vertexBuffer.buffer,vertexBufferSize);
    VkCommandBuffer cmd{};
    Command::allocateCmdBuffer(cmd,true);
    VkBufferCopy bufferCopy{};
    bufferCopy.size = vertexBufferSize;
    bufferCopy.dstOffset = 0;
    bufferCopy.srcOffset = 0;
    vkCmdCopyBuffer(cmd,stagingBuffer.buffer,mesh->_vertexBuffer.buffer,1,&bufferCopy);
    Command::flushCmdBuffer(cmd);

    vkFreeMemory(VulkanContext::getInstance()->m_device,stagingBuffer.memory,nullptr);
    vkDestroyBuffer(VulkanContext::getInstance()->m_device,stagingBuffer.buffer,nullptr);
}

void Scene::quit()
{
    for(auto s:m_scene)
    {
        s.second->clean();
        delete s.second;
    }
}