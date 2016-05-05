#ifndef QMAINPLOT_H
#define QMAINPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <QResizeEvent>
#include <QVector>


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
    inline void initLegend(void);
    inline void initPlotGrid(void);
    inline void initPlotCurve(void);
    inline void initDirectPainter(void);

public:
    enum curveId {
        curveNormalPoints,
        curveTagPoints,
        curveEndPoints,
        curveMax,
    };

public:
    void appendPoint(curveId id, const QPointF &point);
    void clearPoints(curveId id);

private slots:
    void onLegendChecked(const QVariant &itemInfo, bool on, int index);

private:
    inline void showCurve(QwtPlotItem *item, bool on);

private:
    QVector<QwtPlotCurve *> vectorCurve;
    QwtPlotDirectPainter   *directPainter;
};

#endif // QMAINPLOT_H
