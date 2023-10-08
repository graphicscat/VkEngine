#pragma once
#include<vulkan/vulkan.h>
#include<SDL2/SDL.h>
#include<deque>
#include<functional>
#include<Framework/Swapchain.h>
struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& func){
		deletors.push_back(func);
	}

	void flush(){
		for(auto it = deletors.rbegin();it!=deletors.rend();it++){
			(*it)();
		}

		deletors.clear();
	}
};


class VulkanContext final
{
    public:

    static VulkanContext* getInstance();

    static void init();
    
    static void quit();

    void initSwapchain();

    void createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& memory,
		void* data = nullptr
	);

    void createImage(
		uint32_t width,
		uint32_t height,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImage& image,
		VkDeviceMemory& imageMemory
	);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspect);

    int VulkanContext::findMemoryType(int typeFilter,VkMemoryPropertyFlags properties);

    VkInstance m_instance;

    VkSurfaceKHR m_surface;

    VkDevice m_device;

    VkPhysicalDevice m_physicalDevice;

    SDL_Window* window;

    VkQueue m_graphicsQueue;

	uint32_t m_graphicsQueueFamily;

    Swapchain m_swapchain;
    
    VkCommandPool m_commandPool;

    VkRenderPass m_renderpass;

    std::vector<VkFramebuffer> m_framebuffers;

	struct {
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
		VkFormat format;
	} m_depthMap;

    private:

    DeletionQueue m_deletionQueue;

    static VulkanContext* context;

    VulkanContext();

    ~VulkanContext();

    void initInstance();

    void initWindow();

    void findQueueIndex();

    void initCommandPool();

    void testRender();

    void initRenderPass();

	void initFramebuffer();

    void initDepthMap();

};