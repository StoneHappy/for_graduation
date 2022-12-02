#include "AddMeshToEntityDlg.h"
#include "ui_AddMeshToEntityDlg.h"
#include <QStandardItemModel>
AddMeshToEntityDlg::AddMeshToEntityDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMeshToEntityDlg)
{
    ui->setupUi(this);
}

AddMeshToEntityDlg::AddMeshToEntityDlg(QStandardItemModel* meshModel, QItemSelectionModel* meshSelctModel, QStandardItemModel* textureModel, QItemSelectionModel* textureSelectModel, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddMeshToEntityDlg)
{
    ui->setupUi(this);

    ui->modelTableView->setModel(meshModel);
    ui->modelTableView->setSelectionModel(meshSelctModel);

    ui->textureTableView->setModel(textureModel);
    ui->textureTableView->setSelectionModel(textureSelectModel);

    ui->modelTableView->horizontalHeader()->hide();
    ui->modelTableView->verticalHeader()->hide();
    ui->textureTableView->horizontalHeader()->hide();
    ui->textureTableView->verticalHeader()->hide();
}

AddMeshToEntityDlg::~AddMeshToEntityDlg()
{
    delete ui;
}
