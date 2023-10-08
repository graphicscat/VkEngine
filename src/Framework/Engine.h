#include<Framework/Logger.h>
#include<Framework/VulkanContext.h>
#include<Framework/Pipeline.h>
#include<Framework/Mesh.h>
#include <Framework/Descriptor.h>
#include <Framework/Buffer.h>
#include <Pass/BPhongPass.h>
#include <Scene/Scene.h>

class Engine
{
private:
    /* data */
    VulkanContext* ctx;

    glm::mat4 m_mat;

    Buffer m_buffer;
    Descriptor m_descriptor;

    BPhongPass blinn_phong;
    Camera m_cam;
    UboRes ubo;
   // Mesh mesh;
    std::vector<VkCommandBuffer> m_mainCmdBuffers;

    std::vector<VkSemaphore> m_presentSemaphores;
    std::vector<VkSemaphore> m_renderSemaphores;

    std::vector<VkFence> m_fences;

    int m_frameNumber = 0;

    Scene scene;
    

public:
    Engine() = default;
    ~Engine() = default;
    GraphicsPipeline defaultPipeline;
    void init();
    void loop();
    void quit();
    void run();
    void draw();
    void initSync();
    // void initBuffer();
    //void updateMesh();
    void updateBuffer();
};

