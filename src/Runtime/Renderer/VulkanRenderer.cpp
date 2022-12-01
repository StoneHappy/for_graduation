#include "VulkanRenderer.h"
#include <QVulkanDeviceFunctions>
#include <Renderer/VulkanShader.h>
#include "shader_texture_frag.h"
#include "shader_texture_vert.h"

#include <Renderer/VulkanGraphicsPipeline.h>
#include <Renderer/VulkanBuffer.h>
#include <Renderer/VulkanImage.h>
#include <Renderer/Mesh.h>
#include <Scene/Entity.h>
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
		GLOBAL_VULKANCONTEXT.physicalDevice = m_window->physicalDevice();
		GLOBAL_VULKANCONTEXT.logicalDevice = m_window->device();
		GLOBAL_VULKANCONTEXT.commandPool = m_window->graphicsCommandPool();
		GLOBAL_VULKANCONTEXT.graphicsQueue = m_window->graphicsQueue();
		GLOBAL_VULKANCONTEXT.renderPass = m_window->defaultRenderPass();
		

		VkShaderModule vertexShader = createShader(m_window->device(), shader_texture_vert, sizeof(shader_texture_vert));
		VkShaderModule fragShader = createShader(m_window->device(), shader_texture_frag, sizeof(shader_texture_frag));
		createShaderStageInfo(vertexShader, fragShader, GLOBAL_VULKANCONTEXT.shaderStage);
		createDescriptorSetLayout(GLOBAL_VULKANCONTEXT, GLOBAL_VULKANCONTEXT.descriptorSetLayout);
		createPipelineLayout(GLOBAL_VULKANCONTEXT, GLOBAL_VULKANCONTEXT.descriptorSetLayout, GLOBAL_VULKANCONTEXT.pipelineLayout);
		createGraphicsPipeline(GLOBAL_VULKANCONTEXT, GLOBAL_VULKANCONTEXT.shaderStage, GLOBAL_VULKANCONTEXT.pipelineLayout, GLOBAL_VULKANCONTEXT.graphicsPipeline);
		//loadModel("assets/models/viking_room.obj", vertices, indices);
		VulkanImage vkImage;
		createTextureImage(GLOBAL_VULKANCONTEXT, "./assets/models/viking_room.png", vkImage);
		createTextureImageView(GLOBAL_VULKANCONTEXT, vkImage);
		createTextureSampler(GLOBAL_VULKANCONTEXT, vkImage);
		createUniformBuffers(GLOBAL_VULKANCONTEXT, GLOBAL_VULKANCONTEXT.uniformBuffers, GLOBAL_VULKANCONTEXT.uniformBuffersMemory, GLOBAL_VULKANCONTEXT.uniformBuffersMapped);
		createMeshUniformBuffers(GLOBAL_VULKANCONTEXT, GLOBAL_VULKANCONTEXT.meshUniformBuffers, GLOBAL_VULKANCONTEXT.meshUniformBuffersMemory, GLOBAL_VULKANCONTEXT.meshUniformBuffersMapped);
		createDescriptorPool(GLOBAL_VULKANCONTEXT, GLOBAL_VULKANCONTEXT.descriptorPool);
		createDescriptorSets(GLOBAL_VULKANCONTEXT, vkImage, GLOBAL_VULKANCONTEXT.descriptorSetLayout, GLOBAL_VULKANCONTEXT.descriptorPool, GLOBAL_VULKANCONTEXT.descriptorSets);
		createBackgroundPipeline(GLOBAL_VULKANCONTEXT, GLOBAL_VULKANCONTEXT.backgroudPipeline);
		createBoundingBoxPipeline(GLOBAL_VULKANCONTEXT, GLOBAL_VULKANCONTEXT.boundingboxPipeline);

		GLOBAL_VULKANCONTEXT = GLOBAL_VULKANCONTEXT;
		Entity entity = GLOBAL_SCENE.createEntity();
		auto& meshcomponent = entity.addComponent<MeshComponent>();
		meshcomponent.meshID = GLOBAL_ASSET.insertMesh("./assets/models/viking_room.obj");
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
		GLOBAL_TIMETICK();
		g_CoreContext.g_winWidth = m_window->swapChainImageSize().width();
		g_CoreContext.g_winHeight = m_window->swapChainImageSize().height();
		m_Camera.updateView();
		m_Camera.updateProjection();
		GLOBAL_VULKANCONTEXT.swapChainExtent = { (unsigned int)m_window->swapChainImageSize().width(),(unsigned int)m_window->swapChainImageSize().height() };
		updateUniformBuffer(GLOBAL_VULKANCONTEXT, m_Camera, m_window->currentSwapChainImageIndex(), GLOBAL_VULKANCONTEXT.uniformBuffersMapped);
		//updateMeshUniformBuffer(GLOBAL_VULKANCONTEXT, glm::mat4(1), m_window->currentSwapChainImageIndex(), GLOBAL_VULKANCONTEXT.meshUniformBuffersMapped);
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

		// dynamic state
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
		// prepare cammondbuffer
		m_devFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		m_devFuncs->vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
		m_devFuncs->vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

		// BackgroundColor
		m_devFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKANCONTEXT.backgroudPipeline);
		m_devFuncs->vkCmdDraw(cmdBuf, 6, 1, 0, 0);

		// Wireframe
		m_devFuncs->vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKANCONTEXT.boundingboxPipeline);
		m_devFuncs->vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKANCONTEXT.pipelineLayout, 0, 1, &GLOBAL_VULKANCONTEXT.descriptorSets[m_window->currentSwapChainImageIndex()], 0, nullptr);
		m_devFuncs->vkCmdDraw(cmdBuf, 24, 1, 0, 0);

		// 3D model
		GLOBAL_SCENE.renderTick(GLOBAL_VULKANCONTEXT, cmdBuf, m_window->currentSwapChainImageIndex(), GLOBAL_DELTATIME);

		// submit queue
		m_devFuncs->vkCmdEndRenderPass(cmdBuf);
		m_window->frameReady();
		m_window->requestUpdate();
	}
}