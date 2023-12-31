#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <macros.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
struct VertexInputDescription {
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};


struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
	static VertexInputDescription get_vertex_description();
}; 

struct VertexBuffer
{
    /* data */
    VkBuffer buffer;
    VkDeviceMemory memory;
};


struct Mesh {
	std::vector<Vertex> _vertices;

	VertexBuffer _vertexBuffer;
	glm::vec3 objectColor = glm::vec3(0.0f);
	bool load_from_obj(const char* filename);

	void clean();
};