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

    /*
    if(rp.size()==1)
    {
        QVector<RobotPath>().swap(activeRobot);
        activeRobot.append(rp.at(0));
        int index=-1;
        robot->findPathIndexById(robotId,0,index);
        if(ret == false)//new robot all stop
        {
            newRobotNum++;
            rp.erase(rp.begin()+i);
            if(index!=-1&&index<control.size())
            {

            }
        }
        return;//需设为行走
    }
*/

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

    if(rp.size()<=1) //上次活动机器人这次又出现的个数为0或1 则选择的一个为活动
    {
        QVector<RobotPath>().swap(activeRobot);
        if(rp.size()==0)
        {
            RobotPath p = findSafeRobot(rp_no_edit);
            activeRobot.append(p);
        }
#if 0
        else
        {
            bool safe = isRobotSafe(rp_no_edit,rp.at(0));
            if(safe==true)
            {
                activeRobot.append(rp.at(0));
            }
            else
            {
                int index=-1;
                robot->findPathIndexById(rp.at(0).robotId,0,index);

                if(index!=-1&&index<control.size())
                {

                   robot->setRobotControl(index,0);
                   qDebug() << "stop unsafe last valid id "<<QString::number(rp.at(0).robotId,10);
                }
                RobotPath p = findSafeRobot(rp_no_edit);
                qDebug() << "findSafeRobot id "<<QString::number(p.robotId,10);
                activeRobot.append(p);
            }

        }
#endif
        int index=-1;
        robot->findPathIndexById(activeRobot.at(0).robotId,0,index);

        if(index!=-1&&index<control.size())
        {

           robot->setRobotControl(index,1);
           qDebug() << "run first id "<<QString::number(activeRobot.at(0).robotId,10);
        }
        //stop stoped by other region
        qDebug() <<"stop stoped by other region";
            for(int i=0;i<control.size();i++)
            {
                if(control.at(i)==0)
                {
                    qDebug() << "mast stop i "<<QString::number(i,10)<<" id "<<QString::number(robot->getPathRobotIdByIndex(i),10);
                    robot->setRobotControl(i,0);
                }
            }
        qDebug() <<"stop stoped by other region end";
        return;//需设为行走
    }

    //上次活动机器人这次又出现的个数>1

    RobotPath rpFirstAct=rp.at(0);
    QVector<RobotPath> rp_act_no_edit=rp;
    controlRegion.resize(rp.size());

    for(int i=0;i<rp.size();i++)
    {
        controlRegion[i]=1;
    }

    for(int i=0;i<rp.size()-1;i++)
    {
        for(int j=i+1;j<rp.size();j++)
        {
            qDebug() << "OneWayRegion setControl 5 id "<<QString::number(i,10)<<""<<QString::number(j,10);
            RobotPath robotPathi = rp.at(i);
            RobotPoint curPosei = robotPathi.curPose;
            int curIdGoi = robotPathi.pathIdToGo;
            QVector<RobotPathPoint> robotPathNeari = robotPathi.getNearPointById(curIdGoi);
            RobotPoint curDestPointi=robotPathNeari.at(1).point;
            RobotPath::RobotState robotStatei=robotPathi.robotState;
            bool stopi=false;
            int indexi=-1;
            int findIndexi = robot->findPathIndexById(robotPathi.robotId,0,indexi);

            RobotPath robotPathj = rp.at(j);
            RobotPoint curPosej = robotPathj.curPose;
            int curIdGoj = robotPathj.pathIdToGo;
            QVector<RobotPathPoint> robotPathNearj = robotPathj.getNearPointById(curIdGoj);
            RobotPoint curDestPointj=robotPathNearj.at(1).point;
            RobotPath::RobotState robotStatej=robotPathj.robotState;
            bool stopj=false;
            int indexj=-1;
            int findIndexj = robot->findPathIndexById(robotPathj.robotId,0,indexj);

            int front;

            if(robotStatei==RobotPath::AtStartPoint||robotStatei==RobotPath::AtDestPoint)
            {
                stopi=true;
            }
            else
            {
                stopi=false;
            }
            if(robotStatej==RobotPath::AtStartPoint||robotStatej==RobotPath::AtDestPoint)
            {
                stopj=true;
            }
            else
            {
                stopj=false;
            }

            int mode=0;
            if(stopi==false&&stopj==false)
            {
                mode=0;
            }
            else if(stopi==false&&stopj==true)
            {
                mode=1;
            }
            else if(stopi==true&&stopj==false)
            {
                mode=2;
            }
            else
            {
                mode=3;
            }

            if(mode==3) continue; //all stop do not need control


            front = frontRobot(curPosei,curDestPointi,curPosej,curDestPointj,mode);



            //在后面的停 则不用判断mode
           // if(mode==0)//都在行走
            {
                if(front==0)// 方向相反 但不会交叉 无需控制
                {

                }
                else if(front==1)//i 在前 j停
                {
                    if(indexj!=-1&&indexj<control.size())
                    {
                        //control[indexj]=0;
                        robot->setRobotControl(indexj,0);
                        controlRegion[j]=0;
                        qDebug() << "stop id "<<QString::number(robotPathj.robotId,10)<<" front id "<<QString::number(robotPathi.robotId,10);
                    }
                }
                else if(front==2)//j 在前 i停
                {
                    if(indexi!=-1&&indexi<control.size())
                    {
                        //control[indexi]=0;
                        robot->setRobotControl(indexi,0);
                        controlRegion[i]=0;
                        qDebug() << "stop id "<<QString::number(robotPathi.robotId,10)<<" front id "<<QString::number(robotPathj.robotId,10);
                    }
                }
                else if(front==3)//反向 交叉 不允许发生 暂且i停
                {                
                    bool ret = safeRobot(curPosei,curPosej);
                    if(ret==1)
                    {
                        if(indexj!=-1&&indexj<control.size())
                        {
                            //control[indexi]=0;
                            robot->setRobotControl(indexj,0);
                            controlRegion[j]=0;
                            qDebug() << "stop id "<<QString::number(robotPathj.robotId,10)<<" cross with "<<QString::number(robotPathj.robotId,10);
                        }
                    }
                    else
                    {
                        if(indexi!=-1&&indexi<control.size())
                        {
                            //control[indexi]=0;
                            robot->setRobotControl(indexi,0);
                            controlRegion[i]=0;
                            qDebug() << "stop id "<<QString::number(robotPathi.robotId,10)<<" cross with "<<QString::number(robotPathi.robotId,10);
                        }

                    }
                }
            }

        }

    }


    QVector<RobotPath>().swap(activeRobot);
    for(int i=0;i<rp.size();i++)
    {
        if(controlRegion[i]==1)
        {
            activeRobot.append(rp.at(i));
        }
    }

    if(activeRobot.size()==0)
    {
        RobotPath p = findSafeRobot(rp_act_no_edit);
        activeRobot.append(p);
        //activeRobot.append(rpFirstAct);
        int index=-1;
        robot->findPathIndexById(activeRobot.at(0).robotId,0,index);

        if(index!=-1&&index<control.size())
        {
           //control[index]=1;
           robot->setRobotControl(index,1);
           qDebug() << "run first act id "<<QString::number(activeRobot.at(0).robotId,10);
        }
        return;//需设为行走
    }
//stop stoped by other region
qDebug() <<"stop stoped by other region";
    for(int i=0;i<control.size();i++)
    {
        if(control.at(i)==0)
        {
            qDebug() << "mast stop i "<<QString::number(i,10)<<" id "<<QString::number(robot->getPathRobotIdByIndex(i),10);
            robot->setRobotControl(i,0);
        }
    }
qDebug() <<"stop stoped by other region end";

    qDebug() << "Oneway active robot ";
    for(int i=0;i<activeRobot.size();i++)
    {
        qDebug() << "i "<<QString::number(i,10);
        RobotPath rp=activeRobot.at(i);
        qDebug() << "robot id "<<QString::number(rp.robotId,10)<<" "<<QString::number(rp.curPose.x,'f',3)<<" "<<QString::number(rp.curPose.y,'f',3);
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

