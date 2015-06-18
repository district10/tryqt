#ifndef LMSEMULATOR_H
#define LMSEMULATOR_H

#include <QTcpSocket>

class LMSEmulator : public QTcpSocket
{
public:
    LMSEmulator();
    ~LMSEmulator();
};

#endif // LMSEMULATOR_H
