#include "LightpackPluginInterface.hpp"
#include "ApiServer.hpp"
#include "Settings.hpp"

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
        UnLock(lockSessionKey);
}

void LightpackPluginInterface::initColors(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;
    m_colors.clear();
    for (int i = 0; i < numberOfLeds; i++)
        m_colors << 0;
}

void LightpackPluginInterface::setNumberOfLeds(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;

    initColors(numberOfLeds);
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

void LightpackPluginInterface::setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus status)
{
    if (status == DeviceLocked::Unlocked)
    {
        lockSessionKey = "";
        emit ChangeLockStatus(false);
    }
    if (status == DeviceLocked::Api)
    {
        lockSessionKey = "LockApi";
        emit ChangeLockStatus(true);
    }
}

// TODO identification plugin locked
QString LightpackPluginInterface::GetSessionKey()
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
            emit ChangeLockStatus (true);
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
            emit ChangeLockStatus(false);
            return true;
        } else {
            return false;
        }
}
// TODO: setcolor
bool LightpackPluginInterface::SetColors(QString sessionKey, int r, int g, int b)
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

bool LightpackPluginInterface::SetColor(QString sessionKey, int ind,int r, int g, int b)
{
    if (lockSessionKey!=sessionKey) return false;
    lockAlive = true;
    if (ind>m_colors.size()-1) return false;
    m_colors[ind] = qRgb(r,g,b);
    emit updateLedsColors(m_colors);
    return true;
}

bool LightpackPluginInterface::SetGamma(QString sessionKey, int gamma)
{
     if (lockSessionKey!=sessionKey) return false;
     if (gamma >= Profile::Device::GammaMin && gamma <= Profile::Device::GammaMax)
     {
         emit updateGamma(gamma);
         return true;
     } else
         return false;
}

bool LightpackPluginInterface::SetBrightness(QString sessionKey, int brightness)
{
     if (lockSessionKey!=sessionKey) return false;
     if (brightness >= Profile::Device::BrightnessMin && brightness <= Profile::Device::BrightnessMax)
     {
         emit updateBrightness(brightness);
         return true;
     } else
         return false;
}

bool LightpackPluginInterface::SetSmooth(QString sessionKey, int smooth)
{
     if (lockSessionKey!=sessionKey) return false;
     if (smooth >= Profile::Device::SmoothMin && smooth <= Profile::Device::SmoothMax)
     {
             emit updateSmooth(smooth);
             return true;
      } else
         return false;
}

bool LightpackPluginInterface::SetProfile(QString sessionKey,QString profile)
{
     if (lockSessionKey!=sessionKey) return false;
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
     if (lockSessionKey!=sessionKey) return false;
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

int LightpackPluginInterface::GetCountLeds()
{
    return m_colors.count();
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
            default:
                return -1;
                break;
            }
        } else {
            m_isRequestBacklightStatusDone = true;
            return -1;
        }
    }
   return -1;
}

bool LightpackPluginInterface::GetStatusAPI()
{
    return (lockSessionKey!="");
}

QStringList LightpackPluginInterface::GetProfiles()
{
    return Settings::findAllProfiles();
}

QString LightpackPluginInterface::GetProfile()
{
    return Settings::getCurrentProfileName();
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




