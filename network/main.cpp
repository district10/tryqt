#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QTcpSocket>
#include <iostream>
#include <string>
#include <cstdlib>

#include "lmsreader.h"

using std::string;
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LMSReader lmsreader;
    lmsreader.parseLMSScan("G:/lms2.txt", "G:/lms4.txt");

    return a.exec();
}
