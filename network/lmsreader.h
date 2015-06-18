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
    void send(QString msg);
    void test();
    void connectLMS();

signals:
    void hello(QString msg);

public slots:
    void readLMS();
    void world(QString msg);

private:
    enum LMSModes { CONFIG_FREQ_ANGRES, CONFIG_ANGSTART_ANGSTOP, OPEN, CLOSE };
    enum LMSStates { LMS_ON, LMS_OFF } lmsState;
    QMap<LMSModes, QString> lmsConfig;
    QString lmsIP;
    quint16 lmsPort;

};

#endif // LMSREADER_H
