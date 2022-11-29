#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QString>
namespace Ui {
class NewProjectDialog;
}

class QFileSystemModel;


class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = nullptr);
    ~NewProjectDialog();
    QString m_projectDir;
    QString m_projectPath;
    QString m_projectName;
private slots:
    void on_projectDirView_clicked(const QModelIndex& index);
    void slot_accept();

private:
    Ui::NewProjectDialog *ui;

    QFileSystemModel* m_model;
    
};

#endif // NEWPROJECTDIALOG_H
