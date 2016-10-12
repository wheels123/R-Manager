#include "mainwindow.h"
#include <QToolBar>
#include <QIntValidator>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QFileDialog>
#include <QtWidgets>
#include <QtNetwork>
/*read ini config file*/
#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QString>
#include <QDebug>

//
// Constructor and deconstructor
//
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    dlgOutputOption=NULL;
    dlgServer=NULL;
    server=NULL;
    manager=NULL;
    myserialPort=NULL;
    udpReceiver=NULL;
    serialPortInitOK=false;
    useUART=true;
    loadInitFile();

    initToolBar();

    initCentralWidget();

    initStatusBar();

    initTrackClient();

    initDialog();

    initManager();

    if(useUART)
    {
        initComPort();
    }
    else
    {
        //initTrackServer();
        iniUdpServer();
    }




}

MainWindow::~MainWindow()
{
    if(trackClient)
    {
        trackClient->disconnectFromHost();
    }

    if(dlgOutputOption)
    {
        dlgOutputOption->hide();
        delete dlgOutputOption;
    }
    if(dlgServer)
    {
        dlgServer->hide();
        delete dlgServer;
    }

    if ( m_nTimerId != 0 )
        killTimer(m_nTimerId);
}

//
// Initialization functions
//
inline QWidget *MainWindow::initToolBarDrawing(QToolBar *toolBar)
{
    QWidget *hBoxWidget = new QWidget(toolBar);

    //
    // Create a label and a edit for hostname
    //
    pushButtonNoDrawing = new QPushButton("&禁止操作", hBoxWidget);
    pushButtonNoDrawing->setCheckable(true);

    connect(pushButtonNoDrawing, &QPushButton::clicked,
            this, &MainWindow::onPushButtonNoDrawClicked);

    pushButtonShiftPoint = new QPushButton("&移动点", hBoxWidget);
    pushButtonShiftPoint->setCheckable(true);

    connect(pushButtonShiftPoint, &QPushButton::clicked,
            this, &MainWindow::onPushButtonShiftPointClicked);

    pushButtonDrawLine = new QPushButton("&连线", hBoxWidget);
    pushButtonDrawLine->setCheckable(true);

    connect(pushButtonDrawLine, &QPushButton::clicked,
            this, &MainWindow::onPushButtonDrawLineClicked);

    pushButtonDeleteLine = new QPushButton("&删除连线", hBoxWidget);
    pushButtonDeleteLine->setCheckable(true);

    connect(pushButtonDeleteLine, &QPushButton::clicked,
            this, &MainWindow::onPushButtonDeleteLineClicked);


    checkBoxConvert = new QCheckBox("&使用标签序号");
    checkBoxConvert->setCheckable(true);
    checkBoxConvert->setChecked(true);

    pushButtonConvert = new QPushButton("&转换", hBoxWidget);
    pushButtonConvert->setCheckable(true);

    connect(pushButtonConvert, &QPushButton::clicked,
            this, &MainWindow::onPushButtonConvertClicked);

    pushButtonAutoConnect = new QPushButton("&自动连接", hBoxWidget);
    pushButtonAutoConnect->setCheckable(true);

    connect(pushButtonAutoConnect, &QPushButton::clicked,
            this, &MainWindow::onPushButtonAutoConnectClicked);


    pushButtonDownLoad = new QPushButton("&下载路线", hBoxWidget);
    pushButtonDownLoad->setCheckable(true);

    connect(pushButtonDownLoad, &QPushButton::clicked,
            this, &MainWindow::onPushButtonDownloadClicked);

    pushButtonOutputOption = new QPushButton("&选项", hBoxWidget);
    pushButtonOutputOption->setCheckable(true);

    connect(pushButtonOutputOption, &QPushButton::clicked,
            this, &MainWindow::onPushButtonOutputOptionClicked);

    pushButtonServer = new QPushButton("&服务器", hBoxWidget);
    pushButtonServer->setCheckable(true);

    connect(pushButtonServer, &QPushButton::clicked,
            this, &MainWindow::onPushButtonServerClicked);

    //
    // Create a label and a edit for hostname
    //
    const QString hostName = "2.0";

    lineEditAutoConnectDisMin = new QLineEdit(hBoxWidget);
    lineEditAutoConnectDisMin->setText(hostName);

    lineEditAutoConnectDisMax = new QLineEdit(hBoxWidget);
    lineEditAutoConnectDisMax->setText(hostName);
    QLabel *labelDis = new QLabel("&距离:", hBoxWidget);
    labelDis->setBuddy(lineEditAutoConnectDisMin);

    QLabel *labelIndentA = new QLabel(hBoxWidget);
    QLabel *labelC = new QLabel("-", hBoxWidget);
    //
    // Create a label and a edit for port
    //


    //
    // Add hBoxLayout for all items
    //
    QHBoxLayout *hBoxLayout = new QHBoxLayout(hBoxWidget);


    hBoxLayout->addWidget(pushButtonNoDrawing);
    hBoxLayout->addWidget(pushButtonShiftPoint);
    hBoxLayout->addWidget(pushButtonDrawLine);
    hBoxLayout->addWidget(pushButtonDeleteLine);
    hBoxLayout->addWidget(checkBoxConvert);
    hBoxLayout->addWidget(pushButtonConvert);
    hBoxLayout->addWidget(labelIndentA);
    hBoxLayout->addWidget(labelDis);
    hBoxLayout->addWidget(lineEditAutoConnectDisMin);
    hBoxLayout->addWidget(labelC);
    hBoxLayout->addWidget(lineEditAutoConnectDisMax);
    hBoxLayout->addWidget(pushButtonAutoConnect);
    hBoxLayout->addWidget(pushButtonDownLoad);
    hBoxLayout->addWidget(pushButtonOutputOption);
    hBoxLayout->addWidget(pushButtonServer);

    return hBoxWidget;
}



inline QWidget *MainWindow::initToolBarPoint(QToolBar *toolBar)
{
    QWidget *hBoxWidget = new QWidget(toolBar);

    lineEditPointType = new QLineEdit(hBoxWidget);
    lineEditPointType->setText("");

    lineEditPointId = new QLineEdit(hBoxWidget);
    lineEditPointId->setText("");

    lineEditPointX = new QLineEdit(hBoxWidget);
    lineEditPointX->setText("");

    lineEditPointY = new QLineEdit(hBoxWidget);
    lineEditPointY->setText("");

    lineEditPointPhi = new QLineEdit(hBoxWidget);
    lineEditPointPhi->setText("");

    QLabel *labelType = new QLabel("&类型:", hBoxWidget);
    labelType->setBuddy(lineEditPointType);

    QLabel *labelId = new QLabel("&编号:", hBoxWidget);
    labelId->setBuddy(lineEditPointId);

    QLabel *labelX = new QLabel("&X:", hBoxWidget);
    labelX->setBuddy(lineEditPointX);

    QLabel *labelY = new QLabel("&Y:", hBoxWidget);
    labelY->setBuddy(lineEditPointY);

    QLabel *labelPhi = new QLabel("&Phi:", hBoxWidget);
    labelPhi->setBuddy(lineEditPointPhi);

    pushButtonSetPoint = new QPushButton("&设置", hBoxWidget);
    pushButtonSetPoint->setCheckable(true);

    connect(pushButtonSetPoint, &QPushButton::clicked,
            this, &MainWindow::onPushButtonSetPointClicked);

    pushButtonAddPoint = new QPushButton("&增加点", hBoxWidget);
    pushButtonAddPoint->setCheckable(true);

    connect(pushButtonAddPoint, &QPushButton::clicked,
            this, &MainWindow::onPushButtonAddPointClicked);

    pushButtonDeletePoint = new QPushButton("&删除点", hBoxWidget);
    pushButtonDeletePoint->setCheckable(true);

    connect(pushButtonDeletePoint, &QPushButton::clicked,
            this, &MainWindow::onPushButtonDeletePointClicked);

    pushButtonUpdateLabel = new QPushButton("&更新标签", hBoxWidget);
    pushButtonUpdateLabel->setCheckable(true);

    connect(pushButtonUpdateLabel, &QPushButton::clicked,
            this, &MainWindow::onPushButtonUpdateLabelClicked);

    pushButtonAddDest = new QPushButton("&增加目标点", hBoxWidget);
    pushButtonAddDest->setCheckable(true);

    connect(pushButtonAddDest, &QPushButton::clicked,
            this, &MainWindow::onPushButtonAddDestClicked);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(hBoxWidget);
    hBoxLayout->addWidget(labelType);
    hBoxLayout->addWidget(lineEditPointType);
    hBoxLayout->addWidget(labelId);
    hBoxLayout->addWidget(lineEditPointId);
    hBoxLayout->addWidget(labelX);
    hBoxLayout->addWidget(lineEditPointX);
    hBoxLayout->addWidget(labelY);
    hBoxLayout->addWidget(lineEditPointY);
    hBoxLayout->addWidget(labelPhi);
    hBoxLayout->addWidget(lineEditPointPhi);
    hBoxLayout->addWidget(pushButtonSetPoint);
    hBoxLayout->addWidget(pushButtonAddPoint);
    hBoxLayout->addWidget(pushButtonDeletePoint);
    hBoxLayout->addWidget(pushButtonUpdateLabel);
    hBoxLayout->addWidget(pushButtonAddDest);


    return hBoxWidget;
}


inline QWidget *MainWindow::initToolBarShapeItem(QToolBar *toolBar)
{
    QWidget *hBoxWidget = new QWidget(toolBar);


    lineEditShapeItemId = new QLineEdit(hBoxWidget);
    lineEditShapeItemId->setText("");

    QLabel *labelId = new QLabel("&编号:", hBoxWidget);
    labelId->setBuddy(lineEditShapeItemId);

    comboBoxShapeItemType = new QComboBox(hBoxWidget);
    comboBoxShapeItemType->setEditable(false);
    comboBoxShapeItemType->addItem("OneWay");
    comboBoxShapeItemType->addItem("TwoWay");
    comboBoxShapeItemType->addItem("CrossRoad");
    comboBoxShapeItemType->addItem("T_Road");
    QLabel *labelType = new QLabel("&类型:", hBoxWidget);
    labelType->setBuddy(comboBoxShapeItemType);

    pushButtonAddShapeItem = new QPushButton("&增加区域", hBoxWidget);
    pushButtonAddShapeItem->setCheckable(true);
    pushButtonDeleteShapeItem = new QPushButton("&删除区域", hBoxWidget);
    pushButtonAddShapeItem->setCheckable(true);

    //connect(comboBoxShapeItemType, &QComboBox::currentIndexChanged,
    //        this, &MainWindow::onShapeItemTypeChanged);
    connect(comboBoxShapeItemType, SIGNAL(activated(int)),
            this, SLOT(onShapeItemTypeChanged(int)));
    connect(pushButtonAddShapeItem, &QPushButton::clicked,
            this, &MainWindow::onPushButtonAddShapeItemClicked);
    connect(pushButtonDeleteShapeItem, &QPushButton::clicked,
            this, &MainWindow::onPushButtonDeleteShapeItemClicked);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(hBoxWidget);
    hBoxLayout->addWidget(labelId);
    hBoxLayout->addWidget(lineEditShapeItemId);
    hBoxLayout->addWidget(labelType);
    hBoxLayout->addWidget(comboBoxShapeItemType);
    hBoxLayout->addWidget(pushButtonAddShapeItem);
    hBoxLayout->addWidget(pushButtonDeleteShapeItem);
    return hBoxWidget;
}

inline QWidget *MainWindow::initToolBarFile(QToolBar *toolBar)
{
    //
    QWidget *hBoxWidget = new QWidget(toolBar);

    //
    QPushButton *pushButtonLoad = new QPushButton("&Load", hBoxWidget);

    connect(pushButtonLoad, &QPushButton::clicked,
            this, &MainWindow::onPushButtonLoadClicked);

    QPushButton *pushButtonLoadNormal = new QPushButton("&Load Normal", hBoxWidget);

    connect(pushButtonLoadNormal, &QPushButton::clicked,
            this, &MainWindow::onPushButtonLoadNormalClicked);

    QPushButton *pushButtonSave = new QPushButton("&Save", hBoxWidget);

    connect(pushButtonSave, &QPushButton::clicked,
            this, &MainWindow::onPushButtonSaveClicked);

    QPushButton *pushButtonSaveNormal = new QPushButton("&Save Normal", hBoxWidget);

    connect(pushButtonSaveNormal, &QPushButton::clicked,
            this, &MainWindow::onPushButtonSaveNormalClicked);

    QPushButton *pushButtonSaveToTxt = new QPushButton("&SaveToTxt", hBoxWidget);

    connect(pushButtonSaveToTxt, &QPushButton::clicked,
            this, &MainWindow::onPushButtonSavePathTxtClicked);
    QHBoxLayout *hBoxLayout = new QHBoxLayout(hBoxWidget);

    hBoxLayout->addWidget(pushButtonLoad);
    hBoxLayout->addWidget(pushButtonLoadNormal);
    hBoxLayout->addWidget(pushButtonSave);
    hBoxLayout->addWidget(pushButtonSaveNormal);
    hBoxLayout->addWidget(pushButtonSaveToTxt);
    hBoxLayout->addStretch();

    return hBoxWidget;
}

inline QWidget *MainWindow::initToolBarNetwork(QToolBar *toolBar)
{
    QWidget *hBoxWidget = new QWidget(toolBar);

    //
    // Create a label and a edit for hostname
    //
    const QString hostName = "192.168.1.1";

    lineEditHost = new QLineEdit(hBoxWidget);
    lineEditHost->setText(hostName);

    QLabel *labelHost = new QLabel("&Host :", hBoxWidget);
    labelHost->setBuddy(lineEditHost);

    //
    // Create a label as a indent
    //
    QLabel *labelIndentA = new QLabel(hBoxWidget);

    //
    // Create a label and a edit for port
    //
    const QString hostPort = "9999";

    lineEditPort = new QLineEdit(hBoxWidget);
    lineEditPort->setText(hostPort);
    lineEditPort->setValidator(new QIntValidator(1, 65535, lineEditPort));

    QLabel *labelPort = new QLabel("&Port :", hBoxWidget);
    labelPort->setBuddy(lineEditPort);

    //
    // Create a label as a indent
    //
    QLabel *labelIndentB = new QLabel(hBoxWidget);

    //
    // Create a connect button
    //
    pushButtonConnect = new QPushButton("&Connect", hBoxWidget);
    pushButtonConnect->setCheckable(true);

    connect(pushButtonConnect, &QPushButton::clicked,
            this, &MainWindow::onPushButtonConnectClicked);

    //
    // Add hBoxLayout for all items
    //
    QHBoxLayout *hBoxLayout = new QHBoxLayout(hBoxWidget);

    hBoxLayout->addWidget(labelHost);
    hBoxLayout->addWidget(lineEditHost);
    hBoxLayout->addWidget(labelIndentA);
    hBoxLayout->addWidget(labelPort);
    hBoxLayout->addWidget(lineEditPort);
    hBoxLayout->addWidget(labelIndentB);
    hBoxLayout->addWidget(pushButtonConnect);

    return hBoxWidget;
}

inline void MainWindow::initToolBar()
{
    QToolBar *toolBar = new QToolBar(this);

    QWidget *hBoxWidgetNetwork = initToolBarNetwork(toolBar);
    QWidget *hBoxWidgetFile = initToolBarFile(toolBar);
    QWidget *hBoxWidgetDrawing = initToolBarDrawing(toolBar);

    //
    // Add all these things into toolbar
    //
    toolBar->addWidget(hBoxWidgetNetwork);
    toolBar->addSeparator();
    toolBar->addWidget(hBoxWidgetFile);
    toolBar->addSeparator();
    toolBar->addWidget(hBoxWidgetDrawing);
    //addToolBar(toolBar);
    addToolBar(Qt::BottomToolBarArea,toolBar);

    QToolBar *toolBarPoint = new QToolBar(this);
    QWidget *hBoxWidgetPoint = initToolBarPoint(toolBarPoint);
    toolBarPoint->addWidget(hBoxWidgetPoint);
    addToolBar(toolBarPoint);

    QToolBar *toolBarShape = new QToolBar(this);
    QWidget *hBoxWidgetShape = initToolBarShapeItem(toolBarPoint);
    toolBarShape->addWidget(hBoxWidgetShape);
    addToolBar(toolBarShape);
}

inline void MainWindow::initCentralWidget(void)
{
    mainPlotLive = new QMainPlot(this);
    canvasPicker = new CanvasPicker( mainPlotLive );
    setCentralWidget(mainPlotLive);

    connect(canvasPicker,
            &CanvasPicker::selectPoint,
            this,
            &MainWindow::onSelectPoint);
}

inline void MainWindow::initStatusBar()
{
    QStatusBar *statusBar = new QStatusBar(this);

    labelStatus = new QLabel(statusBar);
    labelStatus->setAlignment(Qt::AlignHCenter);

    labelStatus->setText("Status : Ready to connect");

    statusBar->addWidget(labelStatus);

    setStatusBar(statusBar);
}

inline void MainWindow::initTrackClient()
{
    trackClient = new QTrackClient(this);

    connect(trackClient, &QTrackClient::connected,
            this, &MainWindow::onTcpConnected);
    connect(trackClient, &QTrackClient::disconnected,
            this, &MainWindow::onTcpDisconnected);

    using SocketErrorFunc = void(QTcpSocket::*)(QAbstractSocket::SocketError);
    connect(trackClient,
            static_cast<SocketErrorFunc>(&QAbstractSocket::error),
            this,
            &MainWindow::onTcpSocketError);

    connect(trackClient,
            &QTrackClient::newPoint,
            this,
            &MainWindow::onNewPoint);


}


inline void MainWindow::initTrackServer()
{
    const QString ipAddrHead=tcpServerIPAddress;

    server = new FortuneServer(this);

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    QHostAddress selectAddr;
    // use the first non-localhost IPv4 address
    labelStatus->setText("wait for ip 31.200");
    while(1)
    {
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {

                if(ipAddressesList.at(i).toString().indexOf(ipAddrHead)>=0)
                {
                    selectAddr = ipAddressesList.at(i);
                    ipAddress = ipAddressesList.at(i).toString();
                    break;
                }
            }
        }
        if (!ipAddress.isEmpty())
        {
            bool ok=false;
            quint16 port=9999;
            int tPort= tcpServerIPPort.toInt(&ok,10);
            if(ok)
            {
                port=tPort;
            }
            // if we did not find one, use IPv4 localhost
            if (ipAddress.isEmpty())
                ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
            labelStatus->setText(tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                                    "Run the Fortune Client example now.")
                                 .arg(ipAddress).arg(port));

            if (!server->listen(selectAddr,port)) {
                labelStatus->setText(tr("Unable to start the server: %1.")
                                                            .arg(server->errorString()));
                QThread::sleep(1);
            }
            else
            {
              break;
            }
        }
        QThread::sleep(1);
    }
    /*
    quint16 port=9999;
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    labelStatus->setText(tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                            "Run the Fortune Client example now.")
                         .arg(ipAddress).arg(port));

    if (!server->listen(selectAddr,port)) {
       // QMessageBox::critical(this, tr("Threaded Fortune Server"),
       //                       tr("Unable to start the server: %1.")
       //                       .arg(server->errorString()));
        labelStatus->setText(tr("Unable to start the server: %1.")
                                                    .arg(server->errorString()));

        QThread::sleep(1);
        close();
        return;
    }
    */
    loadInitServerFile();

    connect(server,
            &FortuneServer::newClientSN,
            this,
            &MainWindow::onNewClientSN);

    connect(server,
            &FortuneServer::newConnection,
            this,
            &MainWindow::onNewConnection);

    connect(server,
            &FortuneServer::updataRobotPathServer,
            this,
            &MainWindow::onUpdataRobotPathServer);

    connect(server,
            &FortuneServer::updataRobotPathServerState,
            this,
            &MainWindow::onUpdataRobotPathServerState);

    connect(server,
            &FortuneServer::onNewRobotMsg,
            this,
            &MainWindow::onNewRobotMsg);

    connect(server,
            &FortuneServer::newPointServer,
            this,
            &MainWindow::onNewPointServer);


}

inline void MainWindow::iniUdpServer()
{
    const QString ipAddrHead=tcpServerIPAddress;

    udpReceiver = new UdpReceiver(this);

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    QHostAddress selectAddr;
    // use the first non-localhost IPv4 address
    labelStatus->setText("wait for ip 31.200");
    while(1)
    {
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {

                if(ipAddressesList.at(i).toString().indexOf(ipAddrHead)>=0)
                {
                    selectAddr = ipAddressesList.at(i);
                    ipAddress = ipAddressesList.at(i).toString();
                    break;
                }
            }
        }
        if (!ipAddress.isEmpty())
        {
            bool ok=false;
            quint16 port=9999;
            int tPort= tcpServerIPPort.toInt(&ok,10);
            if(ok)
            {
                port=tPort;
            }
            // if we did not find one, use IPv4 localhost
            if (ipAddress.isEmpty())
                ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
            labelStatus->setText(tr("The udp server is running on\n\nIP: %1\nport: %2\n\n"
                                    "Run the Fortune Client example now.")
                                 .arg(ipAddress).arg(port));

            if (!udpReceiver->init(selectAddr,port)) {
                labelStatus->setText(tr("Unable to start the udp server:"));
                QThread::sleep(1);
            }
            else
            {
              break;
            }
        }
        QThread::sleep(1);
    }
    /*
    quint16 port=9999;
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    labelStatus->setText(tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                            "Run the Fortune Client example now.")
                         .arg(ipAddress).arg(port));

    if (!server->listen(selectAddr,port)) {
       // QMessageBox::critical(this, tr("Threaded Fortune Server"),
       //                       tr("Unable to start the server: %1.")
       //                       .arg(server->errorString()));
        labelStatus->setText(tr("Unable to start the server: %1.")
                                                    .arg(server->errorString()));

        QThread::sleep(1);
        close();
        return;
    }
    */
    loadInitUdpServerFile();

/*
    connect(server,
            &FortuneServer::newClientSN,
            this,
            &MainWindow::onNewClientSN);

    connect(server,
            &FortuneServer::newConnection,
            this,
            &MainWindow::onNewConnection);
*/
    connect(udpReceiver,
            &UdpReceiver::updataRobotPathServer,
            this,
            &MainWindow::onUpdataRobotPathServer);

    connect(udpReceiver,
            &UdpReceiver::updataRobotPathServerState,
            this,
            &MainWindow::onUpdataRobotPathServerState);

    connect(udpReceiver,
            &UdpReceiver::onNewRobotMsg,
            this,
            &MainWindow::onNewRobotMsg);
/*
    connect(server,
            &FortuneServer::newPointServer,
            this,
            &MainWindow::onNewPointServer);

*/
}

//
// Button event
//
inline void MainWindow::onButtonConnectClicked()
{
    pushButtonConnect->setText("Dis&connect");

    trackClient->connectToHost(lineEditHost->text(),
                               lineEditPort->text().toInt());

    if(server)
    {
       server->sendDate();
    }

}

inline void MainWindow::onButtonDisconnectClicked()
{
    trackClient->disconnectFromHost();

    pushButtonConnect->setText("&Connect");
}

inline void MainWindow::onNetworkError()
{
    qDebug() << Q_FUNC_INFO << __LINE__ << pushButtonConnect->isChecked();

    if (pushButtonConnect->isChecked()) {
        pushButtonConnect->setChecked(false);

        pushButtonConnect->setText("&Connect");
    }
}

void MainWindow::onPushButtonConnectClicked()
{
    if (pushButtonConnect->isChecked()) {
        onButtonConnectClicked();
    } else {
        onButtonDisconnectClicked();
    }
}

void MainWindow::onPushButtonSaveClicked()
{
    QString fileName;
    QString fileNameNormal;
    fileName = QFileDialog::getSaveFileName(this,
                                            tr("Save to ..."),
                                            ".",
                                            tr("JSON files (*.json)"));
    if(fileName.isEmpty()) {
        return;
    }

    QStringList strlist=fileName.split(".");
    fileNameNormal = strlist.at(0)+"_normal.json";
    QString message;
    QCurveDataCus *dataDest = mainPlotLive->getDestData();
    QCurveDataCus *dataLabel = mainPlotLive->getLabelData();
    QCurveDataCus *dataPath = mainPlotLive->getPathData();

    QVector<EditShapeItem *> shapeItem = mainPlotLive->getShapeItemData();
    if(dataDest&&dataLabel&&dataPath)
    {
        bool result = trackClient->saveToJsonFile(dataDest,dataLabel,dataPath,shapeItem,fileName);
        if (result) {
            message = "Status : " + fileName + " saved";
        } else {
            message = "Status : failed to save to" + fileName;
        }
    }
    else
    {
      message = "Status : failed to save [no data]";
    }

    message+="  ";
    QCurveDataCus *dataNormal = mainPlotLive->getNormalData();
    if(dataNormal)
    {
        bool result = trackClient->saveNormalToJsonFile(dataNormal,fileNameNormal);
        if (result) {
            message += "Status : " + fileNameNormal + " saved";
        } else {
            message += "Status : failed to save to" + fileNameNormal;
        }
    }
    else
    {
      message += "Status : failed to save [no data]";
    }
    labelStatus->setText(message);
}

void MainWindow::onPushButtonSaveNormalClicked()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
                                            tr("Save to ..."),
                                            ".",
                                            tr("JSON files (*.json)"));
    if(fileName.isEmpty()) {
        return;
    }

    QString message;
    QCurveDataCus *dataNormal = mainPlotLive->getNormalData();
    if(dataNormal)
    {
        bool result = trackClient->saveNormalToJsonFile(dataNormal,fileName);
        if (result) {
            message = "Status : " + fileName + " saved";
        } else {
            message = "Status : failed to save to" + fileName;
        }
    }
    else
    {
      message = "Status : failed to save [no data]";
    }

    labelStatus->setText(message);
}
void MainWindow::onPushButtonSavePathTxtClicked()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
                                            tr("Save to ..."),
                                            ".",
                                            tr("TXT files (*.txt)"));
    if(fileName.isEmpty()) {
        return;
    }

    QString message;
    QCurveDataCus *dataDest = mainPlotLive->getDestData();
    QCurveDataCus *dataLabel = mainPlotLive->getLabelData();
    QCurveDataCus *dataPath = mainPlotLive->getPathData();
    if(dataDest&&dataLabel&&dataPath)
    {
        bool result = trackClient->saveToTxtFile(dataDest,dataLabel,dataPath,fileName);
        if (result) {
            message = "Status : " + fileName + " saved";
        } else {
            message = "Status : failed to save to" + fileName;
        }
    }
    else
    {
      message = "Status : failed to save [no data]";
    }


    labelStatus->setText(message);
}


void MainWindow::onPushButtonLoadClicked_hand()
{
    QString fileName;
    QString fileNameNormal;
    fileName = mapFileName;
    if(fileName.isEmpty()) {
        return;
    }
    QStringList strlist=fileName.split(".");
    fileNameNormal = strlist.at(0)+"_normal.json";
    QString message;
    bool result = trackClient->LoadJsonFile(fileName);
    if (result) {
        message = "Status : " + fileName + " loaded";
        QCurveDataCus *datLabel = trackClient->getLabelData();
        QCurveDataCus *dataDest = trackClient->getDestData();
        QCurveDataCus *dataPath = trackClient->getPathData();
        if(datLabel!=NULL)
        {
            mainPlotLive->loadData(datLabel,QwtPointCus::Label);
        }
        else
        {
            message = "Status : failed to load Label " + fileName;
        }

        if(dataDest!=NULL)
        {
            mainPlotLive->loadData(dataDest,QwtPointCus::Dest);
        }
        else
        {
            message = "Status : failed to load Dest " + fileName;
        }

        if(dataDest!=NULL)
        {
            mainPlotLive->loadData(dataPath,QwtPointCus::PathPoint);
        }
        else
        {
            message = "Status : failed to load Path " + fileName;
        }

    } else {
        message = "Status : failed to load " + fileName;
    }

    labelStatus->setText(message);

    message+="  ";
    result = trackClient->LoadJsonFileNormal(fileNameNormal);
    if (result) {
        message += "Status : " + fileNameNormal + " loaded";
        QCurveDataCus *data = trackClient->getNormalData();
        if(data!=NULL)
        {
            mainPlotLive->loadData(data,QwtPointCus::Normal);
            //mainPlotLive->loadData_t(data);

        }
        else
        {
            //message = "Status : failed to load normal " + fileNameNormal;
        }
    } else {
        message += "Status : failed to load " + fileNameNormal;
    }

    labelStatus->setText(message);

    mainPlotLive->LoadJsonFile(fileName);
}

void MainWindow::onPushButtonLoadClicked()
{
    QString fileName;
    QString fileNameNormal;
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open ..."),
                                            ".",
                                            tr("JSON files (*.json)"));
    if(fileName.isEmpty()) {
        return;
    }
    QStringList strlist=fileName.split(".");
    fileNameNormal = strlist.at(0)+"_normal.json";
    QString message;
    bool result = trackClient->LoadJsonFile(fileName);
    if (result) {
        message = "Status : " + fileName + " loaded";
        QCurveDataCus *datLabel = trackClient->getLabelData();
        QCurveDataCus *dataDest = trackClient->getDestData();
        QCurveDataCus *dataPath = trackClient->getPathData();
        if(datLabel!=NULL)
        {
            mainPlotLive->loadData(datLabel,QwtPointCus::Label);
        }
        else
        {
            message = "Status : failed to load Label " + fileName;
        }

        if(dataDest!=NULL)
        {
            mainPlotLive->loadData(dataDest,QwtPointCus::Dest);
        }
        else
        {
            message = "Status : failed to load Dest " + fileName;
        }

        if(dataDest!=NULL)
        {
            mainPlotLive->loadData(dataPath,QwtPointCus::PathPoint);
        }
        else
        {
            message = "Status : failed to load Path " + fileName;
        }

    } else {
        message = "Status : failed to load " + fileName;
    }

    labelStatus->setText(message);

    message+="  ";
    result = trackClient->LoadJsonFileNormal(fileNameNormal);
    if (result) {
        message += "Status : " + fileNameNormal + " loaded";
        QCurveDataCus *data = trackClient->getNormalData();
        if(data!=NULL)
        {
            mainPlotLive->loadData(data,QwtPointCus::Normal);
            //mainPlotLive->loadData_t(data);

        }
        else
        {
            //message = "Status : failed to load normal " + fileNameNormal;
        }
    } else {
        message += "Status : failed to load " + fileNameNormal;
    }

    labelStatus->setText(message);

    mainPlotLive->LoadJsonFile(fileName);
}

void MainWindow::onPushButtonLoadNormalClicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open ..."),
                                            ".",
                                            tr("JSON files (*.json)"));
    if(fileName.isEmpty()) {
        return;
    }

    QString message;
    bool result = trackClient->LoadJsonFileNormal(fileName);
    if (result) {
        message = "Status : " + fileName + " loaded";
        QCurveDataCus *data = trackClient->getNormalData();
        if(data!=NULL)
        {
            mainPlotLive->loadData(data,QwtPointCus::Normal);
        }
        else
        {
            message = "Status : failed to load normal " + fileName;
        }
    } else {
        message = "Status : failed to load " + fileName;
    }

    labelStatus->setText(message);
}
//
// Socket error event
//
void MainWindow::onTcpConnected()
{
    labelStatus->setText("Status : connected");
    trackClient->flush();
}

void MainWindow::onTcpDisconnected()
{
    qDebug() << Q_FUNC_INFO << __LINE__;

    trackClient->close();

    labelStatus->setText("Status : disconnected");

    onNetworkError();
}

//
// Socket error event
//
void MainWindow::onTcpSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    trackClient->close();

    labelStatus->setText("Status : " + trackClient->errorString());

    onNetworkError();
}

//
// New point event
//
void MainWindow::onNewPoint(const QwtPointCus point)
{
    mainPlotLive->appendPoint(point);
}


void MainWindow::onSelectPoint(const QwtPointCus point)
{
    QString s_type="";

    if(point.type()==QwtPointCus::Normal)
        s_type="普通";
    else if(point.type()==QwtPointCus::Label)
        s_type="标签";
    else if(point.type()==QwtPointCus::Dest)
        s_type="目标点";
    else if(point.type()==QwtPointCus::PathPoint)
        s_type="路径点";

    QString s_id;
    s_id=QString::number((int)(point.id()), 10);
    QString s_x;
    s_x=QString::number((double)(point.x()),'f',2);
    QString s_y;
    s_y=QString::number((double)(point.y()),'f',2);
    QString s_phi;
    s_phi=QString::number((double)(point.phi()),'f',3);

    if(point.type()==QwtPointCus::NoType)
    {
        lineEditPointType->setText("");
        lineEditPointId->setText("");
        lineEditPointX->setText("");
        lineEditPointY->setText("");
        lineEditPointPhi->setText("");
    }
    else
    {
        lineEditPointType->setText(s_type);
        lineEditPointId->setText(s_id);
        lineEditPointX->setText(s_x);
        lineEditPointY->setText(s_y);
        lineEditPointPhi->setText(s_phi);
    }


}

void MainWindow::onPushButtonShiftPointClicked()
{
    pushButtonNoDrawing->setChecked(false);
    pushButtonShiftPoint->setChecked(true);
    pushButtonDrawLine->setChecked(false);
    pushButtonDeleteLine->setChecked(false);
    if(canvasPicker!=NULL)
        canvasPicker->setMode(CanvasPicker::ShiftPoint);
}

void MainWindow::onPushButtonDrawLineClicked()
{
    pushButtonNoDrawing->setChecked(false);
    pushButtonShiftPoint->setChecked(false);
    pushButtonDrawLine->setChecked(true);
    pushButtonDeleteLine->setChecked(false);
    if(canvasPicker!=NULL)
        canvasPicker->setMode(CanvasPicker::DrawLine);
}

void MainWindow::onPushButtonDeleteLineClicked()
{
    pushButtonNoDrawing->setChecked(false);
    pushButtonShiftPoint->setChecked(false);
    pushButtonDrawLine->setChecked(false);
    pushButtonDeleteLine->setChecked(true);
    if(canvasPicker!=NULL)
        canvasPicker->setMode(CanvasPicker::DeleteLine);
}

void MainWindow::onPushButtonNoDrawClicked()
{
    pushButtonNoDrawing->setChecked(true);
    pushButtonShiftPoint->setChecked(false);
    pushButtonDrawLine->setChecked(false);
    pushButtonDeleteLine->setChecked(false);
    if(canvasPicker!=NULL)
        canvasPicker->setMode(CanvasPicker::NoDrawing);
}

void MainWindow::onPushButtonConvertClicked()
{
    pushButtonConnect->setChecked(true);
    mainPlotLive->addReceiveDataToLoadData(checkBoxConvert->isChecked());
}

void MainWindow::onPushButtonAutoConnectClicked()
{
    bool toDoubleResultMin=false;
    bool toDoubleResultMax=false;
    pushButtonAutoConnect->setChecked(true);
    double disMin = lineEditAutoConnectDisMin->text().toDouble(&toDoubleResultMin);
    double disMax = lineEditAutoConnectDisMax->text().toDouble(&toDoubleResultMax);
    if(toDoubleResultMin==true&&toDoubleResultMax==true)
     mainPlotLive->autoConnect(disMin,disMax);
}

/*
    QCurveDataCus *dataDest = mainPlotLive->getDestData();
    QCurveDataCus *dataLabel = mainPlotLive->getLabelData();
    QCurveDataCus *dataPath = mainPlotLive->getPathData();
    if(dataDest&&dataLabel&&dataPath)
    {
        bool result = trackClient->saveToJsonFile(dataDest,dataLabel,dataPath,fileName);
        if (result) {
            message = "Status : " + fileName + " saved";
        } else {
            message = "Status : failed to save to" + fileName;
        }
    }
    else
    {
      message = "Status : failed to save [no data]";
    }
 * */
void MainWindow::onPushButtonDownloadClicked()
{
    QCurveDataCus *dataDest = mainPlotLive->getDestData();
    QCurveDataCus *dataPath = mainPlotLive->getPathData();
    QCurveDataCus *dataLabel = mainPlotLive->getLabelData();
    bool retDest=false;
    bool retPath=false;
    QString message="";
    if(dataPath&&dataDest)
    {
        pushButtonConnect->setChecked(true);
        //retDest = trackClient->downloadDest(dataDest);
        retPath = trackClient->downloadPath2(dataLabel);
        retPath = trackClient->downloadPath2(dataDest);
        retPath = trackClient->downloadPath2(dataPath);
        //retPath = trackClient->downloadPath2(dataLabel);
        retPath = trackClient->downloadPathEnd();

    }

    if(retDest)
    {
        message=" Status : download dest succeed";
    }
    else
    {
        message=" Status : failed to download dest";
    }


    if(retPath)
    {
        message+=" Status : download path succeed";
    }
    else
    {
        message+=" Status : failed to download path";
    }
    labelStatus->setText(message);

}

void MainWindow::onPushButtonSetPointClicked()
{
    pushButtonSetPoint->setChecked(true);

    QwtPointCus point(QwtPointCus::Normal,0,0,0,0);
    bool ok;
    point = canvasPicker->getSelectedPoint(&ok);
    if(ok==false) return;

    int id= lineEditPointId->text().toInt(&ok,10);
    if(ok)
    {
        point.rid()=id;
    }
    double x= lineEditPointX->text().toDouble(&ok);
    if(ok)
    {
        point.rx()=x;
    }
    double y= lineEditPointY->text().toDouble(&ok);
    if(ok)
    {
        point.ry()=y;
    }
    double phi= lineEditPointPhi->text().toDouble(&ok);
    if(ok)
    {
        point.rphi()=phi;
    }
    canvasPicker->setSelectedPoint(point);
}

void MainWindow::onPushButtonAddPointClicked()
{
    pushButtonSetPoint->setChecked(true);

    QwtPointCus point(QwtPointCus::Normal,0,0,0,0);
    bool ok;
    point = canvasPicker->getSelectedPoint(&ok);

    QString s_type="";

    QwtPointCus::PointStyle type=QwtPointCus::PathPoint;

    if(ok==false)
    {
        s_type = lineEditPointType->text();
        if(s_type=="普通"||s_type=="n")
        {
            type=QwtPointCus::Normal;
        }
        else if(s_type=="标签"||s_type=="l")
        {
            type=QwtPointCus::Label;
        }
        else if(s_type=="目标点"||s_type=="d")
        {
            type=QwtPointCus::Dest;
        }
        else if(s_type=="路径点"||s_type=="p")
        {
            type=QwtPointCus::PathPoint;
        }
        else
        {
            type=QwtPointCus::PathPoint;
        }
        point.rtype()=type;
    }


    int id= lineEditPointId->text().toInt(&ok,10);
    if(ok)
    {
        point.rid()=id;
    }
    double x= lineEditPointX->text().toDouble(&ok);
    if(ok)
    {
        point.rx()=x;
    }
    double y= lineEditPointY->text().toDouble(&ok);
    if(ok)
    {
        point.ry()=y;
    }
    double phi= lineEditPointPhi->text().toDouble(&ok);
    if(ok)
    {
        point.rphi()=phi;
    }

    point.rnetId()=0;
    for(int i=0;i<point.netSize();i++)
    {
        point.rnet(i)=0;
    }
    mainPlotLive->appendPoint(point);
}


void MainWindow::onPushButtonDeletePointClicked()
{
    pushButtonDeletePoint->setChecked(true);
    QwtPointCus point(QwtPointCus::Normal,0,0,0,0);
    bool ok;
    point = canvasPicker->getSelectedPoint(&ok);
    if(ok==false) return;

    if(point.rid()>100000) return;
    trackClient->sendDeletePoint(point);

    point.rid()=point.id()+100000;
    point.rx()=point.x()+500;
    canvasPicker->setSelectedPoint(point);
}

void MainWindow::onPushButtonUpdateLabelClicked()
{
    pushButtonUpdateLabel->setChecked(true);

    //trackClient->sendUpdateLabel();
    if(server)
    {
        server->sendUpdateLabel();
    }

}

void MainWindow::onPushButtonAddDestClicked()
{
    pushButtonAddDest->setChecked(true);

    bool ok;
    int id= lineEditPointId->text().toInt(&ok,10);
    if(ok)
    {
        trackClient->sendAddDest(id);
    }


}

void MainWindow::onPushButtonOutputOptionClicked()
{
    labelStatus->setText("Status : hello");
    if(dlgOutputOption==NULL)
    {
        dlgOutputOption = new DialogOutputOption();
    }
    if(dlgOutputOption->isVisible())
    {
        dlgOutputOption->hide();
    }
    else
    {
        dlgOutputOption->show();
    }

}

void MainWindow::onPushButtonServerClicked()
{
    //labelStatus->setText("Status : server");
    if(dlgServer==NULL)
    {
        dlgServer = new DialogServer();
    }
    if(dlgServer->isVisible())
    {
        dlgServer->hide();
    }
    else
    {
        dlgServer->show();
    }

}
void MainWindow::initDialog()
{
    dlgOutputOption = new DialogOutputOption();
    dlgServer = new DialogServer();
}

void MainWindow::onNewConnection(ClientSocketList *list, const QTcpSocket* socket ,ClientSocketList::clientStatus status)
{
    labelStatus->setText("S IP:"+socket->peerAddress().toString()+"  PORT:"+QString::number(socket->peerPort(),10)+" status "+QString::number(status,10));
    if(dlgServer)
    {
        QStringList slist;
        int n;
        n=list->getClientVecSize();
        labelStatus->setText("N "+QString::number(n,10)+" ");
        for(int i=0;i<n;i++)
        {
            QTcpSocket* skt=list->getClientVecByIndex(i);
            ClientSocketList::clientStatus sts=list->getStatusByIndex(i);
            int sn=list->getSNByIndex(i);
            slist.append("IP:"+skt->peerAddress().toString()+"  PORT:"+QString::number(skt->peerPort(),10)+" status "+QString::number(sts,10)+" sn "+QString::number(sn,10));
            labelStatus->setText(slist.at(0));
        }

        dlgServer->updateListViewClient(slist);
    }
}

void MainWindow::onUpdataRobotPathServer(Robot* robot)
{
    qDebug()<<"onUpdataRobotPathServer ";
    if(dlgServer)
    {
        dlgServer->updateTreeViewPath(robot);
    }
    qDebug()<<"onUpdataRobotPathServer out";
}

void MainWindow::onUpdataRobotPathServerState(Robot* robot)
{
    //qDebug()<<"onUpdataRobotPathServerState ";

    return;
    if(dlgServer)
    {
        dlgServer->updateTreeViewPath(robot);
    }
    if(0)
    if(server)
    {
        QVector<RobotPoint> ra = server->getPose(0);
        QVector<RobotPoint> rb = server->getPose(1);
        mainPlotLive->showPose(ra,0);
        mainPlotLive->showPose(rb,1);

    }

    if(manager)
    {
        qDebug()<<"manager ";
        QCurveDataCus* path = mainPlotLive->getPathData();
        QVector<EditShapeItem *> item = mainPlotLive->getShapeItemData();
        manager->setRegion(item,path,robot);
    }
}


void MainWindow::loadInitServerFile()
{
    QString fileName;
    //fileName = "initserver.json";
    //fileName = "0729  走廊.json";
    fileName = mapFileName;
    if(fileName.isEmpty()) {
        return;
    }
    QString message;
    bool result = trackClient->LoadJsonFile(fileName);
    if (result) {
        message = "loadInitTcpServerFile : " + fileName + " loaded";
        QCurveDataCus *dataPath = trackClient->getPathData();
        if(dataPath!=NULL&&server)
        {
            server->loadData(dataPath,QwtPointCus::PathPoint);
        }
        else
        {
            message = "loadInitTcpServerFile : failed to load Path " + fileName;
        }
        if(dlgServer&&server)
        {
            dlgServer->updateTreeViewMainPath(server->getRobotHandle());
        }

    } else {
        message = "loadInitTcpServerFile : failed to load " + fileName;
    }

    labelStatus->setText(message);


}

void MainWindow::loadInitUdpServerFile()
{
    QString fileName;
    //fileName = "initserver.json";
    //fileName = "0729  走廊.json";
    fileName = mapFileName;
    if(fileName.isEmpty()) {
        return;
    }
    QString message;
    bool result = trackClient->LoadJsonFile(fileName);
    if (result) {
        message = "loadInitTcpServerFile : " + fileName + " loaded";
        QCurveDataCus *dataPath = trackClient->getPathData();
        if(dataPath!=NULL&&udpReceiver)
        {
            udpReceiver->loadData(dataPath,QwtPointCus::PathPoint);
        }
        else
        {
            message = "loadInitTcpServerFile : failed to load Path " + fileName;
        }
        if(dlgServer&&udpReceiver)
        {
            dlgServer->updateTreeViewMainPath(udpReceiver->getRobotHandle());
        }

    } else {
        message = "loadInitTcpServerFile : failed to load " + fileName;
    }

    labelStatus->setText(message);


}

void MainWindow::loadInitComServerFile()
{
    QString fileName;
    //fileName = "initserver.json";
    //fileName = "0729  走廊.json";
    fileName = mapFileName;
    if(fileName.isEmpty()) {
        return;
    }
    QString message;
    bool result = trackClient->LoadJsonFile(fileName);
    if (result) {
        message = "loadInitComServerFile : " + fileName + " loaded";
        QCurveDataCus *dataPath = trackClient->getPathData();
        QCurveDataCus *dataDest = trackClient->getDestData();
        if(dataPath!=NULL&&myserialPort)
        {
            myserialPort->loadData(dataPath,QwtPointCus::PathPoint);
        }
        else
        {
            message = "loadInitComServerFile : failed to load Path " + fileName;
        }

        if(dataDest!=NULL&&myserialPort)
        {
            myserialPort->loadData(dataDest,QwtPointCus::Dest);
        }
        else
        {
            message = "loadInitComServerFile : failed to load Dest " + fileName;
        }
        if(dlgServer&&myserialPort)
        {
            dlgServer->updateTreeViewMainPath(myserialPort->getRobotHandle());
        }

    } else {
        message = "loadInitComServerFile : failed to load " + fileName;
    }

    labelStatus->setText(message);
}

void MainWindow::onNewClientSN(ClientSocketList* list, const QTcpSocket* socket ,int sn)
{
    labelStatus->setText("S IP:"+socket->peerAddress().toString()+"  PORT:"+QString::number(socket->peerPort(),10)+" sn "+QString::number(sn,10));
    if(dlgServer)
    {
        QStringList slist;
        int n;
        n=list->getClientVecSize();
        labelStatus->setText("N "+QString::number(n,10)+" ");
        for(int i=0;i<n;i++)
        {
            QTcpSocket* skt=list->getClientVecByIndex(i);
            ClientSocketList::clientStatus sts=list->getStatusByIndex(i);
            int sn=list->getSNByIndex(i);
            slist.append("IP:"+skt->peerAddress().toString()+"  PORT:"+QString::number(skt->peerPort(),10)+" status "+QString::number(sts,10)+" sn "+QString::number(sn,10));
            labelStatus->setText(slist.at(0));
        }

        dlgServer->updateListViewClient(slist);
    }
}


void MainWindow::onNewRobotMsg(QString str)
{
    labelStatus->setText(str);
}

void MainWindow::onPushButtonAddShapeItemClicked()
{
    bool ok;

    int id= lineEditShapeItemId->text().toInt(&ok,10);
    int index= comboBoxShapeItemType->currentIndex();
    if(ok)
    {
        mainPlotLive->addShapeItem(id,EditShapeItem::ShapeType(index));
    }
    else
    {
       id= mainPlotLive->getNewShapeItemId();
       mainPlotLive->addShapeItem(id,EditShapeItem::ShapeType(index));
    }

    qDebug()<<"Clicked AddShape id:"<<QString::number(id,10)<<" type "<<QString::number(index,10);
}

void MainWindow::onPushButtonDeleteShapeItemClicked()
{

    bool ok;
    int id= lineEditShapeItemId->text().toInt(&ok,10);
    if(ok&&id>=0)
    {
        mainPlotLive->deleteShapeItem(id);
    }
    qDebug()<<"Clicked DeleteShape id:"<<QString::number(id,10);
}

void MainWindow::onShapeItemTypeChanged(int index)
{
    bool ok;

    int id= lineEditShapeItemId->text().toInt(&ok,10);
    //int index= comboBoxShapeItemType->currentIndex();
    if(ok)
    {
        mainPlotLive->addShapeItem(id,EditShapeItem::ShapeType(index));
    }
    else
    {
       id= mainPlotLive->getNewShapeItemId();
       mainPlotLive->addShapeItem(id,EditShapeItem::ShapeType(index));
    }

    qDebug()<<"Changed AddShape id:"<<QString::number(id,10)<<" type "<<QString::number(index,10);
}

inline void MainWindow::initManager()
{
    manager = new Manager();
    m_nTimerId=0;
    m_nTimerCnt=0;
    m_nTimerId = startTimer(200);
    onPushButtonLoadClicked_hand();
}


void MainWindow::onNewPointServer(const QwtPointCus point)
{
    mainPlotLive->appendPoint(point);
}

void MainWindow::timerEvent( QTimerEvent *event )
{
    static int cnt=0;
    m_nTimerCnt++;
    //qDebug( "MainWindow timerEvent");

    if(m_nTimerCnt>2)
    {
        m_nTimerCnt=0;
        if(dlgServer)
        {
            if(server)
            {
                Robot *robot = server->getRobotHandle();
                dlgServer->updateTreeViewPath(robot);
            }
            if(udpReceiver)
            {
                Robot *robot = udpReceiver->getRobotHandle();
                dlgServer->updateTreeViewPath(robot);
            }
            if(myserialPort)
            {
                Robot *robot = myserialPort->getRobotHandle();
                dlgServer->updateTreeViewPath(robot);
            }
        }
        if(myserialPort!=NULL&&serialPortInitOK==false)
        {
            bool ok=myserialPort->init(serialPortNum);
            QString str="initComPort "+serialPortNum+" "+QString::number((int)cnt++, 10);
            labelStatus->setText(str);
            if(ok==true)
            {
                loadInitComServerFile();
                connect(myserialPort,
                        &MySerialPort::updataRobotPathServer,
                        this,
                        &MainWindow::onUpdataRobotPathServer);

                connect(myserialPort,
                        &MySerialPort::updataRobotPathServerState,
                        this,
                        &MainWindow::onUpdataRobotPathServerState);

                connect(myserialPort,
                        &MySerialPort::onNewRobotMsg,
                        this,
                        &MainWindow::onNewRobotMsg);

                connect(myserialPort,
                        &MySerialPort::newPointServer,
                        this,
                        &MainWindow::onNewPointServer);

                QString str="initComPort "+serialPortNum+" success ";
                labelStatus->setText(str);
                serialPortInitOK=true;
            }
        }
    }

    if(manager)
    {
        if(server)
        {
            QVector<QVector<RobotPathPoint>> pose=server->getPose();
            mainPlotLive->showPose(pose);
        }
        if(udpReceiver)
        {
            QVector<QVector<RobotPathPoint>> pose=udpReceiver->getPose();
            mainPlotLive->showPose(pose);
        }
        if(myserialPort)
        {
            QVector<QVector<RobotPathPoint>> pose=myserialPort->getPose();
            mainPlotLive->showPose(pose);
        }
        //qDebug()<<"manager ";
        QCurveDataCus* path = mainPlotLive->getPathData();
        QVector<EditShapeItem *> item = mainPlotLive->getShapeItemData();
        if(server)
        {
            Robot *robot = server->getRobotHandle();
            robot->updateControlNum();
            robot->resetControlValue();
            manager->setRegion(item,path,robot);
        }
        if(udpReceiver)
        {
            Robot *robot = udpReceiver->getRobotHandle();
            robot->updateControlNum();
            robot->resetControlValue();
            manager->setRegion(item,path,robot);
        }
        if(myserialPort)
        {
            Robot *robot = myserialPort->getRobotHandle();
            robot->updateControlNum();
            robot->resetControlValue();
            manager->setRegion(item,path,robot);
        }
    }
}


void MainWindow::loadInitFile()
{
   QSettings *configIniRead = new QSettings("config.ini", QSettings::IniFormat);
   //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
   QString ipAddress = configIniRead->value("/server/ip","192.168.1.").toString();
   QString ipPort = configIniRead->value("/server/port","9999").toString();
   QString comPort = configIniRead->value("/server/com","COM").toString();
   QString wireLessMode = configIniRead->value("/server/mode","UART").toString();
   QString fileName = configIniRead->value("/map/name","map.json").toString();
   //读入入完成后删除指针
   delete configIniRead;
   //打印得到的结果
   qDebug() << ipAddress;
   qDebug() << ipPort;
   qDebug() << comPort;
   qDebug() << wireLessMode;
   qDebug() << fileName;
   tcpServerIPAddress = ipAddress;
   tcpServerIPPort = ipPort;
   serialPortNum = comPort;
   useUART = (wireLessMode == "UART")?true:false;
   mapFileName = fileName;
}


void MainWindow::initComPort()
{
    myserialPort = new MySerialPort();
}
