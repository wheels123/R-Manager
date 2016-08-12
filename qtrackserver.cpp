#include "qtrackserver.h"

QTrackServer::QTrackServer(QObject *parent)
{
    //this->connect(ui->pushButton_start,SIGNAL(clicked()),this,SLOT(startTcpserver()));
    //this->connect(ui->pushButton_send,SIGNAL(clicked()),this,SLOT(sendMessage()));
}

void QTrackServer::startTcpserver()
{
    listen(QHostAddress::Any,9999); //监听任何连上9999端口的ip

    connect(this,SIGNAL(newConnection()),this,SLOT(newConnect())); //新连接信号触发，调用newConnect()槽函数，这个跟信号函数一样，其实你可以随便取。
}

void QTrackServer::newConnect()
{
    QTcpSocket *client = this->nextPendingConnection(); //得到每个连进来的socket
    m_tcpsocket.inseartClient(client);
    connect(client,SIGNAL(readyRead()),this,SLOT(readMessage())); //有可读的信息，触发读函数槽

}

void QTrackServer::readMessage() //读取信息
{

}

void QTrackServer::sendMessage() //发送信息
{

}
