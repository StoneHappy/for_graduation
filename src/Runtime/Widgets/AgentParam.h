#ifndef ADDAGENTDLG_H
#define ADDAGENTDLG_H

#include <QDialog>

namespace Ui {
class AgentParam;
}

class AgentParam : public QDialog
{
    Q_OBJECT

public:
    explicit AgentParam(QWidget *parent = nullptr);
    ~AgentParam();
private slots:
    void on_pushButtonSet_clicked();
private:
    Ui::AgentParam *ui;
};

#endif // ADDAGENTDLG_H
