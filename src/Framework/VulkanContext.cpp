#include<Framework/VulkanContext.h>

#include <SDL2/SDL_vulkan.h>
#include <macros.h>
#include<vector>
VulkanContext* VulkanContext::context = nullptr;

const std::vector<const char*> validationLayers = {
  "VK_LAYER_KHRONOS_validation" 
  
};

VulkanContext* VulkanContext::getInstance()
{
    return context;
}

void VulkanContext::init()
{

    //m_swapchain.init(this);
    context = new VulkanContext();
}

void VulkanContext::initSwapchain()
{
    m_swapchain.init(this);
    initRenderPass();
    initDepthMap();
    initFramebuffer();
    
}

VulkanContext::VulkanContext()
{
    initWindow();
    initInstance();
  
    initCommandPool();
}

void VulkanContext::initWindow()
{
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0)
	LOG_INFO("INIT JOYSTICK FAILED");
	//SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_WindowFlags windows_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	
    window = SDL_CreateWindow(
        "Vulkan Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH,
        WIN_HEIGHT,
        windows_flags
    );
	SDL_Joystick* gGameController = NULL;
	if( SDL_NumJoysticks() < 1 )
	{ 
    	LOG_INFO("No joysticks connected!");
	}
	else
	{
		gGameController = SDL_JoystickOpen( 0 );
		if( gGameController == NULL )
		{ 
			LOG_WARN(SDL_GetError());
		}
	}

    LOG_TRACE("Init Window Success");
}

void VulkanContext::initInstance()
{
    uint32_t extCount;
    SDL_Vulkan_GetInstanceExtensions(window,&extCount,nullptr);
    std::vector<const char *> extensions(extCount);
    SDL_Vulkan_GetInstanceExtensions(window,&extCount,extensions.data());

    VkInstanceCreateInfo instanceInfo{};
    VkApplicationInfo appInfo{};
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pEngineName = "Vulkan Engine";
    appInfo.pApplicationName = "Vulkan App";

    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = uint32_t(validationLayers.size());
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
    instanceInfo.enabledExtensionCount = uint32_t(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    VK_CHECK(vkCreateInstance(&instanceInfo,nullptr,&m_instance))
    LOG_TRACE("Init Instance Success");

    SDL_Vulkan_CreateSurface(window,m_instance,&m_surface);
    //init physicalDevice
    LOG_TRACE("Init Surface Success");
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance,&physicalDeviceCount,nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_instance,&physicalDeviceCount,physicalDevices.data());

    m_physicalDevice = physicalDevices[0];

    VkPhysicalDeviceProperties physicalDevicePops{};
    vkGetPhysicalDeviceProperties(m_physicalDevice,&physicalDevicePops);

    LOG_TRACE("Init PhysicalDevice Success");
	uint32_t count = 0;
	vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &count, nullptr);
	std::vector<VkExtensionProperties> RTextensions(count);
	vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &count, RTextensions.data());

	// for(int i = 0 ;i<RTextensions.size();i++)
	// {
	// 	if(strcmp(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, RTextensions[i].extensionName) == 0)
	// 	std::cout<<"raytracing ext is supported"<<std::endl;
    //     std::cout<<RTextensions[i].extensionName<< std::endl;
	// }

    findQueueIndex();

    VkDeviceQueueCreateInfo queueInfo{};
    float queuePriority = 1.0f;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;
    queueInfo.queueFamilyIndex = m_graphicsQueueFamily;

    std::vector<const char*> arr = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = arr.data();

    VK_CHECK(vkCreateDevice(m_physicalDevice,&deviceInfo,nullptr,&m_device))
    vkGetDeviceQueue(m_device,m_graphicsQueueFamily,0,&m_graphicsQueue);
    LOG_TRACE("Init Device Success");

}

void VulkanContext::findQueueIndex()
{
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice,&count,nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice,&count,queueFamilyProperties.data());

    for(int i = 0 ;i<queueFamilyProperties.size();i++)
    {
        const auto& prop = queueFamilyProperties[i];
        if(prop.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_graphicsQueueFamily = i;
        }
    }
}

void VulkanContext::initCommandPool()
{
    VkCommandPoolCreateInfo info{};
    info.queueFamilyIndex = m_graphicsQueueFamily;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(m_device,&info,nullptr,&m_commandPool))
    LOG_TRACE("Init CommandPool Success");
}

void VulkanContext::testRender()
{
    

}

VulkanContext::~VulkanContext()
{
    vkDeviceWaitIdle(m_device);
    vkDestroyRenderPass(m_device,m_renderpass,nullptr);
    vkDestroyImage(m_device,m_depthMap.image,nullptr);
    vkDestroyImageView(m_device,m_depthMap.view,nullptr);
    vkFreeMemory(m_device,m_depthMap.mem,nullptr);

    for(int i = 0 ;i<m_framebuffers.size();i++)
    {
        vkDestroyFramebuffer(m_device,m_framebuffers[i],nullptr);
    }
    
    m_swapchain.quit();
    
    vkDestroyCommandPool(m_device,m_commandPool,nullptr);
    vkDestroySurfaceKHR(m_instance,m_surface,nullptr);
    vkDestroyDevice(m_device,nullptr);
    vkDestroyInstance(m_instance,nullptr);
}

void VulkanContext::quit()
{
    delete context;
}

void VulkanContext::createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& memory,
		void* data 
	)
    {
         VkBufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = size;
        createInfo.usage = usage;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VK_CHECK(vkCreateBuffer(m_device,&createInfo,nullptr,&buffer))

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        VkMemoryRequirements memRequirements{};
        vkGetBufferMemoryRequirements(m_device,buffer,&memRequirements);
        auto typeFilter = memRequirements.memoryTypeBits;
        allocInfo.memoryTypeIndex = findMemoryType(typeFilter,properties);
        allocInfo.allocationSize = memRequirements.size;

        VK_CHECK(vkAllocateMemory(m_device,&allocInfo,nullptr,&memory))
        VK_CHECK(vkBindBufferMemory(m_device,buffer,memory,0))
        void* mapped = nullptr;

        if(data != nullptr)
        {
            vkMapMemory(m_device,memory,0,size,0,&mapped);
            memcpy(mapped,data,size);
            vkUnmapMemory(m_device,memory);
        }
    }

int VulkanContext::findMemoryType(int typeFilter,VkMemoryPropertyFlags properties)
{
        VkPhysicalDeviceMemoryProperties memProperties{};
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice,&memProperties);

        for(uint32_t i = 0; i < memProperties.memoryTypeCount;i++)
        {
            if(typeFilter & ( 1 << i ) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i ;
        }

        return 0;
}

void VulkanContext::createImage(
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory
) 
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    VK_CHECK(vkAllocateMemory(m_device,&allocInfo,nullptr,&imageMemory))
    VK_CHECK(vkBindImageMemory(m_device,image,imageMemory,0))
}

VkImageView VulkanContext::createImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspect)
{
    VkImageViewCreateInfo createInfo{};
    VkImageSubresourceRange range{};
    VkImageView  imageView{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.format = format;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    range.aspectMask = aspect;
    range.baseArrayLayer = 0;
    range.baseMipLevel = 0;
    range.layerCount = 1;
    range.levelCount = 1;
    createInfo.subresourceRange = range;
    VK_CHECK(vkCreateImageView(m_device,&createInfo,nullptr,&imageView))
    return imageView;
}

void VulkanContext::initRenderPass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = m_swapchain.m_swapchainImageFormat;
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

    VK_CHECK(vkCreateRenderPass(m_device,&renderpassInfo,nullptr,&m_renderpass))
    LOG_TRACE("Init RenderPass Success");
}

void VulkanContext::initFramebuffer()
{
    VkFramebufferCreateInfo fb_info = vkinit::framebufferCreateInfo(m_renderpass,{WIN_WIDTH,WIN_HEIGHT});

    const uint32_t swapchain_imageCount = uint32_t(m_swapchain.m_swapchainImageViews.size());
    m_framebuffers.resize(swapchain_imageCount);

    for( uint32_t i = 0;i<swapchain_imageCount;i++)
    {
        std::vector<VkImageView> attachments = {m_swapchain.m_swapchainImageViews[i],m_depthMap.view};
        fb_info.pAttachments = attachments.data();
        fb_info.attachmentCount = uint32_t(attachments.size());
        VK_CHECK(vkCreateFramebuffer(m_device,&fb_info,nullptr,&m_framebuffers[i]))
    }
    LOG_TRACE("Init Framebuffers Success");
}

void VulkanContext::initDepthMap()
{
    VkFormat format= VK_FORMAT_D32_SFLOAT;
    m_depthMap.format = format;
    createImage(
        WIN_WIDTH,
        WIN_HEIGHT,
        format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_depthMap.image,
        m_depthMap.mem
    );
    m_depthMap.view = createImageView(m_depthMap.image, format, VK_IMAGE_ASPECT_DEPTH_BIT);
    LOG_TRACE("Init DepthMap Success");
}