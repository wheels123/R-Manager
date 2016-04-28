#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "qmainplot.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initToolBar(void);
    void initCentralWidget(void);
    void initStatusBar(void);

private:
    // Toolbar items
    QLineEdit   *lineEditHost;
    QLineEdit   *lineEditPort;
    QPushButton *pushButtonConnect;
    // Status Bar items
    QLabel      *labelStatus;
    // Plot widget
    QMainPlot   *mainPlotLive;

private:
    // Button events
    inline void onButtonConnectClicked();
    inline void onButtonDisconnectClicked();
    void onPushButtonConnectClicked();
};

#endif // MAINWINDOW_H
