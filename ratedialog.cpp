#include "ratedialog.h"

RateDialog::RateDialog(QWidget *parent, QString zutat) :
    QDialog(parent),
    ui(new Ui::RateDialog)
{
    ui->setupUi(this);
    if(zutat.length()>3) ui->lineEdit->setText(zutat);
}

RateDialog::~RateDialog()
{
    delete ui;
}
