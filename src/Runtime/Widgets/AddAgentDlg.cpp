#include "AddAgentDlg.h"
#include "ui_AddAgentDlg.h"

AddAgentDlg::AddAgentDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAgentDlg)
{
    ui->setupUi(this);
}

AddAgentDlg::~AddAgentDlg()
{
    delete ui;
}
