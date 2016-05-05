#include "qtrackclient.h"
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


const QString QPointData::keyPoint = "points";
const QString QPointData::keyType = "type";
const QString QPointData::keyX = "x";
const QString QPointData::keyY = "y";
const QString QPointData::keyAngle = "angle";
const QString QPointData::keyTag = "tag";

QTrackClient::QTrackClient(QObject *parent)
    :QTcpSocket(parent)
{
    initTcpSocket();
}

inline void QTrackClient::initTcpSocket()
{
    //
    //
    //
    connect(this, &QTcpSocket::connected,
            this, &onTcpConnected);
    connect(this, &QTcpSocket::disconnected,
            this, &onTcpDisconnected);
    connect(this, &QTcpSocket::readyRead,
            this, &onTcpReadyRead);
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
        qDebug() << Q_FUNC_INFO << __LINE__ << list.size();

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

    int id;
    if (type == normalPointType) {
        id = 0;
    } else if (type == tagPointType){
        id = 1;
    } else if (type == endPointType) {
        id = 2;
    } else {
        return;
    }

    //
    //
    //
    pointData << QPointData(id, xValue, yValue, angleValue, tagNo);

    emit newPoint(id, QPointF(xValue, yValue));
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

inline bool QTrackClient::parseJsonFile(QJsonDocument &json)
{
    // Check the key points
    if (!json.object().contains(QPointData::keyPoint)) {
        return false;
    }

    QJsonArray pointArray = (json.object())[QPointData::keyPoint].toArray();

    for (int i = 0; i < pointArray.size(); i++) {
        QJsonObject obj = pointArray[i].toObject();

        int type  = obj[QPointData::keyType].toInt();
        int x     = obj[QPointData::keyX].toInt();
        int y     = obj[QPointData::keyY].toInt();
        int angle = obj[QPointData::keyAngle].toInt();
        int tag   = obj[QPointData::keyTag].toInt();

        qDebug() << Q_FUNC_INFO << __LINE__ << type << x << y << angle << tag;
    }

    return true;
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

bool QTrackClient::saveToJsonFile(const QString &fileName)
{
    // Open file as a JSON file
    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray pointArray;
    QJsonObject pointObject;
    foreach (QPointData point, pointData) {
        pointObject[QPointData::keyType] = point.type;
        pointObject[QPointData::keyX] = point.x;
        pointObject[QPointData::keyY] = point.y;
        pointObject[QPointData::keyAngle] = point.angle;
        pointObject[QPointData::keyTag] = point.tag;

        pointArray.append(pointObject);
    }

    QJsonObject keyPoint;

    keyPoint[QPointData::keyPoint] = pointArray;

    QJsonDocument jsonDoc(keyPoint);

    jsonFile.write(jsonDoc.toJson());

    return true;
}
