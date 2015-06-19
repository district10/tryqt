#ifndef LMSREADER_H
#define LMSREADER_H

#include <QTcpSocket>
#include <QFile>
#include <QMap>
#include <iostream>
#include <QStringList>


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

    void turnOn();
    void turnOff();

signals:

public slots:
    void readLMS();
    void lmsConnectionError(QAbstractSocket::SocketError);
    void lmsConnectionEstablished();

private:
    enum LMSModes { CONFIG_FREQ_ANGRES, CONFIG_ANGSTART_ANGSTOP, OPEN, CLOSE };
    enum LMSStates { LMS_ON, LMS_OFF } lmsState;
    QMap<LMSModes, QString> lmsConfig;
    QString lmsIP;
    quint16 lmsPort;
    char buffer[200];

};

#endif // LMSREADER_H
