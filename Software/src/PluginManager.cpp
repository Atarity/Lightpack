#include "PluginManager.hpp"
#include "LightpackPluginInterface.hpp"
#include "PythonQt.h"
#include "PythonQtImporter.h"
#include "PythonQt_QtAll.h"
#include "gui/PythonQtScriptingConsole.h"
#include "plugins/PyPlugin.h"

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    try{
        //initialize python qt
        PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
        PythonQt::self()->setImporter(0);
        // TODO: make init modules manual
        PythonQt_QtAll::init();

        mainContext = new PythonQtObjectPtr(PythonQt::self()->getMainModule());


        mainContext->evalScript(QString("import sys\n"));
        mainContext->evalScript("sys.path.append(':/plugin')\n");
        //load base plugin class
        mainContext->evalFile(":/plugin/BasePlugin.py");

        // todo remove
//        console = new PythonQtScriptingConsole(NULL,*mainContext);
//        console->appendCommandPrompt();
//        console->show();
        }
        catch (...)
        {

        }
   }

PluginManager::~PluginManager(){
    dropPlugins();
    PythonQt::cleanup();
}

void PluginManager::init(LightpackPluginInterface *pluginInterface, QWidget* settingsBox)
{

    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    mainContext->addObject("Lightpack", pluginInterface);
    mainContext->addObject("SettingsBox", settingsBox);

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

}

void PluginManager::dropPlugins(){
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    //cleanAll();
    for(QMap<QString, PyPlugin*>::iterator it = _plugins.begin(); it != _plugins.end(); ++it){
        PyPlugin* p = it.value();
        delete p;
    }
    _plugins.clear();
}

void PluginManager::loadPlugins(){
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    dropPlugins();

    QStringList pluginPaths = QApplication::libraryPaths();
      QString path = "./plugins";
      QDir dir(path);
      QStringList files = dir.entryList(QStringList("*.py"), QDir::Files);

      foreach(QString fileName, files){
       DEBUG_LOW_LEVEL << fileName;

        QString plugin = QFileInfo (fileName).baseName ();
        //add python plugin to file to path
        // TODO: fail
        //mainContext->evalFile(it.key());
        QFile file(path+"/"+fileName);
        file.open(QIODevice::ReadOnly);
        mainContext->evalScript(file.readAll());

       DEBUG_LOW_LEVEL << plugin;

            //get plugin info
            QString pluginConstructor = plugin + "()\n";
            PythonQtObjectPtr pyPluginObj = mainContext->evalScript(pluginConstructor, Py_eval_input);
            if(pyPluginObj.isNull()){
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
//            connect(p, SIGNAL(aboutToExecute()), this, SLOT(aboutToExecutePlugin()));
//            connect(p, SIGNAL(executed()), this, SLOT(cleanUp()));
            connect(p, SIGNAL(executed()), this, SIGNAL(pluginExecuted()));
           _plugins[plugin] = p;

           if (p->isEnabled())
               p->execute();

         //  QThread* m_PluginThread = new QThread();
           //p->moveToThread(m_PluginThread);
          // m_PluginThread->start();
    }
     emit updatePlugin(_plugins.values());
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
    connect(this, SIGNAL(pluginExecuted()), pyconsole, SLOT(externalUpdate()));
    pyconsole->show();
    return pyconsole;
}


