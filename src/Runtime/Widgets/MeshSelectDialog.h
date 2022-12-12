#ifndef MESHSELECTDIALOG_H
#define MESHSELECTDIALOG_H

#include <QDialog>

namespace Ui {
class MeshSelectDialog;
}
class QStandardItemModel;
class QItemSelectionModel;
class MeshSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MeshSelectDialog(QWidget* parent, QStandardItemModel* m_meshTableModel, QItemSelectionModel* m_meshTableSelectModel);
    ~MeshSelectDialog();

private:
    Ui::MeshSelectDialog *ui;
};

#endif // MESHSELECTDIALOG_H
