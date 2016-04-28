#ifndef QMAINPLOT_H
#define QMAINPLOT_H

#include <qwt_plot.h>

class QMainPlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit QMainPlot(QWidget *parent);

signals:

public slots:

public:
    virtual QSize sizeHint() const;
};

#endif // QMAINPLOT_H
