#ifndef canvaspicker_H
#define canvaspicker_H
#include "qmainplot.h"
#include <qobject.h>
#include <qwt_plot_marker.h>
#include <QVector>
#include <qwt_plot_curve_cus.h>
#include "qcurvedatacus.h"
class QwtPlotMarker;
class QPoint;
class QCustomEvent;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotCurveCus;
class QCurveDataCus;
class CanvasPicker: public QObject
{
    Q_OBJECT
public:
    enum DrawMode {
        NoDrawing,
        ShiftPoint,
        DrawLine,
        DeleteLine
    };
    CanvasPicker( QMainPlot *plot );
    virtual bool eventFilter( QObject *, QEvent * );

    virtual bool event( QEvent * );

    void setMode(DrawMode mode);

signals:
    void selectPoint(const QwtPointCus point);

private:
    void select( const QPoint & );
    void move( const QPoint & );
    void moveConnect( const QPoint & );
    void moveBy( int dx, int dy );

    void release();
    void selectRelease( const QPoint & ,int mode);
    void showCursor( bool enable );
    void shiftPointCursor( bool up );
    void shiftCurveCursor( bool up );
    QwtPlotCurveCus * getConnectCurve(  );
    QwtPlot *plot();
    const QwtPlot *plot() const;

    QwtPlotCurve *d_selectedCurve;
    int d_selectedPoint;

    QwtPlotCurveCus *d_selectedCurveCus;
    int d_selectedPointCus;

    DrawMode drawMode;

    double d_xConnectPoint;
    double d_yConnectPoint;
    int d_idConnectPoint;
    bool d_connectPointSelected;
    QwtPlotCurveCus *d_selectedCurveRelease;
    int d_selectedPointRelease;

    QMainPlot * parentPlot;

public:
    bool getCurveId( QwtPlotCurveCus *curve,int num ,int &id);
    bool setCurveConnection( QwtPlotCurveCus *curve,int pointNumA ,int pointNumB, int mode);
    void setConnectCurveVisible(bool on);
    bool setSelectedPoint( QwtPointCus point);
    QwtPointCus getSelectedPoint( bool *ok);
    void insertItemPoint(QPointF pos);
    void insertItemPointEnd();
};
#endif
