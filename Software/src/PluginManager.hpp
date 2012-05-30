#ifndef PLUGINMANAGER_HPP
#define PLUGINMANAGER_HPP

#include <QObject>
#include "LightpackPluginInterface.hpp"
#include "PythonQt.h"
#include "gui/PythonQtScriptingConsole.h"

class PyPlugin;
class PythonQtScriptingConsole;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    PluginManager(QObject *parent = 0);
    ~PluginManager();
    
signals:
    void updatePlugin(QList<PyPlugin*>);
    void pluginExecuted();

public slots:
    void loadPlugins();

private:
    PythonQtObjectPtr  *mainContext;
    QMap<QString, PyPlugin*> _plugins;

    void dropPlugins();

public:
    void init(LightpackPluginInterface *pluginInterface, QWidget* settingsBox);
    QList<PyPlugin*> getPluginList();
    PyPlugin* getPlugin(const QString& name_);
    PythonQtScriptingConsole* getConsole(QWidget* parent_);


};

#endif // PLUGINMANAGER_HPP
