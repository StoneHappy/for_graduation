#include "RenderData.h"
#include <tiny_obj_loader.h>
namespace GU
{
	VkVertexInputBindingDescription Vertex::getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
    VkVertexInputBindingDescription SkeletalVertex::getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(SkeletalVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
    ::std::array<VkVertexInputAttributeDescription, 5> SkeletalVertex::getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(SkeletalVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(SkeletalVertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(SkeletalVertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SINT;
        attributeDescriptions[3].offset = offsetof(SkeletalVertex, boneIDs);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(SkeletalVertex, weights);

        return attributeDescriptions;
    }
    SkeletalModelUBO::SkeletalModelUBO()
    {
        model = glm::mat4(1);
        for (size_t i = 0; i < MAX_BONES; i++)
        {
            bones[i] = glm::mat4(1);
        }
    }
    AgentModelUBO::AgentModelUBO()
    {
        model = glm::mat4(1);
        for (size_t i = 0; i < MAX_BONES; i++)
        {
            bones[i] = glm::mat4(1);
        }
    }
    AgentDensityUBO::AgentDensityUBO()
    {
        for (size_t i = 0; i < 128; i++)
        {
            pos[i] = { 25.2513, -2.37028, 23.9598 };
        }
    }
}