#pragma once

#include <QVulkanWindowRenderer>
#include <Renderer/VulkanContext.h>
namespace GU
{
	class VulkanRenderer : public QVulkanWindowRenderer
	{
	public:
		VulkanRenderer(QVulkanWindow* w);

		void initResources() override;
		void initSwapChainResources() override;
		void releaseSwapChainResources() override;
		void releaseResources() override;

		void startNextFrame() override;

	private:
		QVulkanWindow* m_window;
		QVulkanDeviceFunctions* m_devFuncs;
		VkPipeline m_graphicsPipeline;
		VkBuffer m_buffer;
		VkDeviceMemory m_memory;
		VulkanContext m_vulkanContext;
	};
}
