#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include "robot.h"
#include "QTimer.h"
#include <QObject>
#include <QDebug>
#include <QThreadPool>
#include <qevent.h>
#include <qwt_point_cus.h>
#include <QTime>
class QTimer;
class QTimerEvent;
class MySerialPort : public QSerialPort
{
    Q_OBJECT                     ////////--------------------------------------------///////// very important
    #define ROBOT_SN_MIN 1
public:
    explicit MySerialPort(QObject *parent = Q_NULLPTR);
    ~MySerialPort();
    bool init(QString port);

signals:
    void updataRobotPathServer(Robot* robot);
    void updataRobotPathServerState(Robot* robot);
    void onNewRobotMsg(QString str);
    void newPointServer(QwtPointCus point);

protected:
    void onUpdateRobotPath(int robotId, int pathId,int pointNum,int pointId,double x,double y,double phi);
    void onUpdateRobotPathByMainPath(int robotId, int pathId,int pointNum,int pointId,int mainPathId);
    void onUpdateRobotState(int robotId,int pathId,double x,double y,double phi,double left,double right,int goMainPathId,int robotState,int robotType);
    void timerEvent( QTimerEvent *event );
    void robotMsg(QString str);
    void onNewPoint(const QwtPointCus point);
    void onUpdateSN(int sn);
    void onUpdateRobotPath(int robotId, int pathId,QVector<int> pointList);
public:
    inline void processData(QString &data);
    inline void transData(QString &data);
    void sendDate();
    void loadData(QCurveDataCus *data,QwtPointCus::PointStyle type);
    Robot* getRobotHandle();
    int getSerialNumberMax();
    int newSerialNumber();
    int findSerialNumber(int sn);
    void sendControlCmd(int sn,int cmd,int pathId);
    QVector<RobotPoint> getPose(int n);
    QVector<QVector<RobotPathPoint>> getPose();

    void sendSN(int sn);
    void sendSNOK(int sn);
//////////////////////
/// build map function
//////////////////////
    void sendUpdateLabel();
    void sendDelelteLabel(int labelNum);
private:
    //QSerialPort m_serialPort;
    Robot robot;
    int m_nTimerId;
    int m_disconnect_times;
    bool m_heart;
    bool m_connected;
    int m_status;
    bool m_sendEnable;
private slots:
    void readyReadSlot();
};

#endif // MYSERIALPORT_H
