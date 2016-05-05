#include "qmainplot.h"
#include <qwt_plot_layout.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_symbol.h>
#include <qwt_painter.h>
#include <qwt_legend.h>
#include <QDebug>


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

QMainPlot::QMainPlot(QWidget *parent):
    QwtPlot(parent)
{
    // Align cavas into scale
    auto layout = plotLayout();
    layout->setAlignCanvasToScales(true);

    initLegend();

    //
    initPlotGrid();

    const double xMin = -100.0;
    const double xMax = 100.0;
    const double yMin = -100.0;
    const double yMax = 100.0;

    setAxisScale(xBottom, xMin, xMax);
    setAxisScale(yLeft, yMin, yMax);

    //
    setCanvasBackground(QColor(29, 100, 141)); // nice blue

    initPlotCurve();

    initDirectPainter();

    setAutoReplot(false);

    // panning with the left mouse button
    (void )new QwtPlotPanner(canvas());

    // zoom in/out with the wheel
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier(canvas());
    magnifier->setMouseButton( Qt::NoButton );

    replot();
}

QMainPlot::~QMainPlot()
{
    foreach(auto curve, vectorCurve) {
        delete curve;
    }
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
            &onLegendChecked);
}

inline void QMainPlot::initPlotGrid()
{
    //
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
    grid->attach(this);
}

inline void QMainPlot::initPlotCurve(void)
{
    QwtPlotCurve *curve;

    // Normal point
    curve = new QwtPlotCurve("Curve Normal Point");
    curve->setData(new QCurveData());
    curve->setPen(Qt::red);
    curve->setStyle(QwtPlotCurve::Dots);
    curve->setSymbol(new QwtSymbol());
    curve->attach(this);

    vectorCurve << curve;

    // Tag point
    curve = new QwtPlotCurve("Curve Tag Point");
    curve->setData(new QCurveData());
    curve->setStyle(QwtPlotCurve::NoCurve);
    curve->setSymbol(new QwtSymbol(QwtSymbol::XCross,
                                   Qt::NoBrush,
                                   QPen(Qt::white),
                                   QSize(6, 6)));
    curve->attach(this);

    vectorCurve << curve;

    // End point
    curve = new QwtPlotCurve("Curve Tag Point");
    curve->setData(new QCurveData());
    curve->setStyle(QwtPlotCurve::NoCurve);
    curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                   Qt::SolidPattern,
                                   QPen(Qt::white),
                                   QSize(6, 6)));
    curve->attach(this);

    vectorCurve << curve;
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

void QMainPlot::appendPoint(curveId id, const QPointF &point)
{
    auto curve = vectorCurve.at(id);

    QCurveData *data = static_cast<QCurveData *>(curve->data());

    data->append(point);

    const bool doClip = !canvas()->testAttribute(Qt::WA_PaintOnScreen);
    if (doClip)
    {
        /*
           Depending on the platform setting a clip might be an important
           performance issue. F.e. for Qt Embedded this reduces the
           part of the backing store that has to be copied out - maybe
           to an unaccelerated frame buffer device.
         */
        const QwtScaleMap xMap = canvasMap(curve->xAxis());
        const QwtScaleMap yMap = canvasMap(curve->yAxis());

        QRegion clipRegion;

        const QSize symbolSize = curve->symbol()->size();
        QRect r(0, 0, symbolSize.width() + 2, symbolSize.height() + 2);

        const QPointF center =
                QwtScaleMap::transform(xMap, yMap, point);
        r.moveCenter(center.toPoint());

        clipRegion += r;

        directPainter->setClipRegion(clipRegion);
    }

    directPainter->drawSeries(curve, data->size() - 1, data->size() - 1);
}

void QMainPlot::clearPoints(curveId id)
{
    auto curve = vectorCurve.at(id);

    QCurveData *data = static_cast<QCurveData *>(curve->data());

    data->clear();

    replot();
}

inline void QMainPlot::showCurve(QwtPlotItem *item, bool on)
{
    qDebug() << Q_FUNC_INFO << __LINE__ << item << on;

    item->setVisible(on);

    replot();
}

void QMainPlot::onLegendChecked(const QVariant &itemInfo, bool on, int index)
{
    Q_UNUSED(index);

    QwtPlotItem *plotItem = infoToItem( itemInfo );
    if (plotItem) {
        showCurve(plotItem, on);
    }
}
