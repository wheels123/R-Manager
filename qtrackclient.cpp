#include "qtrackclient.h"


QTrackClient::QTrackClient(QObject *parent)
    :QTcpSocket(parent)
{
    initTcpSocket();
}

inline void QTrackClient::initTcpSocket()
{
    //
    //
    //
    connect(this, &QTcpSocket::connected,
            this, &onTcpConnected);
    connect(this, &QTcpSocket::disconnected,
            this, &onTcpDisconnected);
    connect(this, &QTcpSocket::readyRead,
            this, &onTcpReadyRead);
    //
    //
    //
    using SocketErrorFunc = void(QTcpSocket::*)(QAbstractSocket::SocketError);
    connect(this,
            static_cast<SocketErrorFunc>(&QAbstractSocket::error),
            this,
            &onTcpSocketError);
}

void QTrackClient::onTcpConnected()
{
    qDebug() << Q_FUNC_INFO << __LINE__;
}

void QTrackClient::onTcpDisconnected()
{
    qDebug() << Q_FUNC_INFO << __LINE__;
}

void QTrackClient::processData(QString &data)
{
    const QString itemSep = ",";

    QStringList list = data.split(itemSep);

    const int itemNo = 5;

    if (list.size() != itemNo) {
        qDebug() << Q_FUNC_INFO << __LINE__ << list.size();

        return;
    }

    const int typeIndex  = 0;
    const int xIndex     = 1;
    const int yIndex     = 2;
    const int angleIndex = 3;
    const int tagIndex   = 4;

    long xValue      = list.at(xIndex).toLong();
    long yValue      = list.at(yIndex).toLong();
    long angleValue  = list.at(angleIndex).toLong();
    long tagNo       = list.at(tagIndex).toLong();

    qDebug() << Q_FUNC_INFO << __LINE__
             << list.at(typeIndex)
             << xValue << yValue << angleValue << tagNo;
}

void QTrackClient::onTcpReadyRead()
{
    QTextStream in(this);

    forever {
        //
        // more data
        //
        if (bytesAvailable() == 0) {
            break;
        }

        //
        // read line by line
        //
        QString data = in.readLine();

        processData(data);
    }
}

void QTrackClient::onTcpSocketError()
{
    qDebug() << Q_FUNC_INFO << __LINE__;
}
