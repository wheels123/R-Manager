#include <QtWidgets>
#include <QtNetwork>
#include "udpreceiver.h"

UdpReceiver::UdpReceiver(QWidget *parent)
    : QWidget(parent)
{


    udpSocket = new QUdpSocket(this);

    irpTime=200;
    lastReadTime=0;
    RxData_100="";
    RxData_102="";
    RxData_104="";
    /*
    udpSocket->bind(45454, QUdpSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(processPendingDatagrams()));

*/
}

bool UdpReceiver::init( QHostAddress &address, quint16 port)
{
    bool ret=udpSocket->bind(address,port, QUdpSocket::ShareAddress);
    if(ret)
    {
        connect(udpSocket, SIGNAL(readyRead()),
                this, SLOT(processPendingDatagrams()));
        m_time.start();
        m_timeSendCtrl = new QTimer(this);
        connect(m_timeSendCtrl, SIGNAL(timeout()), this, SLOT(timerEventSendCtrl()));

        m_timeSendCtrl->start(irpTime);
        return true;
    }
    else
    {
        return false;
    }

}

void UdpReceiver::processPendingDatagrams()
{
    lastReadTime=m_time.elapsed();

    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        //int num = udpSocket->pendingDatagramSize();

        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress address;
        quint16 port=0;
        udpSocket->readDatagram(datagram.data(), datagram.size(),&address,&port);

        //qDebug()<<"udp "<<datagram.data()<<address.toString()<<QString::number(port,10);

        QString s;
        s.prepend(datagram);
        processData(s,address);
    }

    /*
    while (udpSocket->canReadLine()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress address;
        quint16 port=0;
        udpSocket->readDatagram(datagram.data(), datagram.size(),&address,&port);
    }
    while (socket->canReadLine()) {
        QString data = socket->readLine();
        if (data.isEmpty()) {
            qDebug() << Q_FUNC_INFO << __LINE__;

            break;
        }
        qDebug()<<data;
        processData(data);
    }
    */
}


void UdpReceiver::loadData(QCurveDataCus *data,QwtPointCus::PointStyle type)
{
    robot.loadData(data,type);
}

Robot* UdpReceiver::getRobotHandle()
{
    return &robot;
}



 void UdpReceiver::onUpdateRobotPath(int robotId, int pathId,int pointNum,int pointId,double x,double y,double phi)
 {
    RobotPoint rp;
    rp.x=x;
    rp.y=y;
    rp.phi=phi;
    if(findSerialNumber(robotId)<0)
    {
        qDebug() <<"onUpdateRobotPath  sn not exit "<<QString::number(robotId,10);
        return;
    }
    /*
    if(clientList.findSNFromSocket(address)!=robotId)
    {
        qDebug() <<"onUpdateRobotPath  sn not match socket "<<QString::number(robotId,10);
        return;
    }*/
    int ret = robot.insertPathPoint(robotId,pathId,rp,pointNum,pointId);
     qDebug() <<"insertPathPoint ret "<<QString::number(ret,10);

    robot.updateControlNum();
    emit updataRobotPathServer(&robot);
 }

 void UdpReceiver::onUpdateRobotPathByMainPath(int robotId, int pathId,int pointNum,int pointId,int mainPathId)
 {
    if(findSerialNumber(robotId)<0)
    {
     qDebug() <<"onUpdateRobotPath  sn not exit "<<QString::number(robotId,10);
     return;
    }
    /*
    if(clientList.findSNFromSocket(address)!=robotId)
    {
     qDebug() <<"onUpdateRobotPath  sn not match socket "<<QString::number(robotId,10);
     return;
    }*/
    int ret = robot.insertPathPoint(robotId,pathId,mainPathId,pointNum,pointId);
     qDebug() <<"insertPathPoint2 ret "<<QString::number(ret,10);
    emit updataRobotPathServer(&robot);
 }

 int UdpReceiver::findSerialNumber(int sn)
 {
     int size = clientList.getSNSize();
     for(int i=0;i<size;i++)
     {
         if(clientList.getSNByIndex(i)==sn)
         {
             return i;
         }
     }
     return -1;
 }

 inline void UdpReceiver::processData(QString &data,QHostAddress &address)
 {
     const QString itemSep = "\n";
     QString *curData=NULL;
     QStringList list = data.split(itemSep);

     int itemNo = list.size();

     if(itemNo<=0) return;

     if(address.toString()=="192.168.1.100"||address.toString()=="192.168.1.3")
     {
         curData=&RxData_100;
         curData->append(list.at(0));
     }
     else if(address.toString()=="192.168.1.102")
     {
         curData=&RxData_102;
         curData->append(list.at(0));
     }
     else if(address.toString()=="192.168.1.104")
     {
         curData=&RxData_104;
         curData->append(list.at(0));
     }

     if(curData==NULL) return;
     if(itemNo==1)
     {
       return;
     }
     else
     {
         transData(*curData,address);
         *curData="";
         QString str;
         for(int i=1;i<list.size()-1;i++)
         {
             str = list.at(i);
             int size=str.size();
             if(size>0)
             {
                 transData(str,address);
             }
         }
         str = list.at(list.size()-1);
         if(str=="")
         {
             *curData="";
         }
         else
         {
            *curData=str;
         }
     }
 }

 inline void UdpReceiver::transData(QString &data,QHostAddress &address)
 {
     const QString itemSep = ",";
     if(data==""||data==NULL) return;
     QStringList list = data.split(itemSep);
     int itemNo = list.size();
     int dataSize = data.size();
     QVector<QString> splitData;

     if(itemNo<=0) return;

     if(dataSize<6) return;

     QString type = list.at(0);
     if(dataSize==1)
     {
         //int index = data.indexOf("~");
         //if(index>=0)
         /*
         if(data=="~")
         {
             QByteArray buf;
             buf.append("`\n");
             write(buf);
             m_heart=true;
             qDebug()<<"heart!";
         }
         */
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
         onUpdateRobotPath( robotId,pathId, pointNum, pointId, x, y,p);
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
         onUpdateRobotPathByMainPath( robotId,pathId, pointNum, pointId,mainPathId);
     }
     else if(type=="Q")
     {
         bool ret;
         int atId=0;
         int robotId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         int pathId = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         int pointNum = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}

         if(pointNum>=0&&pointNum+5==itemNo)
         {
             QVector<int> path;
             for(int i=0;i<pointNum;i++)
             {
                 int a = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
                 path.append(a);
             }

             int crc = splitData.at(++atId).toInt(&ret,16);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}


             char*  ch;
             QByteArray ba = data.toLatin1();
             ch=ba.data();
             if(strlen(ch)<=4)
             {
                 qDebug()<<"get path Q len <4";
                 return;
             }

             int n=0;
             int lenth=strlen(ch);
             for(int i=lenth-2;i>=lenth-5;i--)
             {
                 if(ch[i]==',')
                 {
                     n=i+1;
                     break;
                 }
             }
             //qDebug()<<"get path Q len "<<n;
             unsigned short crc16=CRC16((unsigned char *)ch,n);

             if(crc == crc16)
             {
                 //qDebug()<<"get path Q date size "<<QString::number(dataSize,10)<<" crc16 "<<QString::number(crc16,16);
                 onUpdateRobotPath( address,robotId,pathId,path);
             }
             else
             {
                 qDebug()<<"get path Q crc err "<<QString::number(crc,16)<<" calcu crc"<<QString::number(crc16,16);
             }
             //


         }

     }
     else if(type=="CSN"&&itemNo==3)
     {
         bool ret;
         int atId=0;
         int sn = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         int crc = splitData.at(++atId).toInt(&ret,16);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}

         char*  ch;
         QByteArray ba = data.toLatin1();
         ch=ba.data();
         if(strlen(ch)<=4)
         {
             qDebug()<<"get CSN len <4";
             return;
         }
         unsigned short crc16=CRC16((unsigned char *)ch,strlen(ch)-4);

         if(crc == crc16)
         {
             qDebug()<<"get sn date size "<<QString::number(dataSize,10)<<" crc16 "<<QString::number(crc16,16);
             //onUpdateSN(sn);
         }
         else
         {
             qDebug()<<"get sn crc err "<<QString::number(crc,16)<<" calcu crc"<<QString::number(crc16,16);
         }

     }
     else if(type=="S"&&itemNo==12)
     {
         bool ret;
         int atId=0;
         int robotId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         int pathId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         double x = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         double y = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         double p = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         double left = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         double right = splitData.at(++atId).toDouble(&ret);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}

         int goMainPathId = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         int robotState = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
         int robotType = splitData.at(++atId).toInt(&ret,10);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}

         int crc = splitData.at(++atId).toInt(&ret,16);if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}

         //qDebug()<<"get robot state crc "<<QString::number(crc,16);

         //qDebug()<<"get robot state";
         //

         char*  ch;
         QByteArray ba = data.toLatin1();
         ch=ba.data();
         if(strlen(ch)<=4)
         {
             qDebug()<<"get robot state len <4";
             return;
         }

         int n=0;
         int lenth=strlen(ch);
         for(int i=lenth-2;i>=lenth-5;i--)
         {
             if(ch[i]==',')
             {
                 n=i+1;
                 break;
             }
         }
         //qDebug()<<"get robot state len "<<n;
         unsigned short crc16=CRC16((unsigned char *)ch,n);

         if(crc == crc16)
         {
             //qDebug()<<"get robot state data size "<<QString::number(dataSize,10)<<" crc16 "<<QString::number(crc16,16);
             onUpdateRobotState(address,robotId,pathId,x, y,p,left,right,goMainPathId,robotState,robotType);
         }
         else
         {
             qDebug()<<"get robot state crc err "<<QString::number(crc,16)<<" calcu crc"<<QString::number(crc16,16);
         }
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
         //onNewPoint(pointData);

     }
 }


 void UdpReceiver::onUpdateRobotState(QHostAddress &address, int robotId,int pathId,double x,double y,double phi,double left,double right,int goMainPathId,int robotState,int robotType)
 {
     RobotPoint rp;
     rp.x=x;
     rp.y=y;
     rp.phi=phi;
    int ret = clientList.inseartClient(address,robotId);

    if(ret==1)
    {
        QByteArray Buffer;
        Buffer.append("\r\nstate get client from ip ");
        Buffer.append(address.toString());
        Buffer.append(" sn ");
        Buffer.append(QString::number(robotId,10));
        qDebug( Buffer.data());
    }
    robot.insertRobotState(robotId,10,rp,left,right,goMainPathId,robotState,robotType,m_time.elapsed());

     //emit updataRobotPathServerState(&robot);

     //emit onNewRobotMsg(robot.getMsg());
 }

 void UdpReceiver::onUpdateRobotPath(QHostAddress &address,int robotId,int pathId,QVector<int> pointList)
 {
     int ret = clientList.inseartClient(address,robotId);

     if(ret==1)
     {
         QByteArray Buffer;
         Buffer.append("\r\npath get client from ip ");
         Buffer.append(address.toString());
         Buffer.append(" sn ");
         Buffer.append(QString::number(robotId,10));
         qDebug( Buffer.data());
     }

     robot.insertPathPointList(robotId,pathId,pointList,m_time.elapsed());

     //emit updataRobotPathServerState(&robot);

     //emit onNewRobotMsg(robot.getMsg());
 }


 static unsigned char auchCRCHi[] = {
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
 0x40
 } ;

 static char auchCRCLo[] = {
 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
 0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
 0x40
 };
 unsigned short UdpReceiver::CRC16(unsigned char *puchMsg,
 unsigned short usDataLen
 )
 {
 unsigned char uchCRCHi = 0xFF;
 unsigned char uchCRCLo = 0xFF;
 unsigned uIndex ;
 while (usDataLen--)
 {
 uIndex = uchCRCLo ^ *puchMsg++;
 uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
 uchCRCHi = auchCRCLo[uIndex];
 }
 return (uchCRCHi << 8 | uchCRCLo);
 }



 void UdpReceiver::timerEventSendCtrl( )
 {
     static int cnt=0;
     static int addTimeNext=0;
     if(cnt<robot.getPathNum())
     {
         int curTime=m_time.elapsed();

         int sn = robot.getPathRobotIdByIndex(cnt);
         RobotPath rp;
         bool ret = robot.getPathById(sn,rp);
         if(sn>=ROBOT_SN_MIN&&ret)
         {
            if(curTime-rp.updateTime>30000)  //no connection  delete robot
            {
               robot.deletePath(sn);
               cnt--;
            }
            else
            {
               sendControlCmd(sn,robot.getRobotControl(cnt),rp.id);
            }
            //sendControlCmd(sn,robot.getRobotControl(cnt),rp.id);
         }
         addTimeNext=0;
         m_timeSendCtrl->setInterval(irpTime);

         cnt++;
         if(cnt>=robot.getPathNum())
         {
             cnt=0;
         }
     }
     else
     {
         cnt=0;
         m_timeSendCtrl->setInterval(irpTime);
     }

 }


 void UdpReceiver::sendControlCmd(int sn,int cmd,int pathId)
 {
     QByteArray Buffer;

   QHostAddress address;
     bool ret = clientList.findSocketBySN(sn,address);
    if(!ret)
    {
        qDebug()<<"sendControlCmd err cannot find sn address"<<QString::number(sn,10);
        return;
    }

    qDebug()<<"sendControlCmd find address"<<QString::number(sn,10)<<address;

     Buffer.append("CTRL,");
     Buffer.append(QString::number(sn,10));
     Buffer.append(",");
     Buffer.append(QString::number(cmd,10));
     Buffer.append(",");
     Buffer.append(QString::number(pathId,10));
     Buffer.append("\n");

/*
     udpSocket->writeDatagram(Buffer, Buffer.size(),
                              QHostAddress::Broadcast, 45454);
                              */
     udpSocket->writeDatagram(Buffer, Buffer.size(),
                              address, 1);
 }


 QVector<QVector<RobotPathPoint>> UdpReceiver::getPose()
 {
     robot.estimateRobotPose();
     return robot.getPose();
 }
