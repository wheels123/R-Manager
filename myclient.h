#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>
#include "mytask.h"
#include "QTimer.h"
#include <qevent.h>
#include <qwt_point_cus.h>
class QTimer;
class QTimerEvent;
class MyClient : public QObject
{
    Q_OBJECT
public:
    explicit MyClient(QObject *parent = 0);
    ~MyClient();
    void SetSocket(int Descriptor);
    inline void processData(QString &data);
    inline void transData(QString &data);
    void sendSN(int sn);
    void sendSNOK(int sn);
signals:
    void updateClientStatus(QTcpSocket* socket,int state);
    void updateRobotPath(QTcpSocket* socket,int robotId, int pathId,int pointNum,int pointId,double x,double y,double phi);
    void updateRobotPathByMainPath(QTcpSocket* socket,int robotId, int pathId,int pointNum,int pointId,int mainPathId);
    void updateClientSN(QTcpSocket* socket,int sn);
    void updateRobotState(QTcpSocket* socket,int robotId,double x,double y,double phi,double left,double right,int goMainPathId,int robotState,int robotMode);

    void newPoint(const QwtPointCus point);

public slots:
    void connected();
    void disconnected();
    void readyRead();
    void TaskResult(int Number);
    void sendBack(QByteArray buf);
    QTcpSocket* getSocket();

private:
    QTcpSocket *socket;

protected:

    void timerEvent( QTimerEvent *event );
    int m_nTimerId;
    int m_disconnect_times;
    bool m_heart;
    bool m_connected;
    int m_status;
};

#endif // MYCLIENT_H
