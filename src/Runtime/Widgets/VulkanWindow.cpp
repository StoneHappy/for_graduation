#include "VulkanWindow.h"
#include <Renderer/VulkanRenderer.h>
#include <QMouseEvent>
#include <QWheelEvent>
#include <Global/CoreContext.h>
namespace GU
{
	QVulkanWindowRenderer* VulkanWindow::createRenderer()
	{
        m_renderer = new VulkanRenderer(this);
		return m_renderer;
	}

    void VulkanWindow::mousePressEvent(QMouseEvent* e)
    {
        m_pressed = true;
        m_lastPos = e->pos();
    }

    void VulkanWindow::mouseReleaseEvent(QMouseEvent*)
    {
        m_pressed = false;
    }

    void VulkanWindow::mouseMoveEvent(QMouseEvent* e)
    {
        if (!m_pressed)
            return;

        int dx = e->pos().x() - m_lastPos.x();
        int dy = e->pos().y() - m_lastPos.y();
        bool shift = e->modifiers().testFlag(Qt::ShiftModifier);
        if (shift)
        {
            m_renderer->getCamera().mousePan({ dx * g_CoreContext.g_editDeltaTime, dy * g_CoreContext.g_editDeltaTime });
        }
        else
        {
            m_renderer->getCamera().mouseRotate({ dx * g_CoreContext.g_editDeltaTime, dy * g_CoreContext.g_editDeltaTime });
        }
        m_lastPos = e->pos();
    }

    void VulkanWindow::keyPressEvent(QKeyEvent* e)
    {

    }

    void VulkanWindow::wheelEvent(QWheelEvent* event)
    {
        m_renderer->getCamera().mouseZoom(event->delta() * g_CoreContext.g_editDeltaTime);
    }
}