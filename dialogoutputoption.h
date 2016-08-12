#ifndef DIALOGOUTPUTOPTION_H
#define DIALOGOUTPUTOPTION_H

#include <QDialog>

namespace Ui {
class DialogOutputOption;
}

class DialogOutputOption : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOutputOption(QWidget *parent = 0);
    ~DialogOutputOption();
    void onLineEditStartCharEdited();
    void onLineEditStartCharChanged();
    void init();
    void onLineEditDivCharInEdited();
    void onLineEditEndCharEdited();
    void onLineEditDivCharOutEdited();
private:
    Ui::DialogOutputOption *ui;
};

#endif // DIALOGOUTPUTOPTION_H
