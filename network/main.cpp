#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QTcpSocket>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "lmsreader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LMSReader lmsreader;
    lmsreader.test();

    return a.exec();
}
