#include "cuedussmann.h"
#include "dussmanbot.cpp"
#include "pwduidialog.h"
#include <QString>
#include <QMessageBox>
using namespace core;

cuedussmann::cuedussmann(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    loadPWDUID();
    //loginandcookie(uid, pwd);
    kalwochen();
    getsel_datums();
    setcombobox(startwoche, startwoche+anzwoche-1);
    parsemenufile(0);
    connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(parsemenufile(int)));
    //TODO:
    /* now the menufiles should be downloaded and BE PARSED, so that the actual week is shown in the table. The combobox must
     * get its entries
     *
     * implementation for ordering only selected days must be written, and how to select some days
     *
     * then the function for ordering must be written: if a menu ist changed manually, this should be recognized --> storing
     * in another array?? or own tablewidgetitemclass ?? rating his clicked item in cell gets value > 10 so that it will be
     * ordered. The other items in that coloumn should get rating 0. --> better, if another cell in same coloumn is clicked
     *
     * for ordering we need a new dialog and therefore changes in rating menu
     */
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

 int cuedussmann::kalwochen()
{
        FILE* wochenliste;
        core::find("kalendera","<select name=\"sel_datum\" class=\"no_print\" onchange=\"document.form_sel_datum.submit()\">",12);
        core::find("findoutput","KW");
        core::find("findoutput","selected=\"selected\"",12);
        wochenliste=fopen("findoutput","r");
        char c;
        while((c=fgetc(wochenliste)) != EOF)
        {
                if(c == '\n') anzwoche++;
        }
        fclose(wochenliste);
        char* puffer=(char*)malloc(150);
        frstln(puffer,150,"findoutput");
        cut2(puffer,":",2,2);
        cut2(puffer," ",1,1);
        for(int i=48; i<=53; i++)
        {
                if(puffer[0]==i)
                {
                        startwoche=10*(i-48);
                        break;
                }
        }
        for(int i=48;i<=57; i++)
        {
                if(puffer[1]==i)
                {
                        startwoche+=(i-48);
                        break;
                }
        }
        //printf("startwoche: %i und anzahlwochen: %i\n",startwoche,anzwoche);
        free(puffer);
        return 1;
}

 void cuedussmann::getsel_datums()
{
        FILE* wochenliste;
        char* buffer;
        buffer=(char*)malloc(150*sizeof(char));
        slynmbwochen=(char**)calloc(anzwoche,sizeof(char*));
        for(int i=0;i<anzwoche;i++) slynmbwochen[i]=(char*)malloc(11);
        wochenliste=fopen("findoutput","r");
        for(int i=0;i<anzwoche;i++)
        {
                fgets(buffer, 150, wochenliste);
                strcpy(slynmbwochen[i],cut2(buffer,"\"",2,2)); //get silly numbers
        }
        fclose(wochenliste);
}

 void cuedussmann::setcombobox(int startweek, int endweek)
{
    for(int i=startweek; i<=endweek; i++)
    {
        comboBox->addItem(QString("Woche ")+QString::number(i)); //Therefore index 0 is standig for startweek, its slynumber can be access then by slynumber[0]
    }
}

 void cuedussmann::createmenufiles()
{
        FILE* menus[anzwoche];
        char postfield[25];
        char menufilename[8];
        char menunumber[2];
        for(int i=0;i<anzwoche;i++)
        {
                strcpy(menufilename,"menu");
                menunumber[0]=48+i; menunumber[1]='\0';
                strcat(menufilename,menunumber); strcat(menufilename,"\0");
                menus[i]=fopen(menufilename, "w");
                strcpy(postfield,"sel_datum=");
                strcat(postfield,slynmbwochen[i]);
                CURLcode ret;
                CURL *hnd = curl_easy_init();
                curl_easy_setopt(hnd, CURLOPT_WRITEDATA, menus[i]);
                curl_easy_setopt(hnd, CURLOPT_INFILESIZE_LARGE, -1);
                curl_easy_setopt(hnd, CURLOPT_URL, "http://dussmann-lpf.rcs.de/index.php?m=1;3");
                curl_easy_setopt(hnd, CURLOPT_PROXY, NULL);
                curl_easy_setopt(hnd, CURLOPT_PROXYUSERPWD, NULL);
                curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postfield);
                curl_easy_setopt(hnd, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux i686; de; rv:1.9.2.12) Gecko/20101027 Firefox/3.6.12");
                curl_easy_setopt(hnd, CURLOPT_COOKIEFILE, "Cookiedatei");
                curl_easy_setopt(hnd, CURLOPT_COOKIEJAR, NULL);
                ret = curl_easy_perform(hnd);
                curl_easy_cleanup(hnd);
                fclose(menus[i]);
        }
}

 void cuedussmann::parsemenufile(int itemindex)
{
     QBrush backcolor;
     for(int i =0; i<7;i++)
     {
         for(int j=0;j<3;j++)
         {
             tableWidget->item(j,i)->setText("");
             tableWidget->item(j,i)->setBackground(backcolor);
         }
     }
    QString menuname("menu");
    menuname.append(QString::number(itemindex));
    //printf("%s",qPrintable(menuname));
    //printf("%s",qPrintable(menuname));
    core::find(qPrintable(menuname),"<table class=\"splanauflistung\"  summary=\"Speiseplan\">",10);
    core::find("findoutput","</th>");
    int numdays=0;
    char c;
    FILE* days;
    days = fopen("findoutput","r");
    while((c=fgetc(days))!= EOF )
    {
            if(c=='\n') numdays++;
    }
    fclose(days);
    numdays--; //wegen &nbsp;
    core::find(qPrintable(menuname),"class=\" auflistung");
    FILE* auflistungen;
    auflistungen=fopen("findoutput","r");
    char* tmp=(char*)malloc(300);
    char* tmp2=(char*)malloc(300);
    //backcolor.setStyle(Qt::Dense4Pattern);
    for(int j=0;j<numdays*3;j++) //21, weil bis dahin es zum sonntag menü 3 geht //fetter bug, was, wenn speiseplan nur bis freitag geht???!!!
    {
            fgets(tmp, 300,auflistungen);
            strcpy(tmp2,tmp);
            cut2(tmp,">",2,7);
            strcpy(tmp,qPrintable(QString::fromAscii(tmp).remove(QRegExp("[0123456,789]")).simplified()));
            removeformattingsigns(tmp);
            tableWidget->item(j/numdays, j%numdays)->setText(QString::fromAscii(tmp));
            backcolor.setColor(QColor(255,0,0));
            backcolor.setStyle(Qt::SolidPattern);
            tableWidget->item(j/numdays, j%numdays)->setBackground(backcolor);
            if(strstr(tmp2,"pointer")!=NULL)
            {
                cut2(tmp,">",2,7);
                strcpy(tmp,qPrintable(QString::fromAscii(tmp).remove(QRegExp("[0123456,789]")).simplified()));
                removeformattingsigns(tmp);
                tableWidget->item(j/numdays, j%numdays)->setText(QString::fromAscii(tmp)); //--> Ausschließen des Tages, da dieser schon bestellt ==> alle folgenden unbestellt oder gruen
                backcolor.setColor(QColor(255,0,0));
                backcolor.setStyle(Qt::Dense4Pattern);
                tableWidget->item(j/numdays,j%numdays)->setBackground(backcolor);
                if(strstr(tmp2,"gruen")!=NULL)
                {
                    cut2(tmp,">",2,7);
                    strcpy(tmp,qPrintable(QString::fromAscii(tmp).remove(QRegExp("[0123456,789]")).simplified()));
                    removeformattingsigns(tmp);
                    tableWidget->item(j/numdays, j%numdays)->setText(QString::fromAscii(tmp));
                    backcolor.setColor(QColor(0,255,0));
                    backcolor.setStyle(Qt::Dense4Pattern);
                    tableWidget->item(j/numdays, j%numdays)->setBackground(backcolor);
                }
            }else if(strstr(tmp2,"gruen")!=NULL)
            {
                cut2(tmp,">",2,7);
                strcpy(tmp,qPrintable(QString::fromAscii(tmp).remove(QRegExp("[0123456,789]")).simplified()));
                removeformattingsigns(tmp);
                tableWidget->item(j/numdays, j%numdays)->setText(QString::fromAscii(tmp));
                backcolor.setColor(QColor(0,255,0));
                backcolor.setStyle(Qt::SolidPattern);
                tableWidget->item(j/numdays, j%numdays)->setBackground(backcolor);
            }
    }
    fclose(auflistungen);
}
