#include "qtrackclient.h"
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString QPointData::keyNormal = "normal";
const QString QPointData::keyLabel = "label";
const QString QPointData::keyDest = "dest";
const QString QPointData::keyPath = "path";
const QString QPointData::keyType = "type";
const QString QPointData::keyX = "x";
const QString QPointData::keyY = "y";
const QString QPointData::keyAngle = "angle";
const QString QPointData::keyTag = "tag";
const QString QPointData::keyNet[10] = {"net0","net1","net2","net3","net4","net5","net6","net7","net8","net9",};
const QString QPointData::keyNetNum = "netnum";
QTrackClient::QTrackClient(QObject *parent)
    :QTcpSocket(parent)
{
    initTcpSocket();
    curveDataLabel=NULL;
    curveDataLabel = new QCurveDataCus();
    curveDataLabel->clear();
    curveDataDest=NULL;
    curveDataDest = new QCurveDataCus();
    curveDataDest->clear();
    curveDataPath=NULL;
    curveDataPath = new QCurveDataCus();
    curveDataPath->clear();
    curveDataNormal=NULL;
    curveDataNormal = new QCurveDataCus();
    curveDataNormal->clear();
}


bool QTrackClient::validId(int id)
{
    const int min=1;
    const int max=99999;

    if(id>=min && id<=max) return true;
    return false;
}

inline void QTrackClient::initTcpSocket()
{
    //
    //
    //
    connect(this, &QTcpSocket::connected,
            this, &QTrackClient::onTcpConnected);
    connect(this, &QTcpSocket::disconnected,
            this, &QTrackClient::onTcpDisconnected);
    connect(this, &QTcpSocket::readyRead,
            this, &QTrackClient::onTcpReadyRead);

}

void QTrackClient::onTcpConnected()
{
    qDebug() << Q_FUNC_INFO << __LINE__;
}

void QTrackClient::onTcpDisconnected()
{
    qDebug() << Q_FUNC_INFO << __LINE__;
}

inline void QTrackClient::processData(QString &data)
{
    const QString itemSep = ",";

    QStringList list = data.split(itemSep);

    const int itemNo = 5;

    if (list.size() != itemNo) {
        //qDebug() << Q_FUNC_INFO << __LINE__ << list.size();

        return;
    }

    const int typeIndex  = 0;
    const int xIndex     = 1;
    const int yIndex     = 2;
    const int angleIndex = 3;
    const int tagIndex   = 4;

    const QString normalPointType = "@";
    const QString endPointType = "#";
    const QString tagPointType = "$";

    QString type = list.at(typeIndex);
    int xValue   = list.at(xIndex).toInt();
    int yValue   = list.at(yIndex).toInt();

    int angleValue = list.at(angleIndex).toInt();
    int tagNo      = list.at(tagIndex).toInt();

    QwtPointCus::PointStyle style=QwtPointCus::Normal;


    if (type == normalPointType) {
        style = QwtPointCus::Normal;
    } else if (type == tagPointType){
        style = QwtPointCus::Label;
    } else if (type == endPointType) {
        style = QwtPointCus::Dest;
    } else {
        //qDebug() << Q_FUNC_INFO << __LINE__ << type;

        return;
    }

    const double xScale = 100.0;
    const double yScale = 100.0;
    const double phiScale = 1000.0;
    QwtPointCus pointData(style,tagNo,(double)xValue/xScale,(double)yValue/yScale,(double)angleValue/phiScale);

    if (type == normalPointType) {
        pointData.rtype()=QwtPointCus::Normal;
        curveDataNormal->append(pointData);
    } else if (type == tagPointType){
        pointData.rtype()=QwtPointCus::Label;
        curveDataLabel->append(pointData);
    } else if (type == endPointType) {
        pointData.rtype()=QwtPointCus::Dest;
        curveDataDest->append(pointData);
    }

    emit newPoint(pointData);
}

void QTrackClient::onTcpReadyRead()
{
    while (canReadLine()) {
        QString data = readLine();
        if (data.isEmpty()) {
            qDebug() << Q_FUNC_INFO << __LINE__;

            break;
        }

        processData(data);
    }
}

inline bool QTrackClient::parseJsonFileNormal(QJsonDocument &json)
{
    // Check the key points
    if (!json.object().contains(QPointData::keyNormal)) {
        return false;
    }

    QJsonArray pointArrayLabel = (json.object())[QPointData::keyNormal].toArray();
    if(curveDataNormal) delete curveDataNormal;
    curveDataNormal = new QCurveDataCus();
    curveDataNormal->clear();
    for (int i = 0; i < pointArrayLabel.size(); i++) {
        QJsonObject obj = pointArrayLabel[i].toObject();

        int type  = obj[QPointData::keyType].toInt();
        int x     = obj[QPointData::keyX].toInt();
        int y     = obj[QPointData::keyY].toInt();
        int angle = obj[QPointData::keyAngle].toInt();
        int tag   = obj[QPointData::keyTag].toInt();
        int netNum   = obj[QPointData::keyNetNum].toInt();
        int net[10];

        netNum=0;
        for(int netId=0;netId<10;netId++)
        {
            net[netId]  = obj[QPointData::keyNet[netId]].toInt();
            if(net[netId]!=0) netNum++;
        }
        QwtPointCus pointData;
        pointData.rtype()=(QwtPointCus::PointStyle)type;
        pointData.rx()=(double)x/100.0;
        pointData.ry()=(double)y/100.0;
        pointData.rphi()=(double)angle/1000.0;
        pointData.rid()=tag;
        pointData.rnetId()=netNum;
        for(int netId=0;netId<10;netId++)
        {
            pointData.rnet(netId) = net[netId];
        }
        curveDataNormal->append(pointData);
        //qDebug() << Q_FUNC_INFO << __LINE__ << type << x << y << angle << tag;
    }
    return true;
}
inline bool QTrackClient::parseJsonFile(QJsonDocument &json)
{
    // Check the key points
    if (!json.object().contains(QPointData::keyLabel)) {
        return false;
    }
    if (!json.object().contains(QPointData::keyDest)) {
        return false;
    }
    if (!json.object().contains(QPointData::keyPath)) {
        return false;
    }

    QJsonArray pointArrayLabel = (json.object())[QPointData::keyLabel].toArray();
    if(curveDataLabel) delete curveDataLabel;
    curveDataLabel = new QCurveDataCus();
    curveDataLabel->clear();
    for (int i = 0; i < pointArrayLabel.size(); i++) {
        QJsonObject obj = pointArrayLabel[i].toObject();

        int type  = obj[QPointData::keyType].toInt();
        int x     = obj[QPointData::keyX].toInt();
        int y     = obj[QPointData::keyY].toInt();
        int angle = obj[QPointData::keyAngle].toInt();
        int tag   = obj[QPointData::keyTag].toInt();
        int netNum   = obj[QPointData::keyNetNum].toInt();
        int net[10];

        netNum=0;
        for(int netId=0;netId<10;netId++)
        {
            net[netId]  = obj[QPointData::keyNet[netId]].toInt();
            if(net[netId]!=0) netNum++;
        }
        QwtPointCus pointData;
        pointData.rtype()=(QwtPointCus::PointStyle)type;
        pointData.rx()=(double)x/100.0;
        pointData.ry()=(double)y/100.0;
        pointData.rphi()=(double)angle/1000.0;
        pointData.rid()=tag;
        pointData.rnetId()=netNum;
        for(int netId=0;netId<10;netId++)
        {
            pointData.rnet(netId) = net[netId];
        }
        curveDataLabel->append(pointData);
        //qDebug() << Q_FUNC_INFO << __LINE__ << type << x << y << angle << tag;
    }
////////////////////////////////////////////////////
    QJsonArray pointArrayDest = (json.object())[QPointData::keyDest].toArray();
    if(curveDataDest) delete curveDataDest;
    curveDataDest = new QCurveDataCus();
    curveDataDest->clear();
    for (int i = 0; i < pointArrayDest.size(); i++) {
        QJsonObject obj = pointArrayDest[i].toObject();

        int type  = obj[QPointData::keyType].toInt();
        int x     = obj[QPointData::keyX].toInt();
        int y     = obj[QPointData::keyY].toInt();
        int angle = obj[QPointData::keyAngle].toInt();
        int tag   = obj[QPointData::keyTag].toInt();
        int netNum   = obj[QPointData::keyNetNum].toInt();
        int net[10];

        netNum=0;
        for(int netId=0;netId<10;netId++)
        {
            net[netId]  = obj[QPointData::keyNet[netId]].toInt();
            if(net[netId]!=0) netNum++;
        }
        QwtPointCus pointData;
        pointData.rtype()=(QwtPointCus::PointStyle)type;
        pointData.rx()=(double)x/100.0;
        pointData.ry()=(double)y/100.0;
        pointData.rphi()=(double)angle/1000.0;
        pointData.rid()=tag;
        pointData.rnetId()=netNum;
        for(int netId=0;netId<10;netId++)
        {
            pointData.rnet(netId) = net[netId];
        }
        curveDataDest->append(pointData);
        //qDebug() << Q_FUNC_INFO << __LINE__ << type << x << y << angle << tag;
    }
    ////////////////////////////////////////////////////
    QJsonArray pointArrayPath = (json.object())[QPointData::keyPath].toArray();
    if(curveDataPath) delete curveDataPath;
    curveDataPath = new QCurveDataCus();
    curveDataPath->clear();
    for (int i = 0; i < pointArrayPath.size(); i++) {
        QJsonObject obj = pointArrayPath[i].toObject();

        int type  = obj[QPointData::keyType].toInt();
        int x     = obj[QPointData::keyX].toInt();
        int y     = obj[QPointData::keyY].toInt();
        int angle = obj[QPointData::keyAngle].toInt();
        int tag   = obj[QPointData::keyTag].toInt();
        int netNum   = obj[QPointData::keyNetNum].toInt();
        int net[10];

        netNum=0;
        for(int netId=0;netId<10;netId++)
        {
            net[netId]  = obj[QPointData::keyNet[netId]].toInt();
            if(net[netId]!=0) netNum++;
        }
        QwtPointCus pointData;
        pointData.rtype()=(QwtPointCus::PointStyle)type;
        pointData.rx()=(double)x/100.0;
        pointData.ry()=(double)y/100.0;
        pointData.rphi()=(double)angle/1000.0;
        pointData.rid()=tag;
        pointData.rnetId()=netNum;
        for(int netId=0;netId<10;netId++)
        {
            pointData.rnet(netId) = net[netId];
        }
        curveDataPath->append(pointData);
        //qDebug() << Q_FUNC_INFO << __LINE__ << type << x << y << angle << tag;
    }

    return true;
}

QCurveDataCus* QTrackClient::getNormalData()
{
    return curveDataNormal;
}

QCurveDataCus* QTrackClient::getDestData()
{
    return curveDataDest;
}

QCurveDataCus* QTrackClient::getLabelData()
{
    return curveDataLabel;
}

QCurveDataCus* QTrackClient::getPathData()
{
    return curveDataPath;
}

bool QTrackClient::LoadJsonFile(const QString &fileName)
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

    return parseJsonFile(jsonData);
}

bool QTrackClient::LoadJsonFileNormal(const QString &fileName)
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

    return parseJsonFileNormal(jsonData);
}

bool QTrackClient::saveToJsonFile(QCurveDataCus *dest,QCurveDataCus *label,
                                  QCurveDataCus *path,QVector<EditShapeItem *> shapeItem, const QString &fileName)
{
    // Open file as a JSON file
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject keyPoint;
    QCurveDataCus *data;
    QJsonArray pointArrayDest;
    QJsonArray pointArrayLabel;
    QJsonArray pointArrayPath;
    QJsonArray shapeItemArray;
    QJsonObject pointObject;
    QJsonObject pointObjectShapeItem;
    data=dest;
    for(unsigned int i=0;i<data->size();i++)
    {
        pointObject[QPointData::keyType] = (int)data->sample(i).type();
        pointObject[QPointData::keyX] = (int)(data->sample(i).x()*100);
        pointObject[QPointData::keyY] = (int)(data->sample(i).y()*100);
        pointObject[QPointData::keyAngle] = (int)(data->sample(i).phi()*1000);
        pointObject[QPointData::keyTag] = (int)data->sample(i).id();
        pointObject[QPointData::keyNetNum] = (int)data->sample(i).rnetId();
        for(int netId=0;netId<10;netId++)
        {
            pointObject[QPointData::keyNet[netId]] = (int)data->sample(i).net(netId);
        }

        pointArrayDest.append(pointObject);
    }
    keyPoint[QPointData::keyDest] = pointArrayDest;

    data=label;
    for(unsigned int i=0;i<data->size();i++)
    {
        pointObject[QPointData::keyType] = (int)data->sample(i).type();
        pointObject[QPointData::keyX] = (int)(data->sample(i).x()*100);
        pointObject[QPointData::keyY] = (int)(data->sample(i).y()*100);
        pointObject[QPointData::keyAngle] = (int)(data->sample(i).phi()*1000);
        pointObject[QPointData::keyTag] = (int)data->sample(i).id();
        pointObject[QPointData::keyNetNum] = (int)data->sample(i).rnetId();
        for(int netId=0;netId<10;netId++)
        {
            pointObject[QPointData::keyNet[netId]] = (int)data->sample(i).net(netId);
        }

        pointArrayLabel.append(pointObject);
    }
    keyPoint[QPointData::keyLabel] = pointArrayLabel;

    data=path;
    for(unsigned int i=0;i<data->size();i++)
    {
        pointObject[QPointData::keyType] = (int)data->sample(i).type();
        pointObject[QPointData::keyX] = (int)(data->sample(i).x()*100);
        pointObject[QPointData::keyY] = (int)(data->sample(i).y()*100);
        pointObject[QPointData::keyAngle] = (int)(data->sample(i).phi()*1000);
        pointObject[QPointData::keyTag] = (int)data->sample(i).id();
        pointObject[QPointData::keyNetNum] = (int)data->sample(i).rnetId();
        for(int netId=0;netId<10;netId++)
        {
            pointObject[QPointData::keyNet[netId]] = (int)data->sample(i).net(netId);
        }

        pointArrayPath.append(pointObject);
    }
    keyPoint[QPointData::keyPath] = pointArrayPath;



    for(int listIndex=0;listIndex<shapeItem.size();listIndex++)
    {
        EditShapeItem *item = shapeItem.at(listIndex);
        QPolygonF ploygon = item->getPolygon();
        for(unsigned int i=0;i<ploygon.size()-1;i++)
        {
            pointObjectShapeItem["id"] = (int)item->id();
            pointObjectShapeItem["type"] = (int)item->type();
            pointObjectShapeItem["x"] = (int)(ploygon.at(i).x()*1000);//QString::number(ploygon.at(i).x(), 'f', 2).toDouble();//ploygon.at(i).x()*100;
            pointObjectShapeItem["y"] = (int)(ploygon.at(i).y()*1000);//QString::number(ploygon.at(i).y(), 'f', 2).toDouble();
            shapeItemArray.append(pointObjectShapeItem);
        }
        //QString keyName="item"+QString::number(item->id(), 10);
        //keyPoint[keyName] = shapeItemArray;
    }

    keyPoint["item"] = shapeItemArray;
    QJsonDocument jsonDoc(keyPoint);

    jsonFile.write(jsonDoc.toJson());

    return true;
}

bool QTrackClient::saveToJsonFile_bak(QCurveDataCus *dest,QCurveDataCus *label,QCurveDataCus *path,const QString &fileName)
{
    // Open file as a JSON file
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject keyPoint;
    QCurveDataCus *data;
    QJsonArray pointArrayDest;
    QJsonArray pointArrayLabel;
    QJsonArray pointArrayPath;
    QJsonObject pointObject;

    data=dest;
    for(unsigned int i=0;i<data->size();i++)
    {
        pointObject[QPointData::keyType] = (int)data->sample(i).type();
        pointObject[QPointData::keyX] = (int)(data->sample(i).x()*100);
        pointObject[QPointData::keyY] = (int)(data->sample(i).y()*100);
        pointObject[QPointData::keyAngle] = (int)(data->sample(i).phi()*1000);
        pointObject[QPointData::keyTag] = (int)data->sample(i).id();
        pointObject[QPointData::keyNetNum] = (int)data->sample(i).rnetId();
        for(int netId=0;netId<10;netId++)
        {
            pointObject[QPointData::keyNet[netId]] = (int)data->sample(i).net(netId);
        }

        pointArrayDest.append(pointObject);
    }
    keyPoint[QPointData::keyDest] = pointArrayDest;

    data=label;
    for(unsigned int i=0;i<data->size();i++)
    {
        pointObject[QPointData::keyType] = (int)data->sample(i).type();
        pointObject[QPointData::keyX] = (int)(data->sample(i).x()*100);
        pointObject[QPointData::keyY] = (int)(data->sample(i).y()*100);
        pointObject[QPointData::keyAngle] = (int)(data->sample(i).phi()*1000);
        pointObject[QPointData::keyTag] = (int)data->sample(i).id();
        pointObject[QPointData::keyNetNum] = (int)data->sample(i).rnetId();
        for(int netId=0;netId<10;netId++)
        {
            pointObject[QPointData::keyNet[netId]] = (int)data->sample(i).net(netId);
        }

        pointArrayLabel.append(pointObject);
    }
    keyPoint[QPointData::keyLabel] = pointArrayLabel;

    data=path;
    for(unsigned int i=0;i<data->size();i++)
    {
        pointObject[QPointData::keyType] = (int)data->sample(i).type();
        pointObject[QPointData::keyX] = (int)(data->sample(i).x()*100);
        pointObject[QPointData::keyY] = (int)(data->sample(i).y()*100);
        pointObject[QPointData::keyAngle] = (int)(data->sample(i).phi()*1000);
        pointObject[QPointData::keyTag] = (int)data->sample(i).id();
        pointObject[QPointData::keyNetNum] = (int)data->sample(i).rnetId();
        for(int netId=0;netId<10;netId++)
        {
            pointObject[QPointData::keyNet[netId]] = (int)data->sample(i).net(netId);
        }

        pointArrayPath.append(pointObject);
    }
    keyPoint[QPointData::keyPath] = pointArrayPath;

    QJsonDocument jsonDoc(keyPoint);

    jsonFile.write(jsonDoc.toJson());

    return true;
}

bool QTrackClient::saveNormalToJsonFile(QCurveDataCus *data,const QString &fileName)
{
    // Open file as a JSON file
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray pointArray;
    QJsonObject pointObject;
    for(unsigned int i=0;i<data->size();i++)
    {
        pointObject[QPointData::keyType] = (int)data->sample(i).type();
        pointObject[QPointData::keyX] = (int)(data->sample(i).x()*100);
        pointObject[QPointData::keyY] = (int)(data->sample(i).y()*100);
        pointObject[QPointData::keyAngle] = (int)(data->sample(i).phi()*1000);
        pointObject[QPointData::keyTag] = (int)data->sample(i).id();
        pointObject[QPointData::keyNetNum] = (int)data->sample(i).rnetId();
        for(int netId=0;netId<10;netId++)
        {
            pointObject[QPointData::keyNet[netId]] = (int)data->sample(i).net(netId);
        }

        pointArray.append(pointObject);
    }
    QJsonObject keyPoint;

    keyPoint[QPointData::keyNormal] = pointArray;

    QJsonDocument jsonDoc(keyPoint);

    jsonFile.write(jsonDoc.toJson());

    return true;
}

bool QTrackClient::downloadPath(QCurveDataCus *data)
{
    int type;
    int x,y,phi;
    int id;
    int net[10];
    int pathPointNum=data->size();
    if(isWritable()==false) return false;
    for(int i=0;i<pathPointNum;i++)
    {
        type = (int)data->sample(i).type();
        x = (int)(data->sample(i).x()*100);
        y = (int)(data->sample(i).y()*100);
        phi = (int)(data->sample(i).phi()*1000);
        id = (int)data->sample(i).id();
        for(int netId=0;netId<10;netId++)
        {
            net[netId] = (int)data->sample(i).net(netId);
        }
        QByteArray dataWrite;
        QString str;
        str.sprintf("P,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",pathPointNum,i,id,x,y,phi,
                    net[0], net[1], net[2], net[3], net[4], net[5], net[6], net[7], net[8], net[9]);
        dataWrite=str.toLatin1();
        if(id<100000)
        write(dataWrite,dataWrite.size());
    }
    return true;
}

bool QTrackClient::downloadPath2(QCurveDataCus *data)
{
    int type;
    int x,y,phi;
    int id;
    int net[10];
    int pathPointNum=data->size();
    if(isWritable()==false) return false;
    int index=0;
    int maxId=0;
    int validNum=0;
    for( int i=0;i<pathPointNum;i++)
    {
        id = (int)data->sample(i).id();
        if(maxId<id&&id<100000)
        {
            maxId=id;
        }
        if(id<100000)
        {
            validNum++;
        }
    }
    for( int j=1;j<=maxId;j++)
    {
        for( int i=0;i<pathPointNum;i++)
        {

            id = (int)data->sample(i).id();

            if(id==j)
            {
                type = (int)data->sample(i).type();
                x = (int)(data->sample(i).x()*100);
                y = (int)(data->sample(i).y()*100);
                phi = (int)(data->sample(i).phi()*1000);

                for(int netId=0;netId<10;netId++)
                {
                    net[netId] = (int)data->sample(i).net(netId);
                }
                QByteArray dataWrite;
                QString str;

                if(type==QwtPointCus::Dest)
                {
                    str.sprintf("D%d,%d,%d,%.3f,%.3f,%.3f\n",validNum,index,id,data->sample(i).x(),data->sample(i).y(),data->sample(i).phi());
                }
                else if(type==QwtPointCus::Label)
                {
                    str.sprintf("L%d,%d,%d,%.3f,%.3f,%.3f\n",validNum,index,id,data->sample(i).x(),data->sample(i).y(),data->sample(i).phi());
                }
                else if(type==QwtPointCus::PathPoint)
                {
                    str.sprintf("P%d,%d,%d,%.3f,%.3f,%.3f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",validNum,index,id,data->sample(i).x(),data->sample(i).y(),data->sample(i).phi(),
                                net[0], net[1], net[2], net[3], net[4], net[5], net[6], net[7], net[8], net[9]);
                }
                index++;
                dataWrite=str.toLatin1();
                write(dataWrite,dataWrite.size());
                break;
            }
        }
    }
    return true;
}

/*
bool QTrackClient::downloadPath2(QCurveDataCus *data)
{
    int type;
    int x,y,phi;
    int id;
    int net[10];
    int pathPointNum=data->size();
    if(isWritable()==false) return false;
    int index=0;
    int maxId=100;
    int validNum=0;
    for(unsigned int i=0;i<pathPointNum;i++)
    {
        id = (int)data->sample(i).id();
        if(maxId<id&&id<100000)
        {
            maxId=id;
        }
        if(id<100000&&id>200)
        {
            validNum++;
        }
    }
    for(unsigned int j=1;j<=maxId;j++)
    {
        for(unsigned int i=0;i<pathPointNum;i++)
        {

            id = (int)data->sample(i).id();

            if(id==j&&id>200)
            {
                type = (int)data->sample(i).type();
                x = (int)(data->sample(i).x()*100);
                y = (int)(data->sample(i).y()*100);
                phi = (int)(data->sample(i).phi()*100);

                for(int netId=0;netId<10;netId++)
                {
                    net[netId] = (int)data->sample(i).net(netId);
                }
                QByteArray dataWrite;
                QString str;

                if(type==QwtPointCus::Dest)
                {
                    str.sprintf("@%d,%d,%d,%d,%d,%d,1111\n",j,1,1,x,y,phi);
                }
                else if(type==QwtPointCus::Label)
                {
                    str.sprintf("L%d,%d,%d,%.3f,%.3f,%.3f\n",validNum,index,id,data->sample(i).x()*100,data->sample(i).y()*100,data->sample(i).phi()*100);
                }
                else if(type==QwtPointCus::PathPoint)
                {
                    str.sprintf("@24,%d,%d,%d,%d,%d,2222\n",validNum,id-200,x,y,phi);
                }
                index++;
                dataWrite=str.toLatin1();
                write(dataWrite,dataWrite.size());
                break;
            }
        }
    }
    return true;
}
*/
bool QTrackClient::downloadPathEnd()
{
    QByteArray dataWrite;
    QString str;
    str.sprintf("#END\n");
    dataWrite=str.toLatin1();
    write(dataWrite,dataWrite.size());
    return true;
}

bool QTrackClient::downloadDest(QCurveDataCus *data)
{
    int type;
    int x,y,phi;
    int id;
    int net[10];
    int pathPointNum=data->size();

    if(isWritable()==false) return false;
    for( int i=0;i<pathPointNum;i++)
    {
        type = (int)data->sample(i).type();
        x = (int)(data->sample(i).x()*100);
        y = (int)(data->sample(i).y()*100);
        phi = (int)(data->sample(i).phi()*1000);
        id = (int)data->sample(i).id();
        for(int netId=0;netId<10;netId++)
        {
            net[netId] = (int)data->sample(i).net(netId);
        }
        QByteArray dataWrite;
        QString str;
        str.sprintf("D,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",pathPointNum,i,id,x,y,phi,
                    net[0], net[1], net[2], net[3], net[4], net[5], net[6], net[7], net[8], net[9]);
        dataWrite=str.toLatin1();
        write(dataWrite,dataWrite.size());
    }
    return true;
}



bool QTrackClient::sendDeletePoint(QwtPointCus point)
{
    QByteArray dataWrite;
    QString str;
    if(isWritable()==false) return false;
    str.sprintf("$DEL%5d\n",point.id());
    dataWrite=str.toLatin1();
    write(dataWrite,dataWrite.size());
    return true;
}

bool QTrackClient::sendUpdateLabel()
{
    QByteArray dataWrite;
    QString str;
    if(isWritable()==false) return false;
    str.sprintf("$UDL%5d\n",0);
    dataWrite=str.toLatin1();
    write(dataWrite,dataWrite.size());
    return true;
}

bool QTrackClient::sendAddDest(int id)
{
    QByteArray dataWrite;
    QString str;
    if(isWritable()==false) return false;
    str.sprintf("$ADD%5d\n",id);
    dataWrite=str.toLatin1();
    write(dataWrite,dataWrite.size());
    return true;
}


bool QTrackClient::saveToTxtFile(QCurveDataCus *dest,QCurveDataCus *label,QCurveDataCus *path,const QString &fileName)
{
    // Open file as a TXT file
    QFile txtFile(fileName);
    if (!txtFile.open(QIODevice::WriteOnly| QIODevice::Text)) {
        return false;
    }
    QCurveDataCus *data;
    QTextStream in(&txtFile);


    data=dest;
    int type;
    int x,y,phi;
    int id;
    int net[10];
    int PointNum=data->size();
    int index=0;
    int maxId=0;
    int validNum=0;
    for( int i=0;i<PointNum;i++)
    {
        id = (int)data->sample(i).id();
        if(maxId<id&&validId(id))
        {
            maxId=id;
        }
        if(validId(id))
        {
            validNum++;
        }
    }
    for( int j=1;j<=maxId;j++)
    {
        for( int i=0;i<PointNum;i++)
        {

            id = (int)data->sample(i).id();

            if(id==j)
            {
                type = (int)data->sample(i).type();
                x = (int)(data->sample(i).x()*100);
                y = (int)(data->sample(i).y()*100);
                phi = (int)(data->sample(i).phi()*1000);

                for(int netId=0;netId<10;netId++)
                {
                    net[netId] = (int)data->sample(i).net(netId);
                }
                QByteArray dataWrite;
                QString str;

                str.sprintf("D%d,%d,%d,%.3f,%.3f,%.3f\n",validNum,index,id,data->sample(i).x(),data->sample(i).y(),data->sample(i).phi());

                index++;
                dataWrite=str.toLatin1();
                in<<dataWrite;
            }
        }
    }

    data=label;
    PointNum=data->size();
    maxId=0;
    validNum=0;
    index=0;
    for( int i=0;i<PointNum;i++)
    {
        id = (int)data->sample(i).id();
        if(maxId<id&&validId(id))
        {
            maxId=id;
        }
        if(validId(id))
        {
            validNum++;
        }
    }

    for( int i=0;i<PointNum;i++)
    {
        type = (int)data->sample(i).type();
        x = (int)(data->sample(i).x()*100);
        y = (int)(data->sample(i).y()*100);
        phi = (int)(data->sample(i).phi()*1000);
        id = (int)data->sample(i).id();

        if(!validId(id)) continue;
        QByteArray dataWrite;
        QString str;
        str.sprintf("L%d,%d,%d,%.3f,%.3f,%.3f\n",validNum,index,id,data->sample(i).x(),data->sample(i).y(),data->sample(i).phi());
        dataWrite=str.toLatin1();
        in<<dataWrite;
        index++;
    }

 /////////////////////////////////////////////
    data=path;
    PointNum=data->size();
    maxId=0;
    validNum=0;
    index=0;
    for( int i=0;i<PointNum;i++)
    {
        id = (int)data->sample(i).id();
        if(maxId<id&&validId(id))
        {
            maxId=id;
        }
        if(validId(id))
        {
            validNum++;
        }
    }
    for( int j=1;j<=maxId;j++)
    {
        for( int i=0;i<PointNum;i++)
        {

            id = (int)data->sample(i).id();

            if(id==j)
            {
                type = (int)data->sample(i).type();
                x = (int)(data->sample(i).x()*100);
                y = (int)(data->sample(i).y()*100);
                phi = (int)(data->sample(i).phi()*1000);

                for(int netId=0;netId<10;netId++)
                {
                    net[netId] = (int)data->sample(i).net(netId);
                }
                QByteArray dataWrite;
                QString str;

                str.sprintf("P%d,%d,%d,%.3f,%.3f,%.3f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",validNum,index,id,data->sample(i).x(),data->sample(i).y(),data->sample(i).phi(),
                            net[0], net[1], net[2], net[3], net[4], net[5], net[6], net[7], net[8], net[9]);

                index++;
                dataWrite=str.toLatin1();
                in<<dataWrite;

            }
        }
    }
    txtFile.close();
    return true;
}
