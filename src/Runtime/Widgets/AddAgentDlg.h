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

private slots:
    void on_pushButtonAddAgent_clicked();
    void on_pushButtonSetTarget_clicked();
private:
    Ui::AddAgentDlg *ui;

protected:
    void closeEvent(QCloseEvent*) override;
};

#endif // ADDAGENTDLG_H
