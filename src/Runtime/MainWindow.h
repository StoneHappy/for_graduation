#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Widgets/VulkanWindow.h>
#include <QtOpenGL/QGLWidget>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void creatorPopMenu();
    Ui::MainWindow *ui;
    CS::VulkanWindow* m_vulkanWindow;
    QVulkanInstance* inst;

private slots:
    void on_actShowViewDock_triggered();
    void on_actShowInfoDock_triggered();
};

#endif // MAINWINDOW_H
