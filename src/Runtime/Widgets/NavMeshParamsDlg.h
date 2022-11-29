#ifndef NAVMESHPARAMSDLG_H
#define NAVMESHPARAMSDLG_H

#include <QDialog>

namespace Ui {
class NavMeshParamsDlg;
}

class NavMeshParamsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NavMeshParamsDlg(QWidget *parent = nullptr);
    ~NavMeshParamsDlg();

private:
    Ui::NavMeshParamsDlg *ui;
};

#endif // NAVMESHPARAMSDLG_H
