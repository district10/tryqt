#include "lmsreader.h"

lmsReader::lmsReader(QTcpSocket *parent) : QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(read()));

}

void lmsReader::send(QString data)
{
    QByteArray by;
    by.append(0x02);
    by.append(data);
    by.append(0x03);
    qDebug() << by;
    write(by);
}

void lmsReader::test()
{
    send("hello a message");
}

void lmsReader::read()
{
        QDataStream in(this);
        in.setVersion(QDataStream::Qt_4_3);
}

