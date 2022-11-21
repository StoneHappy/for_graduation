#pragma once
#include <QVulkanWindow>
namespace CS
{
    class VulkanWindow : public QVulkanWindow
    {
        Q_OBJECT
    public:
        QVulkanWindowRenderer* createRenderer() override;
    };
}