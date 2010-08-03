#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"

aboutDialog::aboutDialog(QString hardwareVersion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
    ui->labelVersionSoftware->setText(tr("<b>Software version:</b> ") + QString(VERSION_STR));
    ui->labelVersionHardware->setText(tr("<b>Hardware version:</b> ") + hardwareVersion);
}

aboutDialog::~aboutDialog()
{
    delete ui;
}

void aboutDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
