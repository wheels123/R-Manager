#ifndef ROBOT_H
#define ROBOT_H
#include <QVector>
#include "qcurvedatacus.h"
#include <qmutex.h>
class QwtPointCus;
class QCurveDataCus;
class RobotPoint
{
public:
    double x;
    double y;
    double phi;
public:
    RobotPoint()
    {

    }
    RobotPoint(double px,double py)
    {
       x=px;
       y=py;
       phi=0;
    }
    RobotPoint(double px,double py, double ph)
    {
       x=px;
       y=py;
       phi=ph;
    }
public:
    RobotPoint& operator =(const RobotPoint& a)
    {
        this->x=a.x;
        this->y=a.y;
        this->phi=a.phi;
        return *this;
    }

    RobotPoint operator +(const RobotPoint& a)
    {
        RobotPoint ret;
        ret.x =	this->x + a.x * cos(this->phi) - a.y * sin(this->phi);
        ret.y = this->y + a.x * sin(this->phi) + a.y * cos(this->phi);
        ret.phi = a.phi + this->phi;
        return ret;
    }
    /*
    TPose2D TPose2D_Add(const TPose2D a,const TPose2D b)
    {
        TPose2D ret;
        //ret.x =	a.x + b.x * cos(a.phi) - b.y * sin(a.phi);
        //ret.y = a.y + b.x * sin(a.phi) + b.y * cos(a.phi);
        ret.x =	a.x + b.x * m_cos(a.phi) - b.y * m_sin(a.phi);
        ret.y = a.y + b.x * m_sin(a.phi) + b.y * m_cos(a.phi);
        ret.phi = a.phi + b.phi;
        //ret.phi = radAdd(a.phi,b.phi);
        return ret;
    }*/
};

class RobotPathPoint
{
public:
    enum {SIZE = 10};
    RobotPoint point;
    int id;
    int mainPathId;
    int d_net[SIZE];
    int d_robotId;
public:
    RobotPathPoint()
    {
        this->point.x=0;
        this->point.y=0;
        this->point.phi=0;
        this->id=0;
        this->mainPathId=0;
        this->d_robotId=0;
        for(int i=0;i<SIZE;i++)
        {
            this->d_net[i]=0;
        }
    }
    RobotPathPoint& operator =(const RobotPathPoint& a)
    {
        this->point=a.point;
        this->id=a.id;
        this->mainPathId=a.mainPathId;
        this->d_robotId=a.d_robotId;
        for(int i=0;i<SIZE;i++)
        {
            this->d_net[i]=a.d_net[i];
        }
        return *this;
    }
    RobotPathPoint& operator =(const QwtPointCus& a)
    {
        this->point.x=a.x();
        this->point.y=a.y();
        this->point.phi=a.phi();
        this->id=a.id();
        this->mainPathId=a.id();
        this->d_robotId=0;
        for(int i=0;i<SIZE;i++)
        {
            this->d_net[i]=a.net(i);
        }
        return *this;
    }

    RobotPathPoint( const QwtPointCus a)
    {
        this->point.x=a.x();
        this->point.y=a.y();
        this->point.phi=a.phi();
        this->id=a.id();
        this->mainPathId=a.id();
        this->d_robotId=0;
        for(int i=0;i<SIZE;i++)
        {
            this->d_net[i]=a.net(i);
        }
    }
    //! \return The z-coordinate of the point.
    inline int netSize() const
    {
        return SIZE;
    }

    //! \return The z-coordinate of the point.
    inline int net(int n) const
    {
        if(n<SIZE)
        return d_net[n];
    }

    //! \return The z-coordinate of the point.
    inline int &rnet(int n)
    {
        if(n>SIZE-1)
        {
            n=SIZE-1;
        }
        return d_net[n];
    }

    //! Sets the z-coordinate of the point to the value specified by z.
    inline void setNet( int n,int net )
    {
        if(n<SIZE)
        {
            d_net[n] = net;
        }
    }
    //! \return The id the point.
    inline int mpid() const
    {
        return mainPathId;
    }
    //! \return The x-coordinate of the point.
    inline double x() const
    {
        return point.x;
    }

    //! \return The y-coordinate of the point.
    inline double y() const
    {
        return point.y;
    }

    //! \return The z-coordinate of the point.
    inline int getRobotId() const
    {
        return d_robotId;
    }

    //! \return The z-coordinate of the point.
    inline void setRobotId(int a)
    {
        d_robotId=a;
    }
};

class RobotPath
{
public:
    enum RobotState{AtStartPoint=0,AtDestPoint,GoToDestPoint,GoToStartPoint};
    enum RobotType{Guide=0,Waiter};
//robot id
    int robotId;
//path
    QVector<RobotPathPoint> point;
    int id;
    int num;
//state
    RobotPoint curPose;
    int pathIdToGo;
    RobotState robotState;
    RobotType robotType;
    double leftSpeed;
    double rightSpeed;
    int robotControl;//
////////////////////
    RobotPath& operator =(RobotPath a)
    {
        for(int i=0;i<a.point.size();i++)
        {
            this->point.append(a.point.at(i));
        }
        this->id=a.id;
        this->num=a.num;
        this->robotId=a.robotId;
        this->curPose=a.curPose;
        this->pathIdToGo=a.pathIdToGo;
        this->robotState=a.robotState;
        this->robotType=a.robotType;
        this->leftSpeed=a.leftSpeed;
        this->rightSpeed=a.rightSpeed;
        this->robotControl=a.robotControl;
    ////////////////////
        return *this;
    }
    QVector<RobotPathPoint> getNearPointById(int id);
    QVector<RobotPathPoint> getNearPointById();
    QVector<int> getNearPointIdById();
    QVector<int> getNearPointIdById(int id);
};

class RobotMainPath
{
public:
    QVector<RobotPathPoint> point;

    RobotMainPath& operator =(const RobotMainPath& a)
    {
        for(int i=0;i<a.point.size();i++)
        {
            this->point.append(a.point.at(i));
        }
        return *this;
    }
};

class RobotState
{
public:

};

class Robot: public QObject
{
public:
#define MAX_MAIN_PATH_ID (9999)
#define MIN_DIS_TWO_PATH (0.6)
#define MIN_DIS_ROBOT_NEED_CHECK (3.0)
#define NEXT_PATH_NEED_CHECK (3)
#define MIN_DIS_ROBOT_COLLIDE (0.8)
#define MAX_DIS (1000.0)

#define PI_M (3.1415926535897932384626)

#define PI_M_2 (M_PI/2)
    Robot();
    int insertPathPoint(int robotId,int pathId,RobotPoint point,int maxPointNum,int pointId);
    int insertPathPoint(int robotId,int pathId,int mainPathId,int maxPointNum,int pointId);
    //QVector<RobotPath> *getPathVector();
    int getPathNum();
    int getPathPointNum(int index);
    int getPathPointNumMax(int index);
    int getPathIdByIndex(int index);
    int getPathRobotIdByIndex(int index);
    int findPathIndexById(int robotId,int id,int &index);
    const QVector<RobotPath> getPath();
    int insertPathPointByIndex(int index,RobotPoint point,int maxPointNum,int pointId);
    int insertPathPointFrommainPathIdByIndex(int index,RobotPoint point,int mainPathId,int maxPointNum,int pointId);
    QStringList pathToString(int index);
    void clearPath(int index);
    void initMainPath();
    //QVector<RobotPathPoint>* getMainPathHandle();
    void clearMainPath();
    void insertMainPath(RobotPathPoint rp);
    void loadData(QCurveDataCus *data,QwtPointCus::PointStyle type);
    QStringList mainPathToString();
    int getMainPathNum();
    int deletePath(int robotId);
    bool erasePathByIndex(int index);
    RobotPath getPathByIndex(int index);
    bool findRobotId(int robotId, int &index);
    int insertRobotState(int robotId,RobotPoint point,double left,double right,int goMainPathId,int robotState,int robotType);
    bool insertRobotStateByIndex(int index,RobotPoint point,double left,double right,int goMainPathId,int robotState,int robotType);
    QString robotStateToString(int index);

    double estimateMinDis(RobotPoint a,RobotPoint b);
    double calcuMinDisWithTwoPath(int indexA,int indexB);
    void pathControl();
    QVector<int>& getRobotControl();
    bool calcuNearIndex(int indexA,int indexB,int &nearIndex);
    bool dirIdentical(RobotPoint pa,RobotPoint pb);
    bool controlRun(int indexA,int indexB,int& ctrlA,int &ctrlB);
    QString getMsg();
    int frontRobot(RobotPoint pa,RobotPoint pb);
    QVector<RobotPoint> getPose(int n);
     void estimateRobotPose(int num);
     void computeFromEncoder(RobotPoint	&out_incr_odom,
         double	left,
         double	right);
     bool getMainPathPointById(int id ,RobotPoint &p);
     void resetControlValue();
     void updateControlNum();
     void setRobotControl(int i,int val);
     bool getMainPathMainIdByPoint(RobotPoint p, int &id);
     void setPathRobotIdById(int id,int robotId);
     int getPathRobotIdById(int id);
     int getMainPathRobotIdByIndex(int index);
     bool getPathById(int robotId,RobotPath&rp);
     void resetAllPathRobotId(int robotId);
     int getRobotControl(int i);
     int findPathIndexById(int robotId);
     bool checkMovableById(int robotId);
     bool checkMovableByIndex(int index);
     QVector<QVector<RobotPathPoint>> getPose();
     void estimateRobotPose();
public:

private:
    QVector<RobotPath> path;
    QVector<RobotPathPoint> mainPath;
    QMutex mutex;
    QVector<int>robotControl;
    QString robotMsg;

    QVector<RobotPoint> vectorPoseA;
    QVector<RobotPoint> vectorPoseB;

    QVector<QVector<RobotPathPoint>> vectorPose;
};

#endif // ROBOT_H
