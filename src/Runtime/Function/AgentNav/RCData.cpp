#include "RCData.h"
#include <stddef.h>
#include <Global/CoreContext.h>
#include <Renderer/VulkanContext.h>
#include <Renderer/VulkanBuffer.h>
namespace GU
{
	inline int bit(int a, int b)
	{
		return (a & (1 << b)) >> b;
	}
	inline unsigned int duRGBA(int r, int g, int b, int a)
	{
		return ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) | ((unsigned int)a << 24);
	}

	unsigned int duIntToCol(int i, int a)
	{
		int	r = bit(i, 1) + bit(i, 3) * 2 + 1;
		int	g = bit(i, 2) + bit(i, 4) * 2 + 1;
		int	b = bit(i, 0) + bit(i, 5) * 2 + 1;
		return duRGBA(r * 63, g * 63, b * 63, a);
	}

	unsigned int areaToCol(unsigned int area)
	{
		if (area == 0)
		{
			// Treat zero area type as default.
			return duRGBA(0, 192, 255, 255);
		}
		else
		{
			return duIntToCol(area, 255);
		}
	}


	void createVertexBuffer(VulkanContext& vkContext, const std::vector<RCVertex>& vertices, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// map memory to cpu host
		void* data;
		vkMapMemory(vkContext.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(vkContext.logicalDevice, stagingBufferMemory);

		createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

		copyBuffer(vkContext, stagingBuffer, buffer, bufferSize);

		vkDestroyBuffer(vkContext.logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(vkContext.logicalDevice, stagingBufferMemory, nullptr);
	}

	RCMesh::RCMesh(const rcPolyMesh& mesh)
	{
		const int nvp = mesh.nvp;
		const float cs = mesh.cs;
		const float ch = mesh.ch;
		const float* orig = mesh.bmin;

		for (int i = 0; i < mesh.npolys; ++i)
		{
			const unsigned short* p = &mesh.polys[i * nvp * 2];
			const unsigned char area = mesh.areas[i];

			unsigned int color;
			if (area == RC_WALKABLE_AREA)
				color = duRGBA(0, 192, 255, 64);
			else if (area == RC_NULL_AREA)
				color = duRGBA(0, 0, 0, 64);
			else
				color = areaToCol(area);

			unsigned short vi[3];
			for (int j = 2; j < nvp; ++j)
			{
				if (p[j] == RC_MESH_NULL_IDX) break;
				vi[0] = p[0];
				vi[1] = p[j - 1];
				vi[2] = p[j];
				for (int k = 0; k < 3; ++k)
				{
					const unsigned short* v = &mesh.verts[vi[k] * 3];
					const float x = orig[0] + v[0] * cs;
					const float y = orig[1] + (v[1] + 1) * ch;
					const float z = orig[2] + v[2] * cs;
					RCVertex vertex;
					vertex.pos = { x, y, z };
					glm::u8vec4 tmpcolor;
					memcpy(&tmpcolor, &color, 4 * sizeof(uint8_t));
					vertex.color.r = (float)tmpcolor.r;
					vertex.color.g = (float)tmpcolor.g;
					vertex.color.b = (float)tmpcolor.b;
					vertex.color.a = (float)tmpcolor.a;
					m_verts.emplace_back(std::move(vertex));
				}
			}
		}

		// generate render buuffer
		createVertexBuffer(*GLOBAL_VULKAN_CONTEXT, m_verts, vertexBuffer, vertexMemory);
	}
	VkVertexInputBindingDescription RCVertex::getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(RCVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	::std::array<VkVertexInputAttributeDescription, 2> RCVertex::getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(RCVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(RCVertex, color);

		return attributeDescriptions;
	}
}