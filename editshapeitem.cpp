#include "editshapeitem.h"

EditShapeItem::EditShapeItem(QString title)
{
    if(title=="") title="item1";
    this->setTitle(title);
    m_closed=false;
}

void EditShapeItem::insertPoint(QPointF p)
{
    if(m_closed)
    {
        pointList.clear();
        m_closed=false;
    }
    pointList+=p;
}


void EditShapeItem::clear()
{
    pointList.clear();
    m_closed=false;
}

QPolygonF EditShapeItem::getPolygon()
{
    return pointList;
}

void EditShapeItem::init(QwtPlot *plot ,QColor color,int id,ShapeType type)
{
    QColor fillColor = color;
    fillColor.setAlpha( 100 );
    QPen pen( color, 3 );
    pen.setJoinStyle( Qt::MiterJoin );
    this->setPen( pen );
    this->setBrush( fillColor );

    QString str=this->title().text();
    str+=QString::number(id,10);
    this->setTitle(str);
    this->attach(plot);
    this->plot=plot;
    this->m_color=color;
    this->m_id=id;
    this->m_type=type;
}

void EditShapeItem::setAlpha(int a)
{
    QColor fillColor = this->m_color;
    fillColor.setAlpha( a );
    this->setBrush( fillColor );
}

void  EditShapeItem::setShapeItem()
{
    QPainterPath path;
    path.addPolygon(pointList);
    this->setShape( path );
}

void  EditShapeItem::setShapeItemEnd()
{
    QPainterPath path;
    if(pointList.size()<3) return;
    pointList.append(pointList.at(0));
    path.addPolygon(pointList);
    this->setShape( path );
    this->setItemAttribute( QwtPlotItem::Legend, true );
    this->setLegendMode( QwtPlotShapeItem::LegendShape );
    this->setLegendIconSize( QSize( 20, 20 ) );
    this->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    this->detach();
    this->attach(plot);
    m_closed=true;
}

void  EditShapeItem::setId(int id)
{
    this->m_id=id;
}

void  EditShapeItem::setType(ShapeType type)
{
    this->m_type=type;
}

int  EditShapeItem::id()
{
    return this->m_id;
}

EditShapeItem::ShapeType EditShapeItem::type()
{
    return this->m_type;
}

bool  EditShapeItem::closed()
{
    return this->m_closed;
}

void EditShapeItem::setClose(bool closed)
{
    this->m_closed = closed;
}

bool EditShapeItem::pointInPolygon(QPointF point)
{
  QPolygonF& polygon =pointList;
  int  polySides = polygon.size()-1;
  if(polySides<4) return false; //last point is the same as first
  int   i,j=polySides-1 ;
  bool  oddNodes=false ;//0 -- not in z

  float x=point.x();
  float y=point.y();

  for (i=0;i<polySides; i++) {
     // qDebug() << " pointInPolygon "<<QString::number(i,10)<<" "<<QString::number(polygon.at(i).x(),'f',3)<<" "
     //          <<" "<<QString::number(polygon.at(i).y(),'f',3);
    if(((polygon.at(i).y()< y && polygon.at(j).y()>=y)
    ||   (polygon.at(j).y()< y && polygon.at(i).y()>=y))
    && (polygon.at(i).x()<=x || polygon.at(j).x()<=x)) {
      oddNodes^=(polygon.at(i).x()+(y-polygon.at(i).y())/(polygon.at(j).y()-polygon.at(i).y())*(polygon.at(j).x()-polygon.at(i).x())<x);}
    j=i;}

  return oddNodes;
}


bool EditShapeItem::segmentsIntr(RobotPoint a, RobotPoint b, RobotPoint c, RobotPoint d,RobotPoint &out){

    // 三角形abc 面积的2倍
    double area_abc = (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);

    // 三角形abd 面积的2倍
    double area_abd = (a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x);

    // 面积符号相同则两点在线段同侧,不相交 (对点在线段上的情况,本例当作不相交处理);
    if ( area_abc*area_abd>=0 ) {
        return false;
    }

    // 三角形cda 面积的2倍
    double area_cda = (c.x - a.x) * (d.y - a.y) - (c.y - a.y) * (d.x - a.x);
    // 三角形cdb 面积的2倍
    // 注意: 这里有一个小优化.不需要再用公式计算面积,而是通过已知的三个面积加减得出.
    double area_cdb = area_cda + area_abc - area_abd ;
    if (  area_cda * area_cdb >= 0 ) {
        return false;
    }

    //计算交点坐标
    double t = area_cda / ( area_abd- area_abc );
    double dx= t*(b.x - a.x);
    double dy= t*(b.y - a.y);

    out.x=a.x + dx;
    out.y=a.y + dy;
    return true;
}

bool EditShapeItem::pointToPolygonDis(RobotPoint pose,double &dis)
{
  QPolygonF& polygon =pointList;
  RobotPoint aa,bb,cc,dd,off;

  off.x=100;
  off.y=0;
  off.phi=0;
  aa=pose;
  bb=aa+off;



  for(int i=0;i<polygon.size()-1;i++)
  {
      cc.x=polygon.at(i).x();
      cc.y=polygon.at(i).y();
      dd.x=polygon.at(i+1).x();
      dd.y=polygon.at(i+1).y();
      //bool ret = intersect(aa,bb,cc,dd);
      //if(ret==true)
      {
          RobotPoint out;
          bool ret = segmentsIntr(aa,bb,cc,dd,out);
          if(ret)
          {
              double x=aa.x-out.x;
              double y=aa.y-out.y;
              dis=sqrt(x*x+y*y);
              return true;
          } 
      }
  }
  return false;
}


///------------alg 3------------
double EditShapeItem::determinant(double v1, double v2, double v3, double v4)  // 行列式
{
    return (v1*v3-v2*v4);
}

bool EditShapeItem::intersect(RobotPoint aa, RobotPoint bb, RobotPoint cc, RobotPoint dd)
{
    double delta = determinant(bb.x-aa.x, cc.x-dd.x, bb.y-aa.y, cc.y-dd.y);
    if ( delta<=(1e-6) && delta>=-(1e-6) )  // delta=0，表示两线段重合或平行
    {
        return false;
    }
    double namenda = determinant(cc.x-aa.x, cc.x-dd.x, cc.y-aa.y, cc.y-dd.y) / delta;
    if ( namenda>1 || namenda<0 )
    {
        return false;
    }
    double miu = determinant(bb.x-aa.x, cc.x-aa.x, bb.y-aa.y, cc.y-aa.y) / delta;
    if ( miu>1 || miu<0 )
    {
        return false;
    }
    return true;
}
