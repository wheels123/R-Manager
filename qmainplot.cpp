#include "qmainplot.h"

QMainPlot::QMainPlot(QWidget *parent):
    QwtPlot(parent)
{

}

QSize QMainPlot::sizeHint() const
{
    return QSize(800, 600);
}
