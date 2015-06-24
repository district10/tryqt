#ifndef LMSREADER_H
#define LMSREADER_H

#include <QtConcurrentRun>
#include <QStringList>
#include <QTcpSocket>
#include <QThread>
#include <QFile>
#include <QMap>

#include <iostream>
#include <fstream>

#define M_PI 3.14159265358979323846

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
    void setAuthToAuthedClient();
    void askforAngle_Beg_End();
    void askforFrequency_Angleresolution();

    void genNewPath();
    void genNewPath(quint64 dt);

    void setFrequencyAngres(int freq, double angres);
    void setAngleBegEnd(double start = -45.0, double end = 225.0);
    void setTimestamp(quint16 year=2015, quint8 month=1, quint8 day=1,
                      quint8 hour=0, quint8 minute=0, quint8 second=0, quint8 microsecond=0);
    void getTimestamp();
    void logout();


    void test();
    void test2();

    void startMeasure();
    void stopMeasure();
    void reboot();
    void pollingOne();

signals:
    void angle_beg_end(int beg, int end);
    void frequency_angleresolution(int freq, double res);
    void authorization_passed(bool auth);

public slots:
    void readLMS();
    void turnOn();
    void turnOff();
    void lmsConnectionError(QAbstractSocket::SocketError);

public:
    QString lmsRawPath;
    QString lmsCoorPath;

private:
    enum LMSAction {
        GET_FREQ_ANGRES,
        SET_FREQ_ANGRES,
        GET_ANGBEG_ANGEND,
        SET_ANGBEG_ANGEND,
        GET_STATUS,
        GET_TIMESTAMP,
        SET_TIMESTAMP,
        AUTHORIZE,
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
        AUTHORIZATION_SUCCESS = 1,
        AUTHORIZATION_FAULURE = 0,
        ANGLE_SCALE_FACTOR = 10000, // 1/10000 Hz
        FREQUENCY_SCALE_FACTOR = 100, // 1/100 Degree
    };

    void parser(const QStringList& bufstrlist);

    enum LMSReadMode {
        READ_TEST,
        READ_NOTHING_RETURN,
        READ_PRINT_PARSE,
        READ_SAVE_PARSE,
        READ_ON_THE_FLY_PARSE
    } lmsReadMode;
    QMap<LMSAction, QString> lmsAction;

    enum LMSAuth {
        AUTH_MAINTENANCE = 0x02,
        AUTH_AUTHEDCLIENT = 0x03,
        AUTH_SERVICELEVEL = 0x04,
    };
    QMap<LMSAuth, QString> lmsAuth;

    QString lmsAddress;
    quint16 lmsPort;

    double lmsFrequency;
    double lmsAngleBeg;
    double lmsAngleEnd;
    double lmsAngleRes;
    size_t lmsCount;

    const static int lmsAngleMin = -45;
    const static int lmsAngleMax = 225;


    // debugging flags
    bool PRINT_BUF;
    bool PRINT_BUF_META;
    bool PRINT_XYZ;
    bool PRINT_R;
    bool PRINT_SCAN_META;
    bool SAVE_SCAN;
    bool SAVE_BUF;
    bool PARSE_SCAN;
    QString lmsFormat;

    QByteArray lmsBA;
    bool lmsMoreToRead;
};

#endif // LMSREADER_H
