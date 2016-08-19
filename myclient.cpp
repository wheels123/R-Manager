#include "myclient.h"
#include "robot.h"
MyClient::MyClient(QObject *parent) :
    QObject(parent)
{
    QThreadPool::globalInstance()->setMaxThreadCount(15);
    m_nTimerId=0;
    m_disconnect_times=0;
    m_heart=true;
    m_connected=true;
}

MyClient::~MyClient()
{
    if ( m_nTimerId != 0 )
        killTimer(m_nTimerId);
}


void MyClient::SetSocket(int Descriptor)
{
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(Descriptor);
    socket->readAll();
    socket->flush();
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));



    m_nTimerId = startTimer(1000);
    m_status=0;
    emit updateClientStatus(socket,m_status);
    qDebug() << "client connected";
}

void MyClient::connected()
{
    qDebug() << "client connected event";
}

void MyClient::disconnected()
{
    if ( m_nTimerId != 0 )
        killTimer(m_nTimerId);

    m_nTimerId=0;
    //emit updateClientStatus(socket,2);
    m_status=2;
    emit updateClientStatus(socket,m_status);
    qDebug() << "client disconnected";
}


inline void MyClient::processData(QString &data)
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

inline void MyClient::transData(QString &data)
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

void MyClient::readyRead()
{
    while (socket->canReadLine()) {
        QString data = socket->readLine();
        if (data.isEmpty()) {
            qDebug() << Q_FUNC_INFO << __LINE__;

            break;
        }
        qDebug()<<data;
        processData(data);
    }

    /*
    QByteArray Buffer = socket->readAll();
    qDebug() << Buffer;
    int index = Buffer.indexOf("~");
    if(index>=0)
    {
        QByteArray buf;
        buf.append("heart!");
        sendBack(buf);
        m_heart=true;
    }
    else
    {
        sendBack(Buffer);
    }

    //Time Consumer
    MyTask *mytask = new MyTask;
    mytask->setAutoDelete(true);
    connect(mytask, SIGNAL(Result(int)), this, SLOT(TaskResult(int)));
    QThreadPool::globalInstance()->start(mytask);
    */
}

void MyClient::TaskResult(int Number)
{
    //right here
    QByteArray Buffer;
    Buffer.append("\r\nTask Result = ");
    Buffer.append(QString::number(Number));

    socket->write(Buffer);
}

void MyClient::sendBack(QByteArray buf)
{
    //right here
    QByteArray Buffer;
    Buffer.append("\r\nTask Result = ");
    Buffer.append(buf);
    socket->write(Buffer);
}

QTcpSocket* MyClient::getSocket()
{
    return socket;
}

void MyClient::timerEvent( QTimerEvent *event )
{
    //qDebug( "timer event, id %d",event->timerId() );
    if(event->timerId()==0)
    {

    }
    int last_m_status=m_status;
    if(m_heart==false)
    {

        if(m_disconnect_times>2)
        {
            m_connected=false;
            if(m_status==0)
            {
                m_status=1;
            }
        }
        else
        {
            m_disconnect_times++;
        }
    }
    else
    {
        m_disconnect_times=0;
        m_connected=true;
        m_status=0;
    }
    m_heart=false;
    if(m_status!=last_m_status)
    {
        emit updateClientStatus(socket,m_status);
    }

}

void MyClient::sendSN(int sn)
{
    QByteArray Buffer;
    Buffer.append("SN,");
    Buffer.append(QString::number(sn,10));
    Buffer.append("\n");
    socket->write(Buffer);
}

void MyClient::sendSNOK(int sn)
{
    QByteArray Buffer;
    Buffer.append("SNOK,");
    Buffer.append(QString::number(sn,10));
    Buffer.append("\n");
    socket->write(Buffer);
}

