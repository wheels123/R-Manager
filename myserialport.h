#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H

#include <QSerialPort>
#include <QSerialPortInfo>
class MySerialPort : public QSerialPort
{
public:
    MySerialPort();
    bool init(QString port);
    inline void processData(QString &data);
    inline void transData(QString &data);
    void sendSN(int sn);
    void sendSNOK(int sn);
private:
    QSerialPort m_serialPort;

private slots:
    void readyReadSlot();

signals:
    void updateClientStatus(QTcpSocket* socket,int state);
    void updateRobotPath(QTcpSocket* socket,int robotId, int pathId,int pointNum,int pointId,double x,double y,double phi);
    void updateRobotPathByMainPath(QTcpSocket* socket,int robotId, int pathId,int pointNum,int pointId,int mainPathId);
    void updateClientSN(QTcpSocket* socket,int sn);
    void updateRobotState(QTcpSocket* socket,int robotId,double x,double y,double phi,double left,double right,int goMainPathId,int robotState,int robotMode);

    void newPoint(const QwtPointCus point);

protected:
    Robot robot;
    void timerEvent( QTimerEvent *event );
    int m_nTimerId;
    int m_disconnect_times;
    bool m_heart;
    bool m_connected;
    int m_status;
};

#endif // MYSERIALPORT_H
