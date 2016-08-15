#include "onewayregion.h"

OneWayRegion::OneWayRegion()
{
    shapeItem=NULL;
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

    for(int i=0;i<robot->getPathNum();i++)
    {
        RobotPath path = robot->getPathByIndex(i);
        RobotPoint point = path.curPose;
        if(shapeItem->pointInPolygon(QPointF(point.x,point.y)))
        {
            robotIn.append(path);
        }
    }

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
       qDebug() << "robotIn i "<<QString::number(m,10)<<"id "<<QString::number(rpm.robotId,10);
    }

    //qDebug() << "setRegion id "<<QString::number(region->id(),10)<<" vectorPath num "<<QString::number(vectorPath.size(),10);
    for(int i=0;i<vectorPath.size();i++)
    {
        //qDebug() << "i "<<QString::number(i,10);
        QVector<QwtPointCus> vp=vectorPath.at(i);
        for(int j=0;j<vp.size();j++)
        {
            QwtPointCus p=vp.at(j);
            //qDebug() << "id "<<QString::number(p.id(),10)<<" "<<QString::number(p.x(),'f',3)<<" "<<QString::number(p.y(),'f',3);
        }
    }
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

    qDebug() << "OneWay setRegion end\n";
}

void OneWayRegion::setControl(Robot *robot)
{
    qDebug() << "OneWayRegion setControl start";
    if(vectorRobot.size()==0) return;

    int robotNum=vectorRobot.at(0).size();
    if(robotNum==0) return;

    QVector<int> control = robot->getRobotControl();
    QVector<int> controlRegion;
    QVector<RobotPath> rp=vectorRobot.at(0);
    QVector<RobotPath> rp_no_edit=vectorRobot.at(0);
    controlRegion.resize(rp.size());



    RobotPath rpFirst=rp.at(0);
    //删除新出现的机器人并设为停止
    int newRobotNum=0;
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
            if(index!=-1&&index<control.size())
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
        if(rp.size()==0)
        {
            RobotPath p = findSafeRobot(rp_no_edit);
            activeRobot.append(p);
        }

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

            if(index!=-1&&index<control.size())
            {

               robot->setRobotControl(index,1);
               qDebug() << "run last robot id "<<QString::number(activeRobot.at(0).robotId,10);
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


    QVector<int> ctrl = robot->getRobotControl();
    qDebug() << "ctrl size "<<QString::number(ctrl.size(),10);
    for(int i=0;i<ctrl.size();i++)
    {
        qDebug() << "ctrl i "<<QString::number(i,10)<<" val "<<QString::number(ctrl.at(i),10);
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

