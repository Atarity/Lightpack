#include <QtGui>
#include "LightpackPluginInterface.hpp"

#include "Settings.hpp"
#include "version.h"
#include "debug.h"

#include <plugins/PyPlugin.h>

using namespace SettingsScope;

const int LightpackPluginInterface::SignalWaitTimeoutMs = 1000; // 1 second

LightpackPluginInterface::LightpackPluginInterface(QObject *parent) :
    QObject(parent)
{
    m_isRequestBacklightStatusDone = true;
    m_backlightStatusResult = Backlight::StatusUnknown;
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
    {
        if (!lockSessionKeys.isEmpty())
            UnLock(lockSessionKeys[0]);
    }
}

void LightpackPluginInterface::initColors(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;
    m_curColors.clear();
    m_setColors.clear();
    for (int i = 0; i < numberOfLeds; i++)
    {
        m_setColors << 0;
        m_curColors << 0;
    }
}

void LightpackPluginInterface::setNumberOfLeds(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;

    initColors(numberOfLeds);
}

void LightpackPluginInterface::updatePlugin(QList<PyPlugin*> plugins)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    if (!lockSessionKeys.isEmpty())
        UnLock(lockSessionKeys[0]);
    lockSessionKeys.clear();
    //emit updateDeviceLockStatus(DeviceLocked::Unlocked, lockSessionKeys);
    _plugins = plugins;

}

void LightpackPluginInterface::resultBacklightStatus(Backlight::Status status)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << status;
    m_isRequestBacklightStatusDone = true;
    m_backlightStatusResult = status;
    switch (m_backlightStatusResult)
    {
    case Backlight::StatusOn:
        emit ChangeStatus(1);
        break;
    case Backlight::StatusOff:
        emit ChangeStatus(0);
        break;
    case Backlight::StatusDeviceError:
    default:
        emit ChangeStatus(-1);
        break;
    }
}

void LightpackPluginInterface::changeProfile(QString profile)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << profile;

    emit ChangeProfile(profile);
}

void LightpackPluginInterface::refreshAmbilightEvaluated(double updateResultMs)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << updateResultMs;

    double secs = updateResultMs / 1000;
    hz = 0;

    if(secs != 0){
        hz = 1 / secs;
    }
}

void LightpackPluginInterface::refreshScreenRect(QRect rect)
{
    screen = rect;
}

void LightpackPluginInterface::updateColors(const QList<QRgb> & colors)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
    m_curColors = colors;
}

QString LightpackPluginInterface::Version()
{
    return API_VERSION;
}

PyPlugin* LightpackPluginInterface::findName(QString name)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << name;
    foreach(PyPlugin* plugin, _plugins){
        if (plugin->getName() == name)
            return plugin;
    }

    return NULL;
}

PyPlugin* LightpackPluginInterface::findSessionKey(QString sessionKey)
{
    foreach(PyPlugin* plugin, _plugins){
        if (plugin->getSessionKey() == sessionKey)
            return plugin;
    }

    return NULL;
}


// TODO identification plugin locked
QString LightpackPluginInterface::GetSessionKey(QString module)
{
    if (module=="API") return "Lock";
    PyPlugin* plugin = findName(module);
    if (plugin == NULL) return "";
    return plugin->getSessionKey();
}

int LightpackPluginInterface::CheckLock(QString sessionKey)
{
    if (lockSessionKeys.isEmpty())
        return 0;
    if (lockSessionKeys[0]==sessionKey)
        return 1;
    return -1;
}


//TODO: lock unlock
bool LightpackPluginInterface::Lock(QString sessionKey)
{
    if (sessionKey == "") return false;
    if (lockSessionKeys.contains(sessionKey)) return true;
    if (sessionKey.indexOf("API", 0) != -1)
    {
        if (lockSessionKeys.count()>0)
            if (lockSessionKeys[0].indexOf("API", 0) != -1) return false;
        lockSessionKeys.insert(0,sessionKey);
        emit updateDeviceLockStatus(DeviceLocked::Api,lockSessionKeys);
    }
    else
    {
          PyPlugin* plugin = findSessionKey(sessionKey);
          if (plugin == NULL) return false;

              foreach (QString key,lockSessionKeys)
              {
                  if (key.indexOf("API", 0) != -1) break;

                         PyPlugin* pluginLock = findSessionKey(key);
                         if (pluginLock == NULL) return false;
                         DEBUG_LOW_LEVEL << Q_FUNC_INFO << lockSessionKeys.indexOf(key);
                         if (plugin->getPriority() > pluginLock->getPriority())
                             lockSessionKeys.insert(lockSessionKeys.indexOf(key),sessionKey);
                         DEBUG_LOW_LEVEL << Q_FUNC_INFO << lockSessionKeys.indexOf(sessionKey);

              }
              if (!lockSessionKeys.contains(sessionKey))
              {
                  DEBUG_LOW_LEVEL << Q_FUNC_INFO << "add to end";
                  lockSessionKeys.insert(lockSessionKeys.count(),sessionKey);
              }
              if (lockSessionKeys[0].indexOf("API", 0) != -1)
                  emit updateDeviceLockStatus(DeviceLocked::Api,lockSessionKeys);
              else
                  emit updateDeviceLockStatus(DeviceLocked::Plugin, lockSessionKeys);
    }

    DEBUG_LOW_LEVEL << Q_FUNC_INFO << "lock end";
    lockAlive = true;
    m_timerLock->start(5000); // check in 5000 ms
    emit ChangeLockStatus (true);
    return true;
}

bool LightpackPluginInterface::UnLock(QString sessionKey)
{
    if (lockSessionKeys.isEmpty()) return true;
        if (lockSessionKeys[0]==sessionKey)
        {
            lockSessionKeys.removeFirst();
            if (lockSessionKeys.count()==0)
            {
                m_timerLock->stop();
                emit updateDeviceLockStatus(DeviceLocked::Unlocked, lockSessionKeys);
                emit ChangeLockStatus(false);
            }
            else
            {
                if (lockSessionKeys[0].indexOf("API", 0) != -1)
                    emit updateDeviceLockStatus(DeviceLocked::Api,lockSessionKeys);
                else
                    emit updateDeviceLockStatus(DeviceLocked::Plugin, lockSessionKeys);
            }
            return true;
        }
        else
            if (lockSessionKeys.indexOf(sessionKey)!= -1)
            {
               lockSessionKeys.removeOne(sessionKey);
               if (lockSessionKeys[0].indexOf("API", 0) != -1)
                   emit updateDeviceLockStatus(DeviceLocked::Api,lockSessionKeys);
               else
                   emit updateDeviceLockStatus(DeviceLocked::Plugin, lockSessionKeys);
               return true;
            }
        return false;

}
// TODO: setcolor
bool LightpackPluginInterface::SetColors(QString sessionKey, int r, int g, int b)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
     lockAlive = true;
    for (int i = 0; i < m_setColors.size(); i++)
    {
            m_setColors[i] = qRgb(r,g,b);
    }
    m_curColors = m_setColors;
    emit updateLedsColors(m_setColors);
    return true;
}

bool LightpackPluginInterface::SetColor(QString sessionKey, int ind,int r, int g, int b)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << sessionKey;
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
    lockAlive = true;
    if (ind>m_setColors.size()-1) return false;
    m_setColors[ind] = qRgb(r,g,b);
    m_curColors = m_setColors;
    emit updateLedsColors(m_setColors);
    return true;
}

bool LightpackPluginInterface::SetGamma(QString sessionKey, int gamma)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
     if (gamma >= Profile::Device::GammaMin && gamma <= Profile::Device::GammaMax)
     {
         emit updateGamma(gamma);
         return true;
     } else
         return false;
}

bool LightpackPluginInterface::SetBrightness(QString sessionKey, int brightness)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
     if (brightness >= Profile::Device::BrightnessMin && brightness <= Profile::Device::BrightnessMax)
     {
         emit updateBrightness(brightness);
         return true;
     } else
         return false;
}

bool LightpackPluginInterface::SetSmooth(QString sessionKey, int smooth)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
     if (smooth >= Profile::Device::SmoothMin && smooth <= Profile::Device::SmoothMax)
     {
             emit updateSmooth(smooth);
             return true;
      } else
         return false;
}

bool LightpackPluginInterface::SetProfile(QString sessionKey,QString profile)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
     QStringList profiles = Settings::findAllProfiles();
     if (profiles.contains(profile))
     {
         emit updateProfile(profile);
         return true;
     } else
         return false;
}

bool LightpackPluginInterface::SetStatus(QString sessionKey, int status)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
     Backlight::Status statusSet = Backlight::StatusUnknown;

     if (status == 1)
         statusSet = Backlight::StatusOn;
     else if (status == 0)
         statusSet = Backlight::StatusOff;

     if (statusSet != Backlight::StatusUnknown)
     {
         emit updateStatus(statusSet);
         return true;
     } else
         return false;
}

bool LightpackPluginInterface::SetLeds(QString sessionKey, QList<QRect> leds)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
    int num =0;
     foreach(QRect rectLed, leds){
        Settings::setLedPosition(num, QPoint(rectLed.x(),rectLed.y()));
        Settings::setLedSize(num,QSize(rectLed.width(),rectLed.height()));
        ++ num;
     }
    QString profile = Settings::getCurrentProfileName();
    emit updateProfile(profile);
    return true;
}

bool LightpackPluginInterface::NewProfile(QString sessionKey, QString profile)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;

     Settings::loadOrCreateProfile(profile);
     DEBUG_LOW_LEVEL << Q_FUNC_INFO << "OK:" << profile;
     emit updateProfile(profile);

     return true;
}

bool LightpackPluginInterface::DeleteProfile(QString sessionKey, QString profile)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
    QStringList profiles = Settings::findAllProfiles();
    if (profiles.contains(profile))
    {
        Settings::loadOrCreateProfile(profile);
        Settings::removeCurrentProfile();
        QString profileLast = Settings::getLastProfileName();
        emit updateProfile(profileLast);
        return true;
    }
    else
        return false;
}

bool LightpackPluginInterface::SetBacklight(QString sessionKey, int backlight)
{
    if (lockSessionKeys.isEmpty()) return false;
    if (lockSessionKeys[0]!=sessionKey) return false;
    Lightpack::Mode status =  Lightpack::UnknowMode;

    if (backlight == 1)
        status = Lightpack::AmbilightMode;
    else if (backlight == 2)
        status = Lightpack::MoodLampMode;

    if (status != Lightpack::UnknowMode)
    {
        DEBUG_LOW_LEVEL << Q_FUNC_INFO << "OK:" << status;

        emit updateBacklight(status);
        return true;
    }
    return false;
}

int LightpackPluginInterface::GetCountLeds()
{
    return m_curColors.count();
}

int LightpackPluginInterface::GetStatus()
{
    if (m_isRequestBacklightStatusDone)
    {
        m_isRequestBacklightStatusDone = false;
        m_backlightStatusResult = Backlight::StatusUnknown;

        emit requestBacklightStatus();

        // Wait signal from SettingsWindow with status of backlight
        // or if timeout -- result will be unknown
        m_time.restart();
        while (m_isRequestBacklightStatusDone == false && m_time.elapsed() < SignalWaitTimeoutMs)
        {
            QApplication::processEvents(QEventLoop::WaitForMoreEvents, SignalWaitTimeoutMs);
        }

        if (m_isRequestBacklightStatusDone)
        {
            switch (m_backlightStatusResult)
            {
            case Backlight::StatusOn:
                return 1;
                break;
            case Backlight::StatusOff:
                return 0;
                break;
            case Backlight::StatusDeviceError:
                return -1;
                break;
            default:
                return -2;
                break;
            }
        } else {
            m_isRequestBacklightStatusDone = true;
            return -2;
        }
    }
   return -2;
}

bool LightpackPluginInterface::GetStatusAPI()
{
    return (!lockSessionKeys.isEmpty());
}

QStringList LightpackPluginInterface::GetProfiles()
{
    return Settings::findAllProfiles();
}

QString LightpackPluginInterface::GetProfile()
{
    return Settings::getCurrentProfileName();
}

QList<QRect> LightpackPluginInterface::GetLeds()
{
    QList<QRect> leds;
    for (int i = 0; i < Settings::getNumberOfLeds(Settings::getConnectedDevice()); i++)
    {
        QPoint top = Settings::getLedPosition(i);
        QSize size = Settings::getLedSize(i);
        leds << QRect(top.x(),top.y(),size.width(),size.height());
    }
    return leds;
}

QList<QRgb> LightpackPluginInterface::GetColors()
{
   return m_curColors;
}

double LightpackPluginInterface::GetFPS()
{
    return hz;
}

QRect LightpackPluginInterface::GetScreenSize()
{
    return screen;
}

int LightpackPluginInterface::GetBacklight()
{
    Lightpack::Mode mode =  Settings::getLightpackMode();

    switch (mode)
    {
    case Lightpack::AmbilightMode:
        return 1;
        break;
    case Lightpack::MoodLampMode:
        return 2;
        break;
    }
}

// TODO: settings (global or profile?)
void LightpackPluginInterface::SetSettingProfile(QString key, QVariant value)
{
    Settings::setValue(key,value);
}

QVariant LightpackPluginInterface::GetSettingProfile(QString key)
{
    return Settings::value(key);
}

void LightpackPluginInterface::SetSettingMain(QString key, QVariant value)
{
    Settings::setValueMain(key,value);
}

QVariant LightpackPluginInterface::GetSettingMain(QString key)
{
    return Settings::valueMain(key);
}


