#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class aboutDialog;
}

class aboutDialog : public QDialog {
    Q_OBJECT
public:
    aboutDialog(QWidget *parent = 0);
    ~aboutDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::aboutDialog *ui;
};

#endif // ABOUTDIALOG_H
