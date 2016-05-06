#include "mainwindow.h"
#include <QToolBar>
#include <QIntValidator>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QFileDialog>


//
// Constructor and deconstructor
//
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initToolBar();

    initCentralWidget();

    initStatusBar();

    initTrackClient();
}

MainWindow::~MainWindow()
{

}

//
// Initialization functions
//
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

inline QWidget *MainWindow::initToolBarFile(QToolBar *toolBar)
{
    //
    QWidget *hBoxWidget = new QWidget(toolBar);

    //
    QPushButton *pushButtonLoad = new QPushButton("&Load", hBoxWidget);

    connect(pushButtonLoad, &QPushButton::clicked,
            this, &MainWindow::onPushButtonLoadClicked);

    QPushButton *pushButtonSave = new QPushButton("&Save", hBoxWidget);

    connect(pushButtonSave, &QPushButton::clicked,
            this, &MainWindow::onPushButtonSaveClicked);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(hBoxWidget);

    hBoxLayout->addWidget(pushButtonLoad);
    hBoxLayout->addWidget(pushButtonSave);
    hBoxLayout->addStretch();

    return hBoxWidget;
}

inline void MainWindow::initToolBar()
{
    QToolBar *toolBar = new QToolBar(this);

    QWidget *hBoxWidgetNetwork = initToolBarNetwork(toolBar);
    QWidget *hBoxWidgetFile = initToolBarFile(toolBar);

    //
    // Add all these things into toolbar
    //
    toolBar->addWidget(hBoxWidgetNetwork);
    toolBar->addSeparator();
    toolBar->addWidget(hBoxWidgetFile);

    addToolBar(toolBar);
}

inline void MainWindow::initCentralWidget(void)
{
    mainPlotLive = new QMainPlot(this);

    setCentralWidget(mainPlotLive);
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

//
// Button event
//
inline void MainWindow::onButtonConnectClicked()
{
    pushButtonConnect->setText("Dis&connect");

    trackClient->connectToHost(lineEditHost->text(),
                               lineEditPort->text().toInt());
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
    fileName = QFileDialog::getSaveFileName(this,
                                            tr("Save to ..."),
                                            ".",
                                            tr("JSON files (*.json)"));
    if(fileName.isEmpty()) {
        return;
    }

    QString message;
    bool result = trackClient->saveToJsonFile(fileName);
    if (result) {
        message = "Status : " + fileName + " saved";
    } else {
        message = "Status : failed to save to" + fileName;
    }

    labelStatus->setText(message);
}

void MainWindow::onPushButtonLoadClicked()
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
    bool result = trackClient->LoadJsonFile(fileName);
    if (result) {
        message = "Status : " + fileName + " loaded";
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
void MainWindow::onNewPoint(const int type, const QPointF &point)
{
    mainPlotLive->appendPoint((QMainPlot::curveId)type, point);
}
