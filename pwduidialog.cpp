/*
This file is part of cuedussmann.

    cuedussmann is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cuedussmann is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cuedussmann.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "pwduidialog.h"
PwduiDialog::PwduiDialog(QWidget *parent, int u, int p) :
    QDialog(parent)
{
    setupUi(this);
    lineEdit_2->setEchoMode(QLineEdit::Password);
    if(u<999 || p<999) buttonBox->setEnabled(false); else
    {
        buttonBox->setEnabled(true) ;
        lineEdit->setText(QString::number(u));
        lineEdit_2->setText(QString::number(p));
    }
    lineEdit->setMaxLength(4);
    lineEdit_2->setMaxLength(4);
    QRegExp regExp("[1-9][0-9]{3}");
    lineEdit->setValidator(new QRegExpValidator(regExp, this));
    QRegExp regExp2("[1-9][0-9]{3}");
    lineEdit_2->setValidator(new QRegExpValidator(regExp2,this));
}

void PwduiDialog::on_lineEdit_textChanged(const QString &arg1)
{
    if((lineEdit->hasAcceptableInput()) && (lineEdit_2->hasAcceptableInput()) && (! lineEdit->text().isEmpty()) && (! lineEdit_2->text().isEmpty()))
    {
        this->buttonBox->setEnabled(true);
    }
    else buttonBox->setEnabled(false);
}

void PwduiDialog::on_lineEdit_2_textChanged(const QString &arg1)
{
    if( (lineEdit->hasAcceptableInput()) && (lineEdit_2->hasAcceptableInput()))
    {
        this->buttonBox->setEnabled(true);
    } else buttonBox->setEnabled(false);
}
