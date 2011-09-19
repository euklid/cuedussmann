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
private:
    int loadPWDUID();


};

#endif // CUEDUSSMANN_H
