#include "lmsreader.h"

LMSReader::LMSReader(QTcpSocket *parent) : QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readLMS()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(lmsConnectionError(QAbstractSocket::SocketError)));

    lmsConfig[CONFIG_FREQ_ANGRES] = "sRN LMPscancfg";
    lmsConfig[CONFIG_ANGSTART_ANGSTOP] = "sRN LMPoutputRange";
    lmsConfig[OPEN] = "sEN LMDscandata 1";
    lmsConfig[CLOSE] = "sEN LMDscandata 0";

    lmsState = LMS_OFF;
    lmsIP = "127.0.0.1";
    lmsPort = 8000;
}

LMSReader::~LMSReader()
{
    // close connection
    closeConnection();
}

void LMSReader::writeLMS(QString data)
{
    QByteArray by;
    by.append(0x02);
    by.append(data);
    by.append(0x03);
    write(by);
}

void LMSReader::readLMS()
{
        qDebug() << "readLMS";
        // qDebug() << this->bytesWritten();
        char buf[100];
        read(buf, 99);
        qDebug() << buf;
}

void LMSReader::lmsConnectionError(QAbstractSocket::SocketError err)
{
        qDebug() << "LMS Connetion Error occured.";
        qDebug() << err;
}

void LMSReader::lmsConnectionEstablished()
{
        qDebug() << "connection Established, avaible bytes: " << this->bytesAvailable();
}

void LMSReader::test()
{
    connectToLMS();

   // closeConnection();
  //  emit readyRead();
    for (int i = 0 ; i < 3; i++) {
        QString tmp(i+1, (char)('A' + i));
       writeLMS(tmp);
    }

    turnOn();
    turnOff();
}

void LMSReader::connectToLMS()
{
    qDebug() << "<-- connectLMS";
    this->connectToHost( lmsIP, lmsPort);
}

void LMSReader::closeConnection()
{
    if ( this->isOpen() ) {
        this->close();
    }
    qDebug() << "connection closed";
}

void LMSReader::turnOn()
{
    writeLMS(lmsConfig[OPEN]);
}

void LMSReader::turnOff()
{
    writeLMS(lmsConfig[CLOSE]);
}
