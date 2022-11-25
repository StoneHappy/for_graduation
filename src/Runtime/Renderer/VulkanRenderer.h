#pragma once

#include <QVulkanWindowRenderer>
#include <Renderer/VulkanContext.h>
#include <Renderer/RenderData.h>
#include <Renderer/EditCamera.h>
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
		EditCamera& getCamera() { return m_Camera; }
	private:
		QVulkanWindow* m_window;
		QVulkanDeviceFunctions* m_devFuncs;
		VulkanContext m_vulkanContext;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		EditCamera m_Camera;
	};
}
