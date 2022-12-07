#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
namespace GU
{
	template <class T>
	class VulkanUniformBuffer;
	struct CameraUBO;
	struct SkeletalModelUBO;
	class VulkanContext
	{
	public:
		VkPhysicalDevice						physicalDevice;
		VkDevice								logicalDevice;
		VkCommandPool							commandPool;
		VkQueue									graphicsQueue;
		VkExtent2D								swapChainExtent;
		VkRenderPass							renderPass;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStage;
		VkPipelineLayout						pipelineLayout;
		VkPipeline								graphicsPipeline;
		VkPipeline								backgroudPipeline;
		VkPipeline								boundingboxPipeline;
		VkDescriptorPool						descriptorPool;
		std::vector<VkDescriptorSet>			descriptorSets;

		std::vector<VkBuffer>					uniformBuffers;
		std::vector<VkDeviceMemory>				uniformBuffersMemory;
		std::vector<void*>						uniformBuffersMapped;

		std::vector<VkBuffer>					meshUniformBuffers;
		std::vector<VkDeviceMemory>				meshUniformBuffersMemory;
		std::vector<void*>						meshUniformBuffersMapped;

		VkDescriptorSetLayout					descriptorSetLayout;
		VkBuffer								vertexBuffer;
		VkDeviceMemory							vertexMemory;
		VkBuffer								indexBuffer;
		VkDeviceMemory							indexMemory;


		VkDescriptorSetLayout					rcDescriptorSetLayout;
		std::vector<VkDescriptorSet>			rcDescriptorSets;
		VkPipelineLayout						rcPipelineLayout;
		VkPipeline								rcPipeline;
		VkPipeline								rcContourPipeline;
		
		VkDescriptorSetLayout					skeletalDescriptorSetLayout;
		VkPipelineLayout						skeletalPipelineLayout;
		std::vector<VkDescriptorSet>			skeletalDescriptorSets;
		VkPipeline								skeletalPipeline;
		VkPipeline								skeletalContourPipeline;

		std::shared_ptr<VulkanUniformBuffer<CameraUBO> > camearUBO;
		std::shared_ptr<VulkanUniformBuffer<SkeletalModelUBO> > skeletalUBO;
		static int MAX_FRAMES_IN_FLIGHT;
	private:

	};
}