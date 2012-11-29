#include <QtGui>

#include "PluginManager.hpp"
#include "LightpackPluginInterface.hpp"
#include "debug.h"
#include "PythonQt.h"
#include "PythonQtImporter.h"
#include "PythonQt_QtAll.h"
#include "gui/PythonQtScriptingConsole.h"
#include "plugins/PyPlugin.h"

#include "Settings.hpp"

using namespace SettingsScope;

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{
    //initialize python qt
    PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
    PythonQt::self()->setImporter(0);
    /// TODO: make init modules manual
    PythonQt_QtAll::init();
}

PluginManager::~PluginManager(){
    dropPlugins();
    deinitPython();
    PythonQt::cleanup();
}

void PluginManager::initPython()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    try{
        DEBUG_LOW_LEVEL << Q_FUNC_INFO;
        mainContext = new PythonQtObjectPtr(PythonQt::self()->getMainModule());

        mainContext->evalScript(QString("import sys\n"));
        mainContext->evalScript("sys.path.append(':/plugin')\n");
        mainContext->evalScript("sys.path.append('./python')\n");
        //load base plugin class
        mainContext->evalFile(":/plugin/BasePlugin.py");

        mainContext->addObject("Lightpack", _pluginInterface);

        // -----------------------------------------------------------------
        // Alternative 1: make CustomObject known and use decorators for wrapping:
        // -----------------------------------------------------------------

        // register the new object as a known classname and add it's wrapper object
        //PythonQt::self()->registerCPPClass("LedDeviceFactory", "","example", PythonQtCreateObject<LedDeviceFactory>);

        // -----------------------------------------------------------------
        // Alternative 2: make CustomObject2 known and use a wrapper factory for wrapping:
        // -----------------------------------------------------------------

        // add a factory that can handle pointers to CustomObject2
        //PythonQt::self()->addWrapperFactory(m_ledDeviceFactory);

        // the following is optional and only needed if you want a constructor:
        // register the new object as a known classname
        //PythonQt::self()->registerCPPClass("LedDeviceFactory", "", "example");
        // add a constructor for CustomObject2
        //PythonQt::self()->addClassDecorators(m_ledDeviceFactory);

        // todo remove
        //        console = new PythonQtScriptingConsole(NULL,*mainContext);
        //        console->appendCommandPrompt();
        //        console->show();
    }
    catch (...)
    {
        qWarning() << Q_FUNC_INFO << "exception occured while initializing python";
    }

}

void PluginManager::deinitPython()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    delete mainContext;
}

void PluginManager::init(LightpackPluginInterface *pluginInterface)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    _pluginInterface = pluginInterface;

    initPython();

    //loadPlugins();
}

void PluginManager::reloadPlugins(){
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    dropPlugins();
    deinitPython();
    initPython();
    loadPlugins();
}

void PluginManager::dropPlugins(){
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    //cleanAll();
    for(QMap<QString, PyPlugin*>::iterator it = _plugins.begin(); it != _plugins.end(); ++it){
        PyPlugin* p = it.value();
        QString name = p->getName();
        p->stop();
        delete p;
        mainContext->evalScript("del "+name);
    }
    _plugins.clear();
    _pluginInterface->updatePlugin(_plugins.values());
}

void PluginManager::loadPlugins(){
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    QStringList pluginPaths = QApplication::libraryPaths();
    QString path = QString(Settings::getApplicationDirPath() + "Plugins");
    QDir dir(path);
    QStringList files = dir.entryList(QStringList("*.py"), QDir::Files);

    QStringList lstDirs = dir.entryList(QDir::Dirs |
                                        QDir::AllDirs |
                                        QDir::NoDotAndDotDot); //Получаем список папок

    foreach(QString pluginDir, lstDirs){
        QString fileName = path+"/"+pluginDir+"/"+pluginDir+".py";
        QString plugin = QFileInfo (fileName).baseName ();
        //add python plugin to file to path
        // TODO: fail
        //mainContext->evalFile(it.key());
        QFile file(fileName);
        if (!file.exists()) continue;
        file.open(QIODevice::ReadOnly);
        mainContext->evalScript(file.readAll());

        //get plugin info
        QString pluginConstructor = plugin + "()\n";
        PythonQtObjectPtr pyPluginObj = mainContext->evalScript(pluginConstructor, Py_eval_input);
        if(pyPluginObj.isNull()) {
            DEBUG_LOW_LEVEL << "Can't load plugin " << plugin ;
            continue;
        }

        QMap<QString, PyPlugin*>::iterator found = _plugins.find(plugin);
        if(found != _plugins.end()){
            DEBUG_LOW_LEVEL << "Already load a plugin named " << plugin << " !";
            continue;
        }

        PyPlugin* p = new PyPlugin(pyPluginObj, NULL);
        DEBUG_LOW_LEVEL <<p->getName()<<  p->getAuthor() << p->getDescription() << p->getVersion();
        connect(p, SIGNAL(executed()), this, SIGNAL(pluginExecuted()));
        _plugins[plugin] = p;

    }

    _pluginInterface->updatePlugin(_plugins.values());

    for(QMap<QString, PyPlugin*>::iterator it = _plugins.begin(); it != _plugins.end(); ++it){
        PyPlugin* p = it.value();
        p->init();
    }

    emit updatePlugin(_plugins.values());

    for(QMap<QString, PyPlugin*>::iterator it = _plugins.begin(); it != _plugins.end(); ++it){
        PyPlugin* p = it.value();
        if (p->isEnabled())
            p->execute();
    }
}

PyPlugin* PluginManager::getPlugin(const QString& name_){
    QMap<QString, PyPlugin*>::iterator found = _plugins.find(name_);
    if(found != _plugins.end())
        return found.value();
    else
        return 0;
}

QList<PyPlugin*> PluginManager::getPluginList(){
    return _plugins.values();
}

PythonQtScriptingConsole* PluginManager::getConsole(QWidget* parent_){
    PythonQtScriptingConsole* pyconsole = new PythonQtScriptingConsole(parent_, *mainContext);
    pyconsole->appendCommandPrompt();
    pyconsole->show();
    return pyconsole;
}


