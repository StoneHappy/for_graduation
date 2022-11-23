#pragma once

#include <QVulkanWindowRenderer>

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
	};
}
