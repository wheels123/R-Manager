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
};

#endif // ROBOTMATH_H
