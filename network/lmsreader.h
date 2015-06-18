#ifndef LMSREADER_H
#define LMSREADER_H

#include <QTcpSocket>

class lmsReader : public QTcpSocket
{
    Q_OBJECT

public:
    explicit lmsReader(QTcpSocket *parent = 0);
    void send(QString msg);
    void test();

signals:

public slots:
    void read();

};

#endif // LMSREADER_H
