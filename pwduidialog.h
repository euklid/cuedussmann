#ifndef PWDUIDIALOG_H
#define PWDUIDIALOG_H

#include <QDialog>
#include"ui_pwduidialog.h"

class PwduiDialog : public QDialog, public Ui::PwduiDialog
{
    Q_OBJECT

public:
    explicit PwduiDialog(QWidget *parent = 0);


};

#endif // PWDUIDIALOG_H
