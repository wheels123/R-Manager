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

       if(info.portName() == port)
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
        processData(data);
    }
}


inline void MySerialPort::processData(QString &data)
{
    const QString itemSep = "\n";

    QStringList list = data.split(itemSep);

    int itemNo = list.size();
    /*
    QByteArray buf;
    buf.append("LINE NUM ");
    buf.append(QString::number(itemNo,10));
    sendBack(buf);
*/
    for(int i=0;i<list.size();i++)
    {
        QString str = list.at(i);
        int size=str.size();
        if(size>0)
        {
            transData(str);
        }
    }
}

inline void MySerialPort::transData(QString &data)
{
    const QString itemSep = ",";
    QStringList list = data.split(itemSep);
    int itemNo = list.size();
    int dataSize = data.size();
    QVector<QString> splitData;

    if(itemNo<=0) return;

    QString type = list.at(0);
    if(dataSize==1)
    {
        //int index = data.indexOf("~");
        //if(index>=0)
        if(data=="~")
        {
            QByteArray buf;
            buf.append("`\n");
            //sendBack(buf);
            socket->write(buf);
            m_heart=true;
            qDebug()<<"heart!";
        }
    }
/////////////////////debug
///
/*
    QByteArray buf;
    buf.append("item[");
    buf.append(QString::number(itemNo,10));
    buf.append("] [");
    for(int i=0;i<itemNo;i++)
    {
        QString str = list.at(i);
        buf.append(str);
        if(i==itemNo-1)
        buf.append("]");
        else
        buf.append(",");
    }
    sendBack(buf);*/
/////////////////////debug end
    for(int i=0;i<itemNo;i++)
    {
        QString str = list.at(i);
        splitData.append(str);
    }
    if(type=="P"&&itemNo==8)
    {
        bool ret;
        int atId=0;
        int robotId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int pathId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int pointNum = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int pointId = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        double x = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        double y = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        double p = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        qDebug()<<"get path";
        emit updateRobotPath( socket,robotId,pathId, pointNum, pointId, x, y,p);
    }
    else if(type=="P"&&itemNo==6)
    {
        bool ret;
        int atId=0;
        int robotId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int pathId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int pointNum = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int pointId = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int mainPathId = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        //double x = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        //double y = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        //double p = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        qDebug()<<"get path2";
        emit updateRobotPathByMainPath(socket, robotId,pathId, pointNum, pointId,mainPathId);
    }
    else if(type=="CSN"&&itemNo==2)
    {
        bool ret;
        int atId=0;
        int sn = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        qDebug()<<"get sn";
        emit updateClientSN(socket,sn);
    }
    else if(type=="S"&&itemNo==10)
    {
        bool ret;
        int atId=0;
        int robotId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        double x = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        double y = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        double p = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        double left = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        double right = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}

        int goMainPathId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int robotState = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int robotType = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}

        //qDebug()<<"get robot state";
        emit updateRobotState(socket,robotId,x, y,p,left,right,goMainPathId,robotState,robotType);
    }
    else if((type=="@"||type=="#"||type=="$")&&itemNo==5)
    {
        bool ret;
        int atId=0;
        int xValue = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int yValue = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int angleValue = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        int tagNo = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        QwtPointCus::PointStyle style=QwtPointCus::Normal;


        if (type == "@") {
            style = QwtPointCus::Normal;
        } else if (type == "$"){
            style = QwtPointCus::Label;
        } else if (type == "#") {
            style = QwtPointCus::Dest;
        } else {
            //qDebug() << Q_FUNC_INFO << __LINE__ << type;
            return;
        }

        const double xScale = 100.0;
        const double yScale = 100.0;
        const double phiScale = 1000.0;
        QwtPointCus pointData(style,tagNo,(double)xValue/xScale,(double)yValue/yScale,(double)angleValue/phiScale);

        qDebug()<<"get bm point";
        emit newPoint(pointData);

    }
}


void MySerialPort::sendSN(int sn)
{
    QByteArray Buffer;
    Buffer.append("SN,");
    Buffer.append(QString::number(sn,10));
    Buffer.append("\n");
    socket->write(Buffer);
}

void MySerialPort::sendSNOK(int sn)
{
    QByteArray Buffer;
    Buffer.append("SNOK,");
    Buffer.append(QString::number(sn,10));
    Buffer.append("\n");
    socket->write(Buffer);
}
