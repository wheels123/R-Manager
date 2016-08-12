#include <qapplication.h>
#include <qevent.h>
#include <qwhatsthis.h>
#include <qpainter.h>
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include "canvaspicker.h"
#include <qwt_plot_marker.h>
#include <qwt_plot_curve_cus.h>
CanvasPicker::CanvasPicker( QMainPlot *plot ):
    QObject( plot ),
    d_selectedCurve( NULL ),
    d_selectedPoint( -1 ),
    d_selectedCurveCus( NULL ),
    d_selectedPointCus( -1 ),
    d_selectedCurveRelease( NULL ),
    d_selectedPointRelease( -1 )
{
    parentPlot=plot;
    QwtPlotCanvas *canvas = qobject_cast<QwtPlotCanvas *>( plot->canvas() );
    canvas->installEventFilter( this );

    // We want the focus, but no focus rect. The
    // selected point will be highlighted instead.

    canvas->setFocusPolicy( Qt::StrongFocus );
#ifndef QT_NO_CURSOR
    canvas->setCursor( Qt::PointingHandCursor );
#endif
    canvas->setFocusIndicator( QwtPlotCanvas::ItemFocusIndicator );
    canvas->setFocus();

    const char *text =
        "All points can be moved using the left mouse button "
        "or with these keys:\n\n"
        "- Up:\t\tSelect next curve\n"
        "- Down:\t\tSelect previous curve\n"
        "- Left, �-�:\tSelect next point\n"
        "- Right, �+�:\tSelect previous point\n"
        "- 7, 8, 9, 4, 6, 1, 2, 3:\tMove selected point";
    canvas->setWhatsThis( text );

    shiftCurveCursor( true );

    drawMode=NoDrawing;
    d_connectPointSelected=false;
}

QwtPlot *CanvasPicker::plot()
{
    return qobject_cast<QwtPlot *>( parent() );
}

const QwtPlot *CanvasPicker::plot() const
{
    return qobject_cast<const QwtPlot *>( parent() );
}

bool CanvasPicker::event( QEvent *ev )
{
    if ( ev->type() == QEvent::User )
    {
        showCursor( true );
        return true;
    }
    return QObject::event( ev );
}

bool CanvasPicker::eventFilter( QObject *object, QEvent *event )
{
    if ( plot() == NULL || object != plot()->canvas() )
        return false;

    switch( event->type() )
    {
        case QEvent::FocusIn:
        {
            showCursor( true );
            break;
        }
        case QEvent::FocusOut:
        {
            showCursor( false );
            break;
        }
        case QEvent::Paint:
        {
            QApplication::postEvent( this, new QEvent( QEvent::User ) );
            break;
        }
        case QEvent::MouseButtonPress:
        {
            d_connectPointSelected=false;
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
            QPoint mousePose = mouseEvent->pos();
            select( mousePose );
            parentPlot->highLightShapeItem(QPointF(mousePose.x(),mousePose.y()));
            if(d_selectedCurve!=NULL||d_selectedCurveCus!=NULL)
            {
                return true;
            }
            else if(mouseEvent->button()==Qt::RightButton)
            {
                insertItemPoint(QPointF(mousePose.x(),mousePose.y()));
            }
            setConnectCurveVisible(false);

            //parentPlot->populate();
            break;
        }
        case QEvent::MouseButtonDblClick:
        {
            d_connectPointSelected=false;
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
            //if(mouseEvent->button()==Qt::RightButton)
            {
                insertItemPointEnd();
            }

            setConnectCurveVisible(false);

            break;
        }
        case QEvent::MouseButtonRelease:
        {
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );

            if(drawMode==DrawLine)
                selectRelease( mouseEvent->pos(),1);
            else if(drawMode==DeleteLine)
                selectRelease( mouseEvent->pos(),0);

            setConnectCurveVisible(false);
            break;
        }
        case QEvent::MouseMove:
        {
            d_connectPointSelected=false;
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
            QPoint mousePose = mouseEvent->pos();

            //parentPlot->highLightShapeItem(QPointF(mousePose.x(),mousePose.y()));
            if(drawMode==ShiftPoint)
            {
                setConnectCurveVisible(false);
                move( mouseEvent->pos() );
            }
            else if(drawMode==DrawLine||drawMode==DeleteLine)
            {
                setConnectCurveVisible(true);
                moveConnect( mouseEvent->pos() );
            }
            if(d_selectedCurve!=NULL||d_selectedCurveCus!=NULL)
            {
                return true;
            }


            break;
        }
        case QEvent::KeyPress:
        {
            const QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );

            const int delta = 5;
            switch( keyEvent->key() )
            {
                case Qt::Key_Up:
                {
                    shiftCurveCursor( true );
                    return true;
                }
                case Qt::Key_Down:
                {
                    shiftCurveCursor( false );
                    return true;
                }
                case Qt::Key_Right:
                case Qt::Key_Plus:
                {
                    if ( d_selectedCurve )
                        shiftPointCursor( true );
                    else
                        shiftCurveCursor( true );
                    return true;
                }
                case Qt::Key_Left:
                case Qt::Key_Minus:
                {
                    if ( d_selectedCurve )
                        shiftPointCursor( false );
                    else
                        shiftCurveCursor( true );
                    return true;
                }

                // The following keys represent a direction, they are
                // organized on the keyboard.

                case Qt::Key_1:
                {
                    moveBy( -delta, delta );
                    break;
                }
                case Qt::Key_2:
                {
                    moveBy( 0, delta );
                    break;
                }
                case Qt::Key_3:
                {
                    moveBy( delta, delta );
                    break;
                }
                case Qt::Key_4:
                {
                    moveBy( -delta, 0 );
                    break;
                }
                case Qt::Key_6:
                {
                    moveBy( delta, 0 );
                    break;
                }
                case Qt::Key_7:
                {
                    moveBy( -delta, -delta );
                    break;
                }
                case Qt::Key_8:
                {
                    moveBy( 0, -delta );
                    break;
                }
                case Qt::Key_9:
                {
                    moveBy( delta, -delta );
                    break;
                }
                default:
                    break;
            }
        }
        default:
            break;
    }

    return QObject::eventFilter( object, event );
}

// Select the point at a position. If there is no point
// deselect the selected point

void CanvasPicker::select( const QPoint &pos )
{
    QwtPlotCurve *curve = NULL;
    double dist = 10e10;
    int index = -1;

    QwtPlotCurveCus *curveCus = NULL;
    double distCus = 10e10;
    int indexCus = -1;

    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve *>( *it );

            double d;
            int idx = c->closestPoint( pos, &d );
            if ( d < dist && idx != -1)
            {
                curve = c;
                index = idx;
                dist = d;
            }
        }
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurveCus )
        {
            QwtPlotCurveCus *c = static_cast<QwtPlotCurveCus *>( *it );
            if(c->isVisible()==true)
            {
                double d;
                int idx = c->closestPoint( pos, &d );
                if ( d < distCus && idx != -1)
                {
                    curveCus = c;
                    indexCus = idx;
                    distCus = d;

                }
            }
        }
    }

    showCursor( false );
    d_selectedCurve = NULL;
    d_selectedPoint = -1;
    d_selectedCurveCus = NULL;
    d_selectedPointCus = -1;

    if ( curve && dist < 15 ) // 10 pixels tolerance
    {
        d_selectedCurve = curve;
        d_selectedPoint = index;
        showCursor( true );
    }

    QwtPointCus pointData(QwtPointCus::NoType,0,0,0,0);
    if ( curveCus && distCus < 15 ) // 10 pixels tolerance
    {
        d_selectedCurveCus = curveCus;
        d_selectedPointCus = indexCus;

        for ( int i = 0;
            i < static_cast<int>( d_selectedCurveCus->dataSize() ); i++ )
        {
            if ( i == d_selectedPointCus )
            {
                pointData = d_selectedCurveCus->sample(i);
                break;
            }
        }

        emit selectPoint(pointData);
        showCursor( true );
    }
    else
    {
         emit selectPoint(pointData);
    }

    QwtPlotCurveCus *connectCurve = getConnectCurve();
    if(connectCurve!=NULL)
    {
        if(pointData.type()==QwtPointCus::NoType)
        {
            connectCurve->setVisible(false);
        }
        else
        {
            connectCurve->setVisible(true);
            QCurveDataCus *connData = static_cast<QCurveDataCus *>(connectCurve->data());
            QwtPointCus pointCus;
            pointCus=connData->sample(0);
            pointCus.rx() = pointData.x();
            pointCus.ry() = pointData.y();

            connData->set(0,pointCus);
            connData->set(1,pointCus);
            connectCurve->setSamples(connData);
            d_connectPointSelected=true;
        }

        QwtPlotCanvas *plotCanvas =
            qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

        plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
        plot()->replot();
        plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

        showCursor( true );
    }
}





bool CanvasPicker::getCurveId( QwtPlotCurveCus *curve,int num ,int &id)
{
    if ( !curve )
        return false;

    for ( int i = 0;
        i < static_cast<int>( d_selectedCurveCus->dataSize() ); i++ )
    {
        QwtPointCus pointCus = d_selectedCurveCus->sample(i);

        if ( i == num )
        {
            id=pointCus.id();
            return true;
        }
    }
    return false;
}


bool CanvasPicker::setSelectedPoint( QwtPointCus point)
{
    QwtPointCus pointCus;
    QwtPointCus pointCusLast;
    if ( !d_selectedCurveCus )
        return false;
    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(d_selectedCurveCus->data());
    for ( int i = 0;
        i < static_cast<int>( d_selectedCurveCus->dataSize() ); i++ )
    {
        if ( i == d_selectedPointCus )
        {
            pointCusLast = d_selectedCurveCus->sample(i);

            pointCus = point;

            dataCus->set(i,pointCus);
            break;
        }
    }
    d_selectedCurveCus->setSamples(dataCus);


    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotMarker )
        {
            QwtPlotMarker *c = static_cast<QwtPlotMarker *>( *it );

            if((c->getId()==pointCusLast.id())&&(c->getType()==pointCusLast.type()))
            {
                QString label;
                c->setId(pointCus.id());
                if(c->getType()==QwtPointCus::Label)
                {
                    label.sprintf( "L[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
                }
                else if(c->getType()==QwtPointCus::Dest)
                {
                    label.sprintf( "D[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
                }
                else if(c->getType()==QwtPointCus::PathPoint)
                {
                    label.sprintf( "P[%.2f,%.2f,%.3f,%d][%d][%d,%d,%d,%d,%d,%d,%d,%d,%d %d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id(),
                                   pointCus.netId(),pointCus.net(0),pointCus.net(1),pointCus.net(2),pointCus.net(3),pointCus.net(4),
                                   pointCus.net(5),pointCus.net(6),pointCus.net(7),pointCus.net(8),pointCus.net(9));
                }
                else
                {
                    label.sprintf( "no type[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
                }
                QwtText text( label );
                text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
                if(c->getId()<100000)
                {
                   text.setColor( Qt::green );
                }
                else
                {
                   text.setColor( Qt::red );
                }

                c->setValue( pointCus.x(),pointCus.y());
                c->setLabel( text );
                break;
            }
        }
    }

    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

    showCursor( true );
    return true;
}

QwtPointCus CanvasPicker::getSelectedPoint( bool *ok)
{
    QwtPointCus pointCus(QwtPointCus::NoType,0,0,0,0);
    *ok=false;
    if ( !d_selectedCurveCus )
        return pointCus;

    for ( int i = 0;
        i < static_cast<int>( d_selectedCurveCus->dataSize() ); i++ )
    {
        if ( i == d_selectedPointCus )
        {
            QwtPointCus pointCus = d_selectedCurveCus->sample(i);

            *ok=true;
            return pointCus;
        }
    }
    return pointCus;
}

bool CanvasPicker::setCurveConnection( QwtPlotCurveCus *curve,int pointNumA ,int pointNumB,int mode)
{
    if ( !curve || pointNumA<0 ||pointNumB<0)
        return false;
    if ( pointNumA == pointNumB)
        return false;
    if(pointNumA+1>(int)(curve->dataSize()))
        return false;
    if(pointNumB+1>(int)(curve->dataSize()))
        return false;

    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(curve->data());
    int idA=0,idB=0;
    for ( int i = 0;
        i < static_cast<int>( curve->dataSize() ); i++ )
    {
        QwtPointCus pointCus(QwtPointCus::PathPoint,0,0,0,0);
        pointCus = curve->sample(i);
        if ( i == pointNumA )
        {
            idA=pointCus.id();
        }
        if ( i == pointNumB )
        {
            idB=pointCus.id();
        }
    }

    for ( int i = 0;
        i < static_cast<int>( curve->dataSize() ); i++ )
    {
        QwtPointCus pointCus(QwtPointCus::PathPoint,0,0,0,0);
        pointCus = curve->sample(i);
        if ( i == pointNumA )
        {
            if(mode==0)
            {
                pointCus.deleteNet(idB);
            }
            else if(mode==1)
            {
                pointCus.insertNet(idB);
            }

        }
        /*
        if ( i == pointNumB )
        {
            if(mode==0)
            {
                pointCus.deleteNet(idA);
            }
            else if(mode==1)
            {
                pointCus.insertNet(idA);
            }
        }
        */
        dataCus->set(i,pointCus);
    }

    curve->setSamples(dataCus);
    return true;
}

void CanvasPicker::selectRelease( const QPoint &pos, int mode)
{
    QwtPlotCurveCus *curveCus = NULL;
    double distCus = 10e10;
    int indexCus = -1;

    if(!d_selectedCurveCus) return;

    QwtPlotCurveCus *c = d_selectedCurveCus;
    if(c->dataSize()>1 && c->isVisible()==true)
    {
        QwtPointCus pointCus = c->sample(0);
        if(pointCus.type()==QwtPointCus::PathPoint)
        {
            double d;
            int idx = c->closestPoint( pos, &d );
            if ( d < distCus && idx != -1)
            {
                curveCus = c;
                indexCus = idx;
                distCus = d;
            }
        }
    }

    showCursor( false );
    d_selectedCurveRelease = NULL;
    d_selectedPointRelease = -1;

    if ( distCus < 20 ) // 10 pixels tolerance
    {
        d_selectedCurveRelease = curveCus;
        d_selectedPointRelease = indexCus;
        showCursor( true );
        if((drawMode==DrawLine||drawMode==DeleteLine) && d_connectPointSelected==true)
        {
            bool ret = true;
            ret = setCurveConnection(d_selectedCurveRelease,d_selectedPointCus,d_selectedPointRelease,mode);
        }

    }
}


// Move the selected point
void CanvasPicker::moveBy( int dx, int dy )
{
    if ( dx == 0 && dy == 0 )
        return;

    if ( !d_selectedCurveCus )
        return;

    const QwtPointCus sampleCus =
        d_selectedCurveCus->sample( d_selectedPointCus );

    QPointF sample;

    sample.rx()=sampleCus.x();
    sample.ry()=sampleCus.y();

    const double x = plot()->transform(
        d_selectedCurveCus->xAxis(), sample.x() );
    const double y = plot()->transform(
        d_selectedCurveCus->yAxis(), sample.y() );

    move( QPoint( qRound( x + dx ), qRound( y + dy ) ) );
}

// Move the selected point
void CanvasPicker::move( const QPoint &pos )
{
//////////
    if ( !d_selectedCurveCus )
        return;

    QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(d_selectedCurveCus->data());
    for ( int i = 0;
        i < static_cast<int>( d_selectedCurveCus->dataSize() ); i++ )
    {
        QwtPointCus pointCus = d_selectedCurveCus->sample(i);

        //if ( pointCus.type()>=QwtPointCus::Label && pointCus.type()<=QwtPointCus::Connect && i == d_selectedPointCus )
        if ( i == d_selectedPointCus )
        {
            pointCus.rx() = plot()->invTransform(
                d_selectedCurveCus->xAxis(), pos.x() );
            pointCus.ry() = plot()->invTransform(
                d_selectedCurveCus->yAxis(), pos.y() );

            dataCus->set(i,pointCus);
            d_selectedCurveCus->setSamples(dataCus);

            const QwtPlotItemList& itmList = plot()->itemList();
            for ( QwtPlotItemIterator it = itmList.begin();
                it != itmList.end(); ++it )
            {
                if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotMarker )
                {
                    QwtPlotMarker *c = static_cast<QwtPlotMarker *>( *it );

                    if((c->getId()==pointCus.id())&&(c->getType()==pointCus.type()))
                    {
                        QString label;
                        if(c->getType()==QwtPointCus::Label)
                        {
                            label.sprintf( "L[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
                        }
                        else if(c->getType()==QwtPointCus::Dest)
                        {
                            label.sprintf( "D[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
                        }
                        else if(c->getType()==QwtPointCus::PathPoint)
                        {
                            label.sprintf( "P[%.2f,%.2f,%.3f,%d][%d][%d,%d,%d,%d,%d,%d,%d,%d,%d %d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id(),
                                           pointCus.netId(),pointCus.net(0),pointCus.net(1),pointCus.net(2),pointCus.net(3),pointCus.net(4),
                                           pointCus.net(5),pointCus.net(6),pointCus.net(7),pointCus.net(8),pointCus.net(9));
                        }
                        else
                        {
                            label.sprintf( "no type[%.2f,%.2f,%.3f,%d]", pointCus.x(),pointCus.y(),pointCus.phi(),pointCus.id());
                        }
                        QwtText text( label );
                        text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
                        if(c->getId()<100000)
                        {
                           text.setColor( Qt::green );
                        }
                        else
                        {
                           text.setColor( Qt::red );
                        };
                        c->setValue( pointCus.x(),pointCus.y());
                        c->setLabel( text );
                        break;
                    }
                }
            }
            break;
        }
    }
///////////
    /*
       Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
       updated before we paint the cursor on it.
     */
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

    showCursor( true );
}


QwtPlotCurveCus * CanvasPicker::getConnectCurve(  )
{
    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurveCus )
        {
             QwtPlotCurveCus *c = static_cast<QwtPlotCurveCus *>( *it );
             QCurveDataCus *dataCus = static_cast<QCurveDataCus *>(c->data());
             if(dataCus->size()>1)
             {
                 QwtPointCus pointCus = dataCus->sample(0);
                 if(pointCus.type()==QwtPointCus::Connect)
                 {
                     return c;
                 }
             }
        }
    }
    return NULL;
}
// Move the selected point
void CanvasPicker::moveConnect( const QPoint &pos )
{
    QwtPlotCurveCus *connectCurve;
    if ( !d_selectedCurveCus )
        return;

    for ( int i = 0;
        i < static_cast<int>( d_selectedCurveCus->dataSize() ); i++ )
    {
        QwtPointCus selectedPointCus = d_selectedCurveCus->sample(i);


        if (selectedPointCus.type()==QwtPointCus::PathPoint &&  i == d_selectedPointCus )
        {
            connectCurve = getConnectCurve();
            if(connectCurve!=NULL)
            {
                QCurveDataCus *connData = static_cast<QCurveDataCus *>(connectCurve->data());
                QwtPointCus pointCus;
                pointCus=connData->sample(0);
                pointCus.rx() = selectedPointCus.x();
                pointCus.ry() = selectedPointCus.y();

                connData->set(0,pointCus);

                pointCus=connData->sample(1);
                pointCus.rx() = plot()->invTransform(
                    d_selectedCurveCus->xAxis(), pos.x() );
                pointCus.ry() = plot()->invTransform(
                    d_selectedCurveCus->yAxis(), pos.y() );
                connData->set(1,pointCus);
                connectCurve->setSamples(connData);
                d_connectPointSelected=true;
            }
            break;
        }
    }
    /*
       Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
       updated before we paint the cursor on it.
     */
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

    showCursor( true );
}

// Hightlight the selected point
void CanvasPicker::showCursor( bool showIt )
{
    return;
    if ( !d_selectedCurveCus )
        return;

    QwtSymbol *symbol = const_cast<QwtSymbol *>( d_selectedCurveCus->symbol() );

    const QBrush brush = symbol->brush();
    if ( showIt )
        symbol->setBrush( symbol->brush().color().dark( 180 ) );

    QwtPlotDirectPainter directPainter;
    directPainter.drawSeries( d_selectedCurveCus, d_selectedPointCus, d_selectedPointCus );

    if ( showIt )
        symbol->setBrush( brush ); // reset brush

}

// Select the next/previous curve
void CanvasPicker::shiftCurveCursor( bool up )
{
    QwtPlotItemIterator it;

    const QwtPlotItemList &itemList = plot()->itemList();

    QwtPlotItemList curveList;
    for ( it = itemList.begin(); it != itemList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
            curveList += *it;
    }
    if ( curveList.isEmpty() )
        return;

    it = curveList.begin();

    if ( d_selectedCurve )
    {
        for ( it = curveList.begin(); it != curveList.end(); ++it )
        {
            if ( d_selectedCurve == *it )
                break;
        }
        if ( it == curveList.end() ) // not found
            it = curveList.begin();

        if ( up )
        {
            ++it;
            if ( it == curveList.end() )
                it = curveList.begin();
        }
        else
        {
            if ( it == curveList.begin() )
                it = curveList.end();
            --it;
        }
    }

    showCursor( false );
    d_selectedPoint = 0;
    d_selectedCurve = static_cast<QwtPlotCurve *>( *it );
    showCursor( true );
}

// Select the next/previous neighbour of the selected point
void CanvasPicker::shiftPointCursor( bool up )
{
    if ( !d_selectedCurve )
        return;

    int index = d_selectedPoint + ( up ? 1 : -1 );
    index = ( index + d_selectedCurve->dataSize() ) % d_selectedCurve->dataSize();

    if ( index != d_selectedPoint )
    {
        showCursor( false );
        d_selectedPoint = index;
        showCursor( true );
    }
}
void CanvasPicker::setMode(DrawMode mode)
{
   drawMode=mode;
}

void CanvasPicker::setConnectCurveVisible(bool on)
{
    QwtPlotCurveCus * curve = getConnectCurve();
    if(curve!=NULL)
    {
        curve->setVisible(on);
    }
}

void CanvasPicker::insertItemPoint(QPointF pos)
{
    QPointF a;
    a.rx() = plot()->invTransform(
        parentPlot->shapeItemxAxis(), pos.x() );
    a.ry() = plot()->invTransform(
        parentPlot->shapeItemyAxis(), pos.y() );
    parentPlot->insertItemPoint(a);
    parentPlot->plotItem();
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

}

void CanvasPicker::insertItemPointEnd()
{
    parentPlot->plotItemEnd();
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

}
