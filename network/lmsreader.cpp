#include "lmsreader.h"
#include <QMutex>

LMSReader::LMSReader()
{
    lmsAction[GET_FREQ_ANGRES] = "sRN LMPscancfg";
    lmsAction[SET_FREQ_ANGRES] = "sMN mLMPsetscancfg";
    lmsAction[GET_ANGBEG_ANGEND] = "sRN LMPoutputRange";
    lmsAction[SET_ANGBEG_ANGEND] = "sWN LMPoutputRange";
    lmsAction[POLLING_ONE] = "sRN LMDscandata";
    lmsAction[TURN_ON] = "sEN LMDscandata 1";
    lmsAction[TURN_OFF] = "sEN LMDscandata 0";
    lmsAction[GET_STATUS] = "sRN LCMstate";
    lmsAction[LOG_IN] = "sMN SetAccessMode";
    lmsAction[LOG_OUT] = "sMN Run";
    lmsAction[STAND_BY] = "sMN LMCstandby";
    lmsAction[START_MEASURE] = "sMN LMCstartmeas";
    lmsAction[STOP_MEASURE] = "sMN LMCstopmeas";
    lmsAction[REBOOT] = "sMN mSCreboot";
    lmsAction[GET_TIMESTAMP] = "sRN STlms";
    lmsAction[SET_TIMESTAMP] = "sMN LSPsetdatetime";
    lmsAction[SAVE_ALL] = "sMN mEEwriteall";

    lmsState = LMS_OFF;
    lmsReadMode = READ_TEST;
    lmsAddress = "192.168.0.184";
    lmsPort = 2111;
    lmsFrequency = 50.0;
    lmsAngleBeg = -45.0;
    lmsAngleEnd = 225.0;
    lmsAngleRes = 0.5;

    for(int i = 0; i < BUFNUM; ++i) {
        buf[i] = (char *)calloc(BUFSIZE, sizeof(buf[i][0])); // malloc
    }
    bufindex = 0;
    scancount = 0;

    lmsRawPath = "";

    connect(this, SIGNAL(readyRead()),
            this, SLOT(readLMS()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(lmsConnectionError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(callToTurnOnLMS()),
            this, SLOT(turnOn()));
    connect(this, SIGNAL(calltoTurnOffLMS()),
            this, SLOT(turnOff()));
}

LMSReader::~LMSReader()
{
    // close connection
    closeConnection();
    for(int i = 0; i < BUFNUM; ++i) {
        free(buf[i]);
    }
}

void LMSReader::writeLMS(QString data)
{
    QByteArray by;
    by.append(0x02);
    by.append(data);
    by.append(0x03);
    write(by);
}

void LMSReader::askforAuthorization()
{
    // set access mode to '03 authorized client'
    QString str;
    str.sprintf("%s %2d %s",
                 lmsAction[LOG_IN].toStdString().c_str(),
                 AUTHORIZED_CLIENT,
                 "F4724744");
    writeLMS(str); // 0: err, 1: success
}

void LMSReader::askforAngle_Beg_End()
{
    writeLMS(lmsAction[GET_ANGBEG_ANGEND]);
}

void LMSReader::askforFrequency_Angleresolution()
{
    writeLMS(lmsAction[GET_FREQ_ANGRES]);
}

void LMSReader::setFrequencyAngres(int freq, double angres)
{
    qDebug() << "LMSReader, set freq&angres: " << freq << ", " << angres;
    QString str;
    str.sprintf("%s %X %d %X %X %X",
                 lmsAction[SET_FREQ_ANGRES].toStdString().c_str(),
                 FREQUENCY_SCALE_FACTOR * (freq), // 25 or 50 Hz
                 1,
                 (int) (ANGLE_SCALE_FACTOR * (angres)), // 0.25 or 0.5 degree
                 (int) (ANGLE_SCALE_FACTOR * (-45 + 0)),
                 (int) (ANGLE_SCALE_FACTOR * (225 - 0))
                 );
    qDebug() << str;
    writeLMS(str);
}

void LMSReader::setAngleBegEnd(double start, double end)
{
    qDebug() << "LMSReader, set angle beg&end: " << start << ", " << end;
    if (start > end) { return; }
    if (start < - 45.0) { start = -45.0; }
    if (end > 225.0) { end = 225.0; }
    QString str;
    str.sprintf("%s %d %X %X %X",
                 lmsAction[SET_ANGBEG_ANGEND].toStdString().c_str(),
                 1, // status code
                 FREQUENCY_SCALE_FACTOR * (25),
                 (int) (ANGLE_SCALE_FACTOR * start),
                 (int) (ANGLE_SCALE_FACTOR * end)
                 );
    qDebug() << str;
    writeLMS(str);
}

void LMSReader::setTimestamp(quint16 year, quint8 month, quint8 day, quint8 hour, quint8 minute, quint8 second, quint8 microsecond)
{
    QString str;
    //               Y  M  D  H  M  S MS
    str.sprintf("%s %X %X %X %X %X %X %X",
                lmsAction[SET_TIMESTAMP].toStdString().c_str(),
                year, month, day,
                hour, minute, second, microsecond);
    qDebug() << str;
    writeLMS(str);
}

void LMSReader::getTimestamp()
{
    writeLMS(lmsAction[GET_TIMESTAMP]);
}

void LMSReader::logout()
{
    writeLMS(lmsAction[LOG_OUT]);
}

void LMSReader::readLMS()
{
    if (this->bytesAvailable() > BUFSIZE) {

        cerr << "BUFSIZE too small.";

        do {
            read(buf[bufindex], BUFSIZE);
        } while (this->bytesAvailable() != 0);

        return;

    }

    int count = this->bytesAvailable();
    cout << "bytesAvailable: " << count << endl;
    read(buf[bufindex], count);
    buf[bufindex][count] = 0; // mark the end
    QString bufstr(buf[bufindex]);
    qDebug() << "Bufstr: " << bufstr;
    QStringList bufstrlist = bufstr.split(" ");

    for(int i = 0; i < bufstrlist.length(); ++i) {
        // qDebug() << i << ": " << bufstrlist.at(i);
    }


    // dispatch

    if( false ) {

        // todo


    } else if(bufstrlist.at(0).toStdString().erase(0, 1) == "sRA"
              && bufstrlist.at(1) == "LMPoutputRange" ) {

        qDebug() << "config angle start & end"; // sRA LMPoutputRange 1 1388 FFF92230 225510
        int angbeg, angend;
        sscanf_s(bufstrlist.at(4).toStdString().c_str(), "%x", &angbeg);
        sscanf_s(bufstrlist.at(5).toStdString().c_str(), "%x", &angend);
        emit angle_beg_end(angbeg/ANGLE_SCALE_FACTOR, angend/ANGLE_SCALE_FACTOR);
        return;

    } else if(bufstrlist.at(0).toStdString().erase(0, 1) == "sRA"
              && bufstrlist.at(1) == "LMPscancfg" ) {

        qDebug() << "config frequency & resolution"; // sRA LMPscancfg 9C4 1 9C4 FFF92230 225510
        int freq, res;
        sscanf_s(bufstrlist.at(2).toStdString().c_str(), "%x", &freq);
        sscanf_s(bufstrlist.at(4).toStdString().c_str(), "%x", &res);
        emit frequency_angleresolution(freq/FREQUENCY_SCALE_FACTOR, (double)res/ANGLE_SCALE_FACTOR);
        return;

    } else if(bufstrlist.at(0).toStdString().erase(0, 1) == "sAN"
              && bufstrlist.at(1) == "SetAccessMode") {

        bufstrlist[2].chop(1); // get rid of ETX
        if(bufstrlist.at(2).toInt() == 1) {
            cout << "Authorization success." << endl;
        } else if(bufstrlist.at(2).toInt() == 0 ) {
            qDebug() << bufstrlist.at(2);
            cout << "Authorization failure." << endl;
        } else {
            cout << "Authorization unknow error." << endl;
        }

    } else {

        // todo

    }

    return;

    buf[bufindex][count] = 0; // mark the end
    fprintf(stdout, "%s\n", buf[bufindex]);
    return;

    if(true || READ_TEST == lmsReadMode) {

        int count = this->bytesAvailable();
        read(buf[bufindex], count);
        buf[bufindex][count] = 0; // mark the end
        // fprintf(stdout, "%s\n", buf[bufindex]);
        //QFuture< void > future = QtConcurrent::run(this, &LMSReader::parser, buf[bufindex], scancount++);
        // if (scancount % 10 == 0) {
            parser(buf[bufindex], scancount++);
        // }
        bufindex = ++bufindex%BUFNUM;
        return;

    } else if(READ_NOTHING_RETURN == lmsReadMode) {
        return;
    } else if(READ_SAVE_PARSE == lmsReadMode) {
        return;
    } else if(READ_PRINT_PARSE == lmsReadMode) {
        return;
    } else if(READ_ON_THE_FLY_PARSE == lmsReadMode) {
        return;
    } else {
        return;
    }
}

void LMSReader::startMeasure()
{
    writeLMS(lmsAction[START_MEASURE]);
}

void LMSReader::stopMeasure()
{
    writeLMS(lmsAction[STOP_MEASURE]);
}

void LMSReader::reboot()
{
    writeLMS(lmsAction[REBOOT]);
}

void LMSReader::pollingOne()
{
    writeLMS(lmsAction[POLLING_ONE]);
}

void LMSReader::test2()
{
    writeLMS(lmsAction[STOP_MEASURE]);
}

void LMSReader::test()
{
    setTimestamp(2015, 8, 9, 23, 4, 9, 9);
    return;

    writeLMS(lmsAction[GET_FREQ_ANGRES]);
    writeLMS(lmsAction[GET_ANGBEG_ANGEND]);
    writeLMS(lmsAction[GET_STATUS]);
    writeLMS(lmsAction[LOG_OUT]);
    return;

    QString str;
    char c2[2] = { 0x35, 0x37 };
    str.sprintf("d: %d, x: %x, s: %s, c: %c, *: %c%c", 0x30, 0x30, "abc", 'E', c2[0], c2[1]);
    qDebug() << str;

    return;
    QByteArray by;
    by.append(0x02);
    by.append("aoei");
    bool b = QString(by).toStdString().erase(0, 1) == "aoei";
    cout << "true or false: ";
    if (b) { cout << "true"; } else { cout << "false" << endl; }
    cout << endl << "---" << endl;
    by.append("aoei");

    return;
    char c[40];
    sprintf_s(c, "%c%s%x%d", "aoei", "aoei", 0x23, 0x23);
    qDebug() << QString(c);
    // fprintf(stdout, "%s\n", c);

    return;
    qDebug() << "LMS Testing.";
    writeLMS(lmsAction[GET_FREQ_ANGRES]);
    // writeLMS(lmsAction[GET_ANGBEG_ANGEND]);
    // writeLMS(lmsAction[TURN_ON]);

    return;
    turnOn();

    return;
    const static char buf[][4] = {
        { 0x00, 0x00, 0x09, 0xC4 }, // 25Hz: 00 00 09 C4, 2500 * 1/100 Hz
        { 0x00, 0x00, 0x13, 0x88 }, // 50Hz: 00 00 13 88, 5000 * 1/100 Hz
    };
    qDebug() << "sizeof(buf): "<< sizeof(buf) << endl                                   // 8
             << "sizeof(buf[0]): " << sizeof(buf[0]) << endl                            // 4
             << "sizeof(buf)/sizeof(buf[0]): " << sizeof(buf)/sizeof(buf[0]) << endl;   // 2

    return;
    static quint8 iooo = 0;
    ++iooo;
    qDebug() << iooo;

    return;
}

void LMSReader::lmsConnectionError(QAbstractSocket::SocketError err)
{
    qDebug() << "LMS Connetion Error Occured: "
             << err;
}

void LMSReader::lmsConnectionEstablished()
{
    qDebug() << "LMS Connection Established, Avaible Bytes: " << this->bytesAvailable();
}

void LMSReader::parser(const char *buf, int index)
{
    QString scanBufString(buf);
    QStringList CurrentScan = scanBufString.split(" ");

    // not data
    if ("sSN" != CurrentScan.at(0).toStdString().erase(0, 1) ||
        "LMDscandata" != CurrentScan.at(1)) {
        cerr << "Parse err occurred, not LMS scan.\n";
        return;
    }

    // data
    int offset = 25;
    int sanNum = 0;
    int distance = 0.0;

    double angle = 0.0;
    sscanf_s(CurrentScan.at(offset++).toStdString().c_str(), "%x", &sanNum);

    qDebug() << "#Points in this scan: " << sanNum;
    for ( int i = 0; i < sanNum; i += 1 ) {
        sscanf_s(CurrentScan.at(i + offset).toStdString().c_str(), "%x", &distance);
        angle = (lmsAngleBeg + i * lmsAngleRes) / 180.0 * M_PI;

            cout << "[" << index << ", " << i << "]: "
                    //<< setw(10) << setprecision(3)
                 << cos(angle) * distance << "                   "
                    // << setw(10) << setprecision(3)
                 << sin(angle) * distance << endl;
    }
}

void LMSReader::connectToLMS()
{
    qDebug() << "<-- connectLMS";
    this->connectToHost( lmsAddress, lmsPort);
}

void LMSReader::closeConnection()
{
    if ( this->isOpen() ) {
        this->close();
    }
    qDebug() << "connection closed";
}

void LMSReader::configureIPPort(QString ip, quint16 port)
{
    lmsAddress = ip;
    lmsPort = port;
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

void LMSReader::readRawParseThenWriteXYZ(string ifilename, string ofilename)
{
        char STX = 0x2; // start of one Scan
        char ETX = 0x3; // end of one Scan
        int Yoffset = -10;

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
                if (infile.eof()) { break; }
                infile.get(scanBuf[0]);

            } while (STX != scanBuf[0]);

            // save to scanBuf
            i = 0;
            do {
                ++i;
                if (infile.eof()) { break; }
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
            Yoffset += 10;
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
