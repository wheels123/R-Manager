#ifndef ONEWAYREGION_H
#define ONEWAYREGION_H


#include "fortuneserver.h"
#include "qmainplot.h"
class OneWayRegion
{
#define safe_dis_to_be_front 0.8
public:
    OneWayRegion();
private:
    EditShapeItem *shapeItem;
    QVector<QVector<QwtPointCus>> vectorPath;
    QVector<QVector<RobotPath>> vectorRobot;
    QVector<RobotPath> activeRobot;

public:
    void setRegion(EditShapeItem *region, QCurveDataCus *path,Robot *robot);

    float calcuLineRad(float dx,float dy, float cx, float cy);
    int frontRobot(RobotPoint a,RobotPoint da, RobotPoint b,RobotPoint db,int mode);
    int frontRobot(RobotPoint a,RobotPoint b);

    void setControl(Robot* robot);
    bool findRobotById(int id);
    RobotPath findSafeRobot(QVector<RobotPath> vrp);
    int getRegionId();
    bool isRobotSafe(QVector<RobotPath> vrp,RobotPath rp);
    int safeRobot(RobotPoint a,RobotPoint b);
};

#endif // ONEWAYREGION_H
