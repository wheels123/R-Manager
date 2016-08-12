#include "control.h"

Manager::Manager()
{

}


void Manager::setRegion(QVector<EditShapeItem *> shapeItem, QCurveDataCus *path,Robot *robot)
{
    //qDebug() << "Manager setRegion start\n";
    for(int i=0;i<shapeItem.size();i++)
    {
        EditShapeItem *item=shapeItem.at(i);
        if(item->type()==EditShapeItem::OneWay)
        {
            bool ok;
            OneWayRegion *region= findOneWayRegion(item,ok);
            if(ok==true)
            {
                region->setRegion(item,path,robot);
                region->setControl(robot);
            }
            else
            {
                region=new OneWayRegion();
                region->setRegion(item,path,robot);
                region->setControl(robot);
                oneWayRegion.append(region);
            }
            qDebug() << "OneWayRegion setControl end\n";
        }
        else if(item->type()==EditShapeItem::TwoWay)
        {
            bool ok;
            TwoWayRegion *region= findTwoWayRegion(item,ok);
            if(ok==true)
            {
                region->setRegion(item,path,robot);
                region->setControl(robot);
            }
            else
            {
                region=new TwoWayRegion();
                region->setRegion(item,path,robot);
                region->setControl(robot);
                twoWayRegion.append(region);
            }
            qDebug() << "TwoWayRegion setControl end\n";
        }
    }
    //qDebug() << "Manager setRegion end\n";
}


OneWayRegion * Manager::findOneWayRegion(EditShapeItem * item,bool &ok)
{
    ok=false;
    for(int i=0;i<oneWayRegion.size();i++)
    {
        if(oneWayRegion.at(i)->getRegionId()==item->id())
        {
            ok=true;
            return oneWayRegion.at(i);
        }
    }
    return NULL;
}

TwoWayRegion * Manager::findTwoWayRegion(EditShapeItem * item,bool &ok)
{
    ok=false;
    for(int i=0;i<twoWayRegion.size();i++)
    {
        if(twoWayRegion.at(i)->getRegionId()==item->id())
        {
            ok=true;
            return twoWayRegion.at(i);
        }
    }
    return NULL;
}

