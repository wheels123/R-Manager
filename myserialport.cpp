#include "myserialport.h"

MySerialPort::MySerialPort(QObject *parent ):
    QSerialPort(parent)
{
    m_nTimerId=0;
    m_nTimerId = startTimer(100);
}
MySerialPort::~MySerialPort()
{
    if ( m_nTimerId != 0 )
        killTimer(m_nTimerId);
}

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

    setPort(portInfo);
    if(open(QIODevice::ReadWrite))
    {
        qDebug() << "m_reader.open(QIODevice::ReadWrite)";
        setBaudRate(QSerialPort::Baud4800);
        setParity(QSerialPort::NoParity);
        setDataBits(QSerialPort::Data8);
        setStopBits(QSerialPort::OneStop);
        setFlowControl(QSerialPort::NoFlowControl);

        clearError();
        clear();
        connect(this, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
        return true;
    }
}

void MySerialPort::readyReadSlot()
{
    while (canReadLine()) {
        QByteArray arr = readLine();
        if (arr.isEmpty()) {
            qDebug() << Q_FUNC_INFO << __LINE__;
            break;
        }
        qDebug()<<arr.data();
        QString s;
        s.prepend(arr);
        processData(s);
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
            write(buf);
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
    else if(type=="CSN"&&itemNo==2)
    {
        bool ret;
        int atId=0;
        int sn = splitData.at(++atId).toInt(&ret,10); if(ret==false){qDebug() << "trans path error data["<<data<<"] at["<<QString::number(atId, 10)<<"]";return;}
        qDebug()<<"get sn";
        onUpdateSN(sn);
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
        onUpdateRobotState(robotId,x, y,p,left,right,goMainPathId,robotState,robotType);
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
        onNewPoint(pointData);

    }
}


 void MySerialPort::onUpdateRobotPath(int robotId, int pathId,int pointNum,int pointId,double x,double y,double phi)
 {
    RobotPoint rp;
    rp.x=x;
    rp.y=y;
    rp.phi=phi;

    int ret = robot.insertPathPoint(robotId,pathId,rp,pointNum,pointId);
     qDebug() <<"insertPathPoint by piont ret "<<QString::number(ret,10);

    robot.updateControlNum();
    emit updataRobotPathServer(&robot);
 }

 void MySerialPort::onUpdateRobotPathByMainPath(int robotId, int pathId,int pointNum,int pointId,int mainPathId)
 {
    int ret = robot.insertPathPoint(robotId,pathId,mainPathId,pointNum,pointId);
     qDebug() <<"insertPathPoint by id ret "<<QString::number(ret,10);
    emit updataRobotPathServer(&robot);
 }

 void MySerialPort::loadData(QCurveDataCus *data,QwtPointCus::PointStyle type)
 {
     robot.loadData(data,type);
 }

 Robot* MySerialPort::getRobotHandle()
 {
     return &robot;
 }


 void MySerialPort::onUpdateRobotState(int robotId,double x,double y,double phi,double left,double right,int goMainPathId,int robotState,int robotType)
 {
     RobotPoint rp;
     rp.x=x;
     rp.y=y;
     rp.phi=phi;

     int ret = robot.insertRobotState(robotId,rp,left,right,goMainPathId,robotState,robotType);

     emit updataRobotPathServerState(&robot);

     emit onNewRobotMsg(robot.getMsg());
 }

 void MySerialPort::timerEvent( QTimerEvent *event )
 {
     for(int i=0;i<robot.getPathNum();i++)
     {
         int sn = robot.getPathRobotIdByIndex(i);
         if(sn>=ROBOT_SN_MIN)
         {
            //sendControlCmd(sn,robot.getRobotControl(i));
         }
     }

 }


 void MySerialPort::sendUpdateLabel()
 {
     QByteArray dataWrite;
     QString str;
     str.sprintf("$UDL%5d\n",0);
     dataWrite=str.toLatin1();
     write(dataWrite,dataWrite.size());
 }

 void MySerialPort::sendDelelteLabel(int labelNum)
 {
     QByteArray dataWrite;
     QString str;
     str.sprintf("$UDL%5d\n",labelNum);
     dataWrite=str.toLatin1();
     write(dataWrite,dataWrite.size());
 }


 void MySerialPort::sendControlCmd(int sn,int cmd)
 {
     QByteArray Buffer;
     Buffer.append("CTRL,");
     Buffer.append(QString::number(sn,10));
     Buffer.append(",");
     Buffer.append(QString::number(cmd,10));
     Buffer.append("\n");
     write(Buffer);
 }

  QVector<RobotPoint> MySerialPort::getPose(int n)
  {
      robot.estimateRobotPose(n);
      return robot.getPose(n);
  }

  QVector<QVector<RobotPathPoint>> MySerialPort::getPose()
  {
      robot.estimateRobotPose();
      return robot.getPose();
  }

  void MySerialPort::onNewPoint(const QwtPointCus point)
  {
      emit newPointServer(point);
  }

  void MySerialPort::onUpdateSN(int sn)
  {
      int index=0;
      bool find = robot.findRobotId(sn,index);
      QByteArray Buffer;

      if(sn<ROBOT_SN_MIN)
      {
          Buffer.append("\r\nonUpdateClientSN new sn invalid ");
          Buffer.append(QString::number(sn,10));
          qDebug( Buffer.data());
          return;
      }
      if(find==true)
      {
          Buffer.append("\r\nonUpdateClientSN new sn exist ");
          Buffer.append(QString::number(sn,10));
          qDebug( Buffer.data());
          return;
      }
      Buffer.append("\r\nonUpdateSN ");
      Buffer.append(QString::number(sn,10));
      qDebug( Buffer.data());

      RobotPoint rp;
      rp.x=0;
      rp.y=0;
      rp.phi=0;
      int ret = robot.insertRobotState(sn,rp,0,0,0,0,0);
      emit updataRobotPathServerState(&robot);
      emit onNewRobotMsg(robot.getMsg());
      sendSNOK(sn);
  }

  void MySerialPort::sendSN(int sn)
  {
      QByteArray Buffer;
      Buffer.append("SN,");
      Buffer.append(QString::number(sn,10));
      Buffer.append("\n");
      write(Buffer);
  }

  void MySerialPort::sendSNOK(int sn)
  {
      QByteArray Buffer;
      Buffer.append("SNOK,");
      Buffer.append(QString::number(sn,10));
      Buffer.append("\n");
      write(Buffer);
  }
