#include "VulkanRenderer.h"
#include <QVulkanDeviceFunctions>
#include <Renderer/VulkanShader.h>
#include "j_shader_modules_vert.h"
#include "j_shader_modules_frag.h"
#include "r_descriptor_layout_buffer_vert.h"
#include "r_descriptor_layout_buffer_frag.h"
#include <Renderer/VulkanGraphicsPipeline.h>
#include <Renderer/VulkanBuffer.h>
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

		VkShaderModule vertexShader = createShader(m_window->device(), r_descriptor_layout_buffer_vert, sizeof(r_descriptor_layout_buffer_vert));
		VkShaderModule fragShader = createShader(m_window->device(), r_descriptor_layout_buffer_frag, sizeof(r_descriptor_layout_buffer_frag));
		createShaderStageInfo(vertexShader, fragShader, m_vulkanContext.shaderStage);
		createDescriptorSetLayout(m_vulkanContext, m_vulkanContext.descriptorSetLayouts);
		createPipelineLayout(m_vulkanContext, m_vulkanContext.descriptorSetLayouts, m_vulkanContext.pipelineLayout);
		createGraphicsPipeline(m_vulkanContext, m_vulkanContext.shaderStage, m_vulkanContext.pipelineLayout, m_vulkanContext.graphicsPipeline);
		std::vector<Vertex> vertices = {
				{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
				{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
				{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};
		std::vector<uint16_t> indices = {
				0, 1, 2, 2, 3, 0
		};
		
		createVertexBuffer(m_vulkanContext, vertices, m_vulkanContext.vertexBuffer, m_vulkanContext.vertexMemory);
		createIndexBuffer(m_vulkanContext, indices, m_vulkanContext.indexBuffer, m_vulkanContext.indexMemory);
		createUniformBuffers(m_vulkanContext, m_vulkanContext.uniformBuffers, m_vulkanContext.uniformBuffersMemory, m_vulkanContext.uniformBuffersMapped);
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
		VkCommandBuffer cmdBuf = m_window->currentCommandBuffer();
		m_devFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			m_devFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vulkanContext.graphicsPipeline);
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)sz.width();
			viewport.height = (float)sz.height();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			m_devFuncs->vkCmdSetViewport(cmdBuf, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			m_devFuncs->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

			VkBuffer vertexBuffers[] = { m_vulkanContext.vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			m_devFuncs->vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
			m_devFuncs->vkCmdBindIndexBuffer(cmdBuf, m_vulkanContext.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
			//m_devFuncs->vkCmdDraw(cmdBuf, 3, 1, 0, 0);
			m_devFuncs->vkCmdDrawIndexed(cmdBuf, 6, 1, 0, 0, 0);
		m_devFuncs->vkCmdEndRenderPass(cmdBuf);

		m_window->frameReady();
		m_window->requestUpdate();
	}
}