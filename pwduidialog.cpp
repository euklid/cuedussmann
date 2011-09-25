#include "pwduidialog.h"

PwduiDialog::PwduiDialog(QWidget *parent, int u, int p) :
    QDialog(parent)
{
    setupUi(this);
    if(u<999 || p<999) buttonBox->setEnabled(false); else
    {
        buttonBox->setEnabled(true) ;
        lineEdit->setText(QString::number(u));
        lineEdit_2->setText(QString::number(p));
    }
    lineEdit->setMaxLength(4);
    lineEdit_2->setMaxLength(4);
    QRegExp regExp("[1-9]{1}[0-9]{2,4}");
    lineEdit->setValidator(new QRegExpValidator(regExp, this));
    QRegExp regExp2("[1-9]{1}[0-9]{2,4}");
    lineEdit_2->setValidator(new QRegExpValidator(regExp2,this));
}

void PwduiDialog::on_lineEdit_textChanged(const QString &arg1)
{
    if((lineEdit->hasAcceptableInput() && lineEdit_2->hasAcceptableInput()) && (! lineEdit->text().isEmpty()) && (! lineEdit_2->text().isEmpty()))
    {
        this->buttonBox->setEnabled(true);
    }
    else buttonBox->setEnabled(false);
}

void PwduiDialog::on_lineEdit_2_textChanged(const QString &arg1)
{
    if( (lineEdit->hasAcceptableInput() && lineEdit_2->hasAcceptableInput()))
    {
        this->buttonBox->setEnabled(true);
    } else buttonBox->setEnabled(false);
}
