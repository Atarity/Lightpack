#include "AboutPluginDialog.hpp"
#include "ui_AboutPluginDialog.h"

#include "plugins/PyPlugin.h"

AboutPluginDialog::AboutPluginDialog(PyPlugin* plugin,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutPluginDialog)
{
    ui->setupUi(this);

    ui->label_5->setText(plugin->getName());
    ui->label_6->setText(plugin->getAuthor());
    ui->label_7->setText(plugin->getVersion());
    ui->textBrowser->setText(plugin->getDescription());
}

AboutPluginDialog::~AboutPluginDialog()
{
    delete ui;
}

void AboutPluginDialog::on_pushButton_clicked()
{
    close();
}
