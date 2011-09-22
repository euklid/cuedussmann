#ifndef PWDUIDIALOG_H
#define PWDUIDIALOG_H

#include <QDialog>
#include"ui_pwduidialog.h"

class PwduiDialog : public QDialog, public Ui::PwduiDialog
{
    Q_OBJECT

public:
    explicit PwduiDialog(QWidget *parent = 0, int p=0, int u=0);

private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_lineEdit_2_textChanged(const QString &arg1);
};

#endif // PWDUIDIALOG_H
