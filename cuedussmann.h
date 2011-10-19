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

#ifndef CUEDUSSMANN_H
#define CUEDUSSMANN_H

#include <QMainWindow>
#include"ui_cuedussmann.h"
class cuedussmann : public QMainWindow, public Ui::cuedussmann
{
    Q_OBJECT

public:
    explicit cuedussmann(QWidget *parent = 0);
    int wegonnaquit;

private slots:

    void on_actionUID_PWD_ndern_triggered();
    void parsemenufile(int itemindex);

    void on_checkBox_8_clicked();

    void on_checkBox_clicked();

    void on_checkBox_2_clicked();

    void on_checkBox_3_clicked();

    void on_checkBox_4_clicked();

    void on_checkBox_5_clicked();

    void on_checkBox_6_clicked();

    void on_checkBox_7_clicked();

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_actionEssen_bestellen_lassen_triggered();

    void on_actionSpeiseplan_drucken_triggered();

    void on_actionBedienungshinweise_triggered();

    void on_actionAbout_Qt_triggered();

    void on_action_ber_triggered();

    void on_actionProxy_einstellen_triggered();

private:
    int initialized;
    int initialize();
    int loadPWDUID();
    int kalwochen();
    void getsel_datums();
    void setcombobox(int startweek, int endweek);
    void createmenufiles();
    void getdatensatz();
    int getratingandbestelldaten();
    void sendbestellung();
    QString nameday(int day);
    //int find(const char inputfile[], const char searchstring[], int linesafter=0);


};

#endif // CUEDUSSMANN_H
