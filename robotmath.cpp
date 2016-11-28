#include "robotmath.h"

RobotMath::RobotMath()
{

}

double RobotMath::dis(RobotPoint a, RobotPoint b)
{
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}
double RobotMath::perDis(RobotPoint a,RobotPoint b,RobotPoint c)
{
    double A=c.y-b.y;
    double B=-(c.x-b.x);
    double C=(c.x-b.x)*b.y-(c.y-b.y)*b.x;
    double D=sqrt(A*A+B*B);
    double E=fabs(A*a.x+B*b.y+C);
    if(D==0)
    {
        return dis(a,b);
    }
    else
    {
        return E/D;
    }
}

void RobotMath::computeFromEncoder(RobotPoint	&out_incr_odom,
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

}

double RobotMath::estimateMinDis(RobotPoint a,double la,double ra,RobotPoint b,double lb,double rb)
{
    const int n=25;
    const double step=0.1;
    double min_dis=MAX_DIS;
    RobotPoint ea,eb;
    ea=a;
    eb=b;

    for(int i=0;i<n;i++)
    {
        RobotPoint da,db;
        if(fabs(la)+fabs(lb)<0.01)
        {
            da.x=0.05;
            da.y=0;
            da.phi=0;
        }
        else
        {
            computeFromEncoder(da,la,ra);
        }
        if(fabs(lb)+fabs(lb)<0.01)
        {
            db.x=0.05;
            db.y=0;
            db.phi=0;
        }
        else
        {
            computeFromEncoder(db,lb,rb);
        }

        ea=ea+da;
        eb=eb+db;
        double x=ea.x-eb.x;
        double y=ea.y-eb.y;
        double dis = sqrt(x*x+y*y);

        if(dis<min_dis)
        {
            min_dis=dis;
        }
    }
    qDebug() << "a "<<QString::number(a.x,'f',3)<<" "<<QString::number(a.y,'f',3)<<" "<<QString::number(a.phi,'f',3)<<"spd "<<QString::number(la,'f',3)<<" "<<QString::number(ra,'f',3);
    qDebug() << "b "<<QString::number(b.x,'f',3)<<" "<<QString::number(b.y,'f',3)<<" "<<QString::number(b.phi,'f',3)<<"spd "<<QString::number(lb,'f',3)<<" "<<QString::number(rb,'f',3);

    qDebug() << "min_dis"<<QString::number(min_dis,'f',3);
    return min_dis;
}

int RobotMath::safeRobot(RobotPoint a,RobotPoint b)
{
    const int n=20;
    const double step=0.1;
    double min_dis_a2b=MAX_DIS;
    double min_dis_b2a=MAX_DIS;
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
    //qDebug() << "safeRobot "<<QString::number(min_dis_a2b,'f',3)<<" "<<QString::number(min_dis_b2a,'f',3);
    if(min_dis_a2b<min_dis_b2a)
    {
      return 2;
      //qDebug() <<"ret 2";
    }
    else
    {
      return 1;
      //qDebug() <<"ret 1";
    }
}

int RobotMath::safeRobot(RobotPoint a,RobotPoint b,double &da,double &db)
{
    const int n=20;
    const double step=0.1;
    double min_dis_a2b=MAX_DIS;
    double min_dis_b2a=MAX_DIS;
    da=MAX_DIS;
    db=MAX_DIS;
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
    //qDebug() << "safeRobot "<<QString::number(min_dis_a2b,'f',3)<<" "<<QString::number(min_dis_b2a,'f',3);
    da=min_dis_a2b;
    db=min_dis_b2a;
    if(min_dis_a2b<min_dis_b2a)
    {
      return 2;
      //qDebug() <<"ret 2";
    }
    else
    {
      return 1;
      //qDebug() <<"ret 1";
    }
}

double RobotMath::estimateMinDisA2B(RobotPoint a,double la,double ra,RobotPoint b)
{
    const int n=25;
    const double step=0.1;
    double min_dis=MAX_DIS;
    RobotPoint ea,eb;
    ea=a;
    eb=b;

    if(fabs(la)+fabs(ra)<0.01)
    {
        la=50;
        ra=50;
    }
    else if(la<-0.1 && ra<-0.1)
    {
        la=20;
        ra=20;
    }
    for(int i=0;i<n;i++)
    {
        RobotPoint da,db;
        computeFromEncoder(da,la,ra);
        ea=ea+da;
        eb=eb;
        double x=ea.x-eb.x;
        double y=ea.y-eb.y;
        double dis = sqrt(x*x+y*y);

        if(dis<min_dis)
        {
            min_dis=dis;
        }
    }
    //qDebug() << "a "<<QString::number(a.x,'f',3)<<" "<<QString::number(a.y,'f',3)<<" "<<QString::number(a.phi,'f',3)<<"spd "<<QString::number(la,'f',3)<<" "<<QString::number(ra,'f',3);
    //qDebug() << "b "<<QString::number(b.x,'f',3)<<" "<<QString::number(b.y,'f',3)<<" "<<QString::number(b.phi,'f',3)<<"spd "<<QString::number(lb,'f',3)<<" "<<QString::number(rb,'f',3);

    //qDebug() << "min_dis"<<QString::number(min_dis,'f',3);
    return min_dis;
}

double RobotMath::calcuLineRad(RobotPoint destPose2D, RobotPoint curPose2D)
{
    double x,y;
    double ang=0;
    x=destPose2D.x-curPose2D.x;
    y=destPose2D.y-curPose2D.y;
    if(y==0)
    {
        if(x>=0) ang=0;
        else ang=M_PI;
    }
    else if(y>0)
    {
        if(x==0) ang=M_PI/2;
        else if(x>0) ang=atan(y/x);
        else ang=M_PI-atan(-y/x);
    }
    else
    {
        if(x==0) ang=-M_PI/2;
        else if(x>0) ang=-atan(-y/x);
        else ang=atan(y/x)-M_PI;
    }
    return ang;
}

RobotPoint RobotMath::avoidMoveVector(RobotPoint rp, RobotPoint op,double r)
{
    RobotPoint result;

    double rpx=rp.x;
    double rpy=rp.y;
    double opx=op.x;
    double opy=op.y;

    double dis=sqrt((rpx-opx)*(rpx-opx)+(rpy-opy)*(rpy-opy));
    if(dis>=r)
    {
        result.x=0;
        result.y=0;
        result.phi=0;
        return result;
    }
    result.phi = calcuLineRad(rp,op);
    double moveDis=r-dis;
    result.x=moveDis*cos(result.phi);
    result.y=moveDis*sin(result.phi);

    return result;
}

RobotPoint RobotMath::vectorAdd(RobotPoint pa, RobotPoint pb)
{
    RobotPoint result;

    double pax=pa.x;
    double pay=pa.y;
    double pbx=pb.x;
    double pby=pb.y;

    result.x=pax+pbx;
    result.y=pay+pby;

    RobotPoint origin(0,0,0);
    result.phi = calcuLineRad(result,origin);
    return result;
}

double RobotMath::calcuAbsRad(double rad1,double rad2)
{
    double dRad;

    dRad = rad2 - rad1;
    if(dRad > M_PI) {
    return M_PI*2 - dRad;
  } else if (dRad < -M_PI) {
        return M_PI*2 + dRad;
    } else {
    return fabs (dRad);
  }

}

double RobotMath::avoidMoveVectorDis(RobotPoint rp, RobotPoint op,RobotPoint sp,double r)
{
    RobotPoint result;
    double rpx=rp.x;
    double rpy=rp.y;
    double opx=op.x;
    double opy=op.y;
    double dis=sqrt((rpx-opx)*(rpx-opx)+(rpy-opy)*(rpy-opy));
    if(dis>=r)
    {
        result.x=0;
        result.y=0;
        result.phi=0;
        return 0;
    }
    result.phi = calcuLineRad(rp,op);
    double moveDis=r-dis;
    double rad = calcuAbsRad(result.phi,sp.phi);
    double minDis;
    minDis=moveDis/cos(rad);
    return minDis;
}


RobotPoint RobotMath::avoidNewPoint(RobotPoint p, double phi,double dis)
{
    RobotPoint result;
    RobotPoint off(dis,0,0);
    p.phi=phi;
    result = TPose2D_Add(p,off);
    return result;
}

RobotPoint RobotMath::TPose2D_Add(const RobotPoint a,const RobotPoint b)
{
    RobotPoint ret;
    ret.x =	a.x + b.x * cos(a.phi) - b.y * sin(a.phi);
    ret.y = a.y + b.x * sin(a.phi) + b.y * cos(a.phi);
    ret.phi = a.phi + b.phi;
    return ret;
}
