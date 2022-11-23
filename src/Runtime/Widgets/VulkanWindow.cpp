#include "VulkanWindow.h"
#include <Renderer/VulkanRenderer.h>
namespace GU
{
	QVulkanWindowRenderer* VulkanWindow::createRenderer()
	{
		return new VulkanRenderer(this);
	}
}