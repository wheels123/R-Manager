#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "qmainplot.h"
#include "qtrackclient.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    inline void initToolBar();
    inline QWidget *initToolBarFile(QToolBar *toolBar);
    inline QWidget *initToolBarNetwork(QToolBar *toolBar);
    inline void initCentralWidget();
    inline void initStatusBar();
    inline void initTrackClient();

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

private:
    // Button events
    inline void onButtonConnectClicked();
    inline void onButtonDisconnectClicked();
    inline void onNetworkError();
    void onPushButtonConnectClicked();
    void onPushButtonSaveClicked();
    void onPushButtonLoadClicked();
    // Socket events
    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpSocketError(QAbstractSocket::SocketError error);
    // New point
    void onNewPoint(const int type, const QPointF &point);
};

#endif // MAINWINDOW_H
