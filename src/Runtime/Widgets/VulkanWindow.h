#pragma once
#include <QVulkanWindow>
namespace GU
{
    class VulkanWindow : public QVulkanWindow
    {
        Q_OBJECT
    public:
        QVulkanWindowRenderer* createRenderer() override;
    };
}