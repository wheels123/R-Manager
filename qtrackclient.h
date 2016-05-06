#ifndef QTRACKCLIENT_H
#define QTRACKCLIENT_H

#include <QTcpSocket>
#include <QPointF>


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

public:
    static const QString keyPoint;
    static const QString keyType;
    static const QString keyX;
    static const QString keyY;
    static const QString keyAngle;
    static const QString keyTag;
};

class QTrackClient : public QTcpSocket
{
    Q_OBJECT

public:
    explicit QTrackClient(QObject *parent);

signals:
    void newPoint(const int type, const QPointF &point);

public:
    bool LoadJsonFile(const QString &filename);
    bool saveToJsonFile(const QString &filename);

public slots:

private:
    inline void initTcpSocket();

private:
    inline void processData(QString &data);
    inline bool parseJsonFile(QJsonDocument &json);

private:
    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpReadyRead();

private:
    QVector<QPointData> pointData;
};

#endif // QTRACKCLIENT_H
