#ifndef QTRACKSERVER_H
#define QTRACKSERVER_H

#include <QTcpSocket>
#include <QTcpServer>
class myClientSocket
{
    #define TCP_SOCKET_MAX 50
    QTcpSocket** clientSocket;
    bool* receiveHeart;
    int* noHeartTimes;
    int curNum;
public:
    myClientSocket()
    {
        *clientSocket=new QTcpSocket[TCP_SOCKET_MAX];//为客户端提供连接个数
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

    int inseartClient(QTcpSocket *client)
    {
        bool exit=false;
        if(curNum>=TCP_SOCKET_MAX-1) return -1;
        for(int i=0;i<curNum;i++)
        {
            QString infoIn=client->peerAddress().toString();
            QString info=clientSocket[i]->peerAddress().toString();
            if(infoIn==info)
            {
                exit=true;
                break;
            }
        }
        if(exit)
        {
            return 0;
        }
        clientSocket[curNum]=client;
        curNum++;
        return 1;
    }
    int deleteClient(int index)
    {
        if(curNum==0) return 0;
        if(index>=curNum) return -1;
        for(int i=index;i<curNum-1;i++)
        {
            clientSocket[i]=clientSocket[i+1];
        }
        curNum-=1;
        return 1;
    }

    int findIdFromSocket(QTcpSocket* client)
    {
        for (int i = 0; i < curNum; i++)
        {
            QString infoIn = client->peerAddress().toString();
            QString info = clientSocket[i]->peerAddress().toString();
            if (infoIn == info)
            {
                return i;
            }
        }
        return -1;
    }
};

class QTrackServer :public QTcpServer
{


public:
    QTrackServer();
    QTrackServer(QObject *parent);
    void startTcpserver();
    void newConnect();
    void readMessage();
    void sendMessage(); //发送信息
private:
    myClientSocket m_tcpsocket;
};

#endif // QTRACKSERVER_H
