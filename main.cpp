#include "mainwindow.h"
#include <QApplication>
#include "QtDebug"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QDateTime>
#include   <fstream>
using namespace std;
std::ofstream g_OutputDebug;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString text;
    switch(type)
    {
    case QtDebugMsg:
    text = QString("Debug: ");
    break;

    case QtWarningMsg:
    text = QString("Warning: ");
    break;

    case QtCriticalMsg:
    text = QString("Critical:");
    break;

    case QtFatalMsg:
    text = QString("Fatal: ");
    break;

    default:
    text = QString("Debug: ");
    }

    //QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    //QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("(%1)").arg(current_date_time);
    //std::string message = qPrintable(QString("%1 %2 \t%3 \t%4").arg(text).arg(context_info).arg(current_date).arg(msg));
    std::string message = qPrintable(QString("%1 %2 \t%3").arg(text).arg(current_date).arg(msg));
    QFile outFile("debuglog.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts <<QString(QString::fromLocal8Bit(message.c_str()))<< endl;
}

int main(int argc, char *argv[])
{
    //qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
