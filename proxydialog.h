#ifndef PROXYDIALOG_H
#define PROXYDIALOG_H

#include <QDialog>
#include"ui_proxydialog.h"

namespace Ui {
    class ProxyDialog;
}

class ProxyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxyDialog(QWidget *parent = 0);
    ~ProxyDialog();
    Ui::ProxyDialog *ui;
private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_lineEdit_2_textChanged(const QString &arg1);
    void on_lineEdit_3_textChanged(const QString &arg1);
    void on_lineEdit_4_textChanged(const QString &arg1);
};

#endif // PROXYDIALOG_H
