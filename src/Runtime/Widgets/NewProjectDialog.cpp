#include "NewProjectDialog.h"
#include "ui_NewProjectDialog.h"
#include <QFileSystemModel>
#include <QDebug>
NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

    m_model = new QFileSystemModel(this);
    QString currPath = QDir::currentPath();  //��ȡ��ǰ·��
    m_model->setRootPath(currPath);          //���ø�Ŀ¼
    ui->projectDirView->setModel(m_model);         //��������ģ��
    connect(ui->pushOK, SIGNAL(clicked()), this, SLOT(slot_accept()));
}

void NewProjectDialog::on_projectDirView_clicked(const QModelIndex& index)
{
     m_projectDir =  m_model->filePath(index);
}


NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::slot_accept()
{
    m_projectName = ui->projectNameEdit->text();
    if (m_projectName.isEmpty()) m_projectName = QString::fromLocal8Bit("project");
    m_projectPath = QString::fromLocal8Bit("%1/%2.gu").arg(m_projectDir).arg(m_projectName);
}