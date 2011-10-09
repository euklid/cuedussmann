#include <QtGui/QApplication>
#include "cuedussmann.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    cuedussmann w;
    if(w.wegonnaquit==1) return 0;
    w.show();

    return a.exec();
}
