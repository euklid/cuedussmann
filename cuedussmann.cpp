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
#include "cuedussmann.h"
#include "dussmanbot.cpp"
#include "pwduidialog.h"
#include "ratedialog.h"
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTextStream>
#include "howtouse.h"
#include "thanks.h"
using namespace core;
int*** changedmenu;
cuedussmann::cuedussmann(QWidget *parent) :
    QMainWindow(parent)
{
    initialized=0;
    wegonnaquit=0;
    setupUi(this);
    initialized=initialize();
    if(initialized==31 || initialized==28) wegonnaquit=1;
    connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(parsemenufile(int))); //combobox must be connected with the checkboxes, to set them checked or not, the checkboxes must be connected with the cell clicked functions...
    //TODO:
    /* now the menufiles should be downloaded (OK) and BE PARSED (OK), so that the actual week is shown in the table(OK). The combobox must
     * get its entries (OK)
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

int cuedussmann::initialize()
{    
    loadPWDUID();
    int log;
    while((log=loginandcookie(uid, pwd))!=1)
    {
        if(log==0)
        {
            QMessageBox msg;
            msg.setText("Falsche Nutzernummer/Passwort Kombination");
            msg.setWindowTitle("Fehler beim Login");
            msg.addButton("Benutzername und Passwort neu setzen",QMessageBox::AcceptRole);
            int ret = msg.exec();
            if(ret==QMessageBox::AcceptRole)
            {
                cuedussmann::on_actionUID_PWD_ndern_triggered();
            }
        }
        if(log==2)
        {
            QMessageBox msg;
            msg.setText(QString::fromLocal8Bit("Der Account für die eingegebene Benutzernummer ist gesperrt. Tut mir sehr leid."));
            msg.setWindowTitle("Fehler beim Login");
            msg.setStandardButtons(QMessageBox::Abort);
            msg.addButton("Anderen Account einstellen",QMessageBox::AcceptRole);
            int ret = msg.exec();
            if(ret==QMessageBox::AcceptRole)
            {
                cuedussmann::on_actionUID_PWD_ndern_triggered();
            }
            if(ret==262144 || ret==0) //is for acceptrole
            {
                qApp->quit(); return 31;

            }
        }
        if(log==3)
        {
            QMessageBox msg;
            msg.setText(QString::fromLocal8Bit("Es besteht keine Internetverbindung. Bitte versuche es später noch einmal"));
            msg.setWindowTitle(QString::fromLocal8Bit("Verbindungsfehler"));
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            qApp->quit();
            return 28;
        }
    }
    if(initialized==0)
    {
        kalwochen();
        slynmbwochen=(char**)calloc(anzwoche,sizeof(char*));
        getsel_datums();
        createmenufiles();
        ratings=(float***)calloc(anzwoche,sizeof(float**)); //--> stores ratings for foods
        setdates=(int**)calloc(anzwoche,sizeof(int*));
        wirkbestellen=(int**)calloc(anzwoche,sizeof(int*));
        changedmenu=(int***)calloc(anzwoche,sizeof(int**));
        for(int i = 0; i<anzwoche; i++)
        {
            ratings[i]=(float**)calloc(7,sizeof(float*));
            setdates[i]=(int*)calloc(7,sizeof(int));
            wirkbestellen[i]=(int*)calloc(7,sizeof(int));
            changedmenu[i]=(int**)calloc(7,sizeof(int*));
            for(int j = 0; j<7; j++)
            {
                ratings[i][j]=(float*)calloc(3,sizeof(float));
                changedmenu[i][j]=(int*)calloc(3,sizeof(int));
                setdates[i][j]=0;
                wirkbestellen[i][j]=0;
                for(int k =0;k <3;k++)
                {
                    // ratings[i][j][k]=(float)malloc(sizeof(float));
                    ratings[i][j][k]=0.0;
                    changedmenu[i][j][k]=0;
                }
            }
        }
        setcombobox(startwoche, startwoche+anzwoche-1);
        parsemenufile(0);
        hidden=(char***)calloc(anzwoche,sizeof(char**));
        bergruen=(char***)calloc(anzwoche, sizeof(char**));
        bergruend=(char***)calloc(anzwoche,sizeof(char**));
        for(int i = 0 ; i < anzwoche; i++)
        {
            hidden[i]=(char**)calloc(35,sizeof(char*));
            bergruen[i]=(char**)calloc(35,sizeof(char*));
            bergruend[i]=(char**)calloc(35,sizeof(char*));

            for(int k=0;k<35;k++)
            {
                hidden[i][k]=(char*)malloc(50); strcpy(hidden[i][k],"\0");
                bergruen[i][k]=(char*)malloc(50);strcpy(bergruen[i][k],"\0");
                bergruend[i][k]=(char*)malloc(50);strcpy(bergruend[i][k],"\0");
            }
        }
        wocheplustagplusdaten=(char****)calloc(anzwoche,sizeof(char***));
        bestelldaten=(char***)calloc(anzwoche,sizeof(char**));
        for(int i=0;i<anzwoche;i++)
        {
            wocheplustagplusdaten[i]=(char***)calloc(7,sizeof(char**));
            bestelldaten[i]=(char**)calloc(7,sizeof(char*));
            for(int j=0;j<7;j++)
            {
                wocheplustagplusdaten[i][j]=(char**)calloc(9,sizeof(char*));
                bestelldaten[i][j]=(char*)malloc(25);
                strcpy(bestelldaten[i][j],"\0");
                for(int k=0;k<9;k++)
                {
                    wocheplustagplusdaten[i][j][k]=(char*)malloc(150*sizeof(char));
                    strcpy(wocheplustagplusdaten[i][j][k],"\0");
                }
            }
        }
        return 1;
    }
}

int cuedussmann::loadPWDUID()
{
    FILE* pwdfile;
    if(((pwdfile=fopen("pwfile","r"))==NULL))
    {
        QMessageBox msg;
        msg.setText("Es wurde noch keine Passwortdatei erstellt.");
        msg.setWindowTitle("Keine Datei vorhanden.");
        msg.addButton("Benutzername und Passwort setzen",QMessageBox::AcceptRole);
        int ret = msg.exec();
        switch(ret) {
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
        strcpy(uid,qPrintable(QString::fromAscii(uid).simplified()));
        uid[4]='\0';
        fgets(pwd,5,pwdfile);
        strcpy(pwd,qPrintable(QString::fromAscii(pwd).simplified()));
        pwd[4]='\0';
        if(QString::fromAscii(uid).toInt()<1000 || QString::fromAscii(uid).toInt()>9999 || QString::fromAscii(pwd).toInt()<1000 || QString::fromAscii(pwd).toInt()>9999)
        {
            QMessageBox msg;
            msg.setText("Die Passwörter sind ");
            msg.setWindowTitle("Keine Datei vorhanden.");
            msg.addButton("Benutzername und Passwort setzen",QMessageBox::AcceptRole);
            int ret = msg.exec();
            switch(ret) {
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
        strcpy(uid,qPrintable(dialog->lineEdit->text().simplified()));
        uid[4]='\0';
        strcpy(pwd,qPrintable(dialog->lineEdit_2->text().simplified()));
        pwd[4]='\0';
        FILE* pwdfile;
        pwdfile=fopen("pwfile","w");
        fputs(strcat(uid,"\n"),pwdfile);
        fputs(strcat(pwd,"\n"),pwdfile);
        fclose(pwdfile);
        if(initialized==1)
        {
            anzwoche=0;startwoche=0;
            free(wirkbestellen);
            free(setdates);
            free(slynmbwochen);
            free(changedmenu);
            free(ratings);
            free(wocheplustagplusdaten);
            free(hidden);
            free(bergruen);
            free(bergruend);
            free(bestelldaten);
            initialized=0;
        }
        initialized=initialize();
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
    free(puffer);
    return 1;
}

void cuedussmann::getsel_datums()
{
    FILE* wochenliste;
    char* buffer;
    buffer=(char*)malloc(150*sizeof(char));
    for(int i=0;i<anzwoche;i++) slynmbwochen[i]=(char*)malloc(11*sizeof(char));
    wochenliste=fopen("findoutput","r");
    for(int i=0;i<anzwoche;i++)
    {
        fgets(buffer, 150, wochenliste);
        strcpy(slynmbwochen[i],cut2(buffer,"\"",2,2)); //get silly numbers
    }
    fclose(wochenliste);
    free(buffer);
}

void cuedussmann::setcombobox(int startweek, int endweek)
{
    for(int i=startweek; i<=endweek; i++)
    {
        comboBox->removeItem(i-startweek);
        comboBox->addItem(QString("Woche ")+QString::number(i)); //Therefore index 0 is standig for startweek, its slynumber can be access then by slynumber[0]
        comboBox->setItemText(i-startweek,QString("Woche ")+QString::number(i));
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
    for(int j=0;j<numdays*3;j++) //21, weil bis dahin es zum sonntag menü 3 geht //fetter bug, was, wenn speiseplan nur bis freitag geht???!!!
    {
        fgets(tmp, 300,auflistungen);
        strcpy(tmp2,tmp);
        cut2(tmp,">",2,7);
        strcpy(tmp,qPrintable(QString::fromAscii(tmp).remove(QRegExp("[0123456,789]")).simplified()));
        removeformattingsigns(tmp);
        tableWidget->item(j/numdays, j%numdays)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable);
        if(qstrlen(tmp)>3)
        {
            tableWidget->item(j/numdays, j%numdays)->setText(QString::fromLocal8Bit(tmp));
            backcolor.setColor(QColor(255,0,0));
            backcolor.setStyle(Qt::SolidPattern);
            tableWidget->item(j/numdays, j%numdays)->setBackground(backcolor); //parsing the checkboxes must be included. setting some checkboxes disabled if this day can't be changed

            if(strstr(tmp2,"pointer")!=NULL)
            {
                //tableWidget->item(j/numdays, j%numdays)->setText(QString::fromLocal8Bit(tmp));
                backcolor.setColor(QColor(255,0,0));
                backcolor.setStyle(Qt::Dense4Pattern);
                tableWidget->item(j/numdays, j%numdays)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable);
                tableWidget->item(j/numdays,j%numdays)->setBackground(backcolor);
                int manually=0;
                for(int k=0;k<3;k++)manually+=(changedmenu[itemindex][j%numdays][k]*(k+1));
                if(strstr(tmp2,"gruen")!=NULL || manually>0)
                {
                    backcolor.setColor(QColor(0,255,0));
                    backcolor.setStyle(Qt::Dense4Pattern);
                    if(manually==0)
                    {
                        tableWidget->item(j/numdays, j%numdays)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable);
                        tableWidget->item(j/numdays, j%numdays)->setBackground(backcolor);
                    } else
                    {
                        //printf("%i\n",manually);
                        switch(manually)
                        {
                        case 1: tableWidget->item(0, j%numdays)->setBackground(backcolor); break;
                        case 2: tableWidget->item(1, j%numdays)->setBackground(backcolor);break;
                        case 3: tableWidget->item(2, j%numdays)->setBackground(backcolor);break;
                        }
                    }
                }
            }else if(strstr(tmp2,"gruen")!=NULL)
            {
                //tableWidget->item(j/numdays, j%numdays)->setText(QString::fromLocal8Bit(tmp));
                backcolor.setColor(QColor(0,255,0));
                backcolor.setStyle(Qt::SolidPattern);
                tableWidget->item(j/numdays, j%numdays)->setBackground(backcolor);
            }
        }
        else
        {
            tableWidget->item(j/numdays, j%numdays)->setFlags(Qt::NoItemFlags);
        }
    }
    fclose(auflistungen);
    checkBox->setChecked(setdates[itemindex][0]);
    checkBox_2->setChecked(setdates[itemindex][1]);
    checkBox_3->setChecked(setdates[itemindex][2]);
    checkBox_4->setChecked(setdates[itemindex][3]);
    checkBox_5->setChecked(setdates[itemindex][4]);
    checkBox_6->setChecked(setdates[itemindex][5]);
    checkBox_7->setChecked(setdates[itemindex][6]);
    checkBox_8->setChecked(false);
}

void cuedussmann::on_checkBox_8_clicked()
{
    if(!checkBox->isChecked())
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,0)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,0)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,0)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,0)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,0)->background().color().green()>0) || (tableWidget->item(i,0)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox->setChecked(true);
            setdates[comboBox->currentIndex()][0]=1;
        }
    }

    if(!checkBox_2->isChecked())
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,1)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,1)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,1)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,1)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,1)->background().color().green()>0) || (tableWidget->item(i,1)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_2->setChecked(true);
            setdates[comboBox->currentIndex()][1]=1;
        }
    }

    if(!checkBox_3->isChecked())
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,2)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,2)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,2)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,2)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,2)->background().color().green()>0) || (tableWidget->item(i,2)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_3->setChecked(true);
            setdates[comboBox->currentIndex()][2]=1;
        }
    }

    if(!checkBox_4->isChecked())
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,3)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,3)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,3)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,3)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,3)->background().color().green()>0) || (tableWidget->item(i,3)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_4->setChecked(true);
            setdates[comboBox->currentIndex()][3]=1;
        }
    }

    if(!checkBox_5->isChecked())
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,4)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,4)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,4)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,4)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,4)->background().color().green()>0) || (tableWidget->item(i,4)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_5->setChecked(true);
            setdates[comboBox->currentIndex()][4]=1;
        }
    }

    if(!checkBox_6->isChecked())
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,5)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,5)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,5)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,5)->flags()!=Qt::NoItemFlags)&& ((tableWidget->item(i,5)->background().color().green()>0) || (tableWidget->item(i,5)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_6->setChecked(true);
            setdates[comboBox->currentIndex()][5]=1;
        }
    }

    if(!checkBox_7->isChecked())
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,6)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,6)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,6)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,6)->flags()!=Qt::NoItemFlags) && ((tableWidget->item(i,6)->background().color().green()>0) || (tableWidget->item(i,6)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_7->setChecked(true);
            setdates[comboBox->currentIndex()][6]=1;
        }
    }
}

void cuedussmann::on_checkBox_clicked()
{
    if(!(checkBox->isChecked()))
    {
        setdates[comboBox->currentIndex()][0]=0;
        checkBox_8->setChecked(false);
    }
    else
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,0)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,0)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,0)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,0)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,0)->background().color().green()>0) || (tableWidget->item(i,0)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox->setChecked(true);
            setdates[comboBox->currentIndex()][0]=1;
        } else checkBox->setChecked(false);
    }
}

void cuedussmann::on_checkBox_2_clicked()
{
    if(!(checkBox_2->isChecked()))
    {
        setdates[comboBox->currentIndex()][1]=0;
        checkBox_8->setChecked(false);
    }
    else
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,1)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,1)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,1)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,1)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,1)->background().color().green()>0) || (tableWidget->item(i,1)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_2->setChecked(true);
            setdates[comboBox->currentIndex()][1]=1;
        }else checkBox_2->setChecked(false);
    }
}

void cuedussmann::on_checkBox_3_clicked()
{
    if(!(checkBox_3->isChecked()))
    {
        setdates[comboBox->currentIndex()][2]=0;
        checkBox_8->setChecked(false);
    }
    else
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,2)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,2)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,2)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,2)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,2)->background().color().green()>0) || (tableWidget->item(i,2)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_3->setChecked(true);
            setdates[comboBox->currentIndex()][2]=1;
        }else checkBox_3->setChecked(false);
    }
}

void cuedussmann::on_checkBox_4_clicked()
{
    if(!(checkBox_4->isChecked()))
    {
        setdates[comboBox->currentIndex()][3]=0;
        checkBox_8->setChecked(false);
    }
    else
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,3)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,3)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,3)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,3)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,3)->background().color().green()>0) || (tableWidget->item(i,3)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_4->setChecked(true);
            setdates[comboBox->currentIndex()][3]=1;
        }else checkBox_4->setChecked(false);
    }
}

void cuedussmann::on_checkBox_5_clicked()
{
    if(!(checkBox_5->isChecked()))
    {
        setdates[comboBox->currentIndex()][4]=0;
        checkBox_8->setChecked(false);
    }
    else
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,4)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,4)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,4)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,4)->flags()!=Qt::NoItemFlags)&&((tableWidget->item(i,4)->background().color().green()>0) || (tableWidget->item(i,4)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_5->setChecked(true);
            setdates[comboBox->currentIndex()][4]=1;
        }else checkBox_5->setChecked(false);
    }
}

void cuedussmann::on_checkBox_6_clicked()
{
    if(!(checkBox_6->isChecked()))
    {
        setdates[comboBox->currentIndex()][5]=0;
        checkBox_8->setChecked(false);
    }
    else
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,5)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,5)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,5)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,5)->flags()!=Qt::NoItemFlags)&& ((tableWidget->item(i,5)->background().color().green()>0) || (tableWidget->item(i,5)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_6->setChecked(true);
            setdates[comboBox->currentIndex()][5]=1;
        }else checkBox_6->setChecked(false);
    }
}

void cuedussmann::on_checkBox_7_clicked()
{
    if(!(checkBox_7->isChecked()))
    {
        setdates[comboBox->currentIndex()][6]=0;
        checkBox_8->setChecked(false);
    }
    else
    {
        float needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,6)->flags()==Qt::NoItemFlags) needed-=0.34;
            if((tableWidget->item(i,6)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,6)->flags()!=Qt::NoItemFlags)) needed=-0.34;
            if((tableWidget->item(i,6)->flags()!=Qt::NoItemFlags) && ((tableWidget->item(i,6)->background().color().green()>0) || (tableWidget->item(i,6)->background().color().red()<255))) needed--;
        }
        if(needed>0)
        {
            checkBox_7->setChecked(true);
            setdates[comboBox->currentIndex()][6]=1;
        }else checkBox_7->setChecked(false);
    }
}

void cuedussmann::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if(tableWidget->item(row,column)->flags()==Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable)
    {
        QBrush background;
        background.setColor(QColor(255,0,0));
        background.setStyle(Qt::Dense4Pattern);
        for(int i=0;i<3;i++)
        {
            if((tableWidget->item(i,column)->background().color().red()>0) || (tableWidget->item(i,column)->background().color().green()>0))
            {
                tableWidget->item(i,column)->setBackground(background);
                ratings[comboBox->currentIndex()][column][i]=-1; //-1 should mean, that the day has been chosen manually and this menu must not be ordered
            }
            changedmenu[comboBox->currentIndex()][column][i]=0;
        }
        setdates[comboBox->currentIndex()][column]=1;
        wirkbestellen[comboBox->currentIndex()][column]=1;
        changedmenu[comboBox->currentIndex()][column][row]=1;
        background.setColor(QColor(0,255,0));
        tableWidget->item(row,column)->setBackground(background);
        ratings[comboBox->currentIndex()][column][row]=11.0; //this will later on mean that the menu's components are not added to rating file and this menu will be definitely ordered.
        switch(column) //an array is needed to store manually changed data. this data would be appended at the end of the postfield so that the previous selection of the menu for one day is internally overwritten. The server always takes the last value of a variable.
        {
        case 0:
            checkBox->setChecked(true);break;
        case 1:
            checkBox_2->setChecked(true);break;
        case 2:
            checkBox_3->setChecked(true);break;
        case 3:
            checkBox_4->setChecked(true);break;
        case 4:
            checkBox_5->setChecked(true);break;
        case 5:
            checkBox_6->setChecked(true);break;
        case 6:
            checkBox_7->setChecked(true);break;
        }
    }
}

void cuedussmann::getdatensatz()
{
        for(int i=0;i<anzwoche;i++)
        {
                for(int j=0;j<7;j++)
                {
                        if(setdates[i][j]==1) wirkbestellen[i][j]=1; //ACHTUNG: Das kann noch zu viel sein!!
                }
        }
        char menufilename[8];
        char menunumber[2];
        for(int i=0;i<anzwoche;i++)
        {
                if(strlen(slynmbwochen[i])>3)
                {
                        strcpy(menufilename,"menu");
                        menunumber[0]=48+i; menunumber[1]='\0';
                        strcat(menufilename,menunumber); strcat(menufilename,"\0");
                        core::find(menufilename,"<table class=\"splanauflistung\"  summary=\"Speiseplan\">",10);
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
                        for(int f=numdays;f<7;f++) wirkbestellen[i][f]=0; //FIXME: distinguish between first 5 days can be ordered and 5 days can be ordered
                        core::find(menufilename,"class=\" auflistung"); //find green marked and not green marked
                        FILE* auflistungen;
                        auflistungen=fopen("findoutput","r");
                        char* tmp=(char*)malloc(300);
                        for(int j=0; j<numdays*3;j++)
                        {
                                fgets(tmp,300,auflistungen);
                                if(wirkbestellen[i][j%numdays]==1)
                                {
                                        cut2(tmp,">",2,7); //extracts name //hier muss noch fallunterscheidung wegen \n am ende rein!!!
                                        if((tmp[0]=='\n') &&(j<numdays)) {wirkbestellen[i][j%numdays]=0; continue;}
                                        if((tmp[0]=='\n') && (j/numdays==1)){continue;} //--> die leeren Menü2s bleiben '\0'
                                        strcpy(wocheplustagplusdaten[i][j%numdays][3*(j/numdays)],tmp); //Menünamen abspeichern, damit man nach ihm in den folgenden Zeilen suchen kann, damit man die restlichen Daten ermitteln kann
                                         //TODO: '\n' am Ende der Zeile löschen ('\r' auch(?) )
                                }
                        }
                        //Nun weiß man die Tage, für welche MENÜS bestellt müssen, man weiß nicht, wo Desserts bestellt werden müssen
                        fclose(auflistungen);
                        FILE* essendata;
                        for(int j=0; j<numdays*3;j++)
                        {
                                if(wirkbestellen[i][j%numdays]==1)
                                {
                                        if(strlen(wocheplustagplusdaten[i][j%numdays][3*(j/numdays)])>2) //--> Ausschließen, dass für ein leeres Menü 2 Daten gesammelt werden sollen
                                        {
                                                core::find(menufilename,wocheplustagplusdaten[i][j%numdays][3*(j/numdays)],2); //Suche nach Zeile über den Essensnamen
                                                core::find("findoutput","<input type=\"radio\" name=\"rad_");
                                                essendata=fopen("findoutput","r");
                                                fgets(tmp, 300, essendata);
                                                cut2(tmp,"\"",4,4);
                                                strcpy(wocheplustagplusdaten[i][j%numdays][3*(j/numdays)+1],tmp);
                                                rewind(essendata);
                                                fgets(tmp,300, essendata);
                                                cut2(tmp,"\"",6,6);
                                                strcpy(wocheplustagplusdaten[i][j%numdays][3*(j/numdays)+2], tmp);
                                                fclose(essendata);
                                        }
                                }
                        }
                        free(tmp);
                                //--> Die Daten für die Menüs für die wirklich zu bestellenden Tage sind abgespeichert.
                                // -->TODO: Die Nummer des entsprechendenden Desserts lässt sich aus der rad_ nummer herleiten ==>
                                // wenn die Suche nach der Nummer ergibt, dass die schon bereits grün ist, wird die nicht bestellt,
                                // ansonsten wird diese bestellt
                }
        }
}

void cuedussmann::getratingandbestelldaten()
{
        FILE* ratinglist;
        char** hackstring;
        for(int i=0; i<anzwoche;i++)
        {
                for(int j=0;j<7;j++)
                {
                        if(wirkbestellen[i][j]==1)
                        {
                                //Nun kommt Menü1, ich stelle fest, dass ich eine for-schleife machen kann:
                                for(int p=0;p<3;p++)
                                {
                                        if(strlen(wocheplustagplusdaten[i][j][3*p])==0) continue; //DAS IST SCHWACHSINN!!! //Dadurch bleiben die Rating für ein leeres Menü 2 0
                                        char* tmp=(char*)malloc(150);
                                        char* pch;
                                        int numwords=1;
                                        tmp=strcpy(tmp,wocheplustagplusdaten[i][j][3*p]);
                                        tmp=removeformattingsigns(tmp); //must be done to get the correct number of words
                                        pch=strtok(tmp," ,.1234567890");
                                        while(pch!=NULL)
                                        {
                                                pch=strtok(NULL," ,.1234567890");
                                                numwords++;
                                        }
                                        numwords--;
                                        hackstring=(char**)calloc(numwords,sizeof(char*));
                                        for(int k=0;k<numwords;k++) {hackstring[k]=(char*)malloc(70*sizeof(char)); strcpy(hackstring[k],"\0");}
                                        tmp=strcpy(tmp, wocheplustagplusdaten[i][j][3*p]); //<-- hier muss jetzt das mit den &amp; s und so weiter rein, damit das erst später zerhackt wird. Auch sollte immer noch ein Leerzeichen für ein Entferntes Ding reingebracht werden. Das wird ja eh wieder DANACH zur Trennung entfernt.
                                        tmp=removeformattingsigns(tmp);
                                        pch=strtok(tmp," ,.1234567890");
                                        strcpy(hackstring[0],pch);
                                        //hackstring[k]=strlwr(hackstring[0]); hier noch nicht, wegen der pseudosubstantiven adjektive (Szegediner Gulasch)
                                        for(int k=1;k<numwords;k++)
                                        {
                                                pch=strtok(NULL," ,.1234567890");
                                                strcpy(hackstring[k],pch);
                                        }
                                        for(int k=numwords-1;k>0;k--)//--> Bindestrichwörter werden geconcatenated ==> mehr leere strings.
                                        {
                                                if( hackstring[k][strlen(hackstring[k])-1] == '\n') hackstring[k][strlen(hackstring[k])-1]='\0'; //TODO: das ist dann nicht mehr, wenn das Todo gemacht wurde!!!
                                                if(hackstring[k-1][strlen(hackstring[k-1])-1]=='-')
                                                {
                                                        hackstring[k-1]=strcat(hackstring[k-1],hackstring[k]);
                                                        for(int m=k;m<numwords-1;m++)
                                                        {
                                                                strcpy(hackstring[m],hackstring[m+1]);
                                                        }
                                                        strcpy(hackstring[numwords-1],"\0");
                                                        //strcpy(hackstring[k],"\0"); <-- Das war doof, nun sind alle hackstrings "hintereinander", endene "\0"-en !!
                                                } //Bindestriche zusammenführen
                                        } //DONE: " im " , " nach " nicht löschen, sowie die substantivierten Adjektive. diese Bindewörter für concatenation benützen
                                        for(int k=1;k<numwords-1;k++) //jetzt kommt die Entfernung von "mit", "dazu", "und", "an"
                                        {
                                                if((strlen(hackstring[k])<=4) && (hackstring[k][0]!='\0'))
                                                {
                                                        if((strstr(hackstring[k],"mit")!=NULL) || (strstr(hackstring[k],"dazu")) ||(strstr(hackstring[k],"und")!=NULL) ||(((strlen(hackstring[k])==2)) && (strstr(hackstring[k],"an")!=NULL)))
                                                        {
                                                                strcpy(hackstring[k],"\0");
                                                        } //--> die hackstrings, die noch zusammengehören wegen adjektive oder im , nach sind noch
                                                          //"hintereinander", auch wenn es zwischen diesen "Komplexen" '\0'-er Lücken gibt. --> bei
                                                          //Bindestrichen muss aber nachgerückt werden!!!! Habe ich gemacht!
                                                }
                                        }
                                        /*for(int k=0;k<numwords-1;k++) //nun die "<br />" s entfernen, falls vorhanden
                                        {
                                            if((strstr(hackstring[k],"<br")!= NULL) && (strstr(hackstring[k+1],"/>")!=NULL))
                                            {
                                                for(int m=k;m<numwords-2;m++)
                                                {
                                                    strcpy(hackstring[m],hackstring[m+2]);
                                                }
                                                strcpy(hackstring[numwords-1],"\0");
                                                strcpy(hackstring[numwords-2],"\0");
                                            }
                                        }*/ //TODO: 	new tactic is needed! BEFORE hacking the string into pieces, delete all <br /> and &amp; 's since they then appear as coherent string.
                                          // 		therefore we seek if there are any of those and then remove them --> new function in function. with strstr find first appearance and then shift all cha
                                          //		characters 5 fields left, in both cases, hell yeah!: DONE
                                        for(int k=0;k<numwords;k++) //nun die "&amp;" s entfernen
                                        {
                                                if(strstr(hackstring[k],"&amp;")!=NULL)
                                                {
                                                        for(int m=k;m<numwords-1;m++)
                                                        {
                                                                strcpy(hackstring[m],hackstring[m+1]);
                                                        }
                                                        strcpy(hackstring[numwords-1],"\0");
                                                }
                                        }
                                        int start=0;
                                        int count=0;
                                        while(count<numwords)
                                        {
                                                if(strlen(hackstring[count])>0)
                                                {
                                                        start=count;
                                                        count++;//der nachfolgende String soll erst angehängt werden, nicht der gleiche zweimal
                                                        while((count<numwords) && (strlen(hackstring[count])>0))//nur die nichtleeren Strings sollen zusammengefügt werden
                                                        {
                                                                hackstring[start]=strcat(hackstring[start],hackstring[count]);//die nachfolgend Strings anhängen, bis ein leerer String erscheint
                                                                strcpy(hackstring[count],"\0");//Wenn dieser String kopiert wurde, ihn löschen
                                                                count++; //bereit für den nächsten String
                                                        }
                                                } else count++; //wenn dieser String gerade leer ist weiter machen bis man einen nichtleeren String findet
                                        } //wenn jetzt nichts schiefgegangen ist, dann sind die String fertig verbunden ;-)
                                        //jetzt kommt die bewertung:

                                        float bew=0;
                                        int summand=0;
                                        char* bewertung=(char*)malloc(5); strcpy(bewertung,"\0");
                                        int foodcount=0;
                                        char* alllow=(char*)malloc(70*sizeof(char));
                                        char* storing=(char*)malloc(100); strcpy(storing,"\0");
                                        char* ratingfilename=(char*)malloc(15); strcpy(ratingfilename,"ratings_");
                                        strcat(ratingfilename,uid);
                                        for(int k=0; k<numwords;k++)
                                        {
                                                strcpy(storing,"\0"); strcpy(bewertung,"\0");
                                                summand=0;
                                                strcpy(alllow, hackstring[k]);
                                                strlwr(alllow);
                                                if((strlen(hackstring[k])>0) && (core::find(ratingfilename,alllow)))
                                                {
                                                        foodcount++;
                                                        tmp=frstln(tmp,100,"findoutput");
                                                        tmp=cut2(tmp," ",1,1);
                                                        bewertung=strcpy(bewertung,tmp);
                                                        for(int n=0;n<strlen(bewertung);n++)
                                                        {
                                                                summand=10*summand+(bewertung[n]-48)*1;
                                                        }
                                                        bew+=summand;
                                                }
                                                if((strlen(hackstring[k])>0) && (core::find(ratingfilename,alllow)==0))
                                                {
                                                        foodcount++;
                                                        RateDialog *rate = new RateDialog(0,QString::fromLatin1(hackstring[k]));
                                                        rate->exec();
                                                        summand=rate->ui->spinBox->value();
                                                        delete rate;
                                                        ratinglist=fopen(ratingfilename,"a+");
                                                        if(summand<10)
                                                        {
                                                                strcpy(storing,"  \0");
                                                                storing[0]=48+summand;
                                                        }
                                                        if(summand==10) strcpy(storing,"10");
                                                        storing=strcat(storing," ");
                                                        storing=strcat(storing,alllow);
                                                        storing=strcat(storing,"\n");
                                                        fputs(storing,ratinglist);
                                                        fclose(ratinglist);
                                                        bew+=summand;
                                                }
                                        }
                                        if(ratings[i][j][p]<=10) ratings[i][j][p]=bew/foodcount;
                                        free(tmp);
                                        free(ratingfilename);
                                        free(alllow);
                                        free(bewertung);
                                        free(hackstring);
                                        free(storing);
                                }

                        }

                }
        }

        for(int i=0; i<anzwoche;i++)
        {
                for(int j=0;j<7;j++)
                {
                        if(wirkbestellen[i][j]==1)
                        {
                                if((ratings[i][j][0]>=ratings[i][j][1]) && (ratings[i][j][0]>=ratings[i][j][2]))
                                {
                                        strcpy(bestelldaten[i][j],wocheplustagplusdaten[i][j][1]);
                                        strcat(bestelldaten[i][j],"=");
                                        strcat(bestelldaten[i][j],wocheplustagplusdaten[i][j][2]);
                                }
                                if((ratings[i][j][1]>=ratings[i][j][0]) && (ratings[i][j][1]>=ratings[i][j][2]))
                                {
                                        strcpy(bestelldaten[i][j],wocheplustagplusdaten[i][j][4]);
                                        strcat(bestelldaten[i][j],"=");
                                        strcat(bestelldaten[i][j],wocheplustagplusdaten[i][j][5]);
                                }
                                if((ratings[i][j][2]>=ratings[i][j][1]) && (ratings[i][j][2]>=ratings[i][j][0]))
                                {
                                        strcpy(bestelldaten[i][j],wocheplustagplusdaten[i][j][7]);
                                        strcat(bestelldaten[i][j],"=");
                                        strcat(bestelldaten[i][j],wocheplustagplusdaten[i][j][8]);
                                }
                        }
                }
        }
        for(int i=0;i<anzwoche;i++)
        {
                for(int j=0;j<7;j++)
                {
                        if(wirkbestellen[i][j]==1)	printf("%s \n", bestelldaten[i][j]);
                }
        }
}

void cuedussmann::sendbestellung() //hier muss sowohl das Senden der daten für die Woche, als auch das senden der bestätigung!!!, //praktisch die letzte Funktion auf dem Schlachtfeld
{
        int bestellsumme=0; //summiert einfach die wirkbestellen, sodass man weiß, ob diese woche überhaupt bestellt werden soll
        char postfield[1000];
        strcpy(postfield,"\0");
        char* menufilename=(char*)malloc(8);
        char* menunumber=(char*)malloc(3);
        FILE* bestbest1[anzwoche]; //Bestellbestätigung 1;
        FILE* bestbest2[anzwoche]; //Der User soll nichts davon sehen...
        for(int i=0;i<anzwoche;i++) //Das ist doof, wir sollten bereits hier schon unterscheiden, ob für die Woche überhaupt bestellt werden soll :DONE Done
        {
                bestellsumme=0;
                for(int j=0;j<7;j++) bestellsumme+=wirkbestellen[i][j];
                if(bestellsumme>0)
                {
                        strcpy(menufilename,"menu");
                        menunumber[0]=48+i; menunumber[1]='\0';
                        strcat(menufilename,menunumber); strcat(menufilename,"\0");
                        strcpy(postfield,"\0");
                        strcpy(postfield,hidden[i][0]);

                        for(int j=1;j<35;j++)
                        {
                                if(strlen(hidden[i][j])>0)
                                {
                                        strcat(postfield,"&");
                                        strcat(postfield,hidden[i][j]);
                                }
                        }
                        strcat(postfield,"&");
                        for(int j=0;j<35;j++)
                        {
                                if(strlen(bergruen[i][j])>0)
                                {
                                        strcat(postfield,"&");
                                        strcat(postfield,bergruen[i][j]);
                                }
                        }
                        for(int j=0;j<35;j++)
                        {
                                if(strlen(bergruend[i][j])>0)
                                {
                                        strcat(postfield,"&");
                                        strcat(postfield,bergruend[i][j]);
                                }
                        }
                        for(int j=0;j<7;j++)
                        {

                                if(wirkbestellen[i][j]==1)
                                {
                                        strcat(postfield,"&");
                                        strcat(postfield,bestelldaten[i][j]);
                                }
                        }
                        for(int j=0;j<35;j++) //nun kommen die fehlenden Desserts, hier menü bereits grün, aber dessert noch nicht
                        {
                                if((strlen(bergruen[i][j])>0) && (strstr(bergruen[i][j],"rad_")!=NULL)/* bereits grün, sodass es rad enthält, die nummer, davon das entsprechende fld, gucken, ob das schon grün ist*/)//(wirkbestellen([i][j]==1) //das ist schwachsinn!//nun die Desserts:-->menünamen benötigt.
                                {
                                        char* dessertstring=(char*)malloc(25);strcpy(dessertstring,"\0");
                                        char* tmp=(char*)malloc(150); tmp=strcpy(tmp,bergruen[i][j]);
                                        tmp=cut2(tmp,"_",2,3);
                                        tmp=cut2(tmp,"=",1,1);
                                        strcpy(dessertstring,"fld_");
                                        strcat(dessertstring,tmp);
                                        strcat(dessertstring,"_4");
                                        core::find(menufilename,dessertstring);
                                        if((core::find("findoutput","gruen pointer")==0)) //&& (strlen(cut2(frstln(tmp,150,"findoutput"),">",2,2))>0))
                                        {
                                                strcat(postfield,"&");
                                                strcat(postfield,dessertstring);
                                                strcat(postfield,"=1");
                                        }
                                        free(dessertstring);
                                        free(tmp);
                                }
                        }
                        for(int j=0;j<7;j++)//hier zu bestellendes Menü und gucken, ob dessert schon bestellt ist oder noch nicht, wenn nicht, dann halt an postfield anhängen	/*if(das gleiche wie oben, bloß bei dem bestelldata menü die nummer nehmen, dann fld gucken, ob das schon grün ist) */
                        {
                                if(wirkbestellen[i][j]==1)
                                {
                                        char* dessertstring=(char*)malloc(25);strcpy(dessertstring,"\0");
                                        char* tmp=(char*)malloc(150); tmp=strcpy(tmp,bestelldaten[i][j]);
                                        tmp=cut2(tmp,"_",2,3);
                                        tmp=cut2(tmp,"=",1,1);
                                        strcpy(dessertstring,"fld_");
                                        strcat(dessertstring,tmp);
                                        strcat(dessertstring,"_4");
                                        core::find(menufilename,dessertstring);
                                        if((core::find("findoutput","gruen pointer")==0)) //&& (strlen(cut2(frstln(tmp,150,"findoutput"),">",2,2))>0))
                                        {
                                                strcat(postfield,"&");
                                                strcat(postfield,dessertstring);
                                                strcat(postfield,"=1");
                                        }
                                        free(dessertstring);
                                        free(tmp);
                                }
                        }//nun ist das POST-Field fertig, jetzt kommt das Senden und das empfangen der neuen Datei.
                        //Da wegen der ersten if-Bedingung eine Bestellung erfolgen muss( scheiße, was ist, wenn nur menü gruen, aber keine desserts???, hmm, die desserts werden dann nicht mehr bestellt)	stimmt. da hat man halt pech, kann weiter entwickelt werden, es geht um das KONZEPT

                        //--------------------------------------------------------------------------------------------
                        //Die erste Bestätigung herunterladen
                        //--------------------------------------------------------------------------------------------
                        printf("Der erste postcode: %s",postfield);
                        fflush(stdout);
                        strcpy(menufilename,"bstst"); //Das soll die erste Bestellbestätigung werden;
                        menunumber[0]=48+i; menunumber[1]='\0';
                        strcat(menufilename,menunumber); strcat(menufilename,"\0");
                        bestbest1[i]=fopen(menufilename,"w");
                        CURLcode ret; //achtung, diese mehrfachen rets führen zu fehlern--> umbenennen
                        CURL *hnd = curl_easy_init();
                        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, bestbest1[i]);
                        curl_easy_setopt(hnd, CURLOPT_INFILESIZE_LARGE, -1);
                        curl_easy_setopt(hnd, CURLOPT_URL, "http://dussmann-lpf.rcs.de/index.php?m=150;0;1;3");
                        curl_easy_setopt(hnd, CURLOPT_PROXY, NULL);
                        curl_easy_setopt(hnd, CURLOPT_PROXYUSERPWD, NULL);
                        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postfield);
                        curl_easy_setopt(hnd, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux i686; de; rv:1.9.2.12) Gecko/20101027 Firefox/3.6.12");
                        curl_easy_setopt(hnd, CURLOPT_COOKIEFILE, "Cookiedatei");
                        curl_easy_setopt(hnd, CURLOPT_COOKIEJAR, NULL);
                        ret = curl_easy_perform(hnd);
                        curl_easy_cleanup(hnd);
                        fclose(bestbest1[i]);

                        //------------------------------------------------------------------------------------------
                        //Die hidden-values wegen bestellen und so aus der ersten Bestätigung extrahieren
                        //------------------------------------------------------------------------------------------
                        /*
                         * action string extrahieren, damit man sich die Suche nach starttag sparen kann
                         * Achtung, die bereits gespeicherten Daten wegen hidden und so nicht nochmal schicken, da dort auch falsche Daten drin sind
                         *  ==> nochmal die neuen hidden werte ermitteln, auf js_hidden aufpassen (nach "hidden suchen) --> bis inklusive zeile bestellpreis einlesen, den rest
                         *      nicht mehr als hidden-zeugs abspeichern
                         * um die stelle für schneiden wegen bestellenfoo zu ermitteln einfach ein strstr(zeile,"best_") machen
                         */
                        char* actionurl=(char*)malloc(120); strcpy(actionurl,"http://dussmann-lpf.rcs.de/index.php?m=150;0;1;3&a=akt_bestellen&");
                        char** newhidden;
                        int lnstillpreis=0;
                        FILE* hiddenposts;
                        char* tmp=(char*)malloc(150); strcpy(tmp,"\0");
                        char* tmp2=(char*)malloc(150);strcpy(tmp2,"\0");
                        core::find(menufilename,"<form action="); //still the same name
                        hiddenposts=fopen("findoutput","r");
                        fgets(tmp, 150,hiddenposts);
                        fclose(hiddenposts);
                        tmp=cut2(tmp,"\"",2,2);
                        tmp=cut2(tmp,";",6,6);
                        strcat(actionurl,tmp);
                        strcat(actionurl,"\0");
                        core::find(menufilename,"\"hidden");
                        hiddenposts=fopen("findoutput","r");
                        do
                        {
                                lnstillpreis++;
                        }while(strstr(fgets(tmp,150,hiddenposts),"bestellpreis")==NULL);
                        newhidden=(char**)calloc(lnstillpreis,sizeof(char*));
                        rewind(hiddenposts);
                        for(int k=0; k<lnstillpreis;k++) newhidden[k]=(char*)malloc(30);
                        for(int k=0;k<lnstillpreis;k++)
                        {
                                fgets(tmp,150,hiddenposts);
                                tmp2=strcpy(tmp2,tmp);
                                if(strstr(tmp,"best_")!=NULL)
                                {
                                        tmp=cut2(tmp,"\"",6,6);
                                        strcpy(newhidden[k],tmp);
                                        strcat(newhidden[k],"=");
                                        strcat(newhidden[k],"\0");
                                }
                                else if(strstr(tmp,"guthaben")==NULL)
                                {
                                        tmp=cut2(tmp,"\"",4,4);
                                        strcpy(newhidden[k],tmp);
                                        strcat(newhidden[k],"=");
                                        tmp2=cut2(tmp2,"\"",6,6);
                                        strcat(newhidden[k],tmp2);
                                        strcat(newhidden[k],"\0");
                                } else if(strstr(tmp,"guthaben")!=NULL)
                                {
                                        strcpy(newhidden[k],"guthaben=");
                                        strcat(newhidden[k],"\0");
                                }
                        }

                        //--------------------------------------------------------------------------------------
                        // Nun kommt das erstellen von postfield
                        //--------------------------------------------------------------------------------------
                        strcpy(postfield,newhidden[0]);
                        for(int k=1; k<lnstillpreis;k++)
                        {
                                strcat(postfield,"&");
                                strcat(postfield,newhidden[k]);
                        }
                        strcat(postfield,"\0");

                        //-------------------------------------------------------------------------------------
                        // Nun kommt das Senden dieses postfields an die zuvor ermittelte action-url
                        // gespeichert wird in einer extra-Datei
                        //-------------------------------------------------------------------------------------
                        strcpy(menufilename,"bstbt"); //Das soll die zweite Bestellbestätigung werden;
                        menunumber[0]=48+i; menunumber[1]='\0';
                        strcat(menufilename,menunumber); strcat(menufilename,"\0");
                        bestbest2[i]=fopen(menufilename,"w");
                        CURLcode ret2;
                        CURL *hnd2 = curl_easy_init();
                        curl_easy_setopt(hnd2, CURLOPT_WRITEDATA, bestbest2[i]);
                        curl_easy_setopt(hnd2, CURLOPT_INFILESIZE_LARGE, -1);
                        curl_easy_setopt(hnd2, CURLOPT_URL, actionurl);
                        curl_easy_setopt(hnd2, CURLOPT_PROXY, NULL);
                        curl_easy_setopt(hnd2, CURLOPT_PROXYUSERPWD, NULL);
                        curl_easy_setopt(hnd2, CURLOPT_POSTFIELDS, postfield);
                        curl_easy_setopt(hnd2, CURLOPT_REFERER, "http://dussmann-lpf.rcs.de/index.php?m=150;0;1;3");
                        curl_easy_setopt(hnd2, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; U; Linux i686; de; rv:1.9.2.12) Gecko/20101027 Firefox/3.6.12");
                        curl_easy_setopt(hnd2, CURLOPT_COOKIEFILE, "Cookiedatei");
                        curl_easy_setopt(hnd2, CURLOPT_COOKIEJAR, NULL);
                        ret2 = curl_easy_perform(hnd2);
                        curl_easy_cleanup(hnd2);
                        fclose(bestbest2[i]);
                        fclose(hiddenposts);
                        free(actionurl);
                        free(newhidden);
                        free(tmp);
                        free(tmp2);
                        printf("Das ist der zweite Postcode: %s",postfield);
                }

        } //STOPP, diese for-schleife darf ich nicht verlassen, da das postfield kein array ist!!
        //free(postfield);
        free(menunumber);
        free(menufilename);
}

void cuedussmann::on_actionEssen_bestellen_lassen_triggered() //now big fat routines are following
{
    gethiddenandbestellt(); //save data in the hidden, bergruen and bergruend arrays
    getdatensatz();
    getratingandbestelldaten();
    sendbestellung();
    free(bestelldaten);
    free(wocheplustagplusdaten);
    anzwoche=0;startwoche=0;
    free(wirkbestellen);
    free(setdates);
    free(slynmbwochen);
    free(changedmenu);
    free(ratings);
    initialized=0;
    initialized=initialize();

}

QString cuedussmann::nameday(int day)
{
    switch(day%7)
    {
    case 0: return QString("Montag\t"); break;
    case 1: return QString("Dienstag\t"); break;
    case 2: return QString("Mittwoch\t"); break;
    case 3: return QString("Donnerstag\t"); break;
    case 4: return QString("Freitag\t");break;
    case 5: return QString("Samstag\t"); break;
    case 6: return QString("Sonntag\t"); break;
    }
}

void cuedussmann::on_actionSpeiseplan_drucken_triggered() //how to access to all names? --> parsemenufile!!!
{
    FILE* druckplan;
    QString planname("Speiseplan_");
    planname.append(QString::fromLocal8Bit(uid).simplified());
    druckplan=fopen(qPrintable(planname),"w+");
    for(int i=0; i<comboBox->count();i++)
    {
        parsemenufile(i);
        fputs(qPrintable(comboBox->itemText(i).append("\n")),druckplan);
        fputs("---------------------------------------\n",druckplan);
        for(int j=0;j<7;j++)
        {
            for(int k=0;k<3;k++)
            {
                if(tableWidget->item(k,j)->background().color().green()==255) fputs( qPrintable(nameday(j).append(QString::fromLocal8Bit("Menü ")).append(QString::number(k+1)).append("\t").append( tableWidget->item(k,j)->text().simplified().append("\n") ) ),druckplan );
            }
        }
        fputs("\n",druckplan);
    }
    fclose(druckplan);
    QFile printplan(planname);
    QTextDocument* foodplan = new QTextDocument();
    if(printplan.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream plantext(&printplan);
        foodplan->setPlainText(plantext.readAll());
        printplan.close();
    }
    foodplan->setDefaultFont(QFont(QString("Times New Roman"),10));
    QPrinter printer;
    printer.setPageMargins(20,20,20,20,QPrinter::Millimeter);
        QPrintDialog *dlg = new QPrintDialog(&printer, this);
        if (dlg->exec() != QDialog::Accepted)
            return;

        foodplan->print(&printer);
        delete dlg;
        delete foodplan;

}

void cuedussmann::on_actionBedienungshinweise_triggered()
{
    HowToUse *help = new HowToUse();
    help->show();
}

void cuedussmann::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void cuedussmann::on_action_ber_triggered()
{
    Thanks *thank = new Thanks();
    thank->show();
}
