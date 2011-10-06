#ifndef RATEDIALOG_H
#define RATEDIALOG_H

#include <QDialog>
#include "ui_ratedialog.h"

namespace Ui {
    class RateDialog;
}

class RateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RateDialog(QWidget *parent = 0, QString zutat = "");
     Ui::RateDialog *ui;
    ~RateDialog();

private:

};

#endif // RATEDIALOG_H
