#include "robot.h"
#include <QDebug>
#include <qwt_plot_canvas.h>
Robot::Robot()
{

}
/*
QVector<RobotPath>* Robot::getPathVector()
{
    return &path;
}
*/
int Robot::insertPathPoint(QHostAddress address,int robotId,int pathId,RobotPoint point,int maxPointNum, int pointId)
{
    int index=-1;
    RobotPathPoint tempPathPoint;
    RobotPath tempPath;
    int ret=1;
    int ret_find=1;
    tempPathPoint.id=pointId;
    tempPathPoint.point=point;


    int mid=MAX_MAIN_PATH_ID;
    bool ret2 = getMainPathMainIdByPoint(point,mid);
    if(ret2==true)
    {
       tempPathPoint.mainPathId=mid;
    }
    else
    {
        tempPathPoint.mainPathId=MAX_MAIN_PATH_ID;
    }

    //tempPathPoint.mainPathId=MAX_MAIN_PATH_ID;
    tempPath.ip=address;
    tempPath.id=pathId;
    tempPath.point.append(tempPathPoint);
    tempPath.num=maxPointNum;
    tempPath.robotId=robotId;
    tempPath.pathIdToGo=0;
    tempPath.robotState=(RobotPath::RobotState)0;
    tempPath.robotType=(RobotPath::RobotType)0;
    tempPath.curPose.x=0;
    tempPath.curPose.y=0;
    tempPath.curPose.phi=0;
    tempPath.leftSpeed=0;
    tempPath.rightSpeed=0;
    tempPath.robotControl=1;
    ret_find=findPathIndexById(robotId,pathId,index);

    if(ret_find==-1)
    {
        if(pointId==0)
        {
            path.append(tempPath);
        }
        else
        {
            ret = -1;
        }
    }
    else if(ret_find==0)
    {
        if(pointId==0)
        {
            clearPath(index);
            RobotPath &m = path[index];
            m.id=pathId;
            m.num=maxPointNum;
            m.robotId=robotId;
            int retInsert = insertPathPointByIndex(index,point,maxPointNum,pointId);
            if(retInsert!=1) ret=-2;
        }
        else
        {
            ret = -3;
        }
    }
    else
    {
        int retInsert = insertPathPointByIndex(index,point,maxPointNum,pointId);
        if(retInsert!=1) ret=-4;
    }

    return ret;
}

bool Robot::getMainPathPointById(int id ,RobotPoint &p)
{
    RobotPoint point;
    for(int i=0;i<mainPath.size();i++)
    {
        if(id==mainPath.at(i).mainPathId)
        {
            p=mainPath.at(i).point;
            return true;
        }
    }
    return false;

}

bool Robot::getDestPointById(int id ,RobotPoint &p)
{
    RobotPoint point;
    for(int i=0;i<dest.size();i++)
    {
        if(id==dest.at(i).id)
        {
            p=dest.at(i).point;
            return true;
        }
    }
    return false;

}

bool Robot::getMainPathMainIdByPoint(RobotPoint p, int &id)
{
    double min_dis=0.2;
    int min_id=-1;
    for(int i=0;i<mainPath.size();i++)
    {
        double x=mainPath.at(i).point.x-p.x;
        double y=mainPath.at(i).point.y-p.y;
        double dis=sqrt(x*x+y*y);
        if(dis<min_dis)
        {
            min_dis=dis;
            min_id=mainPath.at(i).mainPathId;
        }
    }

    if(min_id!=-1)
    {
        id=min_id;
        return true;
    }
    return false;

}

int Robot::insertPathPoint(QHostAddress address,int robotId,int pathId,int mainPathId,int maxPointNum, int pointId)
{
    int index=-1;
    RobotPathPoint tempPathPoint;
    RobotPath tempPath;

    RobotPoint point;
    int ret=1;
    int ret_find=1;

    int mainPathNum=mainPath.size();
    if(pointId>=maxPointNum) return -5;

    bool find = getMainPathPointById(mainPathId,point);

    if(find==false) return -6;

    tempPathPoint.id=pointId;
    tempPathPoint.point=point;
    tempPathPoint.mainPathId=mainPathId;
    tempPath.ip=address;
    tempPath.id=pathId;
    tempPath.point.append(tempPathPoint);
    tempPath.num=maxPointNum;
    tempPath.robotId=robotId;
    tempPath.pathIdToGo=0;
    tempPath.robotState=(RobotPath::RobotState)0;
    tempPath.robotType=(RobotPath::RobotType)0;
    tempPath.curPose.x=0;
    tempPath.curPose.y=0;
    tempPath.curPose.phi=0;
    tempPath.leftSpeed=0;
    tempPath.rightSpeed=0;
    tempPath.robotControl=1;
    ret_find=findPathIndexById(robotId,pathId,index);

    if(ret_find==-1)
    {
        if(pointId==0)
        {
            path.append(tempPath);
        }
        else
        {
            ret = -1;
        }
    }
    else if(ret_find==0)
    {
        if(pointId==0)
        {
            clearPath(index);
            RobotPath &m = path[index];
            m.id=pathId;
            m.num=maxPointNum;
            m.robotId=robotId;
            m.ip=address;
            int retInsert = insertPathPointFrommainPathIdByIndex(index,point,mainPathId,maxPointNum,pointId);
            if(retInsert!=1) ret=-2;
        }
        else
        {
            ret = -3;
        }
    }
    else
    {
        int retInsert = insertPathPointFrommainPathIdByIndex(index,point,mainPathId,maxPointNum,pointId);
        if(retInsert!=1) ret=-4;
    }

    return ret;
}


int Robot::insertPathPointList(QHostAddress address,int robotId, int pathId,QVector<int> pointIdList,int time)
{
    int maxPointNum=pointIdList.size();
    int index=-1;
    RobotPathPoint tempPathPoint;
    RobotPath tempPath;
    RobotPoint point;
    int ret=1;
    int ret_find=1;

    for(int  i=0;i<maxPointNum;i++)
    {
        point.x=0;
        point.y=0;
        point.phi=0;
        int mainPathId=pointIdList.at(i);
        bool find = getMainPathPointById(mainPathId,point);
        tempPathPoint.mainPathId=mainPathId;
        if(find==false)
        {
            tempPathPoint.mainPathId=-mainPathId;
        }
        else
        {
            tempPathPoint.mainPathId=mainPathId;
        }
        tempPathPoint.id=i;
        tempPathPoint.point=point;

        tempPath.point.append(tempPathPoint);
    }

    tempPath.ip=address;
    tempPath.id=pathId;
    tempPath.num=maxPointNum;
    tempPath.robotId=robotId;
    tempPath.pathIdToGo=0;
    tempPath.robotState=(RobotPath::RobotState)0;
    tempPath.robotType=(RobotPath::RobotType)0;
    tempPath.curPose.x=0;
    tempPath.curPose.y=0;
    tempPath.curPose.phi=0;
    tempPath.leftSpeed=0;
    tempPath.rightSpeed=0;
    tempPath.robotControl=1;
    tempPath.updateTime=time;
    ret_find=findPathIndexById(robotId,pathId,index);
    if(ret_find==-1)
    {
       path.append(tempPath);
    }
    else
    {
        RobotPath &m = path[index];
        tempPath.pathIdToGo=m.pathIdToGo;
        tempPath.robotState=m.robotState;
        tempPath.robotType=m.robotType;
        tempPath.curPose=m.curPose;
        tempPath.leftSpeed=m.leftSpeed;
        tempPath.rightSpeed=m.rightSpeed;
        tempPath.robotControl=m.robotControl;
        clearPath(index);
        m=tempPath;
    }

    return ret;
}


int Robot::insertRobotState(QHostAddress address,int robotId,int pathId,RobotPoint point,double left,double right,int goMainPathId,int robotState,int robotType,int time)
{
    int index=-1;
    RobotPath tempPath;

    tempPath.ip=address;
    tempPath.id=pathId;
    tempPath.num=0;
    tempPath.robotId=robotId;
    tempPath.pathIdToGo=0;
    tempPath.robotControl=1;//only set first time
    tempPath.updateTime=time;
    bool ret_find=findRobotId(robotId,index);
    if(ret_find==false)
    {
         path.append(tempPath);
    }
    ret_find=findRobotId(robotId,index);
    if(ret_find==false)
    {
       return -1;
    }
    bool retInsert = insertRobotStateByIndex(index,point,left,right,goMainPathId,robotState,robotType,time);
    if(retInsert==false) return -2;
    return 1;
}

bool Robot::insertRobotStateByIndex(int index,RobotPoint point,double left,double right,int goPathIndex,int robotState,int robotType,int time)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index>=pathNum) return false;
    RobotPath &m = path[index];

    if(goPathIndex>0) goPathIndex-=1;
    if(goPathIndex<m.point.size())
    {
      m.pathIdToGo= m.point.at(goPathIndex).mainPathId;
    }
    else
    {
       m.pathIdToGo=0;
    }
    m.curPose=point;
    //m.pathIdToGo=goMainPathId;
    m.robotState=(RobotPath::RobotState)robotState;
    m.robotType=(RobotPath::RobotType)robotType;
    m.leftSpeed=left;
    m.rightSpeed=right;
    m.updateTime=time;
    return true;
}

int Robot::findPathIndexById(int robotId,int pathId,int &index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    int i;
    index=-1;
    for(i=0;i<pathNum;i++)
    {
     RobotPath rp= path.at(i);
     int rid = rp.robotId;
     int id = rp.id;
     if(rid==robotId && id==pathId)
     {
         index=i;
         return 1;
     }
     else if(rid==robotId && id!=pathId)
     {
         index=i;
         return 0;
     }
    }
    return -1;
}

int Robot::findPathIndexById(int robotId)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    int i;

    for(i=0;i<pathNum;i++)
    {
     RobotPath rp= path.at(i);
     int rid = rp.robotId;
     if(rid==robotId)
     {
         return i;
     }
    }
    return -1;
}


bool Robot::getPathById(int robotId,RobotPath&rp)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    int i;

    for(i=0;i<pathNum;i++)
    {
     RobotPath a= path.at(i);
     int rid = a.robotId;
     if(rid==robotId)
     {
         rp=a;
         return true;
     }
    }
    return false;
}

bool Robot::findRobotId(int robotId, int &index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    int i;
    index=-1;
    for(i=0;i<pathNum;i++)
    {
     RobotPath rp= path.at(i);
     int rid = rp.robotId;
     if(rid==robotId)
     {
         index=i;
         return true;
     }

    }
    return false;
}

bool Robot::findIpByRobotId(int robotId, QHostAddress &address)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    int i;
    address = QHostAddress::Null;
    for(i=0;i<pathNum;i++)
    {
     RobotPath rp= path.at(i);
     int rid = rp.robotId;
     if(rid==robotId)
     {
         address=rp.ip;
         return true;
     }

    }
    return false;
}

bool Robot::findRobotPathIndexById(int pathId ,int &index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    int i;
    index=-1;
    for(i=0;i<pathNum;i++)
    {
     RobotPath rp= path.at(i);
     int pid = rp.pathIdToGo;
     if(pid==pathId)
     {
         index=i;
         return true;
     }

    }
    return false;
}


bool Robot::checkMovableById(int robotId)
{
    QMutexLocker locker(&mutex);

    for(int i=0;i<path.size();i++)
    {
        RobotPath rp= path.at(i);

        if(rp.robotId==robotId)
        {
            if(fabs(rp.leftSpeed)+fabs(rp.rightSpeed)<0.01)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    return true;
}

bool Robot::checkMovableByIndex(int index)
{
    QMutexLocker locker(&mutex);

    if(index<0||index>=path.size())
    {
        return true;
    }
    RobotPath rp= path.at(index);

    if(fabs(rp.leftSpeed)+fabs(rp.rightSpeed)<0.01)
    {
        return false;
    }
    else
    {
        return true;
    }
}


int Robot::getPathIdByIndex(int index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index<0||index>=pathNum) return 0;
    RobotPath rp = path.at(index);
    return rp.id;
}

int Robot::getPathRobotIdByIndex(int index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index>=pathNum) return 0;
    RobotPath rp = path[index];
    return rp.robotId;
}

QHostAddress Robot::getPathIpByIndex(int index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    QHostAddress ip;
    if(index>=pathNum) return ip;
    RobotPath rp = path[index];
    return rp.ip;
}

int Robot::insertPathPointByIndex(int index,RobotPoint point,int maxPointNum,int pointId)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index>=pathNum) return 0;
    RobotPath &m = path[index];
    int pointNum=m.point.size();

    if(pointNum==pointId&&m.num==maxPointNum&&pointId<maxPointNum)
    {
        RobotPathPoint tempPathPoint;
        tempPathPoint.id=pointId;
        tempPathPoint.point=point;
        int mid=MAX_MAIN_PATH_ID;
        bool ret = getMainPathMainIdByPoint(point,mid);
        if(ret==true)
        {
           tempPathPoint.mainPathId=mid;
        }
        else
        {
            tempPathPoint.mainPathId=MAX_MAIN_PATH_ID;
        }

        m.point.append(tempPathPoint);
    }
    else
    {
        return -1;
    }
    return 1;
}

int Robot::insertPathPointFrommainPathIdByIndex(int index,RobotPoint point,int mainPathId,int maxPointNum,int pointId)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index>=pathNum) return 0;
    RobotPath &m = path[index];
    int pointNum=m.point.size();

    if(pointNum==pointId&&m.num==maxPointNum&&pointId<maxPointNum)
    {
        RobotPathPoint tempPathPoint;
        tempPathPoint.id=pointId;
        tempPathPoint.point=point;
        tempPathPoint.mainPathId=mainPathId;
        m.point.append(tempPathPoint);
    }
    else
    {
        return -1;
    }
    return 1;
}


QStringList Robot::pathToString(int index)
{
    QStringList list;
    int pathNum=path.size();
    if(index<0||index>=pathNum) return list;
    RobotPath rp= path.at(index);
    //int robotId=rp.robotId;
    int pointNumMax=rp.num;
    int pointNum=rp.point.size();
    for(int i=0;i<pointNum;i++)
    {
        QString str;
        RobotPathPoint point= rp.point.at(i);
        if(point.mainPathId==rp.pathIdToGo)
        {
            str.append(">>>>>");
        }
        else
        {
            str.append(" ");
        }
        str.append(QString::number(pointNumMax,10));
        str.append(" ");
        str.append(QString::number(i,10));
        str.append(" ");
        str.append(QString::number(point.mainPathId,10));
        str.append(" ");
        str.append(QString::number(point.point.x,'f',3));
        str.append(" ");
        str.append(QString::number(point.point.y,'f',3));
        str.append(" ");
        str.append(QString::number(point.point.phi,'f',3));
        list.append(str);
    }
    return list;
}

QString Robot::robotStateToString(int index)
{
    QString str;
    int pathNum=path.size();
    if(index<0||index>=pathNum) return str;

    RobotPath rp= path.at(index);

    int curPathPointIndex=getPathToGoIndex(index);

    str.append(" CP ");
    str.append(QString::number(rp.curPose.x,'f',2));
    str.append(" ");
    str.append(QString::number(rp.curPose.y,'f',2));
    str.append(" ");
    str.append(QString::number(rp.curPose.phi,'f',2));
    str.append(" L ");
    str.append(QString::number(rp.leftSpeed,'f',1));
    str.append(" R ");
    str.append(QString::number(rp.rightSpeed,'f',1));
    str.append(" GI ");
    str.append(QString::number(curPathPointIndex,10));
    str.append(" GID ");
    str.append(QString::number(rp.pathIdToGo,10));
    str.append(" RS ");
    str.append(QString::number((int)(rp.robotState),10));
    str.append(" RT ");
    str.append(QString::number((int)(rp.robotType),10));
    str.append(" CT ");
    str.append(QString::number((int)(rp.robotControl),10));
    return str;
}

int Robot::getPathToGoIndex(int index)
{
    QString str;
    int pathNum=path.size();
    if(index<0||index>=pathNum) return -1;

    RobotPath rp= path.at(index);

    for(int i=0;i<rp.point.size();i++)
    {
        if(rp.point.at(i).mainPathId==rp.pathIdToGo)
        {
            return i;
        }
    }
    return -1;
}
int Robot::getPathNum()
{
    QMutexLocker locker(&mutex);
    return  path.size();
}

int Robot::getPathPointTime(int index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index<0||index>=pathNum) return 0;
    RobotPath rp= path.at(index);
    int pointNum=rp.updateTime;
    return pointNum;
}

int Robot::getPathPointNum(int index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index<0||index>=pathNum) return 0;
    RobotPath rp= path.at(index);
    int pointNum=rp.point.size();
    return pointNum;
}

int Robot::getPathPointNumMax(int index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index<0||index>=pathNum) return 0;
    RobotPath rp= path.at(index);
    int pointNum=rp.num;
    return pointNum;
}

const QVector<RobotPath> Robot::getPath()
{
    return path;
}

void Robot::clearPath(int index)
{
    QMutexLocker locker(&mutex);
    int pathNum=path.size();
    if(index<0||index>=pathNum) return;
    RobotPath &m = path[index];
    /*
    QString s1;
    s1.append("clearPath ");
    s1.append(QString::number(path[index].point.size(),10));
    s1.append(" ");
    */
    QVector<RobotPathPoint>().swap(m.point);
    //s1.append(QString::number(path[index].point.size(),10));
    //qDebug()<<s1;

}
void Robot::loadData(QCurveDataCus *data,QwtPointCus::PointStyle type)
{
    if(data==NULL) return;
    int sizeLoad=data->size();
    if(sizeLoad<1) return;
    int validId=0;
    if(type==QwtPointCus::PathPoint)
    {
        clearMainPath();

        QwtPointCus pointCus(QwtPointCus::PointStyle::Label,0,0,0,0);
        for(int i=0;i<sizeLoad;i++)
        {
           RobotPathPoint rp;
           pointCus=data->sample(i);
           if(pointCus.id()<100000)
           {
           rp.id=validId++;
           rp.mainPathId=pointCus.id();
           rp.point.x=pointCus.x();
           rp.point.y=pointCus.y();
           insertMainPath(rp);
           }
        }
    }
    else if(type==QwtPointCus::Dest)
    {
        clearDest();

        QwtPointCus pointCus(QwtPointCus::PointStyle::Label,0,0,0,0);
        for(int i=0;i<sizeLoad;i++)
        {
           RobotDestPoint dp;
           pointCus=data->sample(i);
           if(pointCus.id()<100000)
           {
               dp.id=pointCus.id();
               dp.point.x=pointCus.x();
               dp.point.y=pointCus.y();
               dp.point.phi=pointCus.phi();
               insertDest(dp);
           }
        }
    }
}


void Robot::clearMainPath()
{
    QMutexLocker locker(&mutex);
    /*
    QString s1;
    s1.append("clearMainPath ");
    s1.append(QString::number(mainPath.size(),10));
    s1.append(" ");
    */
    QVector<RobotPathPoint>().swap(mainPath);
    //s1.append(QString::number(mainPath.size(),10));
    //qDebug()<<s1;

}
/*
QVector<RobotPathPoint>* Robot::getMainPathHandle()
{
   return &mainPath;
}
*/
void Robot::insertMainPath(RobotPathPoint rp)
{
   QMutexLocker locker(&mutex);
   mainPath.append(rp);
}

QStringList Robot::mainPathToString()
{
    QStringList list;
    int pathNum=mainPath.size();
    for(int i=0;i<pathNum;i++)
    {
        QString str;
        RobotPathPoint point= mainPath.at(i);
        str.append(QString::number(point.id,10));
        str.append(" ");
        str.append(QString::number(pathNum,10));
        str.append(" ");
        str.append(QString::number(point.mainPathId,10));
        str.append(" ");
        str.append(QString::number(point.point.x,'f',3));
        str.append(" ");
        str.append(QString::number(point.point.y,'f',3));
        str.append(" ");
        str.append(QString::number(point.point.phi,'f',3));
        list.append(str);
    }
    return list;
}

int Robot::getMainPathNum()
{
    QMutexLocker locker(&mutex);
    return  mainPath.size();
}


int Robot::deletePath(int robotId)
{
    QMutexLocker locker(&mutex);
    bool find=false;
    for (int i = 0; i < path.size();) {
        RobotPath rp = path.at(i);

        if(rp.robotId == robotId)
        {
            path.erase(path.begin()+i);
            find=true;
        }
        else
        {
            i++;
        }
    }
    if(find==true) return 1;
    else return 0;
}

bool Robot::erasePathByIndex(int index)
{
    QMutexLocker locker(&mutex);
    int num =path.size();
    if(index>=num) return false;
    path.erase(path.begin()+index);
    return true;
}

RobotPath Robot::getPathByIndex(int index)
{
    QMutexLocker locker(&mutex);
    RobotPath rp;
    rp.id=-1;
    rp.num=0;
    rp.robotId=-1;
    int num =path.size();
    /*
    QByteArray Buffer;
    Buffer.append("\r\getPathByIndex index");
    Buffer.append(QString::number(index,10));
    Buffer.append(" num ");
    Buffer.append(QString::number(num,10));

    qDebug( Buffer.data());
*/
    if(index<0||index>=num) return rp;
    rp = path.at(index);
    return rp;
}

double Robot::calcuMinDisWithTwoPath(int indexA,int indexB)
{
    QMutexLocker locker(&mutex);

    int ida=-1,idb=-1;
    QVector<RobotPoint>pa,pb;
    //int i;
    int num=path.size();
    if(indexA<0||indexA>=num||indexB<0||indexB>=num) return -1;
    //qDebug("step 4");
    RobotPath rpa= path.at(indexA);
    RobotPath rpb= path.at(indexB);
    //qDebug("step 5");
    ida=rpa.pathIdToGo;
    idb=rpb.pathIdToGo;

    int na,nb;
    na=rpa.point.size();
    nb=rpb.point.size();

    if(na==0) return -1;
    if(nb==0) return -1;

    if(ida>=na) return -1;
    if(idb>=nb) return -1;
    //qDebug("step 6");

    RobotPath::RobotState rsa = rpa.robotState;
    RobotPath::RobotState rsb = rpb.robotState;

    if(rsa==RobotPath::AtStartPoint)
    {
        pa.append(rpa.point.at(0).point);
    }
    else if(rsa==RobotPath::AtDestPoint)
    {
        pa.append(rpa.point.at(rpa.point.size()-1).point);
    }
    else
    {
        for(int i=ida,k=0;i<na&&k<NEXT_PATH_NEED_CHECK;i++,k++)
        {
            pa.append(rpa.point.at(i).point);
        }
    }

    if(rsb==RobotPath::AtStartPoint)
    {
        pb.append(rpb.point.at(0).point);
    }
    else if(rsb==RobotPath::AtDestPoint)
    {
        pb.append(rpb.point.at(rpb.point.size()-1).point);
    }
    else
    {
        for(int i=idb,k=0;i<nb&&k<NEXT_PATH_NEED_CHECK;i++,k++)
        {
            pb.append(rpb.point.at(i).point);
        }
    }
    //qDebug("step 7");
    double mid_dis=MAX_DIS;
    for(int i=0;i<pa.size();i++)
    {
        for(int j=0;j<pb.size();j++)
        {
            double x=pa.at(i).x-pb.at(j).x;
            double y=pa.at(i).y-pb.at(j).y;
            double dis = sqrt(x*x+y*y);
            if(mid_dis>dis)
            {
                mid_dis=dis;
            }
        }
    }

    if(fabs(mid_dis-MAX_DIS)<0.01) return -1;

    return mid_dis;
}

void Robot::resetControlValue()
{
    /*
    for(int i=0;i<robotControl.size();i++)
    {
        robotControl[i]=1;
    }*/
    for(int i=0;i<path.size();i++)
    {
        path[i].robotControl=1;
    }
}

void Robot::updateControlNum()
{
    //int num=path.size();
    //robotControl.resize(num);
}

void Robot::pathControl()
{
    /*
    int num=path.size();

    robotControl.resize(num);
    for(int i=0;i<robotControl.size();i++)
    {
        robotControl[i]=1;
    }
    for(int i=0;i<path.size();i++)
    {
        for(int j=i+1;j<path.size();j++)
        {
            RobotPoint pa,pb;
            pa=path.at(i).curPose;
            pb=path.at(j).curPose;
            double x=pa.x-pb.x;
            double y=pa.y-pb.y;
            double dis = sqrt(x*x+y*y);
            if(dis<MIN_DIS_ROBOT_NEED_CHECK)
            {
                double minA=MAX_DIS;
                //double minA=calcuMinDisWithTwoPath(i,j);

                double minB=estimateMinDis(pa,pb);


                int front = frontRobot(pa,pb);
                QByteArray Buffer;
                Buffer.append("robot need check ");
                Buffer.append(QString::number(i,10));
                Buffer.append(" ");
                Buffer.append(QString::number(j,10));
                Buffer.append(" path min dis ");
                Buffer.append(QString::number(minA,'f',3));
                Buffer.append(" robot min dis ");
                Buffer.append(QString::number(minB,'f',3));
                qDebug( Buffer.data());

                QByteArray buf;
                buf.append("id ");
                buf.append(QString::number(i,10));
                buf.append(" ");
                buf.append(QString::number(j,10));
                buf.append(" rd ");
                buf.append(QString::number(dis,'f',3));
                buf.append(" pd ");
                buf.append(QString::number(minA,'f',3));
                buf.append(" red ");
                buf.append(QString::number(minB,'f',3));
                buf.append(" frt ");
                buf.append(QString::number(front,10));
                //enum RobotState{AtStartPoint=0,AtDestPoint,GoToDestPoint,GoToStartPoint};
                //enum RobotType{Guide=0,Waiter};
                //minA=10;
                QString str=buf;
               // emit robotMsg(str);
                robotMsg=str;
                if((minA>=0&&minA<MIN_DIS_TWO_PATH) || (minB>=0&&minB<MIN_DIS_ROBOT_COLLIDE)||(front==1||front==2))
                {
                    //int goid=i;
                    int ctrlA=1;
                    int ctrlB=1;
                    bool ret = controlRun(i,j,ctrlA,ctrlB);

                    if(ret==true)
                    {
                        if(robotControl[i]==1)
                        {
                            robotControl[i]=ctrlA;
                        }
                        if(robotControl[j]==1)
                        {
                            robotControl[j]=ctrlB;
                        }
                        QByteArray Buffer;
                        Buffer.append("controlRun true ");
                        Buffer.append(QString::number(i,10));
                        Buffer.append(" ");
                        Buffer.append(QString::number(j,10));
                        Buffer.append(" ");
                        Buffer.append(QString::number(robotControl[i],10));
                        Buffer.append(" ");
                        Buffer.append(QString::number(robotControl[j],10));
                        qDebug( Buffer.data());
                    }
                    else
                    {
                        qDebug("controlRun false ");

                    }
                }
            }
            else
            {
                QByteArray Buffer;
                Buffer.append("robot not need check ");
                Buffer.append(QString::number(i,10));
                Buffer.append(" ");
                Buffer.append(QString::number(j,10));
                qDebug( Buffer.data());
            }
        }
    }
    */
}

double Robot::estimateMinDis(RobotPoint a,RobotPoint b)
{
    const int n=30;
    const double step=0.1;
    double min_dis=MAX_DIS;
    //vectorPoseA.clear();
    //vectorPoseB.clear();
    for(int i=0;i<n;i++)
    {
        RobotPoint d,ea,eb;
        d.x=step*i;
        d.y=0;
        d.phi=0;
        ea=a+d;
        eb=b+d;
        //vectorPoseA.append(ea);
        //vectorPoseB.append(eb);
        double x=ea.x-eb.x;
        double y=ea.y-eb.y;
        double dis = sqrt(x*x+y*y);
        /*
        QByteArray Buffer;
        Buffer.append("estimateMinDis [");
        Buffer.append(QString::number(i,10));
        Buffer.append("] [");
        Buffer.append(QString::number(ea.x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(ea.y,'f',3));
        Buffer.append("] [");
        Buffer.append(QString::number(eb.x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(eb.y,'f',3));
        Buffer.append("] [");
        Buffer.append(QString::number(x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(y,'f',3));
        Buffer.append("] dis[");
        Buffer.append(QString::number(dis,'f',3));
        Buffer.append("] ");
        qDebug( Buffer.data());
        */
        if(dis<min_dis)
        {
            min_dis=dis;
        }
    }
    return min_dis;
}

QVector<int>& Robot::getRobotControl()
{
    QMutexLocker locker(&mutex);

    QVector<int>().swap(robotControl);
    for(int i=0;i<path.size();i++)
    {
       robotControl.append(path.at(i).robotControl);
    }
    return robotControl;
}

int Robot::getRobotControl(int i)
{
    QMutexLocker locker(&mutex);
    /*
    if(i>=0&&i<robotControl.size())
    {
        return robotControl.at(i);
    }
    */
    if(i>=0&&i<path.size())
    {
        return path.at(i).robotControl;
    }
    return 0;
}

void Robot::setRobotControl(int i,int val)
{
    QMutexLocker locker(&mutex);
    if(i>=0&&i<path.size())
    {
        path[i].robotControl=val;
    }
}

bool Robot::calcuNearIndex(int indexA,int indexB,int &nearIndex)
{
    QMutexLocker locker(&mutex);

    int ida=-1,idb=-1;
    QVector<RobotPoint>pa,pb;
    QVector<RobotPoint>pes;
    //int i;

    nearIndex = indexA;
    int num=path.size();

    if(indexA<0||indexA>=num||indexB<0||indexB>=num) return false;
    //qDebug("step 4");
    RobotPath rpa= path.at(indexA);
    RobotPath rpb= path.at(indexB);
    //qDebug("step 5");
    ida=rpa.pathIdToGo;
    idb=rpb.pathIdToGo;

    int na,nb;
    na=rpa.point.size();
    nb=rpb.point.size();

    if(ida>=na) return -1;
    if(idb>=nb) return -1;
    //qDebug("step 6");
    for(int i=ida,k=0;i<na&&k<NEXT_PATH_NEED_CHECK;i++,k++)
    {
        pa.append(rpa.point.at(i).point);
    }

    for(int i=idb,k=0;i<nb&&k<NEXT_PATH_NEED_CHECK;i++,k++)
    {
        pb.append(rpb.point.at(i).point);
    }
    //qDebug("step 7");

    for(int i=0;i<pa.size();i++)
    {
        for(int j=0;j<pb.size();j++)
        {
            double x=pa.at(i).x-pb.at(j).x;
            double y=pa.at(i).y-pb.at(j).y;
            double dis = sqrt(x*x+y*y);
            if(dis<MIN_DIS_TWO_PATH)
            {
                pes.append(pa);
            }
        }
    }

    RobotPoint robotPoseA,robotPoseB;
    robotPoseA=path.at(indexA).curPose;
    robotPoseB=path.at(indexB).curPose;

    if(pes.size()==0) return false;
    int minIndex=-1;
    double mid_dis=MAX_DIS;
    for(int i=0;i<pes.size();i++)
    {
        double xa= pes.at(i).x-robotPoseA.x;
        double ya= pes.at(i).y-robotPoseA.y;
        double disa = sqrt(xa*xa+ya*ya);
        double xb= pes.at(i).x-robotPoseB.x;
        double yb= pes.at(i).y-robotPoseB.y;
        double disb = sqrt(xb*xb+yb*yb);

        if(mid_dis>disa+disb)
        {
            mid_dis=disa+disb;
            minIndex=i;
        }
    }

    if(minIndex==-1)
    {
        return false;
    }

    double xa= pes.at(minIndex).x-robotPoseA.x;
    double ya= pes.at(minIndex).y-robotPoseA.y;
    double disa = sqrt(xa*xa+ya*ya);
    double xb= pes.at(minIndex).x-robotPoseB.x;
    double yb= pes.at(minIndex).y-robotPoseB.y;
    double disb = sqrt(xb*xb+yb*yb);
    if(disa<=disb)
    {
        nearIndex = indexA;
    }
    else
    {
        nearIndex = indexB;
    }
    return true;
}


bool Robot::dirIdentical(RobotPoint pa,RobotPoint pb)
{
    double a=pa.phi;
    double b=pb.phi;

    double r=0;
    if(a<b)
    {
        r=b-a;
        if(r>=PI_M)r=2*PI_M-r;
    }
    else if(b<a)
    {
        r=a-b;
        if(r>=PI_M)r=2*PI_M-r;
    }

    if(r<=PI_M_2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Robot::frontRobot(RobotPoint a,RobotPoint b)
{
    const int n=20;
    const double step=0.1;
    double min_dis_a2b=MAX_DIS;
    double min_dis_b2a=MAX_DIS;
    double rx=a.x-b.x;
    double ry=a.y-b.y;
    double rdis=sqrt(rx*rx+ry*ry);

    if(rdis<=0.1) return 0;
    for(int i=2;i<n;i++)
    {
        RobotPoint d,ea,eb;
        d.x=step*i;
        d.y=0;
        d.phi=0;
        ea=a+d;
        eb=b;
        double x=ea.x-eb.x;
        double y=ea.y-eb.y;
        double dis = sqrt(x*x+y*y);
        /*
        QByteArray Buffer;
        Buffer.append("estimateMinDis [");
        Buffer.append(QString::number(i,10));
        Buffer.append("] [");
        Buffer.append(QString::number(ea.x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(ea.y,'f',3));
        Buffer.append("] [");
        Buffer.append(QString::number(eb.x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(eb.y,'f',3));
        Buffer.append("] [");
        Buffer.append(QString::number(x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(y,'f',3));
        Buffer.append("] dis[");
        Buffer.append(QString::number(dis,'f',3));
        Buffer.append("] ");
        qDebug( Buffer.data());
        */
        if(dis<min_dis_a2b)
        {
            min_dis_a2b=dis;
        }
    }
    for(int i=2;i<n;i++)
    {
        RobotPoint d,ea,eb;
        d.x=step*i;
        d.y=0;
        d.phi=0;
        ea=a;
        eb=b+d;
        double x=ea.x-eb.x;
        double y=ea.y-eb.y;
        double dis = sqrt(x*x+y*y);
        /*
        QByteArray Buffer;
        Buffer.append("estimateMinDis [");
        Buffer.append(QString::number(i,10));
        Buffer.append("] [");
        Buffer.append(QString::number(ea.x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(ea.y,'f',3));
        Buffer.append("] [");
        Buffer.append(QString::number(eb.x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(eb.y,'f',3));
        Buffer.append("] [");
        Buffer.append(QString::number(x,'f',3));
        Buffer.append(" ");
        Buffer.append(QString::number(y,'f',3));
        Buffer.append("] dis[");
        Buffer.append(QString::number(dis,'f',3));
        Buffer.append("] ");
        qDebug( Buffer.data());
        */
        if(dis<min_dis_b2a)
        {
            min_dis_b2a=dis;
        }
    }

    if((min_dis_a2b<rdis-0.05) && (min_dis_b2a<rdis-0.05))//相向而行
    {
      return 3;
    }
    else if((min_dis_a2b<rdis-0.05) && !(min_dis_b2a<rdis-0.05))//
    {
      return 2;
    }
    else if(!(min_dis_a2b<rdis-0.05) && (min_dis_b2a<rdis-0.05))//
    {
      return 1;
    }
    return 3;
}

//enum RobotState{AtStartPoint=0,AtDestPoint,GoToDestPoint,GoToStartPoint};
bool Robot::controlRun(int indexA,int indexB,int& ctrlA,int &ctrlB)
{
    RobotPath rpa,rpb;
    int num=path.size();
    if(indexA<0||indexA>=num||indexB<0||indexB>=num) return false;

    rpa=path.at(indexA);
    rpb=path.at(indexB);

    RobotPoint robotPoseA,robotPoseB;
    robotPoseA=path.at(indexA).curPose;
    robotPoseB=path.at(indexB).curPose;

    RobotPath::RobotState rsa = rpa.robotState;
    RobotPath::RobotState rsb = rpb.robotState;
    //RobotPath::RobotType rta = rpa.robotType;
    //RobotPath::RobotType rtb = rpb.robotType;

    ctrlA=1;
    ctrlB=1;

    QString s1;
    s1.append("controlRun ");
    s1.append(QString::number(indexA,10));
    s1.append(" ");
    s1.append(QString::number(indexB,10));


    int front = frontRobot(robotPoseA,robotPoseB);

    if(rsa==RobotPath::GoToDestPoint)
    {
        if(rsb==RobotPath::AtStartPoint)
        {

        }
        else if(rsb==RobotPath::AtDestPoint)
        {

        }
        else if(rsb==RobotPath::GoToDestPoint)
        {
            if(dirIdentical(robotPoseA,robotPoseB)==true)  //同向
            {
                s1.append(" Iden");
                if(front==2)
                {
                   ctrlA=0;s1.append(" STOP A");
                }
                else
                {
                   ctrlB=0;s1.append(" STOP B");
                }

            }
            else// 反向
            {
                s1.append(" Not Iden");
                if(front==2)
                {
                   ctrlA=0;s1.append(" STOP A");
                }
                else
                {
                    ctrlB=0;s1.append(" STOP B");
                }
            }
        }
        else if(rsb==RobotPath::GoToStartPoint)
        {
            if(dirIdentical(robotPoseA,robotPoseB)==true)  //同向
            {
                s1.append(" Iden");
                if(front==2)
                {
                   ctrlA=0;s1.append(" STOP A");
                }
                else
                {
                   ctrlB=0;s1.append(" STOP B");
                }

            }
            else// 反向
            {
                s1.append(" Not Iden");
                if(front==2)
                {
                   ctrlA=0;s1.append(" STOP A");
                }
                else
                {
                    ctrlB=0;s1.append(" STOP B");
                }
            }
        }
    }
    else if(rsa==RobotPath::GoToStartPoint)
    {
        if(rsb==RobotPath::AtStartPoint)
        {

        }
        else if(rsb==RobotPath::AtDestPoint)
        {

        }
        else if(rsb==RobotPath::GoToDestPoint)
        {
            if(dirIdentical(robotPoseA,robotPoseB)==true)  //同向
            {
                s1.append(" Iden");
                if(front==1)
                {
                   ctrlB=0;s1.append(" STOP B");
                }
                else
                {
                   ctrlA=0;s1.append(" STOP A");
                }

            }
            else// 反向
            {
                s1.append(" Not Iden");
                if(front==1)
                {
                   ctrlB=0;s1.append(" STOP B");
                }
                else
                {
                   ctrlA=0;s1.append(" STOP A");
                }
            }
        }
        else if(rsb==RobotPath::GoToStartPoint)
        {
            if(dirIdentical(robotPoseA,robotPoseB)==true)  //同向
            {
                s1.append(" Iden");
                if(front==1)
                {
                   ctrlB=0;s1.append(" STOP B");
                }
                else
                {
                   ctrlA=0;s1.append(" STOP A");
                }

            }
            else// 反向
            {
                s1.append(" Not Iden");
                if(front==1)
                {
                   ctrlB=0;s1.append(" STOP B");
                }
                else
                {
                   ctrlA=0;s1.append(" STOP A");
                }
            }
        }
    }
    s1.append(" CTRL ");
    s1.append(QString::number(ctrlA,10));
    s1.append(" ");
    s1.append(QString::number(ctrlB,10));
    qDebug()<<s1;
    return true;

}

 QString Robot::getMsg()
 {
    return robotMsg;
 }

 QVector<RobotPoint> Robot::getPose(int n)
 {
    if(n==0)
    {
       return vectorPoseA;
    }
    else if(n==1)
    {

       return vectorPoseB;
    }
 }

 QVector<QVector<RobotPathPoint>> Robot::getPose()
 {
    return vectorPose;
 }

 void Robot::estimateRobotPose(int num)
 {
     const int n=5;
     const double step=0.1;


     RobotPoint a,b;


     if(num<0||num>=path.size()) return;

     if(num==0)
     {
        vectorPoseA.clear();
     }
     else if(num==1)
     {
        vectorPoseB.clear();
     }

     a=path.at(num).curPose;

     double l=path.at(num).leftSpeed;
     double r=path.at(num).rightSpeed;

     if(num==0)
     {
        vectorPoseA.append(a);
     }
     else if(num==1)
     {
        vectorPoseB.append(a);
     }
     for(int i=1;i<n;i++)
     {
         RobotPoint d;

         computeFromEncoder(d,l,r);
         a=a+d;
         if(num==0)
         {
            vectorPoseA.append(a);
         }
         else if(num==1)
         {
            vectorPoseB.append(a);
         }

     }
 }

void Robot::estimateRobotPose()
{
    const int n=5;
    int num = path.size();

    QVector<QVector<RobotPathPoint>>().swap(vectorPose);

     for(int i=0;i<path.size();i++)
     {
         QVector<RobotPathPoint> vectorPoseA;
         RobotPathPoint a;
         a.point=path.at(i).curPose;
         a.d_robotId=path.at(i).robotId;
         double l=path.at(i).leftSpeed;
         double r=path.at(i).rightSpeed;

         vectorPoseA.append(a);
         RobotPoint rpa;
         RobotPathPoint b;
         rpa=a.point;
         b.d_robotId=a.d_robotId;
         for(int i=1;i<n;i++)
         {
             RobotPoint d;

             computeFromEncoder(d,l,r);
             rpa=rpa+d;
             b.point=rpa;
             vectorPoseA.append(b);
         }
         vectorPose.append(vectorPoseA);
     }
 }

 //k=0.167*3.1415926/4000/9.28; //安装北阳的餐车

 ///////////////////////////////
 void Robot::computeFromEncoder(RobotPoint	&out_incr_odom,
     double	left,
     double	right)
 {
         const double K=0.001;
         const double D=0.5;
         const double	As   = 0.5* ( right*K + left*K );
         const double	Aphi = (  right*K-left*K ) / D;

         double  x,y;
         if (Aphi!=0)
         {
             const double R = As/Aphi;
             x=R*sin(Aphi);
             y=R*(1-cos(Aphi));
         }
         else
         {
             x=As;
             y=0;
         }
         out_incr_odom.x=x;
         out_incr_odom.y=y;
         out_incr_odom.phi=Aphi;
         //cout << "odom x: " << x <<"odom y: " << y <<"odom phi: " << Aphi << endl;
         //sprintf(&buffer[0],"computeFromEncoder x: %.03fms  y:%.03fms  phi:%.03fms\n",x,y,Aphi );
         //g_str+=buffer;
 }

QVector<RobotPathPoint> RobotPath::getNearPointById(int id)
{
    QVector<RobotPathPoint> p;
    int startId=0;
    int endId=point.size()-1;
    for(int i=0;i<point.size();i++)
    {
        if(id==point.at(i).mainPathId)
        {
            startId=i-1;
            endId=i+1;
            if(i==0)
            {
                startId=0;
            }
            if(i==point.size()-1)
            {
                endId=point.size()-1;
            }
            p.append(point.at(startId));
            p.append(point.at(i));
            p.append(point.at(endId));
            break;
        }
    }
    return p;
}

QVector<RobotPathPoint> RobotPath::getNearPointById()
{
    QVector<RobotPathPoint> p;
    int startId=0;
    int endId=point.size()-1;
    for(int i=0;i<point.size();i++)
    {
        if(pathIdToGo==point.at(i).mainPathId)
        {
            startId=i-1;
            endId=i+1;
            if(i==0)
            {
                startId=0;
            }
            if(i==point.size()-1)
            {
                endId=point.size()-1;
            }
            p.append(point.at(startId));
            p.append(point.at(i));
            p.append(point.at(endId));
            break;
        }
    }
    return p;
}

int RobotPath::getPointNumToLastById()
{
    int num=0;
    int startId=0;
    int endId=point.size()-1;
    for(int i=0;i<point.size();i++)
    {
        if(pathIdToGo==point.at(i).mainPathId)
        {
            startId=i-1;
            endId=i+1;
            if(i==0)
            {
                startId=0;
            }
            if(i==point.size()-1)
            {
                endId=point.size()-1;
            }
            break;
        }
    }
    return point.size()-endId;
}


void Robot::setPathRobotIdById(int id,int robotId)
{
    int i;
    for(i=0;i<mainPath.size();i++)
    {
     RobotPathPoint rp= mainPath.at(i);
     if(rp.mainPathId==id)
     {
         mainPath[i].setRobotId(robotId);
         break;
     }
    }
}

void Robot::resetAllPathRobotId(int robotId)
{
    int i;
    for(i=0;i<mainPath.size();i++)
    {
      mainPath[i].setRobotId(robotId);
    }
}

int Robot::getPathRobotIdById(int id)
{
    int i;
    for(i=0;i<mainPath.size();i++)
    {
     RobotPathPoint rp= mainPath.at(i);
     if(rp.mainPathId==id)
     {
         return rp.getRobotId();
     }
    }
    return 0;
}

int Robot::getMainPathRobotIdByIndex(int index)
{
    int pathNum=mainPath.size();
    if(index<0||index>=pathNum) return 0;

    RobotPathPoint rp= mainPath.at(index);
    return rp.getRobotId();
}

QVector<int> RobotPath::getNearPointIdById(int id)
{
    QVector<int> p;
    int startId=0;
    int endId=point.size()-1;
    for(int i=0;i<point.size();i++)
    {
        if(id==point.at(i).mainPathId)
        {
            startId=i-1;
            endId=i+1;
            if(i==0)
            {
                startId=0;
            }
            if(i==point.size()-1)
            {
                endId=point.size()-1;
            }
            p.append(point.at(startId).mainPathId);
            p.append(point.at(i).mainPathId);
            p.append(point.at(endId).mainPathId);
            break;
        }
    }
    return p;
}
QVector<int> RobotPath::getNearPointIdById()
{
    QVector<int> p;
    int startId=0;
    int endId=point.size()-1;
    for(int i=0;i<point.size();i++)
    {
        if(pathIdToGo==point.at(i).mainPathId)
        {
            startId=i-1;
            endId=i+1;
            if(i==0)
            {
                startId=0;
            }
            if(i==point.size()-1)
            {
                endId=point.size()-1;
            }
            p.append(point.at(startId).mainPathId);
            p.append(point.at(i).mainPathId);
            p.append(point.at(endId).mainPathId);
            break;
        }
    }
    return p;
}



void Robot::clearDest()
{
    QMutexLocker locker(&mutex);
    /*
    QString s1;
    s1.append("clearMainPath ");
    s1.append(QString::number(mainPath.size(),10));
    s1.append(" ");
    */
    QVector<RobotDestPoint>().swap(dest);
    //s1.append(QString::number(mainPath.size(),10));
    //qDebug()<<s1;

}

void Robot::insertDest(RobotDestPoint rp)
{
   QMutexLocker locker(&mutex);
   dest.append(rp);
}

QStringList Robot::destToString()
{
    QStringList list;
    int pathNum=dest.size();
    for(int i=0;i<pathNum;i++)
    {
        QString str;
        RobotDestPoint point= dest.at(i);
        str.append(QString::number(i,10));
        str.append(" ");
        str.append(QString::number(pathNum,10));
        str.append(" ");
        str.append(QString::number(point.id,10));
        str.append(" ");
        str.append(QString::number(point.point.x,'f',3));
        str.append(" ");
        str.append(QString::number(point.point.y,'f',3));
        str.append(" ");
        str.append(QString::number(point.point.phi,'f',3));
        list.append(str);
    }
    return list;
}

int Robot::getDestNum()
{
    QMutexLocker locker(&mutex);
    return  dest.size();
}
