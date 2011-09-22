#include "cuedussmann.h"
#include "dussmanbot.cpp"
#include "pwduidialog.h"
#include <QString>
#include <QMessageBox>

cuedussmann::cuedussmann(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    loadPWDUID();
}

int cuedussmann::loadPWDUID()
{
    FILE* pwdfile;
    if(((pwdfile=fopen("pwfile","r"))==NULL))
    {
        QMessageBox msg;
        msg.setText("Es wurde noch keine Passwortdatei erstellt.");
        msg.setWindowTitle("Keine Datei vorhanden.");
        //msg.setStandardButtons(QMessageBox::Abort);
        msg.addButton("Benutzername und Passwort setzen",QMessageBox::AcceptRole);
        int ret = msg.exec();
        switch(ret) {
        //case QMessageBox::Abort : return 0;
        case QMessageBox::AcceptRole :
        {
            cuedussmann::on_actionUID_PWD_ndern_triggered();
            return 1;
        }
        }

    }
    else
    {
        fgets(uid,6,pwdfile);
        //uid[4]='\0';
        fgets(pwd,5,pwdfile);
        //pwd[4]='\0';
        if(QString::fromAscii(uid).toInt()<1000 || QString::fromAscii(uid).toInt()>9999 || QString::fromAscii(pwd).toInt()<1000 || QString::fromAscii(pwd).toInt()>9999)
        {
            QMessageBox msg;
            msg.setText("Die Passwörter sind ");
            msg.setWindowTitle("Keine Datei vorhanden.");
            //msg.setStandardButtons(QMessageBox::Abort);
            msg.addButton("Benutzername und Passwort setzen",QMessageBox::AcceptRole);
            int ret = msg.exec();
            switch(ret) {
            //case QMessageBox::Abort : return 0;
            case QMessageBox::AcceptRole :
            {
                cuedussmann::on_actionUID_PWD_ndern_triggered();
                return 1;
            }
            }

        }
        fclose(pwdfile);
    }
}

void cuedussmann::on_actionUID_PWD_ndern_triggered()
{
    PwduiDialog *dialog= new PwduiDialog(this,QString::fromAscii(uid).toInt(), QString::fromAscii(pwd).toInt());
    if( dialog->exec())
    {
        strcpy(uid,qPrintable(dialog->lineEdit->text()));
        strcpy(pwd,qPrintable(dialog->lineEdit_2->text()));
        FILE* pwdfile;
        pwdfile=fopen("pwfile","w");
        fputs(strcat(uid,"\n"),pwdfile);
        fputs(strcat(pwd,"\n"),pwdfile);
        fclose(pwdfile);
    }
    delete dialog;
}
