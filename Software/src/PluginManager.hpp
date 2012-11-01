#ifndef PLUGINMANAGER_HPP
#define PLUGINMANAGER_HPP

#include <QObject>
#include <QWidget>
#include "PythonQt.h"
#include "gui/PythonQtScriptingConsole.h"

class PyPlugin;
class PythonQtScriptingConsole;
class LightpackPluginInterface;

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
    void reloadPlugins();


private:
    PythonQtObjectPtr  *mainContext;
    QMap<QString, PyPlugin*> _plugins;
    LightpackPluginInterface *_pluginInterface;

    void initPython();
    void deinitPython();
    void dropPlugins();

public:
    void init(LightpackPluginInterface *pluginInterface);
    void loadPlugins();
    QList<PyPlugin*> getPluginList();
    PyPlugin* getPlugin(const QString& name_);
    PythonQtScriptingConsole* getConsole(QWidget* parent_);


};

#endif // PLUGINMANAGER_HPP
