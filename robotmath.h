#ifndef ROBOTMATH_H
#define ROBOTMATH_H
#include "robot.h"
#include <QDebug>

class RobotMath
{
public:
    RobotMath();
    double dis(RobotPoint a, RobotPoint b);
    double perDis(RobotPoint a,RobotPoint b,RobotPoint c);
    void computeFromEncoder(RobotPoint	&out_incr_odom,
        double	left,
        double	right);
    int safeRobot(RobotPoint a,RobotPoint b);
    double estimateMinDis(RobotPoint a,double la,double ra,RobotPoint b,double lb,double rb);
    int safeRobot(RobotPoint a,RobotPoint b,double &da,double &db);
    double estimateMinDisA2B(RobotPoint a,double la,double ra,RobotPoint b);
    double calcuLineRad(RobotPoint destPose2D, RobotPoint curPose2D);
    RobotPoint avoidMoveVector(RobotPoint rp, RobotPoint op,double r);
    RobotPoint vectorAdd(RobotPoint pa, RobotPoint pb);
    double avoidMoveVectorDis(RobotPoint rp, RobotPoint op,RobotPoint sp,double r);
    RobotPoint TPose2D_Add(const RobotPoint a,const RobotPoint b);
    RobotPoint avoidNewPoint(RobotPoint p, double phi,double dis);
    double calcuAbsRad(double rad1,double rad2);
};

#endif // ROBOTMATH_H
