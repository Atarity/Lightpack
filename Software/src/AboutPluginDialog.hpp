#ifndef ABOUTPLUGINDIALOG_HPP
#define ABOUTPLUGINDIALOG_HPP

#include <QDialog>

class PyPlugin;

namespace Ui {
class AboutPluginDialog;
}

class AboutPluginDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AboutPluginDialog(PyPlugin* plugin,QWidget *parent = 0);
    ~AboutPluginDialog();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::AboutPluginDialog *ui;
};

#endif // ABOUTPLUGINDIALOG_HPP
