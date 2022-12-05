#ifndef NAVMESHPARAMSDLG_H
#define NAVMESHPARAMSDLG_H

#include <QDialog>
#include <Recast.h>
#include <Function/AgentNav/RCParams.h>
namespace Ui {
class NavMeshParamsDlg;
}

class NavMeshParamsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NavMeshParamsDlg(QWidget *parent = nullptr);
    ~NavMeshParamsDlg();
	::GU::RCParams rc_params;
private slots:
    void slot_accept();

private:
    Ui::NavMeshParamsDlg *ui;
};

#endif // NAVMESHPARAMSDLG_H
