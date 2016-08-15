#include "twowayregion.h"
#include <QDebug>
TwoWayRegion::TwoWayRegion()
{

}

void TwoWayRegion::setRegion(EditShapeItem *region, QCurveDataCus *path,Robot *robot)
{
    qDebug() << "TwoWayRegion setRegion start";
    QVector<RobotPathPoint> pathIn;
    //QVector<RobotPath> robotIn;

    QVector<QVector<RobotPathPoint>>().swap(vectorPath);
    QVector<QVector<RobotPath>>().swap(vectorRobot);
    QVector<RobotHighParam>().swap(vHighParam);
    QVector<RobotPath>().swap(robotIn);
    shapeItem = region;


    QCurveDataCus *pathData = path;

    for(int i=0;i<(int)pathData->size();i++)
    {
        QwtPointCus point=pathData->sample(i);
        if(shapeItem->pointInPolygon(QPointF(point.x(),point.y())))
        {
            RobotPathPoint rpp(point);
            pathIn.append(rpp);
        }
    }


    for(int i=0;i<robot->getPathNum();i++)
    {
        RobotPath path = robot->getPathByIndex(i);
        QVector<RobotPathPoint> pointList = path.getNearPointById();
        //QVector<int> pointIdList = path.getNearPointIdById();
        RobotPoint point = path.curPose;

        bool ret1=false,ret2=false,ret3=false;
        ret1=shapeItem->pointInPolygon(QPointF(point.x,point.y));
        if(pointList.size()>=3)
        {
            ret2=shapeItem->pointInPolygon(QPointF(pointList.at(1).point.x,pointList.at(1).point.y));
            ret3=shapeItem->pointInPolygon(QPointF(pointList.at(2).point.x,pointList.at(2).point.y));
        }
        //qDebug() << "path id "<<QString::number(path.robotId,10)<<"go id "<<QString::number(path.pathIdToGo,10);
        //qDebug() << "pointInPolygon 1 "<<QString::number(ret1,10);
        //qDebug() << "pointInPolygon 2 "<<QString::number(ret2,10)<<"go id "<<QString::number(pointIdList.at(1),10);
        //qDebug() << "pointInPolygon 3 "<<QString::number(ret3,10)<<"go id "<<QString::number(pointIdList.at(2),10);
        if(ret1||ret2||ret3)
        {
            robotIn.append(path);
        }
    }

    qDebug() << "robotIn size "<<QString::number(robotIn.size(),10);
    for(int m=0;m<robotIn.size();m++)
    {
       RobotPath rpm = robotIn.at(m);
       qDebug() << "robotIn i "<<QString::number(m,10)<<"id "<<QString::number(rpm.robotId,10);
    }
    if(robotIn.size()>0)
    {

    }
    else
    {
        qDebug() << "robotIn nothing";
        return;
    }
///////////////////////////
    robot->resetAllPathRobotId(0);
    for(int i=0;i<robotIn.size();i++)
    {
        RobotPath robotPath = robotIn.at(i);
        RobotHighParam hParam;
        hParam.id=robotPath.robotId;
        QVector<int> pointIdList = robotPath.getNearPointIdById();
        QVector<RobotPathPoint> pointList = robotPath.getNearPointById();
        if(pointIdList.size()>=3&&pointList.size()>=3)
        {
            for(int n=0;n<3;n++)
            {
                hParam.nearPointId[i]=pointIdList.at(i);
            }
            hParam.disToCur=m_math.dis(robotPath.curPose,pointList.at(1).point);
            hParam.perDisToCur=m_math.perDis(robotPath.curPose,pointList.at(0).point,pointList.at(1).point);
            hParam.onSamePath=pointOnSameRoad(hParam.nearPointId[0],hParam.nearPointId[1]);
        }
        else
        {
            for(int n=0;n<3;n++)
            {
                hParam.nearPointId[i]=0;
            }
            hParam.disToCur=0;
            hParam.perDisToCur=0;
            hParam.onSamePath=true;
        }
        vHighParam.append(hParam);
    }
    for(int i=0;i<robotIn.size();i++)
    {
        for(int j=i+1;j<robotIn.size();j++)
        {
            RobotPath rpi = robotIn.at(i);
            RobotPath rpj = robotIn.at(j);
            if(rpi.pathIdToGo==rpj.pathIdToGo)
            {
                RobotHighParam hParami=vHighParam.at(i);
                RobotHighParam hParamj=vHighParam.at(j);
                int ret = priorRobot(hParami,hParamj);
                if(ret==1)
                {
                    robot->setPathRobotIdById(rpi.pathIdToGo,hParami.id);
                }
                else
                {
                    robot->setPathRobotIdById(rpj.pathIdToGo,hParamj.id);
                }
            }
        }
    }
///////////////////////////////

    QVector<QVector<RobotPath>> vvrp;

    for(int i=0;i<pathIn.size();i++)
    {
        QVector<RobotPath> vrp;
        RobotPathPoint rpp=pathIn.at(i);

        for(int j=0;j<robotIn.size();j++)
        {
            RobotPath rp=robotIn.at(j);

            if(rpp.mpid()==rp.pathIdToGo)
            {
                vrp.append(rp);
            }
        }
        if(vrp.size()>0)
        {
            vvrp.append(vrp);
}
    }
    qDebug() << "path point use list:";
    for(int i=0;i<vvrp.size();i++)
    {
        QVector<RobotPath> vrp=vvrp.at(i);

        for(int j=0;j<vrp.size();j++)
        {
            RobotPath rp=vrp.at(j);
            if(j==0)
            {
                robot->setPathRobotIdById(rp.pathIdToGo,rp.robotId);
            }
            qDebug() << "i "<<QString::number(i,10)<< "j "<<QString::number(i,10)<<"path id"<<QString::number(rp.robotId,10);
        }
    }

///////////////////////////////////

    bool pathInPointValid=pathPointInRegionValid(pathIn);
    if(pathIn.size()>0&&pathInPointValid)
    {
        for(int i=0;i<pathIn.size();i++)
        {
            int id = pathIn.at(i).mpid();
            if(findIdIn(id)==true) continue;

            bool find=false;
            for(int j=0;j<pathIn.size();j++)
            {
                find = findNet(pathIn.at(j),id);
                if(find==true) break;
            }
            if(find==false)
            {
                QVector<RobotPathPoint> path;
                int n=pathIn.size();
                RobotPathPoint pathPoint = pathIn.at(i);
                path.append(pathPoint);
                while(n--)
                {
                    RobotPathPoint findPoint;
                    bool ret=false;
                    ret=getFirstValidPointIn(pathPoint,pathIn,findPoint);
                    if(ret==true)
                    {
                        pathPoint = findPoint;
                        path.append(pathPoint);
                    }
                    else
                    {
                        break;
                    }
                }
                vectorPath.append(path);
            }
        }
    }

    if(vectorPath.size()>0&&robotIn.size()>0)
    {
        vectorRobot.resize(vectorPath.size());
        for(int i=0;i<robotIn.size();i++)
        {
            RobotPath robotPath = robotIn.at(i);
            int curIdGo = robotPath.pathIdToGo;
            QVector<RobotPathPoint> robotPathNear = robotPath.getNearPointById(curIdGo);
            if(robotPathNear.size()>0)
            {
                int index = robotOnWitchPath(robotPathNear);
                if(index>=0&&index<vectorRobot.size())
                {
                    vectorRobot[index].append(robotPath);
                }
            }
        }
    }


    //qDebug() << "setRegion id "<<QString::number(region->id(),10)<<" vectorPath num "<<QString::number(vectorPath.size(),10);
    for(int i=0;i<vectorPath.size();i++)
    {
        //qDebug() << "i "<<QString::number(i,10);
        QVector<RobotPathPoint> vp=vectorPath.at(i);
        for(int j=0;j<vp.size();j++)
        {
            RobotPathPoint p=vp.at(j);
            //qDebug() << "id "<<QString::number(p.mpid(),10)<<" "<<QString::number(p.x(),'f',3)<<" "<<QString::number(p.y(),'f',3);
        }
    }
    //qDebug() << "setRegion id "<<QString::number(region->id(),10)<<" vectorRobot num "<<QString::number(vectorRobot.size(),10);
    for(int i=0;i<vectorRobot.size();i++)
    {
        //qDebug() << "i "<<QString::number(i,10);
        QVector<RobotPath> vp=vectorRobot.at(i);
        for(int j=0;j<vp.size();j++)
        {
            RobotPath p=vp.at(j);
            //qDebug() << "robot id "<<QString::number(p.robotId,10)<<" "<<QString::number(p.curPose.x,'f',3)<<" "<<QString::number(p.curPose.y,'f',3);
        }
    }

    for(int i=0;i<vectorRobot.size();i++)
    {
        //qDebug() << "i "<<QString::number(i,10);
        QVector<RobotPath> vp=vectorRobot.at(i);
        QVector<RobotPathPoint> pc=vectorPath.at(i);
        RobotPathPoint lastPathPoint = pc.at(pc.size()-1);
        for(int j=0;j<vp.size()-1;j++)
        {
            for(int k=j+1;k<vp.size();k++)
            {
                RobotPath p1=vp.at(j);
                RobotPath p2=vp.at(k);
                double dis1=disPoint2Point(lastPathPoint,p1);
                double dis2=disPoint2Point(lastPathPoint,p2);
                if(dis1>dis2)
                {
                    RobotPath t;
                    t = (vectorRobot[i])[j];
                    (vectorRobot[i])[j]=(vectorRobot[i])[k];
                    (vectorRobot[i])[k]=t;
                }
            }
            //qDebug() << "robot id "<<QString::number(p.robotId,10)<<" "<<QString::number(p.curPose.x,'f',3)<<" "<<QString::number(p.curPose.y,'f',3);
        }
    }
    qDebug() << "setRegion id "<<QString::number(region->id(),10)<<" vectorRobot2 num "<<QString::number(vectorRobot.size(),10);
    for(int i=0;i<vectorRobot.size();i++)
    {
        qDebug() << "i "<<QString::number(i,10);
        QVector<RobotPath> vp=vectorRobot.at(i);
        for(int j=0;j<vp.size();j++)
        {
            RobotPath p=vp.at(j);
            qDebug() << "robot id "<<QString::number(p.robotId,10)<<" "<<QString::number(p.curPose.x,'f',3)<<" "<<QString::number(p.curPose.y,'f',3);
        }
    }
    qDebug() << "setRegion end\n";
}
bool TwoWayRegion::findNet(RobotPathPoint p,int net)
{
    int netSize=p.netSize();
    for(int m=0;m<netSize;m++)
    {
        if(net == p.net(m))
        {
            return true;
        }
    }
    return false;
}

int TwoWayRegion::getFirstValidNet(RobotPathPoint p)
{
    int netSize=p.netSize();
    for(int m=0;m<netSize;m++)
    {
        int net = p.net(m);
        if(net)
        {
            return net;
        }
    }
    return -1;
}

bool TwoWayRegion::getFirstValidPointIn(RobotPathPoint p,QVector<RobotPathPoint> pathIn,RobotPathPoint &findPoint)
{
    int netSize=p.netSize();
    for(int m=0;m<netSize;m++)
    {
        int net = p.net(m);
        if(net>0)
        {
            for(int i=0;i<pathIn.size();i++)
            {
                RobotPathPoint point = pathIn.at(i);
                if(net==point.mpid())
                {
                    findPoint = point;
                    return true;
                }
            }
        }
    }
    return false;
}

bool TwoWayRegion::findIdIn(int id)
{
    for(int i=0;i<vectorPath.size();i++)
    {
        QVector<RobotPathPoint> vp=vectorPath.at(i);
        for(int j=0;j<vp.size();j++)
        {
            RobotPathPoint p=vp.at(j);
            if(p.mpid()==id)
            {
                return true;
            }
        }
    }
    return false;
}

int TwoWayRegion::getNetNum(RobotPathPoint p)
{
    int netSize=p.netSize();
    int n=0;
    for(int m=0;m<netSize;m++)
    {
        int net = p.net(m);
        if(net>0)
        {
            n++;
        }
    }
    return n;
}

int TwoWayRegion::findChildNetInPath(RobotPathPoint p ,QVector<RobotPathPoint> pathIn)
{
    int netSize=p.netSize();
    int n=0;
    for(int m=0;m<netSize;m++)
    {
        int net = p.net(m);
        if(net>0)
        {
            bool ret = findIdInPath(net,pathIn);
            if(ret==true) n++;
        }
    }
    return n;
}

bool TwoWayRegion::findIdInPath(int id ,QVector<RobotPathPoint> pathIn)
{
    for(int i=0;i<pathIn.size();i++)
    {
        int t_id=pathIn.at(i).mpid();
        if(t_id==id)
        {
            return true;
        }
    }
    return false;

}

bool TwoWayRegion::pathPointInRegionValid(QVector<RobotPathPoint> pathIn)
{
    int invalidNum=0;
    for(int i=0;i<pathIn.size();i++)
    {
        RobotPathPoint p=pathIn.at(i);
        int netNum=getNetNum(p);
        if(netNum>1)
        {
            int numIn=findChildNetInPath(p,pathIn);
            if(numIn>1)
            {
                qDebug() << "pathPointInRegionValid error id"<<QString::number(p.mpid(),10)<<" "<<QString::number(numIn,10)<<" net in";
                invalidNum++;
            }
        }

    }
    if(invalidNum>0) return false;
    return true;
}

//QVector<RobotPathPoint> getNearPointById(int id);

int TwoWayRegion::robotOnWitchPath(QVector<RobotPathPoint> robotPath)
{
    if(vectorPath.size()<=0) return -1;
    if(robotPath.size()<=0) return -1;
    for(int i=0;i<vectorPath.size();i++)
    {
        QVector<RobotPathPoint> vp=vectorPath.at(i);
        for(int j=0;j<vp.size();j++)
        {
            RobotPathPoint p=vp.at(j);
            for(int m=0;m<robotPath.size();m++)
            {
                if(robotPath.at(m).mpid()==p.mpid())
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

double TwoWayRegion::disPoint2Point(RobotPathPoint pc,RobotPath rp)
{
    double dis;
    double x=pc.point.x-rp.curPose.x;
    double y=pc.point.y-rp.curPose.y;
    dis=sqrt(x*x+y*y);
    return dis;
}

double TwoWayRegion::disPoint2Point(RobotPath rp,RobotPath rp2)
{
    double dis;
    double x=rp.curPose.x-rp2.curPose.x;
    double y=rp.curPose.y-rp2.curPose.y;
    dis=sqrt(x*x+y*y);
    return dis;
}


void TwoWayRegion::setControl(Robot *robot)
{
    qDebug() << "TwoWayRegion setControl start";
    if(vectorRobot.size()==0) return;

    int ctrlMode=2;
    QVector<int> control = robot->getRobotControl();

    if(vectorRobot.size()>1) ctrlMode=2;
    #if 0
    for(int i=0;i<vectorRobot.size();i++)
    {
        QVector<RobotPath> vp=vectorRobot.at(i);
        //if(vp.size()<=1) continue;
        for(int j=0;j<vp.size();j++)//已排序 序号小的在前面
        {
            RobotPath pj=vp.at(j);

            int index=-1;
            robot->findPathIndexById(pj.robotId,0,index);

            if(index!=-1&&index<control.size())
            {
               robot->setRobotControl(index,ctrlMode);
            }
        }
    }

    for(int i=0;i<vectorRobot.size();i++)
    {
        QVector<RobotPath> vp=vectorRobot.at(i);
        if(vp.size()<=1) continue;
        for(int j=0;j<vp.size()-1;j++)//已排序 序号小的在前面
        {
            RobotPath pj=vp.at(j);
            RobotPath pm=vp.at(j+1);
            double dis = disPoint2Point(pj,pm);
            qDebug() << "dis "<<QString::number(dis,'f',3)<<"id "<<QString::number(pm.robotId,10)<<"near robot id "<<QString::number(pj.robotId,10);
            if(dis<safe_dis_same_road)
            {
                int index=-1;
                robot->findPathIndexById(pm.robotId,0,index);

                if(index!=-1&&index<control.size())
                {
                   //control[index]=0;
                   robot->setRobotControl(index,0);
                   qDebug() << "stop back robot id "<<QString::number(pm.robotId,10)<<"near robot id "<<QString::number(pj.robotId,10);
                }
            }
        }
    }
///////////////////////////

    for(int i=0;i<vectorRobot.size();i++)
    {
        QVector<RobotPath> vrp=vectorRobot.at(i);
        for(int j=0;j<vrp.size();j++)
        {
            RobotPath rp = vrp.at(j);
            RobotHighParam rhp = getParamById(rp.robotId);
            int ret = robot->getPathRobotIdById(rp.pathIdToGo);
            if(ret!=0&&ret!=rp.robotId)
            {
                int index=-1;
                int index2=-1;
                index = robot->findPathIndexById(rp.robotId);
                index2 = robot->findPathIndexById(ret);


                if(index2!=-1&&index2<control.size())
                {
                   int c=robot->getRobotControl(index2);
                   if(c==0)
                   {
                       robot->setRobotControl(index2,ctrlMode);
                   }
                   //qDebug() << "stop robot id "<<QString::number(rp.robotId,10)<<"pathPoint id "<<QString::number(rp.pathIdToGo,10)<<" set by robot"<<QString::number(ret,10);
                }

                if(index!=-1&&index<control.size())
                {
                   robot->setRobotControl(index,0);
                   qDebug() << "stop robot id "<<QString::number(rp.robotId,10)<<"pathPoint id "<<QString::number(rp.pathIdToGo,10)<<" set by robot"<<QString::number(ret,10);
                }
            }
        }

    }
#endif

    qDebug() << "robotIn size "<<QString::number(robotIn.size(),10);
    for(int m=0;m<robotIn.size()-1;m++)
    {
       RobotPath rpm = robotIn.at(m);
       qDebug() << "robotIn i "<<QString::number(m,10)<<"id "<<QString::number(rpm.robotId,10);
    }
    for(int m=0;m<robotIn.size()-1;m++)
    {
        for(int n=m+1;n<robotIn.size();n++)
        {
            RobotPath rpm = robotIn.at(m);
            RobotPath rpn = robotIn.at(n);
            qDebug() << "cmp id "<<QString::number(rpm.robotId,10)<<" and "<<QString::number(rpn.robotId,10);
            double estDis=m_math.estimateMinDis(rpm.curPose,rpm.leftSpeed,rpm.rightSpeed,rpn.curPose,rpn.leftSpeed,rpn.rightSpeed);
            double directDis=m_math.dis(rpm.curPose,rpn.curPose);
            qDebug() << "estDis"<<QString::number(estDis,'f',3)<<"directDis"<<QString::number(directDis,'f',3);
            if(estDis>safe_dis_go_across)//no ctrl
            {
                qDebug() << "no ctrl a1";
            }
            else if(directDis>safe_dis_need_no_ctrl)//no ctrl
            {
                qDebug() << "no ctrl a2";
            }
            else
            {
                int ret = m_math.safeRobot(rpm.curPose,rpn.curPose);
                int index=-1;
                int robotId=0;
                if(ret==1)
                {
                    robotId = rpn.robotId;
                }
                else
                {
                    robotId = rpm.robotId;
                }
                bool movableM = robot->checkMovableById(rpm.robotId);
                bool movableN = robot->checkMovableById(rpn.robotId);
                qDebug()<<"safeRobot"<<QString::number(ret,10)<<"movableM"<<QString::number(movableM,10)<<"movableN"<<QString::number(movableN,10);
                if(ret==1&&movableM==false&&movableN==true) //no ctrl
                {
                    qDebug() << "no ctrl b";
                }
                else if(ret==2&&movableM==true&&movableN==false) //no ctrl
                {
                    qDebug() << "no ctrl c";
                }
                else
                {
                    index = robot->findPathIndexById(robotId);
                    if(index!=-1&&index<control.size())
                    {
                       robot->setRobotControl(index,0);
                       qDebug() << "stop unsafe robot id "<<QString::number(robotId,10);
                    }
                }

            }
        }
    }


    QVector<int> ctrl = robot->getRobotControl();
    qDebug() << "ctrl size "<<QString::number(ctrl.size(),10);
    for(int i=0;i<ctrl.size();i++)
    {
        qDebug() << "ctrl i "<<QString::number(i,10)<<" val "<<QString::number(ctrl.at(i),10);
    }

}


int TwoWayRegion::getRegionId()
{
    if(shapeItem!=NULL)
    {
        return shapeItem->id();
    }
    return -1;
}

/*
 *     for(int i=0;i<vvrp.size();i++)
    {
        QVector<RobotPath> vrp=vvrp.at(i);

        for(int j=0;j<vrp.size();j++)
        {
            RobotPath rp=vrp.at(j);
            if(j==0)
            {
                robot->setPathRobotIdById(rp.pathIdToGo,rp.robotId);
            }
            qDebug() << "i "<<QString::number(i,10)<< "j "<<QString::number(i,10)<<"path id"<<QString::number(rp.robotId,10);
        }
    }
 * */
void TwoWayRegion::checkPathRobotId(Robot *robot)
{
    for(int i=0;i<robot->getMainPathNum();i++)
    {
        int robotId = robot->getMainPathRobotIdByIndex(i);
        RobotPath rp;
        bool ok = robot->getPathById(robotId,rp);
        if(robotId!=rp.pathIdToGo)
        {
            robot->setPathRobotIdById(robotId,0);
        }
    }
}

int TwoWayRegion::getPathIndexPointOn(int id)
{
    if(vectorPath.size()<=0) return -1;
    for(int i=0;i<vectorPath.size();i++)
    {
        QVector<RobotPathPoint> vp=vectorPath.at(i);
        for(int j=0;j<vp.size();j++)
        {
            RobotPathPoint p=vp.at(j);

            if(id==p.mpid())
            {
                return i;
            }
        }
    }
    return -1;
}

bool TwoWayRegion::pointOnSameRoad(int ida,int idb)
{
    int a=getPathIndexPointOn(ida);
    int b=getPathIndexPointOn(idb);
    if(a!=-1&&b!=-1&&a==b)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int TwoWayRegion::priorRobot(RobotHighParam pa,RobotHighParam pb)
{
    char bitMaskA;
    char bitMaskB;
    bitMaskA=0;
    bitMaskB=0;
    if(pa.disToCur<1.0&&pb.disToCur<1.0)
    {
        if(pa.disToCur<pb.disToCur)
        {
            bitMaskA|=0x80;
        }
        else
        {
            bitMaskB|=0x80;
        }
    }
    else if(pa.disToCur<1.0&&pb.disToCur>=1.0)
    {
        bitMaskA|=0x80;
    }
    else if(pb.disToCur<1.0&&pa.disToCur>=1.0)
    {
        bitMaskB|=0x80;
    }
//////////////////////////////
    if(pa.onSamePath)
    {
        bitMaskA|=0x40;
    }
    if(pb.onSamePath)
    {
        bitMaskB|=0x40;
    }

    if(pa.perDisToCur>0.3&&pb.perDisToCur>0.3)
    {
        if(pa.perDisToCur>pb.perDisToCur)
        {
            bitMaskA|=0x20;
        }
        else
        {
            bitMaskB|=0x20;
        }
    }
    else if(pa.perDisToCur>0.3&&pb.perDisToCur<=0.3)
    {
        bitMaskA|=0x20;
    }
    else if(pb.perDisToCur>0.3&&pa.perDisToCur<=0.3)
    {
        bitMaskB|=0x20;
    }

    if(pa.disToCur<pb.disToCur)
    {
        bitMaskA|=0x10;
    }
    else
    {
        bitMaskB|=0x10;
    }

    if(bitMaskA>=bitMaskB)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

RobotHighParam TwoWayRegion::getParamById(int id)
{
    RobotHighParam p;
    for(int i=0;i<vHighParam.size();i++)
    {
        p=vHighParam.at(i);
        if(p.id==id)
        {
            return p;
        }
    }
    return p;
}

