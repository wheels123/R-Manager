/********************************************************************************
** Form generated from reading UI file 'dialogoutputoption.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGOUTPUTOPTION_H
#define UI_DIALOGOUTPUTOPTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_DialogOutputOption
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBoxSetType;
    QCheckBox *checkBoxDest;
    QCheckBox *checkBoxLabel;
    QCheckBox *checkBoxPath;
    QGroupBox *groupBoxSetArea;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QLabel *label;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QLabel *label_7;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QGroupBox *groupBoxSetFormat;
    QLabel *label_2;
    QLineEdit *lineEditStartChar;
    QLabel *label_3;
    QLineEdit *lineEditDivCharIn;
    QLabel *label_4;
    QLineEdit *lineEditEndChar;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *lineEditDivCharOut;
    QLabel *labelStart;
    QLabel *labelDivIn;
    QLabel *labelEnd;
    QLabel *labelDivOut;
    QFrame *line;
    QCheckBox *checkBoxID;
    QCheckBox *checkBoxX;
    QCheckBox *checkBoxY;
    QCheckBox *checkBoxPhi;
    QLabel *label_8;

    void setupUi(QDialog *DialogOutputOption)
    {
        if (DialogOutputOption->objectName().isEmpty())
            DialogOutputOption->setObjectName(QStringLiteral("DialogOutputOption"));
        DialogOutputOption->resize(500, 400);
        buttonBox = new QDialogButtonBox(DialogOutputOption);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(330, 360, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBoxSetType = new QGroupBox(DialogOutputOption);
        groupBoxSetType->setObjectName(QStringLiteral("groupBoxSetType"));
        groupBoxSetType->setGeometry(QRect(10, 10, 71, 111));
        checkBoxDest = new QCheckBox(groupBoxSetType);
        checkBoxDest->setObjectName(QStringLiteral("checkBoxDest"));
        checkBoxDest->setGeometry(QRect(10, 24, 56, 17));
        checkBoxLabel = new QCheckBox(groupBoxSetType);
        checkBoxLabel->setObjectName(QStringLiteral("checkBoxLabel"));
        checkBoxLabel->setGeometry(QRect(10, 47, 45, 17));
        checkBoxPath = new QCheckBox(groupBoxSetType);
        checkBoxPath->setObjectName(QStringLiteral("checkBoxPath"));
        checkBoxPath->setGeometry(QRect(10, 70, 45, 17));
        groupBoxSetArea = new QGroupBox(DialogOutputOption);
        groupBoxSetArea->setObjectName(QStringLiteral("groupBoxSetArea"));
        groupBoxSetArea->setGeometry(QRect(90, 10, 181, 111));
        lineEdit = new QLineEdit(groupBoxSetArea);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(10, 20, 61, 20));
        lineEdit_2 = new QLineEdit(groupBoxSetArea);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(110, 20, 61, 20));
        label = new QLabel(groupBoxSetArea);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(80, 20, 21, 16));
        lineEdit_3 = new QLineEdit(groupBoxSetArea);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(10, 80, 61, 20));
        lineEdit_4 = new QLineEdit(groupBoxSetArea);
        lineEdit_4->setObjectName(QStringLiteral("lineEdit_4"));
        lineEdit_4->setGeometry(QRect(110, 80, 61, 20));
        label_7 = new QLabel(groupBoxSetArea);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(80, 80, 47, 13));
        radioButton = new QRadioButton(groupBoxSetArea);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setGeometry(QRect(60, 50, 31, 17));
        radioButton_2 = new QRadioButton(groupBoxSetArea);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));
        radioButton_2->setGeometry(QRect(100, 50, 31, 17));
        groupBoxSetFormat = new QGroupBox(DialogOutputOption);
        groupBoxSetFormat->setObjectName(QStringLiteral("groupBoxSetFormat"));
        groupBoxSetFormat->setGeometry(QRect(10, 130, 371, 141));
        label_2 = new QLabel(groupBoxSetFormat);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 20, 47, 21));
        lineEditStartChar = new QLineEdit(groupBoxSetFormat);
        lineEditStartChar->setObjectName(QStringLiteral("lineEditStartChar"));
        lineEditStartChar->setGeometry(QRect(70, 20, 61, 21));
        label_3 = new QLabel(groupBoxSetFormat);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 50, 61, 21));
        lineEditDivCharIn = new QLineEdit(groupBoxSetFormat);
        lineEditDivCharIn->setObjectName(QStringLiteral("lineEditDivCharIn"));
        lineEditDivCharIn->setGeometry(QRect(70, 50, 61, 20));
        label_4 = new QLabel(groupBoxSetFormat);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 80, 47, 21));
        lineEditEndChar = new QLineEdit(groupBoxSetFormat);
        lineEditEndChar->setObjectName(QStringLiteral("lineEditEndChar"));
        lineEditEndChar->setGeometry(QRect(70, 80, 61, 20));
        label_5 = new QLabel(groupBoxSetFormat);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 80, 47, 21));
        label_6 = new QLabel(groupBoxSetFormat);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 110, 61, 16));
        lineEditDivCharOut = new QLineEdit(groupBoxSetFormat);
        lineEditDivCharOut->setObjectName(QStringLiteral("lineEditDivCharOut"));
        lineEditDivCharOut->setGeometry(QRect(70, 110, 61, 20));
        labelStart = new QLabel(groupBoxSetFormat);
        labelStart->setObjectName(QStringLiteral("labelStart"));
        labelStart->setGeometry(QRect(140, 20, 91, 21));
        labelDivIn = new QLabel(groupBoxSetFormat);
        labelDivIn->setObjectName(QStringLiteral("labelDivIn"));
        labelDivIn->setGeometry(QRect(140, 50, 91, 21));
        labelEnd = new QLabel(groupBoxSetFormat);
        labelEnd->setObjectName(QStringLiteral("labelEnd"));
        labelEnd->setGeometry(QRect(140, 80, 91, 21));
        labelDivOut = new QLabel(groupBoxSetFormat);
        labelDivOut->setObjectName(QStringLiteral("labelDivOut"));
        labelDivOut->setGeometry(QRect(140, 110, 91, 21));
        line = new QFrame(groupBoxSetFormat);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(240, 20, 20, 111));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        checkBoxID = new QCheckBox(groupBoxSetFormat);
        checkBoxID->setObjectName(QStringLiteral("checkBoxID"));
        checkBoxID->setGeometry(QRect(270, 40, 70, 17));
        checkBoxX = new QCheckBox(groupBoxSetFormat);
        checkBoxX->setObjectName(QStringLiteral("checkBoxX"));
        checkBoxX->setGeometry(QRect(270, 60, 70, 17));
        checkBoxY = new QCheckBox(groupBoxSetFormat);
        checkBoxY->setObjectName(QStringLiteral("checkBoxY"));
        checkBoxY->setGeometry(QRect(270, 80, 70, 17));
        checkBoxPhi = new QCheckBox(groupBoxSetFormat);
        checkBoxPhi->setObjectName(QStringLiteral("checkBoxPhi"));
        checkBoxPhi->setGeometry(QRect(270, 100, 70, 17));
        label_8 = new QLabel(groupBoxSetFormat);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(270, 20, 51, 16));

        retranslateUi(DialogOutputOption);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogOutputOption, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogOutputOption, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogOutputOption);
    } // setupUi

    void retranslateUi(QDialog *DialogOutputOption)
    {
        DialogOutputOption->setWindowTitle(QApplication::translate("DialogOutputOption", "Dialog", 0));
        groupBoxSetType->setTitle(QApplication::translate("DialogOutputOption", "\347\261\273\345\236\213", 0));
        checkBoxDest->setText(QApplication::translate("DialogOutputOption", "\347\233\256\347\232\204\345\234\260", 0));
        checkBoxLabel->setText(QApplication::translate("DialogOutputOption", "\346\240\207\347\255\276", 0));
        checkBoxPath->setText(QApplication::translate("DialogOutputOption", "\350\267\257\347\272\277", 0));
        groupBoxSetArea->setTitle(QApplication::translate("DialogOutputOption", "\346\240\207\345\217\267\350\214\203\345\233\264", 0));
        label->setText(QApplication::translate("DialogOutputOption", "\342\200\224\342\200\224", 0));
        label_7->setText(QApplication::translate("DialogOutputOption", "\342\200\224\342\200\224", 0));
        radioButton->setText(QApplication::translate("DialogOutputOption", "\344\270\216", 0));
        radioButton_2->setText(QApplication::translate("DialogOutputOption", "\346\210\226", 0));
        groupBoxSetFormat->setTitle(QApplication::translate("DialogOutputOption", "\346\240\274\345\274\217", 0));
        label_2->setText(QApplication::translate("DialogOutputOption", "\347\202\271\350\265\267\345\247\213\347\254\246", 0));
        label_3->setText(QApplication::translate("DialogOutputOption", "\347\202\271\345\206\205\345\210\206\351\232\224\347\254\246", 0));
        label_4->setText(QString());
        label_5->setText(QApplication::translate("DialogOutputOption", "\347\202\271\347\273\210\346\255\242\347\254\246", 0));
        label_6->setText(QApplication::translate("DialogOutputOption", "\347\202\271\351\227\264\345\210\206\351\232\224\347\254\246", 0));
        labelStart->setText(QApplication::translate("DialogOutputOption", "\342\200\234\342\200\235", 0));
        labelDivIn->setText(QApplication::translate("DialogOutputOption", "\342\200\234\342\200\235", 0));
        labelEnd->setText(QApplication::translate("DialogOutputOption", "\342\200\234\342\200\235", 0));
        labelDivOut->setText(QApplication::translate("DialogOutputOption", "\342\200\234\342\200\235", 0));
        checkBoxID->setText(QApplication::translate("DialogOutputOption", "ID", 0));
        checkBoxX->setText(QApplication::translate("DialogOutputOption", "X", 0));
        checkBoxY->setText(QApplication::translate("DialogOutputOption", "Y", 0));
        checkBoxPhi->setText(QApplication::translate("DialogOutputOption", "PHI", 0));
        label_8->setText(QApplication::translate("DialogOutputOption", "\350\276\223\345\207\272\345\217\202\346\225\260", 0));
    } // retranslateUi

};

namespace Ui {
    class DialogOutputOption: public Ui_DialogOutputOption {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGOUTPUTOPTION_H
