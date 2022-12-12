#include "MeshSelectDialog.h"
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "ui_MeshSelectDialog.h"

MeshSelectDialog::MeshSelectDialog(QWidget* parent, QStandardItemModel* m_meshTableModel, QItemSelectionModel* m_meshTableSelectModel) :
    QDialog(parent),
    ui(new Ui::MeshSelectDialog)
{
    ui->setupUi(this);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setModel(m_meshTableModel);
    ui->tableView->setSelectionModel(m_meshTableSelectModel);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
}

MeshSelectDialog::~MeshSelectDialog()
{
    delete ui;
}
