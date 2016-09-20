#include "myserialport.h"

MySerialPort::MySerialPort()
{

}

/*read ini config file*/
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QString>
#include <QDebug>

bool MySerialPort::init(QString port)
{
   qDebug() << port;

   QSerialPortInfo portInfo;
   bool portExist=false;
   foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
   {
       qDebug() << "Name : " << info.portName();
       qDebug() << "Description : " << info.description();
       qDebug() << "Manufacturer: " << info.manufacturer();
       qDebug() << "Serial Number: " << info.serialNumber();
       qDebug() << "System Location: " << info.systemLocation();

       if(info.portName() == serialPortNum)
       {
          portInfo=info;
          portExist=true;
       }
   }

    m_serialPort.setPort(portInfo);
    if(m_serialPort.open(QIODevice::ReadWrite))
    {
        qDebug() << "m_reader.open(QIODevice::ReadWrite)";
        m_serialPort.setBaudRate(QSerialPort::Baud4800);
        m_serialPort.setParity(QSerialPort::NoParity);
        m_serialPort.setDataBits(QSerialPort::Data8);
        m_serialPort.setStopBits(QSerialPort::OneStop);
        m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

        m_serialPort.clearError();
        m_serialPort.clear();
        connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
        return true;
    }
}

void MySerialPort::readyReadSlot()
{
    while (m_serialPort.canReadLine()) {
        QByteArray arr = m_serialPort.readLine();
        if (arr.isEmpty()) {
            qDebug() << Q_FUNC_INFO << __LINE__;
            break;
        }
        qDebug()<<arr.data();
        //processData(data);
    }
}
