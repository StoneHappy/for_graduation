#include "VulkanWindow.h"
#include <Renderer/VulkanRenderer.h>
namespace CS
{
	QVulkanWindowRenderer* VulkanWindow::createRenderer()
	{
		return new VulkanRenderer(this);
	}
}