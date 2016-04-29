#ifndef QTRACKCLIENT_H
#define QTRACKCLIENT_H

#include <QTcpSocket>

class QTrackClient : public QTcpSocket
{
    Q_OBJECT

public:
    explicit QTrackClient(QObject *parent);

signals:

public slots:

private:
    inline void initTcpSocket();

private:
    inline void processData(QString &data);

private:
    void onTcpConnected(void);
    void onTcpDisconnected(void);
    void onTcpReadyRead(void);
    void onTcpSocketError(void);
};

#endif // QTRACKCLIENT_H
