#ifndef ADDMESHTOENTITYDLG_H
#define ADDMESHTOENTITYDLG_H

#include <QDialog>

namespace Ui {
class AddMeshToEntityDlg;
}

class QStandardItemModel;
class QItemSelectionModel;

class AddMeshToEntityDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddMeshToEntityDlg(QWidget* parent = nullptr);
    explicit AddMeshToEntityDlg(QStandardItemModel* meshModel, QItemSelectionModel* meshSelctMode, QStandardItemModel* textureModel, QItemSelectionModel* textureSelectMode, QWidget* parent = nullptr);
    ~AddMeshToEntityDlg();                                 

private:
    Ui::AddMeshToEntityDlg *ui;
};

#endif // ADDMESHTOENTITYDLG_H
