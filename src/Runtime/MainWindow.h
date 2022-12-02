#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Widgets/VulkanWindow.h>
#include <QtOpenGL/QGLWidget>
#include <Core/UUID.h>
#include <QMetaType>
namespace Ui {
class MainWindow;
}

class QLabel;
class QStandardItem;
class QStandardItemModel;
class QItemSelectionModel;
class QProgressBar;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void importResource2Table(QString, uint64_t, int type);
    void setStatus(const QString& );
private:
    void createPopMenu();
    void createEntityView();
    void craeteComponentView();
    void craeteResourceView();
    void clearAllComponentProperty();
private:
    Ui::MainWindow *ui;
    GU::VulkanWindow* m_vulkanWindow;
    QVulkanInstance* inst;
    QLabel* m_statusInfo;
    QProgressBar* m_progressBar;
    std::unordered_map<GU::UUID, QStandardItem*> m_entityMap;

    QStandardItemModel* m_entityTreeModel;
    QItemSelectionModel* m_entityTreeSelectModel;
    QStandardItem* m_treeviewEntityRoot;

    QStandardItemModel*     m_meshTableModel;
    QItemSelectionModel*    m_meshTableSelectModel;
    uint32_t m_numMeshInTable = 0;

Q_SIGNALS:
    void signal_importResource2Table(QString, uint64_t, int type);

private slots:
    void on_actShowViewDock_triggered();
    void on_actShowInfoDock_triggered();
    void on_actAbout_triggered();
    void on_actNewProject_triggered();
    void on_actOpenProject_triggered();
    void on_actSaveProject_triggered();
    void on_actPlay_triggered();
    void on_actPause_triggered();
    void on_actStop_triggered();
    void on_actCreateEntity_triggered();
    void on_actCopyEntity_triggered();
    void on_actDeleteEntity_triggered();
    void on_actNavmeshParam_triggered();
    void on_actImportModel_triggered();
    void slot_tagPropertyChanged();
    void slot_treeviewEntity_customcontextmenu(const QPoint&);
    void slot_on_entityTreeSelectModel_currentChanged(const QModelIndex&, const QModelIndex&);
    void slot_on_meshTableSelectModel_currentChanged(const QModelIndex&, const QModelIndex&);
    void slot_importResource2Table(QString, uint64_t, int type);
};

#endif // MAINWINDOW_H
