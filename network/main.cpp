#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QTcpSocket>

#include <iostream>
#include <string>
#include <cstdlib>

#include "lmsreader.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LMSReader lmsreader;
    lmsreader.test();

    return a.exec();
}
