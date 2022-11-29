#include "NavMeshParamsDlg.h"
#include "ui_NavMeshParamsDlg.h"

NavMeshParamsDlg::NavMeshParamsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NavMeshParamsDlg)
{
    ui->setupUi(this);
}

NavMeshParamsDlg::~NavMeshParamsDlg()
{
    delete ui;
}
