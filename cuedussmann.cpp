#include "cuedussmann.h"
#include "dussmanbot.cpp"
#include "pwduidialog.h"
#include <QString>
#include <QMessageBox>
using namespace core;
int*** changedmenu;
cuedussmann::cuedussmann(QWidget *parent) :
    QMainWindow(parent)
{
    initialized=0;
    setupUi(this);
    initialized=initialize();
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
    while(loginandcookie(uid, pwd)==0)
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if((tableWidget->item(i,0)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,0)->flags()!=Qt::NoItemFlags)) needed--;
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if((tableWidget->item(i,1)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,1)->flags()!=Qt::NoItemFlags)) needed--;
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if((tableWidget->item(i,2)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,2)->flags()!=Qt::NoItemFlags)) needed--;
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if((tableWidget->item(i,3)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,3)->flags()!=Qt::NoItemFlags)) needed--;
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if((tableWidget->item(i,4)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,4)->flags()!=Qt::NoItemFlags)) needed--;
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if((tableWidget->item(i,5)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,5)->flags()!=Qt::NoItemFlags)) needed--;
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if((tableWidget->item(i,6)->background().style()==Qt::SolidPattern) && (tableWidget->item(i,6)->flags()!=Qt::NoItemFlags)) needed--;
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,0)->background().style()==Qt::SolidPattern) needed--;
            if((tableWidget->item(i,0)->background().color().green()>0) || (tableWidget->item(i,0)->background().color().red()<255)) needed--;
        }
        if(needed>=0)
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,1)->background().style()==Qt::SolidPattern) needed--;
            if((tableWidget->item(i,1)->background().color().green()>0) || (tableWidget->item(i,1)->background().color().red()<255)) needed--;
        }
        if(needed>=0)
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,2)->background().style()==Qt::SolidPattern) needed--;
            if((tableWidget->item(i,2)->background().color().green()>0) || (tableWidget->item(i,2)->background().color().red()<255)) needed--;
        }
        if(needed>=0)
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,3)->background().style()==Qt::SolidPattern) needed--;
            if((tableWidget->item(i,3)->background().color().green()>0) || (tableWidget->item(i,3)->background().color().red()<255)) needed--;
        }
        if(needed>=0)
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,4)->background().style()==Qt::SolidPattern) needed--;
            if((tableWidget->item(i,4)->background().color().green()>0) || (tableWidget->item(i,4)->background().color().red()<255)) needed--;
        }
        if(needed>=0)
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,5)->background().style()==Qt::SolidPattern) needed--;
            if((tableWidget->item(i,5)->background().color().green()>0) || (tableWidget->item(i,5)->background().color().red()<255)) needed--;
        }
        if(needed>=0)
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
        int needed=1;
        for(int i=0;i<3;i++)
        {
            if(tableWidget->item(i,6)->background().style()==Qt::SolidPattern) needed--;
            if((tableWidget->item(i,6)->background().color().green()>0) || (tableWidget->item(i,6)->background().color().red()<255)) needed--;
        }
        if(needed>=0)
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
