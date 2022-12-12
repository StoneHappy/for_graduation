#ifndef ADDAGENTDLG_H
#define ADDAGENTDLG_H

#include <QDialog>

namespace Ui {
class AddAgentDlg;
}

class AddAgentDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddAgentDlg(QWidget *parent = nullptr);
    ~AddAgentDlg();

private:
    Ui::AddAgentDlg *ui;
};

#endif // ADDAGENTDLG_H
