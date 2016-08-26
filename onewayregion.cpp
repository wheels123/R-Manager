#include "onewayregion.h"

OneWayRegion::OneWayRegion()
{
    shapeItem=NULL;
    loopTimes=0;
}

void OneWayRegion::setRegion(EditShapeItem *region, QCurveDataCus *path,Robot *robot)
{
    qDebug() << "OneWay setRegion start";
    QVector<QwtPointCus> pathIn;
    QVector<RobotPath> robotIn;

    qDebug() << "activeRobot start  ";

    for(int j=0;j<activeRobot.size();j++)
    {
        RobotPath p=activeRobot.at(j);
        qDebug() << "id "<<QString::number(p.robotId,10)<<" "<<QString::number(p.curPose.x,'f',3)<<" "<<QString::number(p.curPose.y,'f',3);
    }

    QVector<QVector<QwtPointCus>>().swap(vectorPath);
    QVector<QVector<RobotPath>>().swap(vectorRobot);
    shapeItem = region;

    QCurveDataCus *pathData = path;

    for(int i=0;i<(int)pathData->size();i++)
    {
        QwtPointCus point=pathData->sample(i);
        if(shapeItem->pointInPolygon(QPointF(point.x(),point.y())))
        {
            pathIn.append(point);
        }
    }
/*
    for(int i=0;i<robot->getPathNum();i++)
    {
        RobotPath path = robot->getPathByIndex(i);
        RobotPoint point = path.curPose;
        if(shapeItem->pointInPolygon(QPointF(point.x,point.y)))
        {
            robotIn.append(path);
        }
    }
    */
//
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
            //ret3=shapeItem->pointInPolygon(QPointF(pointList.at(2).point.x,pointList.at(2).point.y));
        }

        if(ret1)
        {
            path.id=1;
        }
        else if(ret2)
        {
            path.id=2;
        }
        else if(ret3)
        {
            path.id=3;
        }
        //qDebug() << "path id "<<QString::number(path.robotId,10)<<"go id "<<QString::number(path.pathIdToGo,10);
        //qDebug() << "pointInPolygon 1 "<<QString::number(ret1,10);
        //qDebug() << "pointInPolygon 2 "<<QString::number(ret2,10)<<"go id "<<QString::number(pointIdList.at(1),10);
        //qDebug() << "pointInPolygon 3 "<<QString::number(ret3,10)<<"go id "<<QString::number(pointIdList.at(2),10);
        if(ret1||ret2||ret3)
        {
            qDebug() << "path id "<<QString::number(path.robotId,10)<<"go id "<<QString::number(path.pathIdToGo,10);
            qDebug() << "pointInPolygon "<<QString::number(ret1,10)<<QString::number(ret2,10)<<QString::number(ret3,10);
            qDebug() << "pose "<<QString::number(point.x,'f',3)<<" "<<QString::number(point.y,'f',3);
            if(pointList.size()>=3)
            {
                qDebug() << "dest 0"<<QString::number(pointList.at(0).point.x,'f',3)<<" "<<QString::number(pointList.at(0).point.y,'f',3);
                qDebug() << "dest 1"<<QString::number(pointList.at(1).point.x,'f',3)<<" "<<QString::number(pointList.at(1).point.y,'f',3);
                qDebug() << "dest 2"<<QString::number(pointList.at(2).point.x,'f',3)<<" "<<QString::number(pointList.at(2).point.y,'f',3);
            }

            robotIn.append(path);
        }
    }
//
    if(pathIn.size()>0)
    {
        vectorPath.append(pathIn);
    }

    if(robotIn.size()>0)
    {
        vectorRobot.append(robotIn);
    }
    else
    {
        qDebug() << "robotIn nothing";
        return;
    }

    qDebug() << "robotIn size "<<QString::number(robotIn.size(),10);
    for(int m=0;m<robotIn.size();m++)
    {
       RobotPath rpm = robotIn.at(m);
       double disM=100;
       bool retM= shapeItem->pointToPolygonDis(rpm.curPose,disM);
       qDebug() << "robotIn i "<<QString::number(m,10)<<"id "<<QString::number(rpm.robotId,10)<<"dis to poly"<<QString::number(disM,'f',3);
    }

    /*
    //qDebug() << "setRegion id "<<QString::number(region->id(),10)<<" vectorPath num "<<QString::number(vectorPath.size(),10);
    for(int i=0;i<vectorPath.size();i++)
    {
        //qDebug() << "i "<<QString::number(i,10);
        QVector<QwtPointCus> vp=vectorPath.at(i);
        for(int j=0;j<vp.size();j++)
        {
            //QwtPointCus p=vp.at(j);
            //qDebug() << "id "<<QString::number(p.id(),10)<<" "<<QString::number(p.x(),'f',3)<<" "<<QString::number(p.y(),'f',3);
        }
    }
    */
    qDebug() << "setRegion id "<<QString::number(region->id(),10)<<" vectorRobot num "<<QString::number(vectorRobot.size(),10);
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

    qDebug() << "OneWay setRegion end";
}

void OneWayRegion::setControl(Robot *robot)
{
    qDebug() << "OneWayRegion setControl start";
    if(vectorRobot.size()<=0) return;

    int robotNum=vectorRobot.at(0).size();
    if(robotNum==0) return;

    QVector<int> controlRegion;
    QVector<RobotPath> rp=vectorRobot.at(0);
    QVector<RobotPath> rp_no_edit=vectorRobot.at(0);
    controlRegion.resize(rp.size());
    QVector<int> control = robot->getRobotControl();
    RobotPath rpFirst=rp.at(0);

/*
    int num_all=rp.size();
    int num_in=0;
    int num_act=activeRobot.size();
    for(int i=0;i<num_all;i++)
    {
        if(rp.at(i).id==1)
        {
            num_in++;
        }
    }

    if(num_in==0)
    {
        qDebug() <<"no robot abs in";
    }
    else
    {
        if(num_act==0)
        {
            QVector<RobotPath>().swap(activeRobot);
        }
        else if(num_act==1)
        {
            bool ret = findRobotById(activeRobot.at(0));
            if(ret==false)
            {
                qDebug() <<"active robot not in delete";
                QVector<RobotPath>().swap(activeRobot);
            }
        }
            bool ok=false;
            RobotPath p = findSafeRobotIn(rp_no_edit,ok);

            if(!ok)
            {
               p = findSafeRobot(rp_no_edit);
               qDebug() <<"never reached no robot abs in";
            }
            activeRobot.append(p);

            int index=-1;
            robot->findPathIndexById(activeRobot.at(0).robotId,0,index);

            if(index!=-1&&index<control.size())
            {

               robot->setRobotControl(index,1);
               qDebug() << "run first safe robot id "<<QString::number(activeRobot.at(0).robotId,10);
            }
            else
            {
                qDebug() <<"robot id not exist";
            }
        }
        else if(num_act==1)
        {
            bool ret = findRobotById(activeRobot.at(0));
            if(ret==false)
            {
                qDebug() <<"active robot not in del";

            }
        }
        else
        {
            qDebug() <<"never reached active robot >1";
        }
    }
    */

    int num_all=rp.size();
    int num_in=0;
    int num_act=activeRobot.size();
    for(int i=0;i<num_all;i++)
    {
        if(rp.at(i).id==1)
        {
            num_in++;
        }
    }


    //删除新出现的机器人并设为停止
    int newRobotNum=0;

    if(num_in==0)
    {
        qDebug() <<"activeRobot in region not exist. clear activeRobot";
        QVector<RobotPath>().swap(activeRobot);
    }
    else if(num_in==1&&activeRobot.size()==1)
    {
        for(int i=0;i<rp.size();i++)
        {
            if(rp.at(i).robotId==activeRobot.at(0).robotId)
            {
                if(rp.at(i).id!=1)
                {
                    qDebug() <<"activeRobot in region but not in. clear activeRobot";
                    QVector<RobotPath>().swap(activeRobot);
                }
            }
        }
    }

    for(int i=0;i<rp.size();i)
    {
        int robotId = rp.at(i).robotId;
        bool ret = findRobotById(robotId);
        int index=-1;
        robot->findPathIndexById(robotId,0,index);
        if(ret == false)//new robot all stop
        {
            newRobotNum++;
            rp.erase(rp.begin()+i);
            if(index!=-1)
            {
               robot->setRobotControl(index,0);
               qDebug() << "stop new id "<<QString::number(robotId,10);
            }
        }
        else
        {
            i++;
        }
    }


    if(rp.size()==0) //上次活动机器人这次又出现的个数为0或1 则选择的一个为活动
    {
        QVector<RobotPath>().swap(activeRobot);

        //RobotPath p = findSafeRobot(rp_no_edit);
        RobotPath p = findSafeRobot(rp_no_edit,robot);

        activeRobot.append(p);

        int index=-1;
        robot->findPathIndexById(activeRobot.at(0).robotId,0,index);

        if(index!=-1)
        {

           robot->setRobotControl(index,1);
           qDebug() << "run first safe robot id "<<QString::number(activeRobot.at(0).robotId,10);
        }
        else
        {
            qDebug() <<"robot id not exist";
        }


    }
    else if(rp.size()==1&&activeRobot.size()==1) //上次活动机器人这次又出现的个数为0或1 则选择的一个为活动
    {
        if(rp.at(0).robotId!=activeRobot.at(0).robotId)
        {
             qDebug() <<"never reached robotid not equa";
        }
        else
        {
            int index=-1;
            robot->findPathIndexById(activeRobot.at(0).robotId,0,index);

            if(index!=-1)
            {

               robot->setRobotControl(index,1);
               qDebug() << "run last robot id "<<QString::number(activeRobot.at(0).robotId,10);

               //if the active robot is not able to move
               bool movableM = robot->checkMovableById(activeRobot.at(0).robotId);
               bool movableN = true;
               if(movableM==false)
               {
                   QVector<RobotPath> robotInAbs;
                   for(int i=0;i<rp_no_edit.size();i++)
                   {
                       if(rp_no_edit.at(i).id==1 && rp_no_edit.at(i).robotId!=activeRobot.at(0).robotId)
                       {
                           robotInAbs.append(rp_no_edit.at(i));
                       }
                   }
                   if(robotInAbs.size()==1)
                   {
                       RobotPoint rpm=activeRobot.at(0).curPose;
                       RobotPoint rpn=robotInAbs.at(0).curPose;
                       movableN = robot->checkMovableById(robotInAbs.at(0).robotId);
                       double disM=100,disN=100;
                       bool retM= shapeItem->pointToPolygonDis(rpm,disM);
                       bool retN= shapeItem->pointToPolygonDis(rpn,disN);
                       qDebug() <<"pointToPolygonDis M robotid"<<QString::number(activeRobot.at(0).robotId,10)<<"ret"<<QString::number(retM,10)<<"dis "<<QString::number(disM,'f',3);
                       qDebug() <<"pointToPolygonDis N robotid"<<QString::number(robotInAbs.at(0).robotId,10)<<"ret"<<QString::number(retN,10)<<"dis "<<QString::number(disN,'f',3);
                       if(retM&&retN&&movableN)
                       {
                           if(disN+0.25<disM && disM>0 && disM <5 && disN>0 && disN <5)
                           {
                                QVector<RobotPath>().swap(activeRobot);
                                activeRobot.append(robotInAbs.at(0));
                                qDebug() <<"activeRobot changed to id "<<QString::number(robotInAbs.at(0).robotId,10);
                           }
                       }
                   }
                   else if(robotInAbs.size()> 1)
                   {
                       qDebug() <<"too many robotInAbs "<<QString::number(robotInAbs.size(),10);
                       QVector<RobotPath>().swap(activeRobot);

                       RobotPath p = findSafeRobot(robotInAbs,robot);

                       activeRobot.append(p);

                       int index=-1;
                       robot->findPathIndexById(activeRobot.at(0).robotId,0,index);

                       if(index!=-1)
                       {

                          robot->setRobotControl(index,1);
                          qDebug() << "run first safe robot id "<<QString::number(activeRobot.at(0).robotId,10);
                       }
                       else
                       {
                           qDebug() <<"robot id not exist";
                       }
                       //qDebug() <<"never reach robotInAbs "<<QString::number(robotInAbs.size(),10);
                   }
               }
            }
            else
            {
                qDebug() <<"robot id not exist";
            }
        }


    }
    else
    {
        //never reached
        qDebug() <<"never reached active robot >1";
    }

//stop stoped by other region
//qDebug() <<"stop stoped by other region";
    for(int i=0;i<control.size();i++)
    {
        if(control.at(i)==0)
        {
            qDebug() << "mast stop i "<<QString::number(i,10)<<" id "<<QString::number(robot->getPathRobotIdByIndex(i),10);
            robot->setRobotControl(i,0);
        }
    }
//qDebug() <<"stop stoped by other region end";

    qDebug() << "Oneway active robot ";
    for(int i=0;i<activeRobot.size();i++)
    {
        qDebug() << "i "<<QString::number(i,10);
        RobotPath rp=activeRobot.at(i);
        qDebug() << "robot id "<<QString::number(rp.robotId,10)<<" "<<QString::number(rp.curPose.x,'f',3)<<" "<<QString::number(rp.curPose.y,'f',3);
    }


    //QVector<int> ctrl = robot->getRobotControl();
    qDebug() << "ctrl size "<<QString::number(robot->getPathNum(),10);
    for(int i=0;i<robot->getPathNum();i++)
    {
        qDebug() << "ctrl i "<<QString::number(i,10)<<" val "<<QString::number(robot->getRobotControl(i),10);
    }

}

int OneWayRegion::frontRobot(RobotPoint a,RobotPoint da, RobotPoint b,RobotPoint db,int mode)
{
    const int n=20;
    const double step=0.1;
    double min_dis_a2b=MAX_DIS;
    double min_dis_b2a=MAX_DIS;
    double rx=a.x-b.x;
    double ry=a.y-b.y;
    double rdis=sqrt(rx*rx+ry*ry);
    double MIN_DIS=safe_dis_to_be_front;
    double pha,phb;
    if(rdis<=0.1) return 0;

    if(rdis<MIN_DIS) rdis=MIN_DIS;

    if(mode==1)
    {
        min_dis_b2a=rdis;
    }

    if(mode==2)
    {
        min_dis_a2b=rdis;
    }

    if(mode==0||mode==1)
    {
        pha=calcuLineRad(da.x,da.y,a.x,a.y);
        a.phi=pha;
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

            if(dis<min_dis_a2b)
            {
                min_dis_a2b=dis;
            }
        }
    }
    if(mode==0||mode==2)
    {
        phb=calcuLineRad(db.x,db.y,b.x,b.y);
        b.phi=phb;

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

            if(dis<min_dis_b2a)
            {
                min_dis_b2a=dis;
            }
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
    return 0;
}

int OneWayRegion::frontRobot(RobotPoint a,RobotPoint b)
{
    const int n=20;
    const double step=0.1;
    double min_dis_a2b=MAX_DIS;
    double min_dis_b2a=MAX_DIS;
    double rx=a.x-b.x;
    double ry=a.y-b.y;
    double rdis=sqrt(rx*rx+ry*ry);
    double MIN_DIS=safe_dis_to_be_front;
    if(rdis<=0.1) return 0;
    if(rdis<MIN_DIS) rdis=MIN_DIS;
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

        if(dis<min_dis_b2a)
        {
            min_dis_b2a=dis;
        }
    }

    if((min_dis_a2b<rdis-0.05) && (min_dis_b2a<rdis-0.05))//相向而行
    {
        if(min_dis_a2b>min_dis_b2a&&min_dis_a2b>MIN_DIS)
        {
            return 1;
        }
        if(min_dis_a2b<min_dis_b2a&&min_dis_b2a>MIN_DIS)
        {
            return 2;
        }
        else
        {
            return 3;
        }

    }
    else if((min_dis_a2b<rdis-0.05) && !(min_dis_b2a<rdis-0.05))//
    {
      return 2;
    }
    else if(!(min_dis_a2b<rdis-0.05) && (min_dis_b2a<rdis-0.05))//
    {
      return 1;
    }
    return 0;
}

int OneWayRegion::safeRobot(RobotPoint a,RobotPoint b)
{
    const int n=20;
    const double step=0.1;
    double min_dis_a2b=MAX_DIS;
    double min_dis_b2a=MAX_DIS;
    double rx=a.x-b.x;
    double ry=a.y-b.y;
    double rdis=sqrt(rx*rx+ry*ry);
    double MIN_DIS=safe_dis_to_be_front;
    if(rdis<=0.1) return 0;
    if(rdis<MIN_DIS) rdis=MIN_DIS;
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

        if(dis<min_dis_b2a)
        {
            min_dis_b2a=dis;
        }
    }

    if(min_dis_a2b<min_dis_b2a)//
    {
      return 2;
    }
    else
    {
      return 1;
    }
}

float OneWayRegion::calcuLineRad(float dx,float dy, float cx, float cy)
{
    float x,y;
    float ang=0;
    x=dx-cx;
    y=dy-cy;
    if(y==0)
    {
        if(x>=0) ang=0;
        else ang=PI_M;
    }
    else if(y>0)
    {
        if(x==0) ang=PI_M/2;
        else if(x>0) ang=atan(y/x);
        else ang=PI_M-atan(-y/x);
    }
    else
    {
        if(x==0) ang=-PI_M/2;
        else if(x>0) ang=-atan(-y/x);
        else ang=atan(y/x)-PI_M;
    }
    return ang;
}

bool OneWayRegion::findRobotById(int id)
{
    for(int i=0;i<activeRobot.size();i++)
    {
        if(activeRobot.at(i).robotId==id)
        {
            return true;
        }
    }
    return false;
}

RobotPath OneWayRegion::findSafeRobot(QVector<RobotPath> vrp,Robot *robot)
{
    RobotPath rp;
    QVector<bool> movable;
    QVector<double> distSort;
    QVector<RobotPath> robotSort;
    qDebug() << "findSafeRobot start";
    rp.robotId=0;
    if(vrp.size()==0) return rp;
    if(vrp.size()==1) {
        return vrp.at(0);
    }

    rp=vrp.at(0);
    movable.resize(vrp.size());
    distSort.resize(vrp.size());
    robotSort.resize(vrp.size());
    for(int i=0;i<distSort.size();i++)
    {
        distSort[i] = MAX_DIS;
        robotSort[i] = vrp.at(i);
    }
    for(int i=0;i<vrp.size();i++)
    {
        double min_dis=MAX_DIS;
        for(int j=0;j<vrp.size();j++)
        {
            if(j==i)
            {
                continue;//不比较自己
            }
            RobotPath rp1=vrp.at(i);
            RobotPath rp2=vrp.at(j);
            RobotPoint p1=rp1.curPose;
            RobotPoint p2=rp2.curPose;
            //bool movable1=valid.at(i);
            //bool movable2=valid.at(j);
            double l1=rp1.leftSpeed;
            double r1=rp1.rightSpeed;

            double dis = m_math.estimateMinDisA2B(p1,l1,r1,p2);
            if(min_dis>dis)
            {
                min_dis=dis;
            }
        }
        distSort[i]=min_dis;
    }
    //qDebug() << "findSafeRobot 2";
    for(int i=0;i<robotSort.size()-1;i++)
    {
        for(int j=i+1;j<robotSort.size();j++)
        {
            if(distSort.at(i)<distSort.at(j)&&distSort.at(j)<MAX_DIS-1)
            {
                double t_dis;
                RobotPath t_rp;
                t_dis=distSort[i];
                distSort[i]=distSort[j];
                distSort[j]=t_dis;

                t_rp=robotSort[i];
                robotSort[i]=robotSort[j];
                robotSort[j]=t_rp;
            }
        }
    }

    for(int i=0;i<robotSort.size();i++)
    {
        double t_dis;
        RobotPath t_rp;
        t_dis=distSort[i];
        t_rp=robotSort[i];
        qDebug() << "robotSort i "<<QString::number(i,10)<< "id "<<QString::number(t_rp.robotId,10)<< "dis "<<QString::number(t_dis,'f',3);
    }

    //qDebug() << "findSafeRobot 3";
    for(int i=0;i<movable.size();i++)
    {
        bool movableM = robot->checkMovableById(robotSort.at(i).robotId);
        movable[i] = movableM;
    }
    //qDebug() << "findSafeRobot 4";
    //sorted dis and robot
    bool findMovableAndIn=false;
    bool findIn=false;
    bool findMovable=false;
    for(int i=0;i<robotSort.size();i++)
    {
        if(movable.at(i)&&robotSort.at(i).id==1)
        {
            findMovableAndIn=true;
            rp=robotSort.at(i); //movable and dis max and in
            qDebug() << "findSafeRobot findMovableAndIn true";
            break;
        }
    }
    //qDebug() << "findSafeRobot 5";
    if(findMovableAndIn==false)
    {
        qDebug() << "findSafeRobot findMovableAndIn false";
        for(int i=0;i<robotSort.size();i++)
        {
            if(robotSort.at(i).id==1)
            {
                findIn=true;
                rp=robotSort.at(i); //movable and dis max
                break;
            }
        }
        if(findIn==false)
        {
            qDebug() << "findSafeRobot findIn false";
            for(int i=0;i<robotSort.size();i++)
            {
                if(movable.at(i))
                {
                    findMovable=true;
                    rp=robotSort.at(i); //movable and dis max
                    break;
                }
            }

            if(findMovable==false)
            {
                qDebug() << "findSafeRobot findMovable false";
                rp=robotSort.at(0);
            }
        }
    }
    qDebug() << "findSafeRobot end";
    return rp;
}

RobotPath OneWayRegion::findSafeRobotIn(QVector<RobotPath> vrp,Robot *robot,bool& ok)
{
    RobotPath rp;
    QVector<bool> valid;
    ok=false;
    if(vrp.size()==1) {
        return vrp.at(0);
    }
    valid.resize(vrp.size());

    for(int i=0;i<vrp.size();i++)
    {
        bool movableM = robot->checkMovableById(vrp.at(i).robotId);
        valid.append(movableM);
    }
    for(int i=0;i<vrp.size();i++)
    {
        for(int j=i+1;j<vrp.size();j++)
        {
             RobotPoint p1=vrp.at(i).curPose;
             RobotPoint p2=vrp.at(j).curPose;
             int ret = safeRobot(p1,p2);
             int id1=vrp.at(i).id;
             int id2=vrp.at(j).id;
             bool valid1=valid.at(i);
             bool valid2=valid.at(j);
             if(id1==1&&id2==1)
             {
                 if(valid1&&!valid2)
                 {
                    rp=vrp.at(i);ok=true;
                 }
                 else if(!valid1&&valid2)
                 {
                    rp=vrp.at(j);ok=true;
                 }
                 else if(valid1&&valid2)
                 {
                     int ret = safeRobot(p1,p2);
                     if(ret==2)
                     {
                        rp=vrp.at(j);
                        valid[i]=false;
                     }
                     else
                     {
                        rp=vrp.at(i);
                        valid[j]=false;
                     }
                     ok=true;
                 }
                 else
                 {

                 }
             }
             else if(id1==1&&id2!=1)
             {
                 rp=vrp.at(i);
                 valid[j]=false;
                 ok=true;
             }
             else if(id1!=1&&id2==1)
             {
                 rp=vrp.at(j);
                 valid[i]=false;
                 ok=true;
             }
             else
             {
                 ok=false;
             }
        }
    }
    return rp;
}
/*

RobotPath OneWayRegion::findSafeRobot(QVector<RobotPath> vrp)
{
    RobotPath rp;
    if(vrp.size()==1) {
        return vrp.at(0);
    }
    for(int i=0;i<vrp.size();i++)
    {
        for(int j=i+1;j<vrp.size();j++)
        {
             RobotPoint p1=vrp.at(i).curPose;
             RobotPoint p2=vrp.at(j).curPose;
             int ret = safeRobot(p1,p2);
             if(ret==2)
             {
                rp=vrp.at(j);
             }
             else
             {
                rp=vrp.at(i);
             }
        }
    }
    return rp;
}
*/
RobotPath OneWayRegion::findSafeRobotIn(QVector<RobotPath> vrp,bool& ok)
{
    RobotPath rp;
    ok=false;
    if(vrp.size()==1) {
        return vrp.at(0);
    }
    for(int i=0;i<vrp.size();i++)
    {
        for(int j=i+1;j<vrp.size();j++)
        {
             RobotPoint p1=vrp.at(i).curPose;
             RobotPoint p2=vrp.at(j).curPose;
             int ret = safeRobot(p1,p2);
             int id1=vrp.at(i).id;
             int id2=vrp.at(j).id;

             if(id1==1&&id2==1)
             {
                 if(ret==2)
                 {
                    rp=vrp.at(j);
                 }
                 else
                 {
                    rp=vrp.at(i);
                 }
                 ok=true;
             }
             else if(id1==1&&id2!=1)
             {
                 rp=vrp.at(i);
                 ok=true;
             }
             else if(id1!=1&&id2==1)
             {
                 rp=vrp.at(j);
                 ok=true;
             }
             else
             {
                 ok=false;
             }
        }
    }
    return rp;
}

int OneWayRegion::getRegionId()
{
    if(shapeItem!=NULL)
    {
        return shapeItem->id();
    }
    return -1;
}

bool OneWayRegion::isRobotSafe(QVector<RobotPath> vrp,RobotPath rp)
{
    if(vrp.size()==1) {
        return true;
    }
    for(int i=0;i<vrp.size();i++)
    {
         if(vrp.at(i).robotId==rp.robotId)
         {
            continue;
         }
         RobotPoint p1=vrp.at(i).curPose;
         RobotPoint p2=rp.curPose;
         int ret = frontRobot(p1,p2);
         if(ret==1||ret==3)
         {
            return false;
         }
    }
    return true;
}

