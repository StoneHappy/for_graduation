#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Widgets/VulkanWindow.h>
#include <QtOpenGL/QGLWidget>
#include <Core/UUID.h>
namespace Ui {
class MainWindow;
}

class QLabel;
class QStandardItem;
class QStandardItemModel;
class QItemSelectionModel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createPopMenu();
    void createEntityView();
    void craeteComponentView();

private:
    Ui::MainWindow *ui;
    GU::VulkanWindow* m_vulkanWindow;
    QVulkanInstance* inst;
    QLabel* m_mousePosition;
    std::unordered_map<GU::UUID, QStandardItem*> m_entityMap;
    QStandardItemModel* m_model;
    QItemSelectionModel* m_selectModel;
    QStandardItem* rootItem;
private slots:
    void on_actShowViewDock_triggered();
    void on_actShowInfoDock_triggered();
    void on_actAbout_triggered();
    void on_actPlay_triggered();
    void on_actPause_triggered();
    void on_actStop_triggered();
    void on_actCreateEntity_triggered();
    void on_actCopyEntity_triggered();
    void on_actDeleteEntity_triggered();

    void slot_treeviewEntity_customcontextmenu(const QPoint&);
};

#endif // MAINWINDOW_H
