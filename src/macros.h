#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#include<iostream>
#include<Framework/Logger.h>
#include<vector>
#include<vulkan/vulkan.h>
#include <fstream>
#include<string>
#include<Framework/vkinit.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>



#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0);

