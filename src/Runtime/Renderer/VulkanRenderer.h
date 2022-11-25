#pragma once

#include <QVulkanWindowRenderer>
#include <Renderer/VulkanContext.h>
#include <Renderer/RenderData.h>
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
		VulkanContext m_vulkanContext;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};
}
