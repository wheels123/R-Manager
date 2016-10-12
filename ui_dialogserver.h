/********************************************************************************
** Form generated from reading UI file 'dialogserver.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGSERVER_H
#define UI_DIALOGSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QTreeView>

QT_BEGIN_NAMESPACE

class Ui_DialogServer
{
public:
    QDialogButtonBox *buttonBox;
    QListView *listViewClient;
    QTreeView *treeViewPath;
    QTreeView *treeViewMainPath;
    QComboBox *comboBox;

    void setupUi(QDialog *DialogServer)
    {
        if (DialogServer->objectName().isEmpty())
            DialogServer->setObjectName(QStringLiteral("DialogServer"));
        DialogServer->resize(1200, 900);
        buttonBox = new QDialogButtonBox(DialogServer);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(610, 860, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        listViewClient = new QListView(DialogServer);
        listViewClient->setObjectName(QStringLiteral("listViewClient"));
        listViewClient->setGeometry(QRect(30, 20, 200, 800));
        treeViewPath = new QTreeView(DialogServer);
        treeViewPath->setObjectName(QStringLiteral("treeViewPath"));
        treeViewPath->setGeometry(QRect(250, 20, 680, 800));
        treeViewPath->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        treeViewMainPath = new QTreeView(DialogServer);
        treeViewMainPath->setObjectName(QStringLiteral("treeViewMainPath"));
        treeViewMainPath->setGeometry(QRect(950, 20, 250, 800));
        //comboBox = new QComboBox(DialogServer);
        //comboBox->setObjectName(QStringLiteral("comboBox"));
        //comboBox->setGeometry(QRect(130, 560, 69, 22));

        retranslateUi(DialogServer);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogServer, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogServer, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogServer);
    } // setupUi

    void retranslateUi(QDialog *DialogServer)
    {
        DialogServer->setWindowTitle(QApplication::translate("DialogServer", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class DialogServer: public Ui_DialogServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGSERVER_H
