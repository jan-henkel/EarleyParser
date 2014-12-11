#include "earleyparsermainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EarleyParserMainWindow w;
    w.show();

    return a.exec();
}
