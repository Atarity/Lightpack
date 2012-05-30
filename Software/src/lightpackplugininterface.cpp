#include "LightpackPluginInterface.hpp"
#include "ApiServer.hpp"
#include "Settings.hpp"

using namespace SettingsScope;

LightpackPluginInterface::LightpackPluginInterface(QObject *parent) :
    QObject(parent)
{
    initColors(10);
    m_timerLock = new QTimer(this);
    connect(m_timerLock, SIGNAL(timeout()), this, SLOT(timeoutLock()));

}

LightpackPluginInterface::~LightpackPluginInterface()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete m_timerLock;
}

//TODO timeout lock
void LightpackPluginInterface::timeoutLock()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
    if (lockAlive)
    {
        lockAlive = false;
    }
    else
        UnLock(lockSessionKey);
}

void LightpackPluginInterface::setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus status)
{
    if (status == DeviceLocked::Unlocked)
        lockSessionKey = "";
    if (status == DeviceLocked::Api)
        lockSessionKey = "LockApi";
}

//TODO identification plugin locked
QString LightpackPluginInterface::getSessionKey()
{
    return QUuid::createUuid().toString();
}



//TODO: lock unlock
bool LightpackPluginInterface::Lock(QString sessionKey)
{
    if (lockSessionKey=="" || lockSessionKey==sessionKey)
        {
            lockSessionKey = sessionKey;
            lockAlive = true;
            m_timerLock->start(5000); // check in 5000 ms
            emit updateDeviceLockStatus(DeviceLocked::Plugin);
            return true;
        } else
            return false;
}

bool LightpackPluginInterface::UnLock(QString sessionKey)
{
        if (lockSessionKey==sessionKey)
        {
            lockSessionKey = "";
            m_timerLock->stop();
            emit updateDeviceLockStatus(DeviceLocked::Unlocked);
            return true;
        } else {
            return false;
        }
}
// TODO: setcolor
bool LightpackPluginInterface::setColors(QString sessionKey, int r, int g, int b)
{
     if (lockSessionKey!=sessionKey) return false;
     lockAlive = true;
    for (int i = 0; i < m_colors.size(); i++)
    {
            m_colors[i] = qRgb(r,g,b);
    }
    emit updateLedsColors(m_colors);
    return true;
}

bool LightpackPluginInterface::setColor(QString sessionKey, int ind,int r, int g, int b)
{
    if (lockSessionKey!=sessionKey) return false;
    lockAlive = true;
    if (ind>m_colors.size()-1) return false;
    m_colors[ind] = qRgb(r,g,b);
    emit updateLedsColors(m_colors);
    return true;
}

void LightpackPluginInterface::initColors(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;
    m_colors.clear();
    for (int i = 0; i < numberOfLeds; i++)
        m_colors << 0;
}

//todo getnumleds
int LightpackPluginInterface::numberOfLeds()
{
    return m_colors.count();
}

void LightpackPluginInterface::setNumberOfLeds(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;

    initColors(numberOfLeds);
}


// TODO: settings (global or profile?)
void LightpackPluginInterface::setSettingProfile(QString key, QVariant value)
{
    Settings::setValue(key,value);
}

QVariant LightpackPluginInterface::getSettingProfile(QString key)
{
    return Settings::value(key);
}

void LightpackPluginInterface::setSettingMain(QString key, QVariant value)
{
    Settings::setValueMain(key,value);
}

QVariant LightpackPluginInterface::getSettingMain(QString key)
{
    return Settings::valueMain(key);
}
