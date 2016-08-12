#ifndef QTRACKCLIENT_H
#define QTRACKCLIENT_H

#include <QTcpSocket>
#include <QPointF>
#include "qcurvedatacus.h"
#include <qwt_point_cus.h>
#include "editshapeitem.h"
class QwtPointCus;
class QCurveDataCus;

class QPointData {
public:
    explicit QPointData() :
        type(0), x(0), y(0), angle(0), tag(0) { }
    QPointData(int _type, int _x, int _y, int _angle, int _tag) :
        type(_type), x(_x), y(_y), angle(_angle), tag(_tag) { }

public:
    int type;
    int x;
    int y;
    int angle;
    int tag;
    int netNum;
public:
    static const QString keyNormal;
    static const QString keyDest;
    static const QString keyLabel;
    static const QString keyPath;
    static const QString keyType;
    static const QString keyX;
    static const QString keyY;
    static const QString keyAngle;
    static const QString keyTag;
    static const QString keyNetNum;

    static const QString keyNet[10];

};

class QTrackClient : public QTcpSocket
{
    Q_OBJECT

public:
    explicit QTrackClient(QObject *parent);

signals:
    void newPoint(const QwtPointCus point);

public:
    bool LoadJsonFile(const QString &filename);
    bool LoadJsonFileNormal(const QString &fileName);
    bool saveToJsonFile_bak(QCurveDataCus *dest,QCurveDataCus *label,QCurveDataCus *path,const QString &fileName);
    bool saveToJsonFile(QCurveDataCus *dest,QCurveDataCus *label,QCurveDataCus *path,QVector<EditShapeItem *> shapeItem, const QString &fileName);
    bool saveNormalToJsonFile(QCurveDataCus *data,const QString &fileName);
    QCurveDataCus* getNormalData();
    QCurveDataCus* getDestData();
    QCurveDataCus* getLabelData();
    QCurveDataCus* getPathData();
    bool downloadPath(QCurveDataCus *data);
    bool downloadDest(QCurveDataCus *data);
    bool sendDeletePoint(QwtPointCus point);
    bool downloadPath2(QCurveDataCus *data);
    bool downloadPathEnd();
    bool sendUpdateLabel();
    bool sendAddDest(int id);
    bool saveToTxtFile(QCurveDataCus *dest,QCurveDataCus *label,QCurveDataCus *path,const QString &fileName);
    bool validId(int id);
public slots:

private:
    inline void initTcpSocket();

private:
    inline void processData(QString &data);
    inline bool parseJsonFile(QJsonDocument &json);
    inline bool parseJsonFileNormal(QJsonDocument &json);
private:
    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpReadyRead();

private:
    QVector<QPointData> d_pointData;
    QCurveDataCus *curveDataLabel;
    QCurveDataCus *curveDataDest;
    QCurveDataCus *curveDataPath;
    QCurveDataCus *curveDataNormal;
    QVector<EditShapeItem *> shapeItemData;
};

#endif // QTRACKCLIENT_H
