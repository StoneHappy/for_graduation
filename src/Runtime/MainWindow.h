#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Widgets/VulkanWindow.h>
#include <QtOpenGL/QGLWidget>
namespace Ui {
class MainWindow;
}

class QLabel;

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
    QLabel* m_mousePosition;

private slots:
    void on_actShowViewDock_triggered();
    void on_actShowInfoDock_triggered();
    void on_actAbout_triggered();
    void on_actPlay_triggered();
    void on_actPause_triggered();
};

#endif // MAINWINDOW_H
