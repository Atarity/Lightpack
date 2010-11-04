/*
 * aboutdialog.h
 *
 *  Created on: 26.07.2010
 *      Author: brunql
 *
 *      Project: AmbilightUSB
 */

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class aboutDialog;
}

class aboutDialog : public QDialog {
    Q_OBJECT
public:
    aboutDialog(QString hardwareVersion, QWidget *parent = 0);
    ~aboutDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::aboutDialog *ui;
};

#endif // ABOUTDIALOG_H
