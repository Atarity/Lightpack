#include "Plugin.hpp"
#include <QSettings>
#include <QIcon>
#include <QFileInfo>
#include <QDir>
#include "Settings.hpp"
#include "../common/defs.h"

using namespace SettingsScope;

#if defined(Q_OS_WIN)
    const QString kOsSpecificExecuteKey = "ExecuteOnWindows";
#elif defined(MAC_OS)
    const QString kOsSpecificExecuteKey = "ExecuteOnOSX";
#elif defined(Q_OS_UNIX)
    const QString kOsSpecificExecuteKey = "ExecuteOnNix";
#endif

Plugin::Plugin(QString name, QString path, QObject *parent) :
    QObject(parent)
{
    _pathPlugin = path;
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << name << path;

    QString fileName = path+"/"+name+".ini";
    QSettings settings( fileName, QSettings::IniFormat );
    settings.beginGroup("Main");
    this->_name = settings.value( "Name", "Error").toString();
    if (settings.contains(kOsSpecificExecuteKey)) {
        this->_exec = settings.value( kOsSpecificExecuteKey, "").toString();
    } else {
        this->_exec = settings.value( "Execute", "").toString();
    }
    this->_guid = settings.value( "Guid", "").toString();
    this->_author = settings.value( "Author", "").toString();
    this->_description = settings.value( "Description", "").toString();
    this->_version = settings.value( "Version", "").toString();
    this->_icon = settings.value( "Icon", "").toString();
    settings.endGroup();

    process = new QProcess(this);

}

Plugin::~Plugin()
{
    Stop();
}

QString Plugin::Name() const
{
    return _name;
}

QString Plugin::Guid() const
{
    return _guid;
}

QString Plugin::Author() const  {
    return _author;
}

QString Plugin::Description() const  {
    return _description;
}

QString Plugin::Version() const  {
    return _version;
}


QIcon Plugin::Icon() const  {
    // TODO path to image
   QFileInfo f(_icon);
   if (f.exists())
       return QIcon(_icon);
   return QIcon(":/icons/plugins.png");
}


int Plugin::getPriority() const {
    QString key = this->_name+"/Priority";
    return Settings::valueMain(key).toInt();
}

void Plugin::setPriority(int priority) {
    QString key = this->_name+"/Priority";
    Settings::setValueMain(key,priority);
}

bool Plugin::isEnabled() const {
    QString key = this->_name+"/Enable";
    return Settings::valueMain(key).toBool();
}

void Plugin::setEnabled(bool enable){
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << enable;
    QString key = this->_name+"/Enable";
    Settings::setValueMain(key,enable);
    if (!enable) this->Stop();
    if (enable) this->Start();
}


void Plugin::Start()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << _exec;

    QString program = _exec;
    //QStringList arguments;
    //arguments << "-style" << "fusion";

    QDir dir(_pathPlugin);
    QDir::setCurrent(dir.absolutePath());

    process->disconnect();
    connect(process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SIGNAL(stateChanged(QProcess::ProcessState)));

    process->setEnvironment(QProcess::systemEnvironment());
//    process->setProcessChannelMode(QProcess::ForwardedChannels);
    process->start(program,NULL);
}

void Plugin::Stop()
{
    process->kill();
}

QProcess::ProcessState Plugin::state() const
{
    return process->state();
}

