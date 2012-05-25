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
    explicit PluginManager(QObject *parent = 0);
    
signals:
    void updatePlugin(QList<PyPlugin*>);
    void pluginExecuted();

public slots:

private:
    PythonQtObjectPtr  *mainContext;
    PythonQtScriptingConsole *console;
    PythonQtObjectPtr loader;

    QMap<QString, PyPlugin*> _plugins;
    QMap<QString, QStringList> _availablePlugins;


    void dropPlugins();

public:
    void init(LightpackPluginInterface *pluginInterface, QWidget* settingsBox);
    void loadPlugins();
    QList<PyPlugin*> getPluginList();
    PyPlugin* getPlugin(const QString& name_);
    PythonQtScriptingConsole* getConsole(QWidget* parent_);


};

#endif // PLUGINMANAGER_HPP
