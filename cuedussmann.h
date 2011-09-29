#ifndef CUEDUSSMANN_H
#define CUEDUSSMANN_H

#include <QMainWindow>
#include"ui_cuedussmann.h"
class cuedussmann : public QMainWindow, public Ui::cuedussmann
{
    Q_OBJECT

public:
    explicit cuedussmann(QWidget *parent = 0);;

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

private:
    void initialize();
    int loadPWDUID();
    int kalwochen();
    void getsel_datums();
    void setcombobox(int startweek, int endweek);
    void createmenufiles();
    int find(const char inputfile[], const char searchstring[], int linesafter=0);


};

#endif // CUEDUSSMANN_H
