#ifndef EDITSHAPEITEM_H
#define EDITSHAPEITEM_H

#include <qwt_plot_shapeitem.h>
#include <robot.h>
class EditShapeItem : public QwtPlotShapeItem
{
public:
    enum ShapeType{OneWay=0,TwoWay,CrossRoad,T_Road};
    EditShapeItem(QString title="edit1");

    void insertPoint(QPointF p);
    void clear();
    QPolygonF getPolygon();
    void init(QColor color);
    void  attachM(QwtPlot *plot);

    void init(QwtPlot *plot ,QColor color,int id,ShapeType type);
    void setShapeItem();
    void setShapeItemEnd();
    void setId(int id);
    void setType(ShapeType type);
    int id();
    ShapeType type();
    bool closed();
    void setClose(bool closed);
    void setAlpha(int a);
    bool pointInPolygon(QPointF point);
    bool segmentsIntr(RobotPoint a, RobotPoint b, RobotPoint c, RobotPoint d,RobotPoint &out);
    bool pointToPolygonDis(RobotPoint pose,double &dis);
    double determinant(double v1, double v2, double v3, double v4);
    bool intersect(RobotPoint aa, RobotPoint bb, RobotPoint cc, RobotPoint dd);
private:
    QPolygonF pointList;
    bool m_closed;
    QwtPlot *plot;
    QColor m_color;
    int m_id;
    ShapeType m_type;
};

#endif // EDITSHAPEITEM_H
