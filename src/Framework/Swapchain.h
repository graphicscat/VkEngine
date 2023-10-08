#pragma once

#include <vulkan/vulkan.h>
#include <macros.h>

class VulkanContext;
class Swapchain
{
public:
    const VulkanContext* m_context;

    // Swapchain images.
    std::vector<VkImage> m_swapchainImages = {};

    // Swapchain image views.
    std::vector<VkImageView> m_swapchainImageViews = {};

    // Swapchain format.
    VkFormat m_swapchainImageFormat = {};

    // Extent of swapchain.
    VkExtent2D m_swapchainExtent = {};

    // Swapchain handle.
    VkSwapchainKHR m_swapchain = {};

    // Current swapchain using surface format.
    VkSurfaceFormatKHR m_surfaceFormat = {};

    // Current swapchain present mode.
    VkPresentModeKHR m_presentMode = {};

    struct SwapchainDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR format;
		VkPresentModeKHR present;
		VkSurfaceTransformFlagBitsKHR transform;
		VkExtent2D imageExtent;
		uint32_t imageCount;
	}m_details;


    void init(const VulkanContext* ) ;
    void quit();
    private:
    
    void querySwapchainSupport();

};