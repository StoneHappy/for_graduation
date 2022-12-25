#include "VulkanWindow.h"
#include <Renderer/VulkanRenderer.h>
#include <QMouseEvent>
#include <QWheelEvent>
#include <Global/CoreContext.h>
#include <Function/AgentNav/RCScheduler.h>
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
        float x = m_lastPos.x();
        float y = m_lastPos.y();
        auto p = m_renderer->getCamera().getProjectionMatrix();
        auto v = m_renderer->getCamera().getViewMatrix();
        p[1][1] *= -1;
        auto inversePVMat = glm::inverse(p * v);
        glm::vec4 viewport = { 0, 0, GLOBAL_VULKAN_CONTEXT->swapChainExtent.width,GLOBAL_VULKAN_CONTEXT->swapChainExtent.height };
        auto worldPosStart = glm::unProjectNO({ x, y, 0.0f }, v, p, viewport);
        auto worldPosEnd = glm::unProjectNO({ x, y, 1.0f },v, p, viewport);
        float min = 0.0;
        float rayStart[3] = { worldPosStart.x, worldPosStart.y, worldPosStart.z };
        float rayEnd[3] = { worldPosEnd.x, worldPosEnd.y, worldPosEnd.z };
        if (GLOBAL_RCSCHEDULER->m_heightFieldSolid == nullptr) return;
        GLOBAL_RCSCHEDULER->raycastMesh(rayStart, rayEnd, min);
        auto hitpoint = worldPosStart + min * (worldPosEnd - worldPosStart);
        GLOBAL_RCSCHEDULER->hitPos = hitpoint;

        /*qDebug() << QString("mousepos:%1, %2").arg(x).arg(y);
        qDebug() << QString("worldPosStart:%1, %2, %3").arg(worldPosStart.x).arg(worldPosStart.y).arg(worldPosStart.z);
        qDebug() << QString("worldPosEnd:%1, %2, %3").arg(worldPosEnd.x).arg(worldPosEnd.y).arg(worldPosEnd.z);
        qDebug() << QString("hitpoint:%1, %2, %3").arg(hitpoint.x).arg(hitpoint.y).arg(hitpoint.z);*/
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
    void VulkanWindow::mouseDoubleClickEvent(QMouseEvent*)
    {
        GLOBAL_RCSCHEDULER->setAgent(GLOBAL_RCSCHEDULER->hitPos);
        GLOBAL_RCSCHEDULER->setCurrentTarget(GLOBAL_RCSCHEDULER->hitPos);
    }
}