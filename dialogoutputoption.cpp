#include "dialogoutputoption.h"
#include "ui_dialogoutputoption.h"

DialogOutputOption::DialogOutputOption(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOutputOption)
{
    ui->setupUi(this);
    init();
}

DialogOutputOption::~DialogOutputOption()
{
    delete ui;
}


void DialogOutputOption::init()
{
    connect(ui->lineEditStartChar, &QLineEdit::textEdited,
            this, &DialogOutputOption::onLineEditStartCharEdited);
    //connect(ui->lineEditStartChar, &QLineEdit::textChanged,
    //        this, &DialogOutputOption::onLineEditStartCharChanged);
    connect(ui->lineEditDivCharIn, &QLineEdit::textEdited,
            this, &DialogOutputOption::onLineEditDivCharInEdited);
    connect(ui->lineEditEndChar, &QLineEdit::textEdited,
            this, &DialogOutputOption::onLineEditEndCharEdited);
    connect(ui->lineEditDivCharOut, &QLineEdit::textEdited,
            this, &DialogOutputOption::onLineEditDivCharOutEdited);

    ui->checkBoxPath->setChecked(true);
    ui->checkBoxID->setChecked(true);
    ui->checkBoxX->setChecked(true);
    ui->checkBoxY->setChecked(true);
    ui->checkBoxPhi->setChecked(true);

    ui->lineEditDivCharIn->setText(",");
    ui->lineEditDivCharOut->setText("\\n");
}


void DialogOutputOption::onLineEditStartCharEdited()
{
    ui->labelStart->setText("\""+ui->lineEditStartChar->text()+"\"");
}


void DialogOutputOption::onLineEditStartCharChanged()
{
    ui->labelStart->setText("\""+ui->lineEditStartChar->text()+"\"");
}


void DialogOutputOption::onLineEditDivCharInEdited()
{
    ui->labelDivIn->setText("\""+ui->lineEditDivCharIn->text()+"\"");
}

void DialogOutputOption::onLineEditEndCharEdited()
{
    ui->labelEnd->setText("\""+ui->lineEditEndChar->text()+"\"");
}

void DialogOutputOption::onLineEditDivCharOutEdited()
{
    ui->labelDivOut->setText("\""+ui->lineEditDivCharOut->text()+"\"");
}
