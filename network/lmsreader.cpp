#include "lmsreader.h"
#include <QMutex>
#include <QTime>

LMSReader::LMSReader()
{
    // for debugging
    PRINT_SCAN_META = false;
    PRINT_BUF = false;
    SAVE_BUF = true;
    PRINT_BUF_META = false;
    PRINT_XYZ = false;
    PRINT_R = false;
    PARSE_SCAN = true;
    SAVE_SCAN = true;
    lmsFormat = "";

    lmsMoreToRead = false;

    lmsBA.append("");

    lmsAction[GET_FREQ_ANGRES] = "sRN LMPscancfg";
    lmsAction[SET_FREQ_ANGRES] = "sMN mLMPsetscancfg";
    lmsAction[GET_ANGBEG_ANGEND] = "sRN LMPoutputRange";
    lmsAction[SET_ANGBEG_ANGEND] = "sWN LMPoutputRange";
    lmsAction[POLLING_ONE] = "sRN LMDscandata";
    lmsAction[TURN_ON] = "sEN LMDscandata 1";
    lmsAction[TURN_OFF] = "sEN LMDscandata 0";
    lmsAction[GET_STATUS] = "sRN LCMstate";
    lmsAction[AUTHORIZE] = "sMN SetAccessMode";
    lmsAction[LOG_OUT] = "sMN Run";
    lmsAction[STAND_BY] = "sMN LMCstandby";
    lmsAction[START_MEASURE] = "sMN LMCstartmeas";
    lmsAction[STOP_MEASURE] = "sMN LMCstopmeas";
    lmsAction[REBOOT] = "sMN mSCreboot";
    lmsAction[GET_TIMESTAMP] = "sRN STlms";
    lmsAction[SET_TIMESTAMP] = "sMN LSPsetdatetime";
    lmsAction[SAVE_ALL] = "sMN mEEwriteall";

    lmsReadMode = READ_PRINT_PARSE;
    lmsAddress = "192.168.0.184";
    lmsPort = 2111;
    lmsFrequency = 50.0;
    lmsAngleBeg = -45.0;
    lmsAngleEnd = 225.0;
    lmsAngleRes = 0.5;
    lmsCount = 0;

    lmsAuth[AUTH_MAINTENANCE] = "B21ACE26";
    lmsAuth[AUTH_AUTHEDCLIENT] = "F4724744";
    lmsAuth[AUTH_SERVICELEVEL] = "81BE23AA";

    lmsRawPath = "";

    connect(this, SIGNAL(readyRead()),
            this, SLOT(readLMS()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(lmsConnectionError(QAbstractSocket::SocketError)));
}

LMSReader::~LMSReader()
{
    // close connection
    closeConnection();
}

void LMSReader::writeLMS(QString data)
{
    QByteArray ba;
    ba.append(0x02);
    ba.append(data);
    ba.append(0x03);
    write(ba);
}

void LMSReader::setAuthToAuthedClient()
{
    QString str;
    str.sprintf("%s %2d %s",
                lmsAction[AUTHORIZE].toStdString().c_str(),
                AUTH_AUTHEDCLIENT,
                lmsAuth[AUTH_AUTHEDCLIENT].toStdString().c_str()); // the auth word
    qDebug() << str;
    writeLMS(str);
}

void LMSReader::askforAngle_Beg_End()
{
    writeLMS(lmsAction[GET_ANGBEG_ANGEND]);
}

void LMSReader::askforFrequency_Angleresolution()
{
    writeLMS(lmsAction[GET_FREQ_ANGRES]);
}

void LMSReader::genNewPath()
{
    quint64 dt = QDateTime::currentMSecsSinceEpoch();
    lmsRawPath.sprintf("%lld-LMS-XYZ.txt", dt);
}

void LMSReader::genNewPath(quint64 dt)
{
    lmsRawPath.sprintf("%lld-LMS-XYZ.txt", dt);
}

void LMSReader::setFrequencyAngres(int freq, double angres)
{
    QString str;
    str.sprintf("%s %X %d %X %X %X",
                lmsAction[SET_FREQ_ANGRES].toStdString().c_str(),
                FREQUENCY_SCALE_FACTOR * (freq),         // 25 or 50 Hz
                1,                                       // reserved
                (int) (ANGLE_SCALE_FACTOR * (angres)),   // 0.25 or 0.5 degree
                (int) (ANGLE_SCALE_FACTOR * (-45 + 0)),  // place holder, no real effect
                (int) (ANGLE_SCALE_FACTOR * (225 - 0))   // place holder
                );
    writeLMS(str);
}

void LMSReader::setAngleBegEnd(double start, double end)
{
    if (start > end) { return; }
    if (start < lmsAngleMin) { start = lmsAngleMin; }
    if (end > lmsAngleMax) { end = lmsAngleMax; }

    QString str;
    str.sprintf("%s %d %X %X %X",
                lmsAction[SET_ANGBEG_ANGEND].toStdString().c_str(),
                1,                                  // status code
                FREQUENCY_SCALE_FACTOR * (25),      // place holder, no real effect
                (int) (ANGLE_SCALE_FACTOR * start),
                (int) (ANGLE_SCALE_FACTOR * end)
                );
    // qDebug() << str;
    writeLMS(str);
}

void LMSReader::setTimestamp(quint16 year, quint8 month, quint8 day, quint8 hour, quint8 minute, quint8 second, quint8 microsecond)
{
    QString str;
    //               Y  M  D  H  M  S MS
    str.sprintf("%s %X %X %X %X %X %X %X",
                lmsAction[SET_TIMESTAMP].toStdString().c_str(),
                year, month, day,
                hour, minute, second,
                microsecond);
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
    // if(PRINT_BUF_META) { cout << "--BytesAvailable: " << bytesAvailable() << endl; }

    QByteArray ba = readAll();
    lmsBA.append(ba);

    if (ba.at(ba.length() -1) != 0x03) {
        return;
    }

    QString bufstr(lmsBA);
    lmsBA.clear();

    if(PRINT_BUF) { qDebug() << "Bufstr: " << bufstr; }

    QStringList bufstrlist = bufstr.split(" ");

    // dispatch
    if( false ) {

        // todo
    } else if( false ) {

        // todo

    } else if( bufstrlist.at(1) == lmsAction[TURN_ON].split(" ").at(1) ) {

        if (PARSE_SCAN) { parser(bufstrlist); }

    } else if( bufstrlist.at(1) == lmsAction[GET_ANGBEG_ANGEND].split(" ").at(1) ) { // sRA LMPoutputRange 1 1388 FFF92230 225510

        int angbeg, angend;
        sscanf_s(bufstrlist.at(4).toStdString().c_str(), "%x", &angbeg);
        sscanf_s(bufstrlist.at(5).toStdString().c_str(), "%x", &angend);
        emit angle_beg_end(angbeg/ANGLE_SCALE_FACTOR, angend/ANGLE_SCALE_FACTOR);
        return;

    } else if( bufstrlist.at(1) == lmsAction[GET_FREQ_ANGRES].split(" ").at(1) ) { // sRA LMPscancfg 9C4 1 9C4 FFF92230 225510

        int freq, res;
        sscanf_s(bufstrlist.at(2).toStdString().c_str(), "%x", &freq);
        sscanf_s(bufstrlist.at(4).toStdString().c_str(), "%x", &res);
        emit frequency_angleresolution(freq/FREQUENCY_SCALE_FACTOR, (double)res/ANGLE_SCALE_FACTOR);
        return;

    } else if( bufstrlist.at(1) == lmsAction[AUTHORIZE].split(" ").at(1) ) { // SetAccessMode<ETX>

        bufstrlist[2].chop(1); // rip of ETX
        if(bufstrlist.at(2).toInt() == AUTHORIZATION_SUCCESS) {            // TODO
            cout << "Authorization success." << endl;
            emit authorization_passed(true);
        } else if(bufstrlist.at(2).toInt() == AUTHORIZATION_FAULURE ) {
            cout << "Authorization failure." << endl;
            emit authorization_passed(false);
        } else {
            emit authorization_passed(false);
            cout << "Authorization unknow error." << endl;
        }

    } else {

        // todo
    }

    return;
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

void LMSReader::lmsConnectionError(QAbstractSocket::SocketError err)
{
    qDebug() << "--LMS Connetion Error Occured: "
             << err;
}

void LMSReader::parser(const QStringList &bufstrlist)
{
    // already "LMDscandata" data
    // No: "sEA LMDscandata 0", Yes: "sSN LMDscandata 1" or "sRA LMDscandata"

    string ct = bufstrlist.at(0).toStdString().erase(0, 1); // command type
    if ("sSN" != ct && "sRA" != ct) {
        // cerr << "sEA data, not LMS scan.\n";
        return;
    }

    // data
    int scannednum = 0;
    int distance = 0.0;
    double anglebeg = 0.0;
    double angleres = 0.0;
    double anglecur = 0.0;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    int tmp;
    size_t utmp;

    int i = 0;
    // 7, Telegram Counter
    // 8, scan counter
    // 9, time since start up
    i = 9;
    sscanf_s(bufstrlist.at(i).toStdString().c_str(), "%x", &utmp);
    qDebug() << "stamp: " << bufstrlist.at(i) << " --> " << utmp;
    // y = (double)utmp/10000000; // s
    y = (double)utmp/10000; // ms
    // y = (double)utmp/10; // μs

    sscanf_s(bufstrlist.at(23).toStdString().c_str(), "%x", &tmp); anglebeg = (double) tmp / ANGLE_SCALE_FACTOR;
    sscanf_s(bufstrlist.at(24).toStdString().c_str(), "%x", &tmp); angleres = (double) tmp / ANGLE_SCALE_FACTOR;
    sscanf_s(bufstrlist.at(25).toStdString().c_str(), "%x", &scannednum);

    if (PRINT_SCAN_META) {
    cout << "anglebeg: " << anglebeg << ", "
         << "angleres: " << angleres << ", "
         << "scannum: " << scannednum << endl;
    }

    if (lmsRawPath == "") { genNewPath(); }
    FILE *fp = fopen(lmsRawPath.toStdString().c_str(), "a+");
    if (NULL == fp) {
        cerr << "err, cant open file.";
        return;
    }

    int cnt = 0;
    for ( int i = 0; i < scannednum; i ++ ) {
        ++cnt;
        sscanf_s(bufstrlist.at(i + 26).toStdString().c_str(), "%x", &distance); // metric: mm
        anglecur = (anglebeg + i * angleres) / 180.0 * M_PI;
        x = sin(anglecur) * distance;
        z = cos(anglecur) * distance;

        // continue;
        if (PRINT_R) {
            cout << distance << endl;
        }
        if(PRINT_XYZ) {
        cout << x << "                "
             << y << "                "
             << z << endl;
        }

        if( SAVE_SCAN ) {
            fprintf_s(fp, "%20.10f %20d %20.10f\n", x, (int)y, z);
        }
    }

    if (scannednum != cnt) {
        cerr << "total num: " << scannednum << ", scanned: " << cnt << endl;
    } else {
        cout << "total num: " << scannednum << ", scanned: " << cnt << endl;
    }

    fclose(fp);
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
}

void LMSReader::turnOff()
{
    writeLMS(lmsAction[TURN_OFF]);
}

void LMSReader::test2()
{
    // Set Device Name
    // <-- "sWN LocationName D CVRS";
    // --> "sFA 8"

    QString str = "sWN LocationName D CVRS";
    writeLMS(str);
}

void LMSReader::test()
{

    genNewPath();
    qDebug() << lmsRawPath;


    return;
    FILE *fp = fopen("output.txt", "w");
    fprintf_s(fp, "%0.0f %f %f\n" , 3.4, 1.25, 0.6);
    fprintf_s(fp, "%f %f %f\n", 1.2, 3.4, 5.6);
    qDebug()  << "here";
    fclose(fp);


    return;
    QString str = "sRA LMDscandata 1 1 D6FF21 0 0 286B 3C84 EE4D4845 EE4DA2ED 0 0 7 0 0 1388 168 0 1 DIST1 3F800000 00000000 FFF92230 1388 218 112 105 103 103 102 107 10B 10D 112 117 119 11F 121 125 127 12E 12F 12D 12F 13A 13E 143 146 14D 14F 154 15A 15A 161 166 16A 16A 166 167 17A 183 182 18E 195 19C 19F 1A6 1A8 1AD 1B4 1BE 1A2 0 1B7 1BF 1CB 1ED 1E7 1F7 220 23C 24E 265 27D 2A1 2B1 2D0 2F3 312 323 344 35F 37D 3A5 3DD 40C 455 48E 4CA 521 598 59C 552 2C4 290 29F 30F 512 525 257 1EF 284 2E8 270 246 23F 25A 2D6 434 4C8 4E3 4F6 501 50F 511 517 51F 521 519 51D 550 5D8 5E5 5E9 5EC 5EA 5EC 5ED 5F7 5F9 5F3 5FD 5FD 605 607 60C 60B 611 616 61E 623 629 630 634 63B 646 66B 670 676 680 68D 693 68E 6C2 898 8D7 8E2 8EF 8F5 902 90C 918 929 936 948 951 962 976 985 991 9A2 9B4 9C2 9D4 9E9 9F4 A05 A14 A22 A35 A3D A48 A5D A78 A8A AA2 AC1 AD6 AF5 B0A B18 B2B B42 B57 B65 B56 B44 B4D BE5 C3A 3 3 0 47F9 3 472D D50 D27 CF6 CD1 CBA CB1 CB3 CA1 C90 C7C C8A CAA CD0 CF0 D10 D31 D53 D7C D9E DC6 DEF E1A E42 E6C E90 EC8 EFE F2D F47 F37 F2D F1D F0B F00 EF1 EE0 ED4 ECA EBF EAC EA6 E9E E8D E86 E7A E76 E68 E5C E58 E4D E45 E3E E38 E2D E2A E25 E1E E13 E0E E08 E03 DFE DFD DFA DED DEF DEF DEB DE9 DE4 DE5 DDE DE1 DDE DDB DD9 DDB DDE DD8 DDA DDA DD8 DDA DE0 DDE DDD DE4 DE5 DE6 DE8 DF0 DF4 DF4 DFC E00 E03 E0C E11 E15 E17 E21 E26 E2B E35 E3B E3D E4C E52 E57 E60 E6A E71 E7A E83 E8E E96 EA2 EB0 EBC EC6 ED2 EF8 F55 417 401 3F2 3F1 41B F6A F6C F80 F9C F9F 316 2F8 2D8 2CA 2C4 2C8 2B4 2A2 28F 28A 27B 275 26A 260 256 24F 246 241 235 230 228 221 218 211 20D 204 1FF 1F9 1F2 1ED 1E4 1DE 1D7 1D6 1CC 1CA 1C4 1C1 1BB 1B7 1B6 1B1 1AB 1AA 1A3 1A3 19E 19A 194 191 18D 189 183 180 17C 176 173 16E 169 168 15F 15E 158 158 156 154 151 153 151 150 152 151 154 157 157 151 152 159 159 15A 15B 14E 12E 10C F9 F9 F9 100 104 10E 10F 113 11C 120 122 125 126 12C 130 12B 12B 129 128 121 11D 11C 11A 115 111 10E 10D 103 107 103 107 104 103 101 108 105 F3 D5 C0 AA A2 A2 A2 A6 AE C5 E3 F2 F8 100 FE FF 100 FC FF FA FB FD FC FB FB FD FB FD FE FD FE FA FC F9 FF FA FA F9 FC FC FF FF 101 103 102 105 103 105 104 104 FE 103 104 105 106 108 107 107 109 118 135 13E 141 13D 136 13E 13C 139 140 13C 13A 133 12B 126 124 11B 113 10B 106 100 F7 EF E8 E4 E4 DF DB DC DE DD E3 E9 0 0 0 0 0 0";
    QStringList bufstrlist = str.split(" ");
    for (int i = 0; i < 3; ++i) {
        parser(bufstrlist);
    }
    return;
    str.sprintf("%s %2d %s",
                lmsAction[AUTHORIZE].toStdString().c_str(),
                AUTH_SERVICELEVEL,
                lmsAuth[AUTH_SERVICELEVEL].toStdString().c_str()); // the auth word
    qDebug() << str;
    writeLMS(str);

    return;
    str.sprintf("%s %2d %s",
                lmsAction[AUTHORIZE].toStdString().c_str(),
                AUTH_MAINTENANCE,
                lmsAuth[AUTH_MAINTENANCE].toStdString().c_str()); // the auth word
    qDebug() << str;
    writeLMS(str);
    // QString str = "";
    // writeLMS(str);

    // Fog Filter
    // <-- "sWN MSsuppmode 1"; // 0: glitch, 1: fog
    // --> "sWA MSsuppmode"

    // n-Pulse to 1-Pulse Filter
    // <-- "sWN LFPnto1filter 1"; // 1: active, 0: inactive
    // --> "sWA LFPnto1filter"

    // Mean Filter
    // <-- "sWN LFPmeanfilter 1 +10 0"; // 0: inactive, 1: active. number of scans: +2..+100. 0
    // --> "sWA LFPmeanfilter"

    // Configure the data content for the scan
    // QString str = "sWN LMDscandatacfg";
    // channel: c1: 1, c2: 2, c1+c2: 3
    // Remission data output: 0: no, 1: yes
    // Resolution of Remission Data: 0: 8bit, 1: 16bit
    // Unit of Remission Data:
    // double AngRes = 2.0 / round(2.0 / GivenAngRes);

    // Function Front Panel
    // <-- "sWN LMLfpFcn 1 0 1"; // 1: reserved, 0-2: Q1/Q2, 0-2: Okay/Stop, 0-1: display function
    // 0: no function, 1: application, 2: command
    // 0: application, 1: command
    // --> "sFA 8"

    // Synchronization Phase
    // <-- "sWN SYPhase +90";
    // --> "sWA SYPhase"

    // Set factory defaults
    // <-- "sMN mSCloadfacdef"

    // Set IP-Address
    // <-- "sWN EIIpAddr C0 A8 0 1";
    // --> "sWA EIIpAddr"

    // Power On Counter
    // <-- "sRN ODpwrc";
    // --> "sRA ODpwrc A7"

    // Operating hours
    // <-- "sRN ODoprh";
    // --> "sRA ODoprh 4F4", 4F4 * 1/10h = 126.8 hours

    // Ask Device Name
    // <-- "sRN LocationName";
    // --> "sRA LocationName B not defined"

    // Device State
    // <-- "sRN SCdevicestate";
    // --> "sRA SCdevicestate 1", 0: busy, 1: ready, 2: error

    // Device Ident
    // <-- "sRN DeviceIdent/sRI 0";
    // --> "sFA 3"?

    // Reset output counter
    // <-- "sMN LIDrstoutpcnt";
    // --> "sAN LIDrstoutpcnt 0", 0: success

    // Set output state
    // <-- "sMN mDOSetOutput 1 1"; // output number: 1-3, output state: 0:inactive / 1:active
    // "sAN mDOSetOutput 0", 0: err, 1: success

    // Ask state of the outputs
    // <-- "sRN LIDoutputstate";
    // --> "sRA LIDoutputstate 1 E88CDA0F 1 A 1 A 1 A 2 0 2 0 2 0 2 0 2 0 2 0 2 0 2 0 1 7DD 1 1 0 1D 3B 5B8D8"

    // Ask speed threshold
    // "sRN LICSpTh";
    // "sRA LICSpTh 5"

    // Fixed speed
    // <-- "sWN LICFixVel +5"; // +0.001..+10
    // --> "sWA LICFixVel"

    // Encoder resolution
    // QString str = "sWN LICencres +1000"; // +0.001..+2000
    // "sWA LICencres"

    // Encoder Settings
    /*
     * 0 = Off
     * 1 = single Increment/INC1
     * 2 = Direction recognition (phase)
     * 3 = Direction recognition (level)
     **/
    // <-- "sWN LICencset 2"
    // --> "sWA LICencset"

    // Increment source
    // <-- "sWN LICsrc 1"); // 0: fixed speed, 1: encoder
    // --> "sWA LICsrc"


    // <-- "sMN LMCstartmeas";
    // "sAN LMCstartmeas 0"

    // <-- "sMN LMCstandby"; // cant turn on, but can poll one
    // "sAN LMCstandby 0"

    // <-- "sMN LMCstopmeas";
    // ""sAN LMCstopmeas 0"

    // <-- "sMN mSCreboot";
    // "sAN mSCreboot"

    // <-- "sRN STlms";
    // "sRA STlms 7 0 8 00:34:19 A 01.01.1970 0 0 0"

    // <-- "sMN LSPsetdatetime";
    // "sAN LSPsetdatetime 1"

    // <-- "sMN mEEwriteall";
    // "sAN mEEwriteall 1"
    return;
}

/*
     * Telegram structure: sRA LMDscandata / sSN LMDscandata
00   * command type: "sRA" or "sSN"
 1   * cammand: "LMDscandata"
 2   * version number:
     * device info:
 3   *     + device number
 4   *     + serial number
 5   *     + device status
     * status info:
 6   *     + telegram counter
 7   *     + scan counter
 8   *     + time since start up
 9   *     + time of transmission
10   *     + status of digital inputs: 0: all inputs low, 3: all input high
 1   *     + status of digital outputs: 0: all outputs low, 3: all outputs high
 2   *     + reserved: 2bytes
     * frequencies:
 3   *     + freq: 1/100 Hz (25/50 Hz for LMS 1xx)
 4   *     + inverse of the time between two measurement shots: e.g. 50Hz, 0.5° ==> 720 shots/20ms ==> 36 kHz
 5   * amount of encoders: 0..3, if 0, then no
 6   *     + encoder position: info in ticks
 7   *     + encoder speed: ticks/mm
     *
 8   * amount of 16bit channels: 1/2 for LMS1xx
     * output channels:
 9   *     + content: DIST1: radial Values of first pulse in mm, RSSI1:Energy Values of first pulse, DIST2: radial Values of 2nd pulse in mm, RSSI2:Energy Values of 2nd pulse
20   *     + scale factor: x1: 3F800000h, x2: 40000000h
 1   *     + scale factor offset:
 2   *     + start angle: 1/10.0 deg
 3   *     + steps: 1/10.0 deg
 4   *     + amoust of data: N
     *     + data: data1..dataN
     *
     * amount of 8 Bit channels: 1..2 channels for LMS1xx
     * ...
 **/
