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

#ifndef FORTUNESERVER_H
#define FORTUNESERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include "robot.h"
#include "QTimer.h"
class QTcpServer;
class QTcpSocket;

class ClientSocketList
{
public:
    #define TCP_SOCKET_MAX 50
    enum clientStatus{Connected=0,NoHeart,Disconnected};
    qintptr* socketDescriptor;
    bool* receiveHeart;
    int* noHeartTimes;
    int curNum;
private:
    QTcpSocket *socket;
    QVector<QTcpSocket *> vectorSocket;
    QVector<int> vectorPort;
    QVector<clientStatus> vectorSocketState;
    QVector<int> vectorSocketSerialNumber;
public:
    ClientSocketList()
    {
        socketDescriptor=new qintptr[TCP_SOCKET_MAX];
        receiveHeart = new bool[TCP_SOCKET_MAX];
        noHeartTimes = new int[TCP_SOCKET_MAX];
        curNum=0;
        socket=NULL;
    }

    void reset()
    {
        for (int i = 0; i < TCP_SOCKET_MAX; i++)
        {
            socketDescriptor[i]=0;
            receiveHeart[i] = true;
            noHeartTimes[i] = 0;
            curNum = 0;
        }
        socket=NULL;
    }

    int inseartClient2(QTcpSocket *skt)
    {
        if(curNum>=TCP_SOCKET_MAX-1) return -1;
        socket = skt;
        curNum++;
        return 1;
    }

    int inseartClient(QTcpSocket *skt)
    {
        if(curNum>=TCP_SOCKET_MAX-1) return -1;
        vectorSocket.append(skt);
        vectorSocketState.append(Connected);
        vectorSocketSerialNumber.append(-1);
        curNum++;
        return 1;
    }

    int deleteClient(int index)
    {
        int n=vectorSocket.size();
        if(n<=0) return 0;
        if(index>n-1) return 0;
        vectorSocket.erase(vectorSocket.begin()+index);
        vectorSocketState.erase(vectorSocketState.begin()+index);
        vectorSocketSerialNumber.erase(vectorSocketSerialNumber.begin()+index);
        return 1;
    }

    int deleteClient(QTcpSocket *skt)
    {
        bool find=false;
        for (int i = 0; i < vectorSocket.count();) {
            QTcpSocket* l_socket = vectorSocket.at(i);

            if((l_socket->peerAddress().toString()==skt->peerAddress().toString())&&(l_socket->peerPort()==skt->peerPort()))
            {
                vectorSocket.erase(vectorSocket.begin()+i);
                vectorSocketState.erase(vectorSocketState.begin()+i);
                vectorSocketSerialNumber.erase(vectorSocketSerialNumber.begin()+i);
                find=true;
            }
            else
            {
                i++;
            }
        }
        if(find==true) return 1;
        else return 0;
    }
    int deleteClient(QString str, int port)
    {
        bool find=false;
        for (int i = 0; i < vectorSocket.count();) {
            QTcpSocket* l_socket = vectorSocket.at(i);

            if((l_socket->peerAddress().toString()==str)&&(l_socket->peerPort()==port))
            {
                vectorSocket.erase(vectorSocket.begin()+i);
                vectorSocketState.erase(vectorSocketState.begin()+i);
                vectorSocketSerialNumber.erase(vectorSocketSerialNumber.begin()+i);
                find=true;
            }
            else
            {
                i++;
            }
        }
        if(find==true) return 1;
        else return 0;
    }
    int findIdFromSocket(QTcpSocket *skt)
    {
        bool find=false;
        int i=0;
        for (i = 0; i < vectorSocket.count();i++) {
            QTcpSocket* l_socket = vectorSocket.at(i);

            if((l_socket->peerAddress().toString()==skt->peerAddress().toString())&&(l_socket->peerPort()==skt->peerPort()))
            {
                find=true;
                break;
            }
        }
        if(find==true) return i;
        else return -1;
    }
    int findIdFromSocket(QString str, int port)
    {
        bool find=false;
        int i=0;
        for (i = 0; i < vectorSocket.count();i++) {
            QTcpSocket* l_socket = vectorSocket.at(i);

            if((l_socket->peerAddress().toString()==str)&&(l_socket->peerPort()==port))
            {
                find=true;
                break;
            }
        }
        if(find==true) return i;
        else return -1;
    }

    int findSNFromSocket(QTcpSocket *skt)
    {
        int i=0;
        for (i = 0; i < vectorSocket.count();i++) {
            QTcpSocket* l_socket = vectorSocket.at(i);

            if((l_socket->peerAddress().toString()==skt->peerAddress().toString())&&(l_socket->peerPort()==skt->peerPort()))
            {
                int sn = vectorSocketSerialNumber.at(i);
                return sn;
            }
        }
        return -1;
    }
    bool findSNExist(int sn)
    {
        int i=0;
        for (i = 0; i < vectorSocketSerialNumber.size();i++) {
            int t = vectorSocketSerialNumber.at(i);
            if(sn==t)
            {
                return true;
            }
        }
        return false;
    }

    int getSNByIndex(int index)
    {
        int num=vectorSocketSerialNumber.size();
        if(index>=num) return -1;
        int sn = vectorSocketSerialNumber.at(index);
        return sn;
    }

    int getSNSize()
    {
        return vectorSocketSerialNumber.size();
    }
    int setStatusFromSocket(QTcpSocket *skt, clientStatus status)
    {
        bool find=false;
        int i=0;
        if(vectorSocketState.size()!=vectorSocket.size()) return -1;
        for (i = 0; i < vectorSocket.count();i++) {
            QTcpSocket* l_socket = vectorSocket.at(i);

            if((l_socket->peerAddress().toString()==skt->peerAddress().toString())&&(l_socket->peerPort()==skt->peerPort()))
            {
                *(vectorSocketState.begin() + i)=status;
                find=true;
                break;
            }
        }
        if(find==true) return i;
        else return -1;
    }

    int setSerialNumberFromSocket(QTcpSocket *skt, int sn)
    {
        bool find=false;
        int i=0;
        if(vectorSocketSerialNumber.size()!=vectorSocket.size()) return -1;
        for (i = 0; i < vectorSocket.count();i++) {
            QTcpSocket* l_socket = vectorSocket.at(i);

            if((l_socket->peerAddress().toString()==skt->peerAddress().toString())&&(l_socket->peerPort()==skt->peerPort()))
            {
                *(vectorSocketSerialNumber.begin() + i)=sn;
                find=true;
                break;
            }
        }
        if(find==true) return i;
        else return -1;
    }



    //QVector<QTcpSocket *> getClientVector() const
    //{
    //    return vectorSocket;
    //}

    QTcpSocket* getClientVecByIndex(int index)
    {
        int num=vectorSocket.size();
        if(index>=num) return NULL;
        QTcpSocket* skt = vectorSocket.at(index);
        return skt;
    }
    QTcpSocket* findSocketBySN(int sn)
    {
        QTcpSocket* skt=NULL;
        int i=0;
        for (i = 0; i < vectorSocketSerialNumber.count();i++) {
            int t_sn = vectorSocketSerialNumber.at(i);
            if(t_sn==sn)
            {
                skt = vectorSocket.at(i);
                break;
            }
        }
        return skt;
    }
    int getClientVecSize()
    {
        return vectorSocket.size();
    }

    //int getClientVectorSize() const
    //{
    //    return vectorSocket.size();
    //}

    //QVector<clientStatus> getClientStatusVector()
    //{
    //    return vectorSocketState;
    //}
    clientStatus getStatusByIndex(int index)
    {
        int num=vectorSocketState.size();
        if(index>=num) return (clientStatus)0;
        clientStatus sta = vectorSocketState.at(index);
        return sta;
    }

    int getStatusSize()
    {
        return vectorSocketState.size();
    }

};

//! [0]
class FortuneServer : public QTcpServer
{
    Q_OBJECT
#define ROBOT_SN_MIN 1
public:
    FortuneServer(QObject *parent = 0);
    ~FortuneServer();
    void sendDate();
    ClientSocketList *getClientList();
    void loadData(QCurveDataCus *data,QwtPointCus::PointStyle type);
    Robot* getRobotHandle();
    int getSerialNumberMax();
    int newSerialNumber();
    int findSerialNumber(int sn);
    int getClientSNBySocket(QTcpSocket *socket);
    int deletePathWithClientList();
    void sendSNOK(QTcpSocket* socket,int sn);
    void sendControlCmd(QTcpSocket* socket,int cmd);
     QVector<RobotPoint> getPose(int n);
signals:
    void newConnection(ClientSocketList* list, const QTcpSocket *socket,ClientSocketList::clientStatus status);
    void updataRobotPathServer(Robot* robot);
    void updataRobotPathServerState(Robot* robot);
    void newClientSN(ClientSocketList* list, const QTcpSocket *socket,int sn);
    void onNewRobotMsg(QString str);    
    void newPointServer(const QwtPointCus point);

protected:
    void onUpdateClientStatus(QTcpSocket* socket,int state);
    void onUpdateClientSN(QTcpSocket* socket,int sn);
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
    void onUpdateRobotPath(QTcpSocket* socket,int robotId, int pathId,int pointNum,int pointId,double x,double y,double phi);
    void onUpdateRobotPathByMainPath(QTcpSocket* socket,int robotId, int pathId,int pointNum,int pointId,int mainPathId);
    void onUpdateRobotState(QTcpSocket* socket,int robotId,double x,double y,double phi,double left,double right,int goMainPathId,int robotState,int robotType);
    void timerEvent( QTimerEvent *event );
    void robotMsg(QString str);
    void onNewPoint(const QwtPointCus point);
private:
    QStringList fortunes;
    ClientSocketList clientList;
    Robot robot;
    int serialNumber;
    int m_nTimerId;
    int m_nTimerCnt;
};
//! [0]

#endif
