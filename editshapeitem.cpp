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
