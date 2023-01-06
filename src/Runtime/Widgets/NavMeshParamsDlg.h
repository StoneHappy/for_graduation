#ifndef NAVMESHPARAMSDLG_H
#define NAVMESHPARAMSDLG_H

#include <QDialog>
#include <Recast.h>
#include <Function/AgentNav/RCParams.h>
namespace Ui {
class NavMeshParamsDlg;
}
class QStandardItemModel;
class QItemSelectionModel;

class NavMeshParamsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NavMeshParamsDlg(QWidget* parent, QStandardItemModel* m_meshTableModel, QItemSelectionModel* m_meshTableSelectModel);
    ~NavMeshParamsDlg();
	::GU::RCParams rc_params;
private slots:
    void on_pushButtonOK_clicked();
    void on_toolButton_clicked();
    void on_IsRenderHFStateChanged(int state);
    void on_IsRenderCTStateChanged(int state);
    void on_IsRenderDMStateChanged(int state);
    void on_IsRenderTContourChanged(int state);
    void on_IsRenderTCompactFieldChanged(int state);
private:
    Ui::NavMeshParamsDlg *ui;
    QStandardItemModel* m_meshTableModel;
    QItemSelectionModel* m_meshTableSelectModel;
};

#endif // NAVMESHPARAMSDLG_H
