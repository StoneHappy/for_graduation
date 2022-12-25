#pragma once
#include <QVulkanWindow>
#include <Renderer/VulkanRenderer.h>
namespace GU
{
    class VulkanWindow : public QVulkanWindow
    {
        Q_OBJECT
    public:
        QVulkanWindowRenderer* createRenderer() override;

    private:
        void mousePressEvent(QMouseEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void keyPressEvent(QKeyEvent*) override;
        void wheelEvent(QWheelEvent* event);
        virtual void mouseDoubleClickEvent(QMouseEvent*) override;
        bool m_debug;
        VulkanRenderer* m_renderer;
        bool m_pressed = false;
        QPoint m_lastPos;
    };
}