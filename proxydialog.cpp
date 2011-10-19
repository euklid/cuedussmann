#include "proxydialog.h"
#include "ui_proxydialog.h"

ProxyDialog::ProxyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProxyDialog)
{
    ui->setupUi(this);
    QRegExp ipregex("^([0-9]|[1-9][0-9]|1[0-9]{0,2}|2[0-4][0-9]|25[0-5])$");
    QRegExp portregex("^([0-9]|[1-9][0-9]{1,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$");
    ui->lineEdit->setValidator(new QRegExpValidator(ipregex,this));
    ui->lineEdit_2->setValidator(new QRegExpValidator(ipregex,this));
    ui->lineEdit_3->setValidator(new QRegExpValidator(ipregex,this));
    ui->lineEdit_4->setValidator(new QRegExpValidator(ipregex,this));
    ui->lineEdit_5->setValidator(new QRegExpValidator(portregex,this));
}

ProxyDialog::~ProxyDialog()
{
    delete ui;
}

void ProxyDialog::on_lineEdit_textChanged(const QString &arg1)
{
    if(ui->lineEdit->text().length()==3) ui->lineEdit_2->setFocus(Qt::OtherFocusReason);
}

void ProxyDialog::on_lineEdit_2_textChanged(const QString &arg1)
{
    if(ui->lineEdit_2->text().length()==3) ui->lineEdit_3->setFocus(Qt::OtherFocusReason);
}

void ProxyDialog::on_lineEdit_3_textChanged(const QString &arg1)
{
    if(ui->lineEdit_3->text().length()==3) ui->lineEdit_4->setFocus(Qt::OtherFocusReason);
}

void ProxyDialog::on_lineEdit_4_textChanged(const QString &arg1)
{
    if(ui->lineEdit_4->text().length()==3) ui->lineEdit_5->setFocus(Qt::OtherFocusReason);
}
