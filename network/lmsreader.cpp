#include "lmsreader.h"

LMSReader::LMSReader(QTcpSocket *parent) : QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readLMS()));
    connect(this, SIGNAL(hello(QString)),
            this, SLOT(world(QString)));
    // connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),



    lmsConfig[CONFIG_FREQ_ANGRES] = "sRN LMPscancfg";
    lmsConfig[CONFIG_ANGSTART_ANGSTOP] = "sRN LMPoutputRange";
    lmsConfig[OPEN] = "sEN LMDscandata 1";
    lmsConfig[CLOSE] = "sEN LMDscandata 0";

    lmsState = LMS_OFF;
    // lmsIP = "127.0.0.1";
    lmsIP = "localhost";
    lmsPort = 8000;
}

void LMSReader::send(QString data)
{
    QByteArray by;
    by.append(0x02);
    by.append(data);
    by.append(0x03);
    qDebug() << by;
    write(by);
}


void LMSReader::readLMS()
{
    QTextStream in(this);
//    in.setVersion(QDataStream::Qt_4_0);
    QString msg;
    in >> msg;
    qDebug() << msg;
    qDebug() << "readLMS";

    QString filename = "/home/tzx/Data.txt";
    QFile file( filename );
    if ( file.open(QIODevice::Append) ) {
    // if ( file.open(QIODevice::ReadWrite) ) {
        QTextStream stream( &file );
        stream.setCodec("UTF-8");
        stream <<  std::rand() % 200 << endl;
    }
}

void LMSReader::world(QString msg)
{
    qDebug() << msg;
}



void LMSReader::test()
{
    qDebug() << lmsConfig[OPEN];
    emit hello(QString("hello world via signal/slot & emit"));
    connectLMS();
}

void LMSReader::connectLMS()
{
    connectToHost(lmsIP,
                  lmsPort);
}
