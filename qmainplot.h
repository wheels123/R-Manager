#ifndef QMAINPLOT_H
#define QMAINPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_plot_marker.h>
#include <QVector>
#include <qwt_point_cus.h>
#include <qwt_plot_curve_cus.h>
#include <robot.h>

#include "qcurvedatacus.h"
#include <QHash>
#include <qwt_plot_shapeitem.h>
#include <shapefactory.h>
#include "editshapeitem.h"
#include "robotmath.h"
class QCurveDataCus;
class QwtPlotMarker;
class QwtPointCus;

class QCurveData: public QwtArraySeriesData<QPointF>
{
public:
    QCurveData() {
    }

    virtual QRectF boundingRect() const {
        if (d_boundingRect.width() < 0.0)
            d_boundingRect = qwtBoundingRect(*this);

        return d_boundingRect;
    }

    inline void append(const QPointF &point) {
        d_samples += point;
    }

    void clear() {
        d_samples.clear();
        d_samples.squeeze();
        d_boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    }
};
/*
class QCurveDataCus: public QwtArraySeriesData<QwtPointCus>
{
public:
    QCurveDataCus() {
    }

    virtual QRectF boundingRect() const {
        if (d_boundingRect.width() < 0.0)
            d_boundingRect = qwtBoundingRect(*this);

        return d_boundingRect;
    }

    inline void append(const QwtPointCus &point) {
        d_samples += point;
    }

    void clear() {
        d_samples.clear();
        d_samples.squeeze();
        d_boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    }
};
*/
class QMainPlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit QMainPlot(QWidget *parent);
    ~QMainPlot();

signals:

public slots:

public:
    virtual QSize sizeHint() const;
private:
    inline void initLegend();
    inline void initPlotGrid();
    inline void initPlotCurves();
    inline void initPlotMarkers();
    inline void initDirectPainter();

public:
    enum curveId {
        curveNormalPoints,
        curveTagPoints,
        curveEndPoints,
        curveMax,
    };

public:
    //void appendPoint(curveId id, const QPointF &point,const double angleValue,const int tagNo);
    void appendPoint(QwtPointCus dataAppend);
    void clearPoints(curveId id);
    void setCanvasColor( const QColor &c );
    virtual bool eventFilter( QObject *, QEvent * );
    void insertCurve( Qt::Orientation o,
        const QColor &c, double base );
    void insertCurveConnect(const QColor &c);
    void showPose( int id,double x,double y );
    void loadData(QCurveDataCus *data, QwtPointCus::PointStyle type);
    QCurveDataCus* getNormalData();
    QCurveDataCus* getPathData();
    QCurveDataCus* getDestData();
    QCurveDataCus* getLabelData();
    QCurveData* getNormalData_t();
    void addReceiveDataToLoadData(bool useLabelId);
    void autoConnect(double autoConnectDis,double autoConnectDisMax);
    void showPose(QVector<RobotPoint> vrp,int n);
    inline void addCover(const QPointF &point);
    inline QPainterPath coverPath(const QPointF &point, const QSizeF &size);
    inline uint qHash(const QPointF &point);


    void paintEvent();
    void DrawCustomLine(QPainter& _painter, double _angle, double _len, const QString& _name, const QColor& _color);

    void addShape( const QString &title,
        ShapeFactory::Shape shape, const QColor &color,
        const QPointF &pos, const QSizeF &size );
    void populate();
    void clearItem();
    void insertItemPoint(QPointF p);
    void plotItem();

    void plotItemEnd();
    int shapeItemxAxis();

    int shapeItemyAxis();
    int getNewShapeItemId();
    bool findShapeItemId(int id);
    void addShapeItem(int id,EditShapeItem::ShapeType type);
    QVector<EditShapeItem *> getShapeItemData();

    bool LoadJsonFile(const QString &fileName);
    inline bool parseJsonFile(QJsonDocument &json);
    void deleteShapeItem(int id);
    //bool pointInPolygon(QPointF point,QPolygonF polygon);
    void highLightShapeItem(QPointF p);
    void loadData_t(QCurveDataCus *data);
    void showPose(QVector<QVector<RobotPathPoint>> vvrp);
    void autoAvoidNormalPoint(double autoMoveDisMax);
private slots:
    void onLegendChecked(const QVariant &itemInfo, bool on, int index);

private:
    inline void updateMarkers(curveId id, const QPointF &point);
    inline void showCurve(QwtPlotItem *item, bool on);

private:
    QVector<QwtPlotCurve *> vectorCurve;
    QVector<QwtPlotCurveCus *> vectorCurveCus;
    QwtPlotDirectPainter   *directPainter;
    QwtPlotMarker          *tagMarker;
    QwtPlotMarker          *endMarker;
    QVector<QwtPlotMarker *> d_marker;

    QwtPlotCurveCus * CurveNormal;
    QwtPlotCurveCus * CurvePath;
    QwtPlotCurveCus * CurveDest;
    QwtPlotCurveCus * CurveLabel;
    QwtPlotCurveCus * CurveConnectPoint;
    int pointNum;
    bool markerShow[10];
    QVector<QwtPlotCurve *> vectorCurvePose;
    QVector<QwtPlotCurveCus *> vectorCurveCusPose;
    QHash<QPointF, QwtPlotShapeItem *> mapCover;

    EditShapeItem * shapeItem;
    QVector<EditShapeItem *> shapeItemList;

    QwtPlotCurve * CurveNormal_t;
    RobotMath m_math;
};

#endif // QMAINPLOT_H
