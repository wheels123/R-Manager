#ifndef DIALOGSERVER_H
#define DIALOGSERVER_H

#include <QDialog>
#include "fortuneserver.h"
#include<QStringListModel>
#include<QListView>
#include<QStandardItemModel>
#include<robot.h>
class QTreeView; //forward declarations
class QStandardItemModel;
class QStandardItem;
namespace Ui {
class DialogServer;
}

class DialogServer : public QDialog
{
    Q_OBJECT

public:
    explicit DialogServer(QWidget *parent = 0);
    ~DialogServer();
    void updateListViewClient(QStringList list);
    void updateTreeViewPath(QString pathId,QString pathPoint);
    void updateTreeViewPath(Robot* robot);
    void updateTreeViewMainPath(Robot* robot);

private:
    Ui::DialogServer *ui;
    QStringList *strList;
};

#endif // DIALOGSERVER_H
