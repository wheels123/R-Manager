#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QObject>
#include <QWidget>

class QUdpSocket;
class QAction;
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include "qmainplot.h"
#include "qtrackclient.h"
#include "canvaspicker.h"
#include "qcurvedatacus.h"
#include <qwt_point_cus.h>
#include "dialogoutputoption.h"
#include "qtrackserver.h"
#include "fortuneserver.h"

class ClientUdpList
{
public:
    #define TCP_SOCKET_MAX 50
    enum clientStatus{Connected=0,NoHeart,Disconnected};
    bool* receiveHeart;
    int* noHeartTimes;
    int curNum;
private:
    QHostAddress socket;
    QVector<QHostAddress> vectorAddress;
    QVector<int> vectorPort;
    QVector<clientStatus> vectorSocketState;
    QVector<int> vectorRobotId;
public:
    ClientUdpList()
    {
        receiveHeart = new bool[TCP_SOCKET_MAX];
        noHeartTimes = new int[TCP_SOCKET_MAX];
        curNum=0;
    }

    void reset()
    {
        for (int i = 0; i < TCP_SOCKET_MAX; i++)
        {
            receiveHeart[i] = true;
            noHeartTimes[i] = 0;
            curNum = 0;
        }
    }

    int inseartClient2(QHostAddress skt)
    {
        if(curNum>=TCP_SOCKET_MAX-1) return -1;
        socket = skt;
        curNum++;
        return 1;
    }

    int inseartClient(QHostAddress skt)
    {
        if(curNum>=TCP_SOCKET_MAX-1) return -1;
        vectorAddress.append(skt);
        vectorSocketState.append(Connected);
        vectorRobotId.append(-1);
        curNum++;
        return 1;
    }
    int inseartClient(QHostAddress skt,int sn)
    {
        if(curNum>=TCP_SOCKET_MAX-1) return -1;

        bool find=false;
        int i=0;
        for (i = 0; i < vectorAddress.size();i++) {
            QHostAddress l_socket = vectorAddress.at(i);

            if(l_socket==skt)
            {
                find=true;
                break;
            }
        }
        if(find==true) return -1;

        vectorAddress.append(skt);
        vectorSocketState.append(Connected);
        vectorRobotId.append(sn);
        curNum++;
        return 1;
    }

    int deleteClient(int index)
    {
        int n=vectorAddress.size();
        if(n<=0) return 0;
        if(index>n-1||index<0) return 0;
        if(vectorRobotId.size()!=vectorAddress.size()) return 0;
        vectorAddress.erase(vectorAddress.begin()+index);
        vectorSocketState.erase(vectorSocketState.begin()+index);
        vectorRobotId.erase(vectorRobotId.begin()+index);
        return 1;
    }

    int deleteClient(QHostAddress skt)
    {
        bool find=false;
        if(vectorRobotId.size()!=vectorAddress.size()) return 0;
        for (int i = 0; i < vectorAddress.size();) {
            QHostAddress l_socket = vectorAddress.at(i);

            if(l_socket==skt)
            {
                vectorAddress.erase(vectorAddress.begin()+i);
                vectorSocketState.erase(vectorSocketState.begin()+i);
                vectorRobotId.erase(vectorRobotId.begin()+i);
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
        if(vectorRobotId.size()!=vectorAddress.size()) return 0;
        for (int i = 0; i < vectorAddress.size();) {
            QHostAddress l_socket = vectorAddress.at(i);

            if(l_socket.toString()==str)
            {
                vectorAddress.erase(vectorAddress.begin()+i);
                vectorSocketState.erase(vectorSocketState.begin()+i);
                vectorRobotId.erase(vectorRobotId.begin()+i);
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
    int findIdFromSocket(QHostAddress skt)
    {
        bool find=false;
        int i=0;
        for (i = 0; i < vectorAddress.size();i++) {
            QHostAddress l_socket = vectorAddress.at(i);

            if(l_socket==skt)
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
        for (i = 0; i < vectorAddress.size();i++) {
            QHostAddress l_socket = vectorAddress.at(i);

            if(l_socket.toString()==str)
            {
                find=true;
                break;
            }
        }
        if(find==true) return i;
        else return -1;
    }

    int findSNFromSocket(QHostAddress skt)
    {
        int i=0;
        if(vectorRobotId.size()!=vectorAddress.size()) return -1;
        for (i = 0; i < vectorAddress.size();i++) {
            QHostAddress l_socket = vectorAddress.at(i);

            if(l_socket==skt)
            {
                int sn = vectorRobotId.at(i);
                return sn;
            }
        }
        return -1;
    }
    bool findSNExist(int sn)
    {
        int i=0;
        for (i = 0; i < vectorRobotId.size();i++) {
            int t = vectorRobotId.at(i);
            if(sn==t)
            {
                return true;
            }
        }
        return false;
    }

    int getSNByIndex(int index)
    {
        int num=vectorRobotId.size();
        if(index>=num||index<0) return -1;
        int sn = vectorRobotId.at(index);
        return sn;
    }

    int getSNSize()
    {
        return vectorRobotId.size();
    }
    int setStatusFromSocket(QHostAddress skt, clientStatus status)
    {
        bool find=false;
        int i=0;
        if(vectorSocketState.size()!=vectorAddress.size()) return -1;
        for (i = 0; i < vectorAddress.size();i++) {
            QHostAddress l_socket = vectorAddress.at(i);

            if(l_socket==skt)
            {
                *(vectorSocketState.begin() + i)=status;
                find=true;
                break;
            }
        }
        if(find==true) return i;
        else return -1;
    }

    int setSerialNumberFromSocket(QHostAddress skt, int sn)
    {
        bool find=false;
        int i=0;
        if(vectorRobotId.size()!=vectorAddress.size()) return -1;
        for (i = 0; i < vectorAddress.size();i++) {
            QHostAddress l_socket = vectorAddress.at(i);

            if(l_socket==skt)
            {
                *(vectorRobotId.begin() + i)=sn;
                find=true;
                break;
            }
        }
        if(find==true) return i;
        else return -1;
    }



    //QVector<QTcpSocket *> getClientVector() const
    //{
    //    return vectorAddress;
    //}

    bool  getClientVecByIndex(int index,QHostAddress &address)
    {
        int num=vectorAddress.size();
        if(index>=num||index<0) return false;
        QHostAddress skt = vectorAddress.at(index);
        return true;
    }
    bool findSocketBySN(int sn,QHostAddress &address)
    {
        int i=0;
        if(vectorRobotId.size()!=vectorAddress.size()) return false;
        for (i = 0; i < vectorRobotId.size();i++) {
            int t_sn = vectorRobotId.at(i);
            if(t_sn==sn)
            {
                address = vectorAddress.at(i);
                return true;
            }
        }
        return false;
    }
    int getClientVecSize()
    {
        return vectorAddress.size();
    }

    //int getClientVectorSize() const
    //{
    //    return vectorAddress.size();
    //}

    //QVector<clientStatus> getClientStatusVector()
    //{
    //    return vectorSocketState;
    //}
    clientStatus getStatusByIndex(int index)
    {
        int num=vectorSocketState.size();
        if(index>=num||index<0) return (clientStatus)0;
        clientStatus sta = vectorSocketState.at(index);
        return sta;
    }

    int getStatusSize()
    {
        return vectorSocketState.size();
    }

};

class UdpReceiver : public QWidget
{
    Q_OBJECT

public:
    UdpReceiver(QWidget *parent = 0);
    bool init(QHostAddress &address, quint16 port = 0);
private slots:
    void processPendingDatagrams();
    void timerEventSendCtrl();
signals:
    void updataRobotPathServer(Robot* robot);
    void updataRobotPathServerState(Robot* robot);
    void onNewRobotMsg(QString str);
    void newPointServer(const QwtPointCus point);

private:
    QUdpSocket *udpSocket;
    Robot robot;
    ClientUdpList clientList;
    QTime m_time;
    QTimer *m_timeSendCtrl;
    int irpTime;
    int lastReadTime;
    QString RxData_100;
    QString RxData_102;
    QString RxData_104;
public:
    void loadData(QCurveDataCus *data,QwtPointCus::PointStyle type);
    Robot* getRobotHandle();
    int findSerialNumber(int sn);
    inline void processData(QString &data,QHostAddress &address);
    inline void transData(QString &data,QHostAddress &address);

    void onUpdateRobotPath(int robotId, int pathId,int pointNum,int pointId,double x,double y,double phi);
    void onUpdateRobotPathByMainPath(int robotId, int pathId,int pointNum,int pointId,int mainPathId);
    void onUpdateRobotState(QHostAddress &address,int robotId,int pathId,double x,double y,double phi,double left,double right,int goMainPathId,int robotState,int robotType);
    void onUpdateRobotPath(QHostAddress &address,int robotId,int pathId,QVector<int> pointList);

    unsigned short CRC16(unsigned char *puchMsg,
    unsigned short usDataLen
    );
    void sendControlCmd(int sn,int cmd,int pathId);
     QVector<QVector<RobotPathPoint>> getPose();
};

#endif // UDPRECEIVER_H
