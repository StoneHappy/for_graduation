#ifndef NAVMESHPARAMSDLG_H
#define NAVMESHPARAMSDLG_H

#include <QDialog>
#include <Recast.h>
namespace Ui {
class NavMeshParamsDlg;
}

class NavMeshParamsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NavMeshParamsDlg(QWidget *parent = nullptr);
    ~NavMeshParamsDlg();
    rcConfig rc_cfg;
private slots:
    void slot_accept();

private:
    Ui::NavMeshParamsDlg *ui;
};

#endif // NAVMESHPARAMSDLG_H
