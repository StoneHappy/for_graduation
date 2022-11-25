#include "VulkanRenderer.h"
#include <QVulkanDeviceFunctions>
#include <Renderer/VulkanShader.h>
#include "shader_texture_frag.h"
#include "shader_texture_vert.h"

#include <Renderer/VulkanGraphicsPipeline.h>
#include <Renderer/VulkanBuffer.h>
#include <Renderer/VulkanImage.h>
namespace GU
{
	VulkanRenderer::VulkanRenderer(QVulkanWindow* w)
		:m_window(w)
	{
	}

	void VulkanRenderer::initResources()
	{
		qDebug("initResources");

		m_devFuncs = m_window->vulkanInstance()->deviceFunctions(m_window->device());
		m_vulkanContext.physicalDevice = m_window->physicalDevice();
		m_vulkanContext.logicalDevice = m_window->device();
		m_vulkanContext.commandPool = m_window->graphicsCommandPool();
		m_vulkanContext.graphicsQueue = m_window->graphicsQueue();
		m_vulkanContext.renderPass = m_window->defaultRenderPass();

		VkShaderModule vertexShader = createShader(m_window->device(), shader_texture_vert, sizeof(shader_texture_vert));
		VkShaderModule fragShader = createShader(m_window->device(), shader_texture_frag, sizeof(shader_texture_frag));
		createShaderStageInfo(vertexShader, fragShader, m_vulkanContext.shaderStage);
		createDescriptorSetLayout(m_vulkanContext, m_vulkanContext.descriptorSetLayout);
		createPipelineLayout(m_vulkanContext, m_vulkanContext.descriptorSetLayout, m_vulkanContext.pipelineLayout);
		createGraphicsPipeline(m_vulkanContext, m_vulkanContext.shaderStage, m_vulkanContext.pipelineLayout, m_vulkanContext.graphicsPipeline);
		loadModel("assets/models/viking_room.obj", vertices, indices);
		VulkanImage vkImage;
		createTextureImage(m_vulkanContext, "./assets/models/viking_room.png", vkImage);
		createTextureImageView(m_vulkanContext, vkImage);
		createTextureSampler(m_vulkanContext, vkImage);
		createVertexBuffer(m_vulkanContext, vertices, m_vulkanContext.vertexBuffer, m_vulkanContext.vertexMemory);
		createIndexBuffer(m_vulkanContext, indices, m_vulkanContext.indexBuffer, m_vulkanContext.indexMemory);
		createUniformBuffers(m_vulkanContext, m_vulkanContext.uniformBuffers, m_vulkanContext.uniformBuffersMemory, m_vulkanContext.uniformBuffersMapped);
		createDescriptorPool(m_vulkanContext, m_vulkanContext.descriptorPool);
		createDescriptorSets(m_vulkanContext, vkImage, m_vulkanContext.descriptorSetLayout, m_vulkanContext.descriptorPool, m_vulkanContext.descriptorSets);
		createBackgroundPipeline(m_vulkanContext, m_vulkanContext.backgroudPipeline);
	}

	void VulkanRenderer::initSwapChainResources()
	{
		qDebug("initSwapChainResources");
	}

	void VulkanRenderer::releaseSwapChainResources()
	{
		qDebug("releaseSwapChainResources");
	}

	void VulkanRenderer::releaseResources()
	{
		qDebug("releaseResources");
	}

	void VulkanRenderer::startNextFrame()
	{
		g_CoreContext.timeTick();
		m_vulkanContext.swapChainExtent = { (unsigned int)m_window->swapChainImageSize().width(),(unsigned int)m_window->swapChainImageSize().height() };
		updateUniformBuffer(m_vulkanContext, m_window->currentSwapChainImageIndex(), m_vulkanContext.uniformBuffersMapped);
		const QSize sz = m_window->swapChainImageSize();
		VkClearColorValue clearColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		VkClearDepthStencilValue clearDS = { 1.0f, 0.0f };
		VkClearValue clearValues[2];
		memset(clearValues, 0, sizeof(clearValues));
		clearValues[0].color = clearColor;
		clearValues[1].depthStencil = clearDS;

		VkRenderPassBeginInfo rpBeginInfo;
		memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
		rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpBeginInfo.renderPass = m_window->defaultRenderPass();
		rpBeginInfo.framebuffer = m_window->currentFramebuffer();
		rpBeginInfo.renderArea.extent.width = sz.width();
		rpBeginInfo.renderArea.extent.height = sz.height();
		rpBeginInfo.clearValueCount = 2;
		rpBeginInfo.pClearValues = clearValues;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)sz.width();
		viewport.height = (float)sz.height();
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = viewport.width;
		scissor.extent.height = viewport.height;


		VkCommandBuffer cmdBuf = m_window->currentCommandBuffer();

		

		m_devFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_devFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vulkanContext.backgroudPipeline);
		m_devFuncs->vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
		m_devFuncs->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);
		m_devFuncs->vkCmdDraw(cmdBuf, 6, 1, 0, 0);

		m_devFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vulkanContext.graphicsPipeline);
		/*m_devFuncs->vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
		m_devFuncs->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);*/
		VkBuffer vertexBuffers[] = { m_vulkanContext.vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		m_devFuncs->vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
		m_devFuncs->vkCmdBindIndexBuffer(cmdBuf, m_vulkanContext.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		m_devFuncs->vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vulkanContext.pipelineLayout, 0, 1, &m_vulkanContext.descriptorSets[m_window->currentSwapChainImageIndex()], 0, nullptr);
		m_devFuncs->vkCmdDrawIndexed(cmdBuf, indices.size(), 1, 0, 0, 0);
		m_devFuncs->vkCmdEndRenderPass(cmdBuf);
		m_window->frameReady();
		m_window->requestUpdate();
	}
}