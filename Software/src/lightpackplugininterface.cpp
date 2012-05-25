#include "LightpackPluginInterface.hpp"
#include "ApiServer.hpp"
#include "Settings.hpp"

using namespace SettingsScope;

LightpackPluginInterface::LightpackPluginInterface(QObject *parent) :
    QObject(parent)
{
initColors(10);
}

//TODO: lock unlock
bool LightpackPluginInterface::Lock()
{
        if (!m_locked)
        {
            m_locked = true;
            emit updateDeviceLockStatus(Api::DeviceLocked);
            return true;
        } else {
            return false;
        }
}

bool LightpackPluginInterface::UnLock()
{
        if (m_locked)
        {
            m_locked = false;
            emit updateDeviceLockStatus(Api::DeviceUnlocked);
            return true;
        } else {
            return false;
        }
}
// TODO: setcolor
void LightpackPluginInterface::setColors(int r, int g, int b)
{
    for (int i = 0; i < m_colors.size(); i++)
    {
            m_colors[i] = qRgb(r,g,b);
    }
    emit updateLedsColors(m_colors);
}
void LightpackPluginInterface::setColor(int ind,int r, int g, int b)
{
    if (ind>m_colors.size()-1) return;
    m_colors[ind] = qRgb(r,g,b);
    emit updateLedsColors(m_colors);
}

void LightpackPluginInterface::initColors(int numberOfLeds)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << numberOfLeds;

    m_colors.clear();

    for (int i = 0; i < numberOfLeds; i++)
        m_colors << 0;
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
