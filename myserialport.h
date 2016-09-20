#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H

#include <QSerialPort>
#include <QSerialPortInfo>
class MySerialPort : public QSerialPort
{
public:
    MySerialPort();
    bool init(QString port);
private:
    QSerialPort m_serialPort;

private slots:
    void readyReadSlot();
};

#endif // MYSERIALPORT_H
