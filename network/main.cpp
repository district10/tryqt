#include <QCoreApplication>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include <QTcpSocket>

#include "lmsreader.h"






int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "good";


    lmsReader lr;
    lr.test();










    return a.exec();
}
