#include <QtGui/QApplication>
#include "cuedussmann.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    cuedussmann w;
    w.show();

    return a.exec();
}
