#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include "qmainplot.h"
#include "qtrackclient.h"
#include "canvaspicker.h"
#include "qcurvedatacus.h"
#include <qwt_point_cus.h>
#include "dialogoutputoption.h"
#include "qtrackserver.h"
#include "fortuneserver.h"
#include "fortuneserver.h"
#include "dialogserver.h"
#include "robot.h"
#include "QComboBox"
#include "twowayregion.h"
#include "control.h"
#include "editshapeitem.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include "myserialport.h"
class QwtPointCus;
class QCurveDataCus;
class DialogOutputOption;
class ClientSocketList;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private:
    inline void initToolBar();
    inline void initDialog();
    inline QWidget *initToolBarFile(QToolBar *toolBar);
    inline QWidget *initToolBarNetwork(QToolBar *toolBar);
    inline void initCentralWidget();
    inline void initStatusBar();
    inline void initTrackClient();
    inline void initTrackServer();
    inline void initManager();
    inline QWidget *initToolBarDrawing(QToolBar *toolBar);
    inline QWidget *initToolBarPoint(QToolBar *toolBar);
    inline QWidget *initToolBarShapeItem(QToolBar *toolBar);
    void loadInitFile();
    void initComPort();
private:
    // Toolbar items
    QLineEdit    *lineEditHost;
    QLineEdit    *lineEditPort;
    QPushButton  *pushButtonConnect;
    // Status Bar items
    QLabel       *labelStatus;
    // Plot widget
    QMainPlot    *mainPlotLive;
    // TCP socket
    QTrackClient *trackClient;
    // TCP server
    QTrackServer *trackServer;
    //manager
    Manager *manager;
    //drawing
    QPushButton  *pushButtonNoDrawing;
    QPushButton  *pushButtonShiftPoint;
    QPushButton  *pushButtonDrawLine;
    QPushButton  *pushButtonDeleteLine;
    QPushButton  *pushButtonConvert;
    QPushButton  *pushButtonAutoConnect;
    QPushButton  *pushButtonDownLoad;
    QPushButton  *pushButtonOutputOption;
    QPushButton  *pushButtonServer;
    CanvasPicker *canvasPicker;
    QCheckBox    *checkBoxConvert;
    QLineEdit    *lineEditAutoConnectDisMin;
    QLineEdit    *lineEditAutoConnectDisMax;

    QLineEdit *lineEditPointType;
    QLineEdit *lineEditPointId;
    QLineEdit *lineEditPointX;
    QLineEdit *lineEditPointY;
    QLineEdit *lineEditPointPhi;
    QPushButton *pushButtonSetPoint;
    QPushButton *pushButtonAddPoint;
    QPushButton *pushButtonDeletePoint;

    QPushButton *pushButtonUpdateLabel;
    QPushButton *pushButtonAddDest;
    DialogOutputOption *dlgOutputOption;
    DialogServer *dlgServer;
    FortuneServer *server;
    //
    QLineEdit *lineEditShapeItemId;
    QLineEdit *lineEditShapeItemType;
    QComboBox *comboBoxShapeItemType;
    QPushButton *pushButtonAddShapeItem;
    QPushButton *pushButtonDeleteShapeItem;

    int m_nTimerId;
    int m_nTimerCnt;
    QString tcpServerIPAddress;
    QString tcpServerIPPort;
    QString serialPortNum;
    MySerialPort *myserialPort;
    bool serialPortInitOK;
    bool useUART;
    QString mapFileName;
private:
    // Button events
    inline void onButtonConnectClicked();
    inline void onButtonDisconnectClicked();
    inline void onNetworkError();
    void onPushButtonConnectClicked();
    void onPushButtonSaveClicked();
    void onPushButtonLoadClicked();
    void onPushButtonLoadNormalClicked();
    void onPushButtonSaveNormalClicked();
    inline void onPushButtonNoDrawClicked();
    inline void onPushButtonShiftPointClicked();
    inline void onPushButtonDrawLineClicked();
    inline void onPushButtonDeleteLineClicked();
    inline void onPushButtonConvertClicked();
    inline void onPushButtonAutoConnectClicked();
    inline void onPushButtonDownloadClicked();
    inline void onPushButtonSetPointClicked();
    inline void onPushButtonAddPointClicked();
    inline void onPushButtonDeletePointClicked();
    inline void onPushButtonOutputOptionClicked();
    inline void onPushButtonServerClicked();

    inline void onPushButtonUpdateLabelClicked();
    inline void onPushButtonAddDestClicked();

    inline void onPushButtonSavePathTxtClicked();
    inline void onPushButtonAddShapeItemClicked();
    inline void onPushButtonDeleteShapeItemClicked();

//QObject::connect: No such slot MainWindow::onShapeItemTypeChanged(int) in mainwindow.cpp:283
    void loadInitServerFile();
    void loadInitComServerFile();
    // Socket events
    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpSocketError(QAbstractSocket::SocketError error);
    // New point
    //void onNewPoint(const int type, const QPointF &point,const double angleValue,const int tagNo);
    void onNewPoint(const QwtPointCus point);
    void onSelectPoint(const QwtPointCus point);
    void onNewConnection(ClientSocketList* list, const QTcpSocket* socket ,ClientSocketList::clientStatus status);
    void onUpdataRobotPathServer(Robot* robot);
    void onUpdataRobotPathServerState(Robot* robot);
    void onNewClientSN(ClientSocketList* list, const QTcpSocket* socket ,int sn);

    void onNewRobotMsg(QString str);
    void onNewPointServer(const QwtPointCus point);
    void timerEvent( QTimerEvent *event );
    void onPushButtonLoadClicked_hand();
private slots:
    void onShapeItemTypeChanged(int index);

};

#endif // MAINWINDOW_H
