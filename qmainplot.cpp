#include "qmainplot.h"
#include <qwt_plot_layout.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_magnifier.h>
#include <qwt_symbol.h>
#include <qwt_painter.h>
#include <qwt_legend.h>
#include <QDebug>
#include <qevent.h>
#include <qwt_plot_canvas.h>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QMainPlot::QMainPlot(QWidget *parent):
    QwtPlot(parent)
{
    // Align cavas into scale
    auto layout = plotLayout();
    layout->setAlignCanvasToScales(true);

    initLegend();

    //
    initPlotGrid();

    const double xMin = -50.0;
    const double xMax = 50.0;
    const double yMin = -50.0;
    const double yMax = 50.0;
    pointNum=0;
    setAxisScale(xBottom, xMin, xMax);
    setAxisScale(yLeft, yMin, yMax);

    //
    setCanvasBackground(Qt::white);

    initPlotCurves();

    initPlotMarkers();

    initDirectPainter();

    setAutoReplot(false);

    // panning with the left mouse button
    (void )new QwtPlotPanner(canvas());

    // zoom in/out with the wheel
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(canvas());
    magnifier->setMouseButton( Qt::NoButton );
    //
    setCanvasColor( Qt::darkCyan );

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setMajorPen( Qt::white, 0, Qt::DotLine );
    grid->attach( this );

    // marker
    QwtPlotMarker *d_marker1;
    d_marker1 = new QwtPlotMarker();
    d_marker1->setValue( 0.0, 0.0 );
    d_marker1->setLineStyle( QwtPlotMarker::NoLine );
    d_marker1->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    d_marker1->setLinePen( Qt::green, 0, Qt::DashDotLine );
    d_marker1->attach( this );
    d_marker<<d_marker1;
    //
    //insertCurve( Qt::Vertical, Qt::blue, 30.0 );
    //insertCurve( Qt::Vertical, Qt::magenta, 70.0 );
    //insertCurve( Qt::Horizontal, Qt::red, 30.0 );
    //insertCurve( Qt::Horizontal, Qt::green, 70.0 );
    //insertCurve2(Qt::blue);
    insertCurveConnect(Qt::red);


    for(int i=0;i<10;i++)
    {
        markerShow[i]=false;
    }

/*
    QwtPlotCurve *curveCus1=new QwtPlotCurve("P1");
    curveCus1->setVisible(true);
    curveCus1->setData(new QCurveData());
    curveCus1->setStyle(QwtPlotCurve::NoCurve);
    curveCus1->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                   Qt::NoBrush,
                                   QPen(Qt::red),
                                   QSize(5, 5)));
    curveCus1->attach(this);
    vectorCurvePose<<curveCus1;
    //delete(curveCus1);

    QwtPlotCurve *curveCus2=new QwtPlotCurve("P2");
    curveCus2->setVisible(true);
    curveCus2->setData(new QCurveData());
    curveCus2->setStyle(QwtPlotCurve::NoCurve);
    curveCus2->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                   Qt::NoBrush,
                                   QPen(Qt::green),
                                   QSize(5, 5)));
    curveCus2->attach(this);
    vectorCurvePose<<curveCus2;
    //delete(curveCus1);

    QwtPlotCurve *curveCus3=new QwtPlotCurve("P3");
    curveCus3->setVisible(true);
    curveCus3->setData(new QCurveData());
    curveCus3->setStyle(QwtPlotCurve::Lines);
    curveCus3->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                   Qt::NoBrush,
                                   QPen(Qt::yellow),
                                   QSize(5, 5)));
    curveCus3->attach(this);
    vectorCurvePose<<curveCus3;
    */
    //delete(curveCus1);


    //EditShapeItem *newShapeItem = new EditShapeItem(QString("Item"));
    //newShapeItem->init(this,"RoyalBlue",1,EditShapeItem::OneWay);
    //shapeItemList.append(shapeItem);
    shapeItem=NULL;
    replot();
}

QMainPlot::~QMainPlot()
{
    foreach(auto curve, vectorCurve) {
        delete curve;
    }

    delete CurveDest;
    delete CurveNormal;
    delete CurveLabel;
    delete CurvePath;
    delete CurveNormal_t;

}
void QMainPlot::setCanvasColor( const QColor &c )
{
    setCanvasBackground( c );
    replot();
}
QSize QMainPlot::sizeHint() const
{
    return QSize(800, 600);
}

inline void QMainPlot::initLegend()
{
    QwtLegend *legend = new QwtLegend(this);
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    insertLegend(legend, QwtPlot::BottomLegend);

    connect(legend,
            &QwtLegend::checked,
            this,
            &QMainPlot::onLegendChecked);
}

inline void QMainPlot::initPlotGrid()
{
    //
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
    grid->attach(this);
}

inline void QMainPlot::initPlotCurves(void)
{
    ////////////////////////////////////
    /// \brief curveCus
    ///
    ///

    CurveNormal_t = new QwtPlotCurve("Nomal_t");
    CurveNormal_t->setVisible(false);
    CurveNormal_t->setData(new QCurveData());
    CurveNormal_t->setStyle(QwtPlotCurve::NoCurve);
    CurveNormal_t->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                   Qt::NoBrush,
                                   QPen(Qt::red),
                                   QSize(4, 4)));
    CurveNormal_t->attach(this);

    CurveNormal = new QwtPlotCurveCus("Nomal");
    CurveNormal->setVisible(false);
    CurveNormal->setData(new QCurveDataCus());
    CurveNormal->setStyle(QwtPlotCurveCus::NoCurve);
    CurveNormal->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                   Qt::NoBrush,
                                   QPen(Qt::gray),
                                   QSize(4, 4)));
    CurveNormal->attach(this);
//////
    CurveDest = new QwtPlotCurveCus("Dest");
    CurveDest->setVisible(true);
    CurveDest->setData(new QCurveDataCus());
    CurveDest->setStyle(QwtPlotCurveCus::NoCurve);
    CurveDest->setSymbol(new QwtSymbol(QwtSymbol::Star1,
                                   Qt::NoBrush,
                                   QPen(Qt::red),
                                   QSize(8, 8)));
    CurveDest->attach(this);
///////
    CurveLabel = new QwtPlotCurveCus("Label");
    CurveLabel->setVisible(false);
    CurveLabel->setData(new QCurveDataCus());
    CurveLabel->setStyle(QwtPlotCurveCus::NoCurve);
    CurveLabel->setSymbol(new QwtSymbol(QwtSymbol::Rect,
                                   Qt::NoBrush,
                                   QPen(Qt::blue),
                                   QSize(6, 6)));
    CurveLabel->attach(this);
//////
    CurvePath = new QwtPlotCurveCus("Path");
    CurvePath->setVisible(true);
    CurvePath->setData(new QCurveDataCus());
    CurvePath->setStyle(QwtPlotCurveCus::NetWorkLines);
    CurvePath->setSymbol(new QwtSymbol(QwtSymbol::RTriangle,
                                   Qt::NoBrush,
                                   QPen(Qt::blue),
                                   QSize(8, 8)));
    CurvePath->attach(this);
//////

    CurveConnectPoint = new QwtPlotCurveCus("connectPoint");
    CurveConnectPoint->setVisible(false);
    CurveConnectPoint->setData(new QCurveDataCus());
    CurveConnectPoint->setStyle(QwtPlotCurveCus::Lines);
    CurveConnectPoint->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                   Qt::NoBrush,
                                   QPen(Qt::red),
                                   QSize(14, 14)));
    CurveConnectPoint->attach(this);

}

inline void QMainPlot::initPlotMarkers()
{
    tagMarker = new QwtPlotMarker();
    tagMarker->setValue(0.0, 0.0);
    tagMarker->setVisible(false);
    tagMarker->setLineStyle(QwtPlotMarker::NoLine);
    tagMarker->setSymbol(new QwtSymbol(QwtSymbol::Rect,
                                       Qt::NoBrush,
                                       QPen(Qt::blue),
                                       QSize(8, 8)));
    tagMarker->attach(this);

    endMarker = new QwtPlotMarker();
    endMarker->setValue(0.0, 0.0);
    endMarker->setVisible(false);
    endMarker->setLineStyle(QwtPlotMarker::NoLine);
    endMarker->setSymbol(new QwtSymbol(QwtSymbol::Rect,
                                       Qt::NoBrush,
                                       QPen(Qt::green),
                                       QSize(8, 8)));
    endMarker->attach(this);
}

void QMainPlot::initDirectPainter()
{
    directPainter = new QwtPlotDirectPainter(this);

    if (QwtPainter::isX11GraphicsSystem())
    {
#if QT_VERSION < 0x050000
        canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
#endif
        canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
    }
}

void QMainPlot::updateMarkers(curveId id, const QPointF &point)
{
    switch (id) {
    case curveNormalPoints:
        break;

    case curveTagPoints:
        tagMarker->setValue(point);

        replot();
        break;

    case curveEndPoints:
        endMarker->setValue(point);

        replot();
        break;

    default:
        break;
    }
}

void QMainPlot::addReceiveDataToLoadData(bool useLabelId)
{
    QwtPlotCurveCus* curveReceive = CurveLabel;
    QwtPlotCurveCus* curveLoad = CurvePath;
    int receivePointNum=curveReceive->dataSize();
    QCurveDataCus *rDataCus = static_cast<QCurveDataCus *>(curveReceive->data());
    QCurveDataCus *nDataCus = static_cast<QCurveDataCus *>(curveLoad->data());
    int validIndex=1;
    for(int i=0;i<receivePointNum;i++)
    {
        QwtPointCus point=rDataCus->sample(i);
        point.rtype()=QwtPointCus::PathPoint;
        if(point.id()>100000) continue;
        if(useLabelId==false)
        {
           point.setId(validIndex++);
        }
        nDataCus->append(point);

        QString label;
        label.sprintf( "P[%.2f,%.2f,%.3f,%d]", point.x(),point.y(),point.phi(),point.id());
        QwtText text( label );
        text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
        text.setColor( Qt::green );
        QwtPlotMarker *d_marker1;
        d_marker1 = new QwtPlotMarker();
        d_marker1->setType(QwtPointCus::PathPoint);
        d_marker1->setValue( point.x(),point.y());
        if(useLabelId==true)
        {
           d_marker1->setId(point.id());
        }
        else
        {
           d_marker1->setId(i+1);
        }

        d_marker1->setLineStyle( QwtPlotMarker::NoLine );
        d_marker1->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
        d_marker1->setLinePen( Qt::green, 0, Qt::DashDotLine );
        d_marker1->setLabel( text );
        d_marker1->attach( this );
        d_marker<<d_marker1;

    }
    curveLoad->setSamples( nDataCus );
    curveLoad->attach( this );
}
/*
inline void QwtPointCus::deleteNet( int net )
{
    int i,j,delId=-1;
    for( i=0;i<SIZE;)
    {
        if(d_net[i]==net)
        {
            d_net[i]=0;
            delId=i;
            for( j=i;j<SIZE-1;j++)
            {
                d_net[j]=d_net[j+1];
            }
            if(delId!=-1&&delId<d_netId&&d_netId>0)
            {
                d_netId-=1;
            }
        }
        else
        {
            i++;
        }
    }

}
*/
void QMainPlot::autoConnect(double autoConnectDisMin,double autoConnectDisMax)
{

    QwtPlotCurveCus* curveLoad = CurvePath;
    int loadPointNum=curveLoad->dataSize();

    QCurveDataCus *nDataCus = static_cast<QCurveDataCus *>(curveLoad->data());

    /*
    int SIZE=10;
    int d_net[10];
    int i,j,delId=-1;
    int net = 260;
    d_net[0]=270;
    d_net[1]=202;
    d_net[2]=272;
    d_net[3]=288;
    d_net[4]=274;
    d_net[5]=276;
    d_net[6]=269;
    d_net[7]=1234;
    d_net[8]=260;
    d_net[9]=260;
    int d_netId=0;


    for( i=0;i<SIZE;)
    {
        if(d_net[i]==net)
        {
            d_net[i]=0;
            delId=i;
            for( j=i;j<SIZE-1;j++)
            {
                d_net[j]=d_net[j+1];
            }
            d_net[SIZE-1]=0;
            if(delId!=-1&&delId<d_netId&&d_netId>0)
            {
                d_netId-=1;
            }
        }
        else
        {
            i++;
        }
    }
*/

    /*
    for(int i=0;i<loadPointNum;i++)
    {
        for(int j=i+1;j<loadPointNum;j++)
        {
            QwtPointCus pointi=nDataCus->sample(i);
            QwtPointCus pointj=nDataCus->sample(j);

            double xDelta = pointi.x()-pointj.x();
            double yDelta = pointi.y()-pointj.y();
            double dis=sqrt(xDelta*xDelta+yDelta*yDelta);
            if(dis>=autoConnectDisMin&&dis<=autoConnectDisMax)
            {
                pointi.insertNet(pointj.id());
                pointj.insertNet(pointi.id());
                nDataCus->set(i,pointi);
                nDataCus->set(j,pointj);
            }
        }
    }

    for(int i=0;i<loadPointNum;i++)
    {
        for(int j=0;j<loadPointNum;j++)
        {
            QwtPointCus pointi=nDataCus->sample(i);
            QwtPointCus pointj=nDataCus->sample(j);

            double xDelta = pointi.x()-pointj.x();
            double yDelta = pointi.y()-pointj.y();
            double dis=sqrt(xDelta*xDelta+yDelta*yDelta);
            if(dis>=autoConnectDisMin&&dis<=autoConnectDisMax)
            {

            }
            else
            {
                pointi.deleteNet(pointj.id());
                pointj.deleteNet(pointi.id());
                nDataCus->set(i,pointi);
                nDataCus->set(j,pointj);
            }
        }
    }
*/

    for(int i=0;i<loadPointNum;i++)
    {
        QwtPointCus pointi=nDataCus->sample(i);
        for(int n=0;n<pointi.netSize();n++)
        {
            pointi.setNet(n,0);
            nDataCus->set(i,pointi);
        }
    }
    for(int i=0;i<loadPointNum;i++)
    {
        for(int j=i+1;j<loadPointNum;j++)
        {
            QwtPointCus pointi=nDataCus->sample(i);
            QwtPointCus pointj=nDataCus->sample(j);

            double xDelta = pointi.x()-pointj.x();
            double yDelta = pointi.y()-pointj.y();
            double dis=sqrt(xDelta*xDelta+yDelta*yDelta);
            if(dis>=autoConnectDisMin&&dis<=autoConnectDisMax)
            {
                pointi.insertNet(pointj.id());
                pointj.insertNet(pointi.id());
                nDataCus->set(i,pointi);
                nDataCus->set(j,pointj);
            }
        }
    }
    curveLoad->setSamples( nDataCus );
    curveLoad->attach( this );

    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );
}

//void QMainPlot::appendPoint(curveId id, const QPointF &point,const double angleValue,const int tagNo)
void QMainPlot::appendPoint(QwtPointCus dataAppend)
{
     qDebug()<<"QMainPlot appendPoint";
    QwtPlotCurveCus *curveCus;
    if(dataAppend.type()==QwtPointCus::Normal)
    {
        curveCus=CurveNormal;
    }
    else if(dataAppend.type()==QwtPointCus::Label)
    {
        curveCus=CurveLabel;
    }
    else if(dataAppend.type()==QwtPointCus::Dest)
    {
        curveCus=CurveDest;
    }
    else if(dataAppend.type()==QwtPointCus::PathPoint)
    {
        curveCus=CurvePath;
    }
    else
    {
        return;
    }


    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(curveCus->data());

    QwtPointCus pointCus=dataAppend;

    dataCus->append(pointCus);

    QPointF point;
    point.rx()=pointCus.x();
    point.ry()=pointCus.y();

    if (!curveCus->isVisible()) {
        curveCus->setVisible(true);
    }

    const bool doClip = !canvas()->testAttribute(Qt::WA_PaintOnScreen);
    if (doClip)
    {
        /*
           Depending on the platform setting a clip might be an important
           performance issue. F.e. for Qt Embedded this reduces the
           part of the backing store that has to be copied out - maybe
           to an unaccelerated frame buffer device.
         */
        const QwtScaleMap xMap = canvasMap(curveCus->xAxis());
        const QwtScaleMap yMap = canvasMap(curveCus->yAxis());

        QRegion clipRegion;

        const QSize symbolSize = curveCus->symbol()->size();
        QRect r(0, 0, symbolSize.width() + 2, symbolSize.height() + 2);

        const QPointF center =
                QwtScaleMap::transform(xMap, yMap, point);
        r.moveCenter(center.toPoint());

        clipRegion += r;

        directPainter->setClipRegion(clipRegion);
    }

    directPainter->drawSeries(curveCus, 0, dataCus->size()-1);
    curveCus->setSamples(curveCus->data());
    curveCus->attach( this );

    if((dataAppend.type()==QwtPointCus::Label)||(dataAppend.type()==QwtPointCus::Dest)||(dataAppend.type()<=QwtPointCus::PathPoint))
    {
        QString label;

        if(dataAppend.type()==QwtPointCus::Label)
        {
            label.sprintf( "L[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
        }
        else if(dataAppend.type()==QwtPointCus::Dest)
        {
            label.sprintf( "D[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
        }
        else if(dataAppend.type()==QwtPointCus::PathPoint)
        {
            label.sprintf( "P[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
        }
        QwtText text( label );
        text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
        if(dataAppend.id()<100000)
        {
           text.setColor( Qt::green );
        }
        else
        {
           text.setColor( Qt::red );
        };

        QwtPlotMarker *d_marker1;
        d_marker1 = new QwtPlotMarker();

        d_marker1->setType(pointCus.type());
        d_marker1->setValue( pointCus.x(),pointCus.y());
        d_marker1->setId(pointCus.id());
        d_marker1->setLineStyle( QwtPlotMarker::NoLine );
        d_marker1->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
        d_marker1->setLinePen( Qt::green, 0, Qt::DashDotLine );
        d_marker1->setLabel( text );
        d_marker1->attach( this );
        d_marker1->setVisible(true);
        d_marker<<d_marker1;

        QwtPlotCanvas *plotCanvas =
            qobject_cast<QwtPlotCanvas *>( canvas() );

        plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
        replot();
        plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

    }
}

void QMainPlot::clearPoints(curveId id)
{
    /*
    auto curve = vectorCurve.at(id);

    QCurveData *data = static_cast<QCurveData *>(curve->data());

    data->clear();

    replot();*/
    int a=0;
    a=id;
}

inline void QMainPlot::showCurve(QwtPlotItem *item, bool on)
{
    QwtPointCus::PointStyle type=QwtPointCus::Normal;
    item->setVisible(on);

    if(item->rtti()==QwtPlotItem::Rtti_PlotCurveCus)
    {
         QwtPlotCurveCus *c = static_cast<QwtPlotCurveCus *>( item );
         if(c->dataSize()>0)
         {
            if(c->sample(0).type()>=QwtPointCus::Label&&c->sample(0).type()<=QwtPointCus::PathPoint)
            {
                type=c->sample(0).type();
            }
         }
    }
    if(type!=QwtPointCus::Normal)
    {
        const QwtPlotItemList& itmList = itemList();

        if(on==true)
        {
            markerShow[(int)type]=!markerShow[(int)type];
        }
        for ( QwtPlotItemIterator it = itmList.begin();
            it != itmList.end(); ++it )
        {
            if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotMarker )
            {
                QwtPlotMarker *c = static_cast<QwtPlotMarker *>( *it );

                if(c->getType()==type)
                {
                    if(on==true)
                    {
                        c->setVisible(markerShow[(int)type]);
                    }
                    else
                    {
                        c->setVisible(false);
                    }
                }
            }
        }
    }
    /*
    // update markers
    if(item == vectorCurve.at(curveTagPoints)) {
        tagMarker->setVisible(on);
    } else if (item == vectorCurve.at(curveEndPoints)) {
        endMarker->setVisible(on);
    }
*/
    //replot();

    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );
}

void QMainPlot::onLegendChecked(const QVariant &itemInfo, bool on, int index)
{
    Q_UNUSED(index);

    QwtPlotItem *plotItem = infoToItem( itemInfo );
    if (plotItem) {
        showCurve(plotItem, on);
    }
}

void QMainPlot::insertCurve( Qt::Orientation o,
    const QColor &c, double base )
{
    QwtPlotCurve *curve = new QwtPlotCurve();

    curve->setPen( c );
    curve->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,
        Qt::gray, c, QSize( 8, 8 ) ) );

    double x[10];
    double y[sizeof( x ) / sizeof( x[0] )];

    for ( uint i = 0; i < sizeof( x ) / sizeof( x[0] ); i++ )
    {
        double v = 5.0 + i * 10.0;
        if ( o == Qt::Horizontal )
        {
            x[i] = v;
            y[i] = base;
        }
        else
        {
            x[i] = base;
            y[i] = v;
        }
    }

    curve->setSamples( x, y, sizeof( x ) / sizeof( x[0] ) );
    curve->attach( this );
}

QCurveData* QMainPlot::getNormalData_t()
{
    QCurveData *dataCus = static_cast<QCurveData *>(CurveNormal_t->data());
    return dataCus;
}

QCurveDataCus* QMainPlot::getNormalData()
{
    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(CurveNormal->data());
    return dataCus;
}

QCurveDataCus* QMainPlot::getPathData()
{
    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(CurvePath->data());
    return dataCus;
}

QCurveDataCus* QMainPlot::getDestData()
{
    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(CurveDest->data());
    return dataCus;
}

QCurveDataCus* QMainPlot::getLabelData()
{
    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(CurveLabel->data());
    return dataCus;
}

void QMainPlot::insertCurveConnect(const QColor &c)
{

    QwtPlotCurveCus *curveCus = CurveConnectPoint;
    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(curveCus->data());

    QwtPointCus pointCus;
    for(int i=0;i<2;i++)
    {
        pointCus.setType(QwtPointCus::Connect);
        pointCus.setId(i);
        pointCus.setX(0);
        pointCus.setY(0);
        pointCus.setPhi(0);
        dataCus->append(pointCus);
    }

    curveCus->setSamples( dataCus );
    curveCus->attach( this );
}

bool QMainPlot::eventFilter( QObject *object, QEvent *e )
{
    if ( e->type() == QEvent::Resize )
    {
        QSize size= this->size();

        if(0)
        if(size.height()>0&&size.width()>0)
        {
            double xMin = -100;
            double xMax = 100;
            double yMin = xMin*size.height()/size.width();
            double yMax = xMax*size.height()/size.width();
            setAxisScale(xBottom, xMin, xMax);
            setAxisScale(yLeft, yMin, yMax);
        }

    }

    return QwtPlot::eventFilter( object, e );
}

void QMainPlot::showPose( int id,double x,double y )
{
    int size=d_marker.size();
    if(id>size-1) return;
    QString label;
    label.sprintf( "[%.0f,%.0f]", x,y );

    QwtText text( label );
    text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
    text.setColor( Qt::green );
    auto marker = d_marker.at(id);
    marker->setValue( x, y );
    marker->setId(id);
    marker->setLabel( text );
}

void QMainPlot::loadData(QCurveDataCus *data,QwtPointCus::PointStyle type)
{
    if(data==NULL) return;
    int sizeLoad=data->size();
    if(sizeLoad<1) return;

    QwtPlotCurveCus *curveCus=CurveLabel;
    if(type==QwtPointCus::Label)
    {
       curveCus = CurveLabel;
    }
    else if(type==QwtPointCus::Dest)
    {
       curveCus = CurveDest;
    }
    else if(type==QwtPointCus::PathPoint)
    {
       curveCus = CurvePath;
    }
    else if(type==QwtPointCus::Normal)
    {
       curveCus = CurveNormal;
    }
    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(curveCus->data());
    int size=dataCus->size();

    int sizeLoadTotalNeed=0;//=sizeLoad*sizeLoad+sizeLoad;


    sizeLoadTotalNeed=sizeLoad;

    int numNeedAdd=0;
    if(sizeLoadTotalNeed<=size)
    {
      numNeedAdd=0;
    }
    else
    {
      numNeedAdd=sizeLoadTotalNeed-size;
    }

    QwtPointCus pointCus(QwtPointCus::PointStyle::Label,0,0,0,0);
    for(int i=0;i<numNeedAdd;i++)
    {       
        dataCus->append(pointCus);
    }

    for(int i=0;i<sizeLoad;i++)
    {
        pointCus=data->sample(i);
        dataCus->set(i,pointCus);
    }

    curveCus->setSamples( dataCus );
    curveCus->attach( this );

    if(type==QwtPointCus::Label||type==QwtPointCus::Dest||type==QwtPointCus::PathPoint)
    {
        for(int i=0;i<sizeLoad;i++)
        {
            QString label;
            pointCus=dataCus->sample(i);
            if(type==QwtPointCus::Label)
            {
                label.sprintf( "L[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
            }
            else if(type==QwtPointCus::Dest)
            {
                 label.sprintf( "D[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
            }
            else if(type==QwtPointCus::PathPoint)
            {
                 label.sprintf( "P[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
            }
            QwtText text( label );
            text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
            if(pointCus.id()<100000)
            {
                text.setColor( Qt::green );
            }
            else
            {
               text.setColor( Qt::red );
            }


            QwtPlotMarker *d_marker1;
            d_marker1 = new QwtPlotMarker();
            d_marker1->setValue( pointCus.x(),pointCus.y());
            d_marker1->setType(pointCus.type());
            d_marker1->setId(pointCus.id());
            d_marker1->setLineStyle( QwtPlotMarker::NoLine );
            d_marker1->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
            d_marker1->setLinePen( Qt::green, 0, Qt::DashDotLine );
            d_marker1->setLabel( text );
            d_marker1->attach( this );
            d_marker1->setVisible(false);
            d_marker<<d_marker1;
        }
    }
}

void QMainPlot::loadData_t(QCurveDataCus *data)
{
    if(data==NULL) return;
    int sizeLoad=data->size();
    if(sizeLoad<1) return;

    QwtPlotCurve *curve=NULL;

    curve = CurveNormal_t;

    QCurveData *data_t = static_cast<QCurveData *>(curve->data());
    int size=data->size();

    int sizeLoadTotalNeed=0;//=sizeLoad*sizeLoad+sizeLoad;


    sizeLoadTotalNeed=sizeLoad;

    int numNeedAdd=0;
    if(sizeLoadTotalNeed<=size)
    {
      numNeedAdd=0;
    }
    else
    {
      numNeedAdd=sizeLoadTotalNeed-size;
    }

    QwtPointCus pointCus(QwtPointCus::PointStyle::Label,0,0,0,0);
    QPointF point;
    /*
    for(int i=0;i<numNeedAdd;i++)
    {
        data_t->append(point);
    }
*/
    for(int i=0;i<sizeLoad;i++)
    {
        pointCus=data->sample(i);
        point.rx()=pointCus.x();
        point.ry()=pointCus.y();
        //data_t->set(i,point);
        data_t->append(point);
    }

    curve->setSamples( data_t );
    curve->attach( this );
}

void QMainPlot::showPose(QVector<RobotPoint> vrp,int n)
{
    if(n>=vectorCurvePose.size()) return;
    auto curve = vectorCurvePose.at(n);
    QPointF point;

    if(vrp.size()<=0) return;
    QwtPlotCurve *curveCus=curve;
    QCurveData *dataCus = static_cast<QCurveData *>(curveCus->data());

    //qDebug() <<"showPose "<<QString::number(vrp.size(),10);
    //qDebug() <<"showPose "<<QString::number(vrp.at(0).x,'f',3)<<QString::number(vrp.at(0).y,'f',3);
    //qDebug() <<"showPose "<<QString::number(vrp.at(1).x,'f',3)<<QString::number(vrp.at(1).y,'f',3);

    dataCus->clear();
    for(int i=0;i<vrp.size();i++)
    {
        point.setX(vrp.at(i).x);
        point.setY(vrp.at(i).y);
        dataCus->append(point);
       // if(i==0)
           // addCover((point));
    }

    if (!curveCus->isVisible()) {
        curveCus->setVisible(true);
    }
    const bool doClip = !canvas()->testAttribute(Qt::WA_PaintOnScreen);
    if (doClip)
    {
        /*
           Depending on the platform setting a clip might be an important
           performance issue. F.e. for Qt Embedded this reduces the
           part of the backing store that has to be copied out - maybe
           to an unaccelerated frame buffer device.
         */
        const QwtScaleMap xMap = canvasMap(curveCus->xAxis());
        const QwtScaleMap yMap = canvasMap(curveCus->yAxis());

        QRegion clipRegion;

        const QSize symbolSize = curveCus->symbol()->size();
        QRect r(0, 0, symbolSize.width() + 2, symbolSize.height() + 2);

        const QPointF center =
                QwtScaleMap::transform(xMap, yMap, point);
        r.moveCenter(center.toPoint());

        clipRegion += r;

        directPainter->setClipRegion(clipRegion);
    }

    directPainter->drawSeries(curveCus, 0, dataCus->size()-1);
    curveCus->setSamples(curveCus->data());
    curveCus->attach( this );


    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );
}


void QMainPlot::showPose(QVector<QVector<RobotPathPoint>> vvrp)
{
    int num=vvrp.size();
    if(num<=0)return;

    Qt::GlobalColor color[12]={Qt::red,Qt::green,Qt::blue,Qt::cyan,Qt::magenta,Qt::yellow,Qt::darkRed,Qt::darkGreen,Qt::darkBlue,Qt::darkCyan,Qt::darkMagenta,Qt::darkYellow};

    int poseNum=vectorCurvePose.size();
    if(poseNum<num)
    {
        for(int i=0;i<num-poseNum;i++)
        {
            //QVector<RobotPathPoint> vrp = vvrp.at(poseNum+i);
            int rid=0;
            QString name = "P"+QString::number(rid,10);
            QwtPlotCurve *curveCus=new QwtPlotCurve(name);
            curveCus->setVisible(true);
            curveCus->setData(new QCurveData());
            curveCus->setStyle(QwtPlotCurve::NoCurve);
            int n=(poseNum+i)%12;
            Qt::GlobalColor c=color[n];
            curveCus->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                           Qt::NoBrush,
                                           QPen(c),
                                           QSize(5, 5)));
            curveCus->attach(this);
            vectorCurvePose.append(curveCus);
        }
    }

    QPointF point;

    //if(vectorCurvePose.size()!=vvrp.size()) return;
    for(int i=0;i<vectorCurvePose.size();i++)
    {
        QwtPlotCurve *curveCus=vectorCurvePose.at(i);
        QCurveData *dataCus = static_cast<QCurveData *>(curveCus->data());

        if(i<vvrp.size())
        {
            QVector<RobotPathPoint> vrp = vvrp.at(i);

            int rid=0;
            if(vrp.size()>0)
            {
                rid=vrp.at(0).getRobotId();
            }
            QString name = "P"+QString::number(rid,10);
            curveCus->setTitle(name);
            dataCus->clear();
            for(int j=0;j<vrp.size();j++)
            {
                point.setX(vrp.at(j).point.x);
                point.setY(vrp.at(j).point.y);
                dataCus->append(point);
            }
        }
        else
        {
            QString name = "Px";
            curveCus->setTitle(name);
            dataCus->clear();
        }


        if (!curveCus->isVisible()) {
            curveCus->setVisible(true);
        }
        const bool doClip = !canvas()->testAttribute(Qt::WA_PaintOnScreen);
        if (doClip)
        {
            /*
               Depending on the platform setting a clip might be an important
               performance issue. F.e. for Qt Embedded this reduces the
               part of the backing store that has to be copied out - maybe
               to an unaccelerated frame buffer device.
             */
            const QwtScaleMap xMap = canvasMap(curveCus->xAxis());
            const QwtScaleMap yMap = canvasMap(curveCus->yAxis());

            QRegion clipRegion;

            const QSize symbolSize = curveCus->symbol()->size();
            QRect r(0, 0, symbolSize.width() + 2, symbolSize.height() + 2);

            const QPointF center =
                    QwtScaleMap::transform(xMap, yMap, point);
            r.moveCenter(center.toPoint());

            clipRegion += r;

            directPainter->setClipRegion(clipRegion);
        }

        directPainter->drawSeries(curveCus, 0, dataCus->size()-1);
        curveCus->setSamples(curveCus->data());
        curveCus->attach( this );
    }



    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );
}



inline void QMainPlot::addCover(const QPointF &point)
{

    QwtPlotShapeItem *item ;
    QHash<QPointF, QwtPlotShapeItem *> ::iterator mi;
    mi = mapCover.find(point);

    const QSizeF size{0.5, 0.5};
    if(mi!=mapCover.end())
    {
        item=mi.value();
        item->detach();
        item->setShape(coverPath(point, size));
        item->attach(this);
    }
    else
    {
        item = new QwtPlotShapeItem("Tag Coverage");
        //qDebug() << Q_FUNC_INFO << __LINE__ << mapCover.isEmpty();
        if (mapCover.isEmpty()) {
            item->setItemAttribute(QwtPlotItem::Legend, true);
        }


        item->setShape(coverPath(point, size));

        QColor color("RoyalBlue");

        QColor fillColor = color;
        fillColor.setAlpha(100);

        QPen pen(color, 3);
        pen.setJoinStyle(Qt::MiterJoin);
        item->setPen(pen);
        item->setBrush(fillColor);

        item->attach(this);
        mapCover.insert(point, item);
    }




}


inline QPainterPath QMainPlot::coverPath(const QPointF &point,
                                         const QSizeF &size)
{
    QRectF rect;
    rect.setSize(size);
    rect.moveCenter(point);

    QPainterPath path;

    path.addEllipse(point, size.height()/2.0, size.width()/2.0);

    path.closeSubpath();

    return path;
}

inline uint qHash(const QPointF &point)
{
    qreal x = point.x();
    qreal y = point.y();

    char *px = (char *)&x;
    char *py = (char *)&y;

    QByteArray array(px, sizeof(qreal));

    array.append(py, sizeof(qreal));

    uint hash = qHash(array);

    //qDebug() << Q_FUNC_INFO << __LINE__ << hash;

    return hash;
}

void QMainPlot::addShape( const QString &title,
    ShapeFactory::Shape shape, const QColor &color,
    const QPointF &pos, const QSizeF &size )
{
    QwtPlotShapeItem *item = new QwtPlotShapeItem( title );
    item->setItemAttribute( QwtPlotItem::Legend, true );
    item->setLegendMode( QwtPlotShapeItem::LegendShape );
    item->setLegendIconSize( QSize( 20, 20 ) );
    item->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    item->setShape( ShapeFactory::path( shape, pos, size ) );

    QColor fillColor = color;
    fillColor.setAlpha( 200 );

    QPen pen( color, 3 );
    pen.setJoinStyle( Qt::MiterJoin );
    item->setPen( pen );
    item->setBrush( fillColor );

    item->attach( this );
}

void QMainPlot::populate()
{
    addShape( "Rectangle", ShapeFactory::Rect, "RoyalBlue",
        QPointF( 30.0, 50.0 ), QSizeF( 40.0, 50.0 ) );
    addShape( "Ellipse", ShapeFactory::Ellipse, "IndianRed",
        QPointF( 80.0, 130.0 ), QSizeF( 50.0, 40.0 ) );
    addShape( "Ring", ShapeFactory::Ring, "DarkOliveGreen",
        QPointF( 30.0, 165.0 ), QSizeF( 40.0, 40.0 ) );
    addShape( "Triangle", ShapeFactory::Triangle, "SandyBrown",
        QPointF( 165.0, 165.0 ), QSizeF( 60.0, 40.0 ) );
    addShape( "Star", ShapeFactory::Star, "DarkViolet",
        QPointF( 165.0, 50.0 ), QSizeF( 40.0, 50.0 ) );
    addShape( "Hexagon", ShapeFactory::Hexagon, "DarkSlateGray",
        QPointF( 120.0, 70.0 ), QSizeF( 50.0, 50.0 ) );

}


void QMainPlot::clearItem()
{
   if(shapeItem==NULL) return;
   shapeItem->clear();
}

void QMainPlot::insertItemPoint(QPointF p)
{
   if(shapeItem==NULL) return;
   shapeItem->insertPoint(p);
   qDebug() << "insertItemPoint";
}

void QMainPlot::plotItem()
{
   if(shapeItem==NULL) return;
   shapeItem->setShapeItem();
   qDebug() << "plotItem";
}

void QMainPlot::plotItemEnd()
{
    if(shapeItem==NULL) return;
    shapeItem->setShapeItemEnd();
    shapeItem=NULL;
    qDebug() << "plotItemEnd";
}

int QMainPlot::shapeItemxAxis()
{
   if(shapeItem==NULL) return 0;
   return shapeItem->xAxis();
}

int QMainPlot::shapeItemyAxis()
{
   if(shapeItem==NULL) return 0;
   return shapeItem->yAxis();
}

int QMainPlot::getNewShapeItemId()
{
    int num=shapeItemList.size();
    if(num<=0) return 1;
    int maxId=shapeItemList.at(0)->id();
    for(int i=0;i<num;i++)
    {
        int id =shapeItemList.at(i)->id();
        if(maxId<id)
        {
            maxId=id;
        }
    }
    return maxId+1;
}

bool QMainPlot::findShapeItemId(int id)
{
    int num=shapeItemList.size();
    if(num<=0) return false;
    for(int i=0;i<num;i++)
    {
        int t =shapeItemList.at(i)->id();
        if(id==t)
        {
            return true;
        }
    }
    return false;
}

void QMainPlot::addShapeItem(int id,EditShapeItem::ShapeType type)
{
    QColor color;
    QString name;
    switch(type)
    {
      case EditShapeItem::OneWay: color="Orange";name="OneWay";break;
      case EditShapeItem::TwoWay: color="Blue";name="TwoWay";break;
      case EditShapeItem::CrossRoad: color="Purple";name="CrossRoad";break;
      case EditShapeItem::T_Road: color="Tomato";name="T_Road";break;
      default:color="IndianRed";name="UnKnowWay";break;
    }

    EditShapeItem *newshapeItem = new EditShapeItem(name);
    newshapeItem->init(this,color,id,type);


    for(int i=0;i<shapeItemList.size();i++)
    {
        if(shapeItemList.at(i)->closed()==false)
        {
            shapeItemList.remove(i);
            i--;
        }
    }
    shapeItemList.append(newshapeItem);
    if(shapeItemList.size()>0)
    {
        shapeItem=shapeItemList.at(shapeItemList.size()-1);
    }
    qDebug() << "addShapeItem";
}

void QMainPlot::deleteShapeItem(int id)
{
    if(id<0) return;
    for(int i=0;i<shapeItemList.size();)
    {
        if(shapeItemList.at(i)->id()==id)
        {
            if(shapeItem!=NULL)
            {
                if(shapeItem->id()==id)
                {
                    shapeItem=NULL;
                }
            }
            shapeItemList.at(i)->detach();
            shapeItemList.erase(shapeItemList.begin()+i);
            break;
        }
        else
        {
            i++;
        }
    }
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

    qDebug() << "deleteShapeItem "<<QString::number(shapeItemList.size(),10);
}

QVector<EditShapeItem *> QMainPlot::getShapeItemData()
{
    return shapeItemList;
}

bool QMainPlot::LoadJsonFile(const QString &fileName)
{
    // Open file as a JSON file
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
            return false;
    }

    // Read all data
    QByteArray data = jsonFile.readAll();

    QJsonParseError parseError;
    QJsonDocument jsonData(QJsonDocument::fromJson(data, &parseError));
    if (parseError.error != QJsonParseError::NoError) {
        return false;
    }

    bool ok = parseJsonFile(jsonData);
    jsonFile.close();
    return ok;
}


inline bool QMainPlot::parseJsonFile(QJsonDocument &json)
{
    // Check the key points
    if (!json.object().contains("item")) {
        return false;
    }

    QJsonArray pointArrayShape = (json.object())["item"].toArray();

    int last_id=-1;
    QColor color;
    QString name;
    for (int i = 0; i < pointArrayShape.size(); i++) {
        QJsonObject obj = pointArrayShape[i].toObject();

        int t_type  = obj["type"].toInt();
        EditShapeItem::ShapeType type;
        int x     = obj["x"].toInt();
        int y     = obj["y"].toInt();
        int id   = obj["id"].toInt();

        switch(t_type)
        {
          case 0:type= EditShapeItem::OneWay; color="Orange";name="OneWay";break;
          case 1:type= EditShapeItem::TwoWay; color="Blue";name="TwoWay";break;
          case 2:type= EditShapeItem::CrossRoad; color="Purple";name="CrossRoad";break;
          case 3:type= EditShapeItem::T_Road; color="Tomato";name="T_Road";break;
          default:type= EditShapeItem::OneWay; color="Red";name="UnknowWay";break;
        }

        if(id<0) continue;

        if(last_id!=id)//new id
        {
            if(last_id!=-1)
            {
                plotItemEnd();
            }
            addShapeItem(id,type);
            shapeItem->setVisible(true);
            last_id=id;
        }

        insertItemPoint(QPointF((double)x/1000.0,(double)y/1000.0));
    }
    if(last_id!=-1)
    {
        plotItemEnd();
    }
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

}




void QMainPlot::highLightShapeItem(QPointF p)
{

    QPointF point;
    point.rx() = invTransform(
        xBottom, p.x() );
    point.ry() = invTransform(
        yLeft, p.y() );

    //qDebug() << " Axis "<<QString::number(this->shapeItemxAxis(),'f',3)<<" "<<QString::number(this->shapeItemyAxis(),'f',3);
    //qDebug() << " mouse screen "<<QString::number(p.rx(),'f',3)<<" "<<QString::number(p.ry(),'f',3);
    //qDebug() << " mouse "<<QString::number(point.rx(),'f',3)<<" "<<QString::number(point.ry(),'f',3);
    for(int i=0;i<shapeItemList.size();i++)
    {
        //QPolygonF polygon=shapeItemList.at(i)->getPolygon();
       // qDebug() << " mouse "<<QString::number(point.rx(),'f',3)<<" "<<QString::number(point.ry(),'f',3);

        if(shapeItemList.at(i)->pointInPolygon(point))
        {
             //qDebug() << "id "<<QString::number(shapeItemList.at(i)->id(),10)<<" IN";
             shapeItemList.at(i)->setAlpha(50);
        }
        else
        {
             //qDebug() << "id "<<QString::number(shapeItemList.at(i)->id(),10)<<" OUT";
             shapeItemList.at(i)->setAlpha(25);
        }
    }
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );



}
