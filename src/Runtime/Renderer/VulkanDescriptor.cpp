#include "VulkanDescriptor.h"
#include <Global/CoreContext.h>
#include <Renderer/VulkanUniformBuffer.hpp>
namespace GU
{
    void createDescriptorSetLayout(VulkanContext& vulkanContext, VkDescriptorSetLayout& descriptorSetLayout)
    {
        DEBUG_LOG("正在创建descriptorSetLayout");
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding meshuboLayoutBinding{};
        meshuboLayoutBinding.binding = 1; /* bingding point must different with each other */
        meshuboLayoutBinding.descriptorCount = 1;
        meshuboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        meshuboLayoutBinding.pImmutableSamplers = nullptr;
        meshuboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 2;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 3> bindings = { uboLayoutBinding, meshuboLayoutBinding, samplerLayoutBinding };
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(vulkanContext.logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            FATAL_LOG("failed to create descriptor set layout!");
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createDescriptorPool(VulkanContext& vkContext, VkDescriptorPool& descriptorPool)
    {
        DEBUG_LOG("正在创建DescriptorPool...");
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(VulkanContext::MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(VulkanContext::MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1000;

        if (vkCreateDescriptorPool(vkContext.logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            FATAL_LOG("failed to create descriptor pool!");
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createDescriptorSets(VulkanContext& vkContext, VulkanImage& vulkanImage, std::vector<VkBuffer>& modelUBOs, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool, std::vector<VkDescriptorSet>& descriptorSets)
    {
        std::vector<VkDescriptorSetLayout> layouts(VulkanContext::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanContext::MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(vkContext.logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < VulkanContext::MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = vkContext.camearUBO->uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(CameraUBO);

            VkDescriptorBufferInfo meshbufferInfo{};
            meshbufferInfo.buffer = modelUBOs[i];
            meshbufferInfo.offset = 0;
            meshbufferInfo.range = sizeof(ModelUBO);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = vulkanImage.view;
            imageInfo.sampler = vulkanImage.sampler;

            std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &meshbufferInfo;

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = descriptorSets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(vkContext.logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
}