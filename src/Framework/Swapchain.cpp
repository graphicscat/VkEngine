#include<Framework/Swapchain.h>
#include<Framework/VulkanContext.h>


void Swapchain::init(const VulkanContext* ctx)
{
    m_context = ctx;

    querySwapchainSupport();
    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageColorSpace = m_details.format.colorSpace;
    swapchainInfo.imageExtent = m_details.imageExtent;
    swapchainInfo.imageFormat = m_details.format.format;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.minImageCount = m_details.imageCount;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.presentMode = m_details.present;
    swapchainInfo.preTransform = m_details.transform;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapchainInfo.surface = m_context->m_surface;
    VK_CHECK(vkCreateSwapchainKHR(m_context->m_device,&swapchainInfo,nullptr,&m_swapchain))

    LOG_TRACE("Init Swapchain Success");

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(m_context->m_device,m_swapchain,&imageCount,nullptr);
    m_swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_context->m_device,m_swapchain,&imageCount,m_swapchainImages.data());

    m_swapchainImageViews.resize(m_swapchainImages.size());

    for(int i = 0;i<m_swapchainImageViews.size();i++)
    {
        auto createInfo = vkinit::imageview_begin_info(m_swapchainImages[i],m_details.format.format,VK_IMAGE_ASPECT_COLOR_BIT);
        VK_CHECK(vkCreateImageView(m_context->m_device,&createInfo,nullptr,&m_swapchainImageViews[i]))
    }

    m_swapchainImageFormat = m_details.format.format;
}
void Swapchain::querySwapchainSupport()
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_context->m_physicalDevice,m_context->m_surface,&m_details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_context->m_physicalDevice,m_context->m_surface,&formatCount,nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_context->m_physicalDevice,m_context->m_surface,&formatCount,surfaceFormats.data());

    m_details.format = surfaceFormats[0];

    for(const auto& format:surfaceFormats)
    {
        if(format.format == VK_FORMAT_R8G8B8_SRGB&&
        format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){
            m_details.format = format;
            break;
        }
    }

    uint32_t imageCount = m_details.capabilities.minImageCount + 1;
    if (m_details.capabilities.maxImageCount > 0 && imageCount > m_details.capabilities.maxImageCount)
    {
        imageCount = m_details.capabilities.maxImageCount;
    }

    m_details.imageCount = imageCount;

    LOG_INFO("Swapchain Images Counts {0}",imageCount);
    m_details.imageExtent.width = WIN_WIDTH;
    m_details.imageExtent.height = WIN_HEIGHT;
    m_details.transform = m_details.capabilities.currentTransform;

    m_details.present = VK_PRESENT_MODE_FIFO_KHR;
}

void Swapchain::quit()
{
    for(int i = 0;i<m_swapchainImageViews.size();i++)
    {
        vkDestroyImageView(m_context->m_device,m_swapchainImageViews[i],nullptr);
    }
    vkDestroySwapchainKHR(m_context->m_device,m_swapchain,nullptr);
    m_context = nullptr;
   
}