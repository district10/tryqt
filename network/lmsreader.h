#ifndef LMSREADER_H
#define LMSREADER_H

#include <QTcpSocket>
#include <QThread>
#include <QFile>
#include <QMap>
#include <QStringList>
#include <QtConcurrentRun>
#include <iostream>
#include <fstream>

#define M_PI 3.14

using namespace std;


class LMSReader : public QTcpSocket
{
    Q_OBJECT

public:
    explicit LMSReader();
    ~ LMSReader();
    void writeLMS(QString msg);
    void connectToLMS();
    void closeConnection();
    void configureIPPort(QString ip, quint16 port);
    void askforAuthorization();
    void askforAngle_Beg_End();
    void askforFrequency_Angleresolution();

    void setFrequencyAngres(int freq, double angres);
    void setAngleBegEnd(double start = -45.0, double end = 225.0);
    void setTimestamp(quint16 year=2015, quint8 month=1, quint8 day=1,
                      quint8 hour=0, quint8 minute=0, quint8 second=0, quint8 microsecond=0);
    void getTimestamp();
    void logout();

    void saveData(QString filename, char *buf);
    void readRawParseThenWriteXYZ(string ifilename, string ofilename);

    void test();
    void test2();

    void startMeasure();
    void stopMeasure();
    void reboot();
    void pollingOne();

signals:
    void callToTurnOnLMS();
    void calltoTurnOffLMS();
    void angle_beg_end(int beg, int end);
    void frequency_angleresolution(int freq, double res);

public slots:
    void readLMS();
    void turnOn();
    void turnOff();
    void lmsConnectionError(QAbstractSocket::SocketError);
    void lmsConnectionEstablished();

public:
    QString lmsRawPath;
    QString lmsCoorPath;

private:
    enum LMSActions {
        GET_FREQ_ANGRES,
        SET_FREQ_ANGRES,
        GET_ANGBEG_ANGEND,
        SET_ANGBEG_ANGEND,
        GET_STATUS,
        GET_TIMESTAMP,
        SET_TIMESTAMP,
        LOG_IN,
        LOG_OUT,
        TURN_ON,
        TURN_OFF,
        STAND_BY,
        REBOOT,
        START_MEASURE,
        STOP_MEASURE,
        POLLING_ONE,
        SAVE_ALL
    };

    enum LMSUserLevel {
        MAINTENANCE = 0x02,
        AUTHORIZED_CLIENT = 0x03,
        SERVICE = 0x04
    };

    enum {
        BUFSIZE = 5000,
        BUFNUM = 10,
        AUTHORIZATION_SUCCESS = 1,
        AUTHORIZATION_FAULURE = 0,
        ANGLE_SCALE_FACTOR = 10000, // 1/10000 Hz
        FREQUENCY_SCALE_FACTOR = 100, // 1/100 Degree
    };

    char *buf[BUFNUM];
    int bufindex;
    size_t scancount;
    void parser(const char *buf, int index);

    enum LMSStates { LMS_ON, LMS_OFF } lmsState;
    enum LMSReadModes {
        READ_TEST,
        READ_NOTHING_RETURN,
        READ_PRINT_PARSE,
        READ_SAVE_PARSE,
        READ_ON_THE_FLY_PARSE
    } lmsReadMode;

    QMap<LMSActions, QString> lmsAction;
    QString lmsAddress;
    quint16 lmsPort;

    double lmsFrequency;
    double lmsAngleBeg;
    double lmsAngleEnd;
    double lmsAngleRes;
};

#endif // LMSREADER_H
