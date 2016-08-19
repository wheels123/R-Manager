/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "fortuneserver.h"
#include "fortunethread.h"
#include "myclient.h"
#include <stdlib.h>

//! [0]
FortuneServer::FortuneServer(QObject *parent)
    : QTcpServer(parent)
{
    fortunes << tr("You've been leading a dog's life. Stay off the furniture.")
             << tr("You've got to think about tomorrow.")
             << tr("You will be surprised by a loud noise.")
             << tr("You will feel hungry again in another hour.")
             << tr("You might have mail.")
             << tr("You cannot kill time without injuring eternity.")
             << tr("Computers are not intelligent. They only think they are.");

    serialNumber=ROBOT_SN_MIN;
    m_nTimerId=0;
    m_nTimerCnt=0;
    m_nTimerId = startTimer(100);
/*
    connect(&robot,
            &Robot::robotMsg,
            this,
            &FortuneServer::robotMsg);*/
}
//! [0]

FortuneServer::~FortuneServer()
{
    if ( m_nTimerId != 0 )
        killTimer(m_nTimerId);
}

//! [1]
void FortuneServer::incomingConnection(qintptr socketDescriptor)
{
    //QString fortune = fortunes.at(qrand() % fortunes.size());

    /*
    FortuneThread *thread = new FortuneThread(socketDescriptor, fortune, this);

    int ret = clientSocket.inseartClient(socketDescriptor);
    if(ret >=0)
    {
        QTcpSocket *tcpSocket=new QTcpSocket();
        //! [1] //! [2]
            if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {

                return;
            }
        //! [2] //! [3]
        emit newConnection(tcpSocket);
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
    }
*/
    MyClient *client = new MyClient(this);
    client->SetSocket(socketDescriptor);
    clientList.inseartClient(client->getSocket());



    connect(client,
            &MyClient::updateClientSN,
            this,
            &FortuneServer::onUpdateClientSN);

    connect(client,
            &MyClient::updateClientStatus,
            this,
            &FortuneServer::onUpdateClientStatus);

    connect(client,
            &MyClient::updateRobotPath,
            this,
            &FortuneServer::onUpdateRobotPath);

    connect(client,
            &MyClient::updateRobotPathByMainPath,
            this,
            &FortuneServer::onUpdateRobotPathByMainPath);

    connect(client,
            &MyClient::updateRobotState,
            this,
            &FortuneServer::onUpdateRobotState);

    connect(client,
            &MyClient::newPoint,
            this,
            &FortuneServer::onNewPoint);
    //client->sendSN(newSerialNumber());

    for(int i=0;i<clientList.getClientVecSize();)
    {
        QTcpSocket *skt = clientList.getClientVecByIndex(i);
        if(skt->peerAddress().toString()==""||skt->peerPort()==0)
        {
            clientList.deleteClient(i);
        }
        else
        {
            i++;
        }
    }

    emit newConnection(&clientList,client->getSocket(),(ClientSocketList::clientStatus)0);
    //emit newConnection(client->getSocket());

}

void FortuneServer::sendDate()
{
    for(int i=0;i<clientList.getClientVecSize();)
    {
        QByteArray Buffer;
        QTcpSocket *skt=clientList.getClientVecByIndex(i);
        Buffer.append("\r\nserver send to num [");
        Buffer.append(QString::number(clientList.getClientVecSize(),10));
        Buffer.append(" ");
        Buffer.append(QString::number(i,10));
        Buffer.append("] ip");
        Buffer.append(skt->peerAddress().toString());
        Buffer.append(" port");
        Buffer.append(QString::number(skt->peerPort(),10));
        qint64 num = skt->write(Buffer);
        skt->waitForBytesWritten(100);
        if(num==-1)
        {
            clientList.deleteClient(skt);
        }
        else
        {
            i++;
        }
    }

}
void FortuneServer::onUpdateClientStatus(QTcpSocket* socket,int state)
{
    clientList.setStatusFromSocket(socket,(ClientSocketList::clientStatus)state);
    QByteArray Buffer;
    Buffer.append("\r\nserver emit from ip ");
    Buffer.append(socket->peerAddress().toString());
    Buffer.append(" port ");
    Buffer.append(QString::number(socket->peerPort(),10));
    Buffer.append(" status ");
    Buffer.append(QString::number(state,10));
    qDebug( Buffer.data());

    if(state==2)
    {
        clientList.deleteClient(socket);
        int ret = deletePathWithClientList();
        if(ret>0)
        {
             emit updataRobotPathServer(&robot);
        }
    }
    for(int i=0;i<clientList.getClientVecSize();)
    {
        QTcpSocket *skt = clientList.getClientVecByIndex(i);
        if(skt->peerAddress().toString()==""||skt->peerPort()==0)
        {
            clientList.deleteClient(i);
        }
        else
        {
            i++;
        }
    }
    emit newConnection(&clientList,socket,(ClientSocketList::clientStatus)state);
}



 void FortuneServer::onUpdateRobotPath(QTcpSocket* socket,int robotId, int pathId,int pointNum,int pointId,double x,double y,double phi)
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
    if(clientList.findSNFromSocket(socket)!=robotId)
    {
        qDebug() <<"onUpdateRobotPath  sn not match socket "<<QString::number(robotId,10);
        return;
    }
    int ret = robot.insertPathPoint(robotId,pathId,rp,pointNum,pointId);
     qDebug() <<"insertPathPoint ret "<<QString::number(ret,10);

    robot.updateControlNum();
    emit updataRobotPathServer(&robot);
 }

 void FortuneServer::onUpdateRobotPathByMainPath(QTcpSocket* socket,int robotId, int pathId,int pointNum,int pointId,int mainPathId)
 {
    if(findSerialNumber(robotId)<0)
    {
     qDebug() <<"onUpdateRobotPath  sn not exit "<<QString::number(robotId,10);
     return;
    }
    if(clientList.findSNFromSocket(socket)!=robotId)
    {
     qDebug() <<"onUpdateRobotPath  sn not match socket "<<QString::number(robotId,10);
     return;
    }
    int ret = robot.insertPathPoint(robotId,pathId,mainPathId,pointNum,pointId);
     qDebug() <<"insertPathPoint2 ret "<<QString::number(ret,10);
    emit updataRobotPathServer(&robot);
 }

 void FortuneServer::loadData(QCurveDataCus *data,QwtPointCus::PointStyle type)
 {
     robot.loadData(data,type);
 }

 Robot* FortuneServer::getRobotHandle()
 {
     return &robot;
 }

int FortuneServer::getSerialNumberMax()
{
   int size = clientList.getSNSize();
   int newSN=0;
   for(int i=0;i<size;i++)
   {
       int id=clientList.getSNByIndex(i);
       if(newSN<id)
       {
           newSN = id;
       }
   }

   return newSN;
}



void FortuneServer::onUpdateClientSN(QTcpSocket* socket,int sn)
{
    int find = findSerialNumber(sn);
    QByteArray Buffer;

    if(sn<ROBOT_SN_MIN)
    {
        Buffer.append("\r\nonUpdateClientSN new sn invalid ");
        Buffer.append(QString::number(sn,10));
        qDebug( Buffer.data());
        return;
    }
    if(find>=0)
    {
        Buffer.append("\r\nonUpdateClientSN new sn exist ");
        Buffer.append(QString::number(sn,10));
        qDebug( Buffer.data());
        return;
    }
    clientList.setSerialNumberFromSocket(socket,sn);
    Buffer.append("\r\nserver emit from ip ");
    Buffer.append(socket->peerAddress().toString());
    Buffer.append(" port ");
    Buffer.append(QString::number(socket->peerPort(),10));
    Buffer.append(" sn ");
    Buffer.append(QString::number(sn,10));
    qDebug( Buffer.data());

    sendSNOK(socket,sn);
    emit newClientSN(&clientList,socket,sn);
}

int FortuneServer::findSerialNumber(int sn)
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

int FortuneServer::newSerialNumber()
{
    int ret=serialNumber;
    int max=getSerialNumberMax();
    if(ret<=max)
    {
        ret=max+1;
    }
    serialNumber++;
    return ret;
}

int FortuneServer::getClientSNBySocket(QTcpSocket *socket)
{
    int a=(int)socket;
    return a;
}

int FortuneServer::deletePathWithClientList()
{
    int deleteNum=0;
    for(int i=0;i<robot.getPathNum();i++)
    {
        RobotPath rp=robot.getPathByIndex(i);
        int sn = rp.robotId;
        if(sn<ROBOT_SN_MIN)
        {
           continue;
        }
        bool exist = clientList.findSNExist(sn);
/*
        QByteArray Buffer;
        Buffer.append("\r\deletePathWithClientList i");
        Buffer.append(QString::number(i,10));
        Buffer.append(" sn ");
        Buffer.append(QString::number(sn,10));
        Buffer.append(" exist ");
        Buffer.append(QString::number((int)exist,10));
        //Buffer.append(" clientnum ");
        qDebug( Buffer.data());
*/
        if(exist==false)
        {
            robot.erasePathByIndex(i);
            deleteNum++;
            i--;
        }
    }
    return deleteNum;
}


void FortuneServer::onUpdateRobotState(QTcpSocket* socket,int robotId,double x,double y,double phi,double left,double right,int goMainPathId,int robotState,int robotType)
{
    if(findSerialNumber(robotId)<0)
    {
     qDebug() <<"onUpdateRobotState  sn not exit "<<QString::number(robotId,10);
     return;
    }
    if(clientList.findSNFromSocket(socket)!=robotId)
    {
     qDebug() <<"onUpdateRobotState  sn not match socket "<<QString::number(robotId,10);
     return;
    }
    RobotPoint rp;
    rp.x=x;
    rp.y=y;
    rp.phi=phi;

    int ret = robot.insertRobotState(robotId,rp,left,right,goMainPathId,robotState,robotType);
     //qDebug() <<"insertRobotState ret "<<QString::number(ret,10);

    //robot.updateControlNum();
    //robot.resetControlValue();
    emit updataRobotPathServerState(&robot);

    emit onNewRobotMsg(robot.getMsg());
}

void FortuneServer::timerEvent( QTimerEvent *event )
{
    m_nTimerCnt++;
    //qDebug( "FortuneServer timer event, id %d",event->timerId() );
    //robot.pathControl();


    QVector<int> a=robot.getRobotControl();

    //qDebug() <<"FortuneServer timer event "<<QString::number(a.size(),10)<<QString::number(robot.getPathNum(),10);
    if(a.size()!=robot.getPathNum()) return;

    for(int i=0;i<a.size();i++)
    {
        int sn = robot.getPathRobotIdByIndex(i);
        if(sn>=ROBOT_SN_MIN)
        {
            QTcpSocket* skt = clientList.findSocketBySN(sn);
            if(skt!=NULL)
            {
                sendControlCmd(skt,a.at(i));
            }
            int index=-1;
            bool ret = robot.findRobotId(sn,index);
            if(ret==false)
            {
                robot.clearPath(i);
            }
        }
    }
#if 1
    if(m_nTimerCnt>20)
    {
        m_nTimerCnt=0;
        for(int i=0;i<clientList.getClientVecSize();i++)
        {
            QTcpSocket *skt = clientList.getClientVecByIndex(i);
            if(skt==NULL) continue;
            int ret =clientList.findSNFromSocket(skt);
            if(ret<0)
            {
                int sn=newSerialNumber();
                QByteArray Buffer;
                Buffer.append("SN,");
                Buffer.append(QString::number(sn,10));
                Buffer.append("\n");
                skt->write(Buffer);
            }
        }
    }
#endif
}

void FortuneServer::sendSNOK(QTcpSocket* socket,int sn)
{
    QByteArray Buffer;
    Buffer.append("SNOK,");
    Buffer.append(QString::number(sn,10));
    Buffer.append("\n");
    socket->write(Buffer);
}

void FortuneServer::sendControlCmd(QTcpSocket* socket,int cmd)
{
    QByteArray Buffer;
    Buffer.append("CTRL,");
    Buffer.append(QString::number(cmd,10));
    Buffer.append("\n");
    socket->write(Buffer);
}

void FortuneServer::robotMsg(QString str)
{
    emit onNewRobotMsg(str);

}

 QVector<RobotPoint> FortuneServer::getPose(int n)
 {
     robot.estimateRobotPose(n);
     return robot.getPose(n);
 }

 QVector<QVector<RobotPathPoint>> FortuneServer::getPose()
 {
     robot.estimateRobotPose();
     return robot.getPose();
 }

 void FortuneServer::onNewPoint(const QwtPointCus point)
 {
     emit newPointServer(point);
 }
