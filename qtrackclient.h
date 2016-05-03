#ifndef QTRACKCLIENT_H
#define QTRACKCLIENT_H

#include <QTcpSocket>
#include <QPointF>


class QPointData {
public:
    explicit QPointData() :
        x(0), y(0), angle(0), tag(0) { };
    QPointData(int _x, int _y, int _angle, int _tag) :
        x(_x), y(_y), angle(_angle), tag(_tag) { };

public:
    int x;
    int y;
    int angle;
    int tag;

public:
    static const QString keyPoint;
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
    inline bool parseJsonFile(QJsonDocument &json);
    bool LoadJsonFile(const QString &filename);
    bool saveToJsonFile(const QString &filename);

public slots:

private:
    inline void initTcpSocket();

private:
    inline void processData(QString &data);

private:
    void onTcpConnected(void);
    void onTcpDisconnected(void);
    void onTcpReadyRead(void);

private:
    QVector<QPointData> pointData;
};

#endif // QTRACKCLIENT_H
