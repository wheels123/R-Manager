#ifndef TWOWAYREGION_H
#define TWOWAYREGION_H

#include "fortuneserver.h"
#include "qmainplot.h"
#include "robotmath.h"

class RobotHighParam
{
public:
    int id;
    int nearPointId[3];
    double disToCur;
    double perDisToCur;
    bool onSamePath;
};

class TwoWayRegion
{
#define safe_dis_same_road 1.8
#define safe_dis_go_across 1.0
#define safe_dis_need_no_ctrl 3.0
public:
    TwoWayRegion();
private:
    EditShapeItem *shapeItem;
    QVector<QVector<RobotPathPoint>> vectorPath;
    QVector<QVector<RobotPath>> vectorRobot;
    QVector<RobotPath> robotIn;
    RobotMath m_math;
    QVector<RobotHighParam> vHighParam;
public:
    void setRegion(EditShapeItem *region, QCurveDataCus *path,Robot *robot);
    bool findNet(RobotPathPoint p,int net);
    int getFirstValidNet(RobotPathPoint p);
    bool getFirstValidPointIn(RobotPathPoint p,QVector<RobotPathPoint> pathIn,RobotPathPoint &findPoint);
    bool findIdIn(int id);
    int getNetNum(RobotPathPoint p);
    int findChildNetInPath(RobotPathPoint point ,QVector<RobotPathPoint> pathIn);
    bool findIdInPath(int id ,QVector<RobotPathPoint> pathIn);
    bool pathPointInRegionValid(QVector<RobotPathPoint> pathIn);
    int robotOnWitchPath(QVector<RobotPathPoint> robotPath);

    double disPoint2Point(RobotPathPoint pc,RobotPath rp);
    double disPoint2Point(RobotPath rp,RobotPath rp2);
    void setControl(Robot* robot);
    int getRegionId();
    void checkPathRobotId(Robot *robot);
    int getPathIndexPointOn(int id);
    bool pointOnSameRoad(int ida,int idb);
    int priorRobot(RobotHighParam pa,RobotHighParam pb);
    RobotHighParam getParamById(int id);
};

#endif // TWOWAYREGION_H
