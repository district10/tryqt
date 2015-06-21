#ifndef LMSREADER_H
#define LMSREADER_H

#include <QTcpSocket>
#include <QFile>
#include <QMap>
#include <iostream>
#include <QStringList>

#include <vector>

using namespace std;

class LMSReader : public QTcpSocket
{
    Q_OBJECT

public:
    explicit LMSReader(QTcpSocket *parent = 0);
    ~ LMSReader();
    void writeLMS(QString msg);
    void test();
    void connectToLMS();
    void closeConnection();

    void saveData(QString filename, char *buf);
    void parseLMSScan(string ifilename, string ofilename);

signals:
    void callToTurnOnLMS();
    void calltoTurnOffLMS();

public slots:
    void readLMS();
    void turnOn();
    void turnOff();
    void lmsConnectionError(QAbstractSocket::SocketError);
    void lmsConnectionEstablished();

private:
    enum LMSActions { GET_FREQ_ANGRES, GET_ANGBEG_ANGEND, TURN_ON, TURN_OFF };
    enum LMSStates { LMS_ON, LMS_OFF } lmsState;
    QMap<LMSActions, QString> lmsAction;
    QString lmsIP;
    quint16 lmsPort;
    char buffer[200];

    double lmsFrequency;
    double lmsAngleBeg;
    double lmsAngleEnd;
    double lmsAngleRes;
    double sz;

    enum { BUFSIZE = 5000 };

#endif // LMSREADER_H
