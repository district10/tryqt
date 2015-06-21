#include "lmsreader.h"


LMSReader::LMSReader(QTcpSocket *parent) : QTcpSocket(parent)
{
    lmsAction[GET_FREQ_ANGRES] = "sRN LMPscancfg";
    lmsAction[GET_ANGBEG_ANGEND] = "sRN LMPoutputRange";
    lmsAction[TURN_ON] = "sEN LMDscandata 1";
    lmsAction[TURN_OFF] = "sEN LMDscandata 0";

    lmsState = LMS_OFF;
    lmsIP = "192.168.0.184";
    lmsPort = 2111;

    lmsFrequency = 50.0;
    lmsAngleBeg = -45.0;
    lmsAngleEnd = 225.0;
    lmsAngleRes = 0.5;

    sz = 0;

    connect(this, SIGNAL(readyRead()),
            this, SLOT(readLMS()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(lmsConnectionError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(callToTurnOnLMS()),
            this, SLOT(turnOn()));
    connect(this, SIGNAL(calltoTurnOffLMS()),
            this, SLOT(turnOff()));
    connect(timeout, SIGNAL(timeout()),
            this, SLOT(turnOff()));

    // timeout->start(2000);
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
    // if (LMS_OFF == lmsState) { return; }

    char buf[BUFSIZE];
    read(buf, BUFSIZE);
    QString bufstr(buf);
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

    writeLMS(lmsAction[GET_FREQ_ANGRES]);
    // writeLMS(lmsConfig[CONFIG_ANGSTART_ANGSTOP]);

    turnOn();
   // turnOff();

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
    writeLMS(lmsAction[TURN_ON]);
    lmsState = LMS_ON;
}

void LMSReader::turnOff()
{
    writeLMS(lmsAction[TURN_OFF]);
    lmsState = LMS_OFF;
}

void LMSReader::saveData(QString filename, char* buf)
{
    QFile file( filename );
    if ( file.open(QIODevice::Append) ) {
        QTextStream stream( &file );
        stream.setCodec("UTF-8");
        stream << buf;
    }
}

void LMSReader::parseLMSScan(string ifilename, sting ofilename)
{
        char STX = 0x2; // start of one Scan
        char ETX = 0x3; // end of one Scan
        int Yoffset = -1;

        ifstream infile(ifilename);
        ofstream outfile(ofilename, ios::out);
        if(!infile || !outfile) {
            cerr << "cannot open for read&write";
            return;
        }

        char scanBuf[BUFSIZE] = { 0 };
        int i = 0;

        while(!infile.eof())
        {
            do {
                infile.get(scanBuf[0]);
            } while (STX != scanBuf[0]);

            // save to scanBuf
            i = 0;
            do {
                ++i;
                infile.get(scanBuf[i]);
            } while (ETX != scanBuf[i]);

            QString scanBufString(scanBuf);
            QStringList CurrentScan = scanBufString.split(" ");

            // not data
            if ("sSN" != CurrentScan.at(0).toStdString().erase(0, 1) ||
                "LMDscandata" != CurrentScan.at(1)) {
                continue;
            }

            // data
            ++Yoffset;
            int offset = 25;
            int sanNum = 0;
            int distance = 0.0;

            double angle = 0.0;
            sscanf_s(CurrentScan.at(offset).toStdString().c_str(), "%x", &sanNum);
            qDebug() << "#Points in this scan: " << sanNum;
            offset++;
            for ( int i = 0; i < sanNum; ++i ) {
                sscanf_s(CurrentScan.at(i + offset).toStdString().c_str(), "%x", &distance);
                angle = (lmsAngleBeg + i * lmsAngleRes) / 180.0 * M_PI;
                outfile
                    << cos(angle) * distance << "       "
                    << Yoffset << "      "
                    << sin(angle) * distance << endl;
            }
        }
        outfile.close();
        infile.close();
        qDebug() << "done";
}
