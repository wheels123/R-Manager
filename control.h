#ifndef CONTROL_H
#define CONTROL_H
#include "fortuneserver.h"
#include "qmainplot.h"
#include "twowayregion.h"
#include "onewayregion.h"
class Manager
{
public:
    Manager();
    void setRegion(QVector<EditShapeItem *> region, QCurveDataCus *path,Robot *robot);
OneWayRegion * findOneWayRegion(EditShapeItem * item,bool &ok);
TwoWayRegion * findTwoWayRegion(EditShapeItem * item,bool &ok);
// TwoWayRegion
private:
    QVector<TwoWayRegion *> twoWayRegion;
    QVector<OneWayRegion *> oneWayRegion;
};

#endif // CONTROL_H
