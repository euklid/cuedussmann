#ifndef CUEDUSSMANN_H
#define CUEDUSSMANN_H

#include <QMainWindow>
#include"ui_cuedussmann.h"
class cuedussmann : public QMainWindow, public Ui::cuedussmann
{
    Q_OBJECT

public:
    explicit cuedussmann(QWidget *parent = 0);

private slots:

    void on_actionUID_PWD_ndern_triggered();
    void parsemenufile(int itemindex);

private:
    int loadPWDUID();
    int kalwochen();
    void getsel_datums();
    void setcombobox(int startweek, int endweek);
    void createmenufiles();
    int find(const char inputfile[], const char searchstring[], int linesafter=0);


};

#endif // CUEDUSSMANN_H
