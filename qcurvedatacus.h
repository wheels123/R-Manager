#ifndef QCURVEDATACUS_H
#define QCURVEDATACUS_H
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_plot_marker.h>
#include <QVector>
#include <qwt_point_cus.h>
#include <qwt_plot_curve_cus.h>

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
    inline void set(int id,const QwtPointCus &point) {
        d_samples[id] = point;
    }
    void clear() {
        d_samples.clear();
        d_samples.squeeze();
        d_boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    }
};

#endif // QCURVEDATACUS_H
