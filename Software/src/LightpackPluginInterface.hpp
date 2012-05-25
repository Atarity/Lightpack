#ifndef LIGHTPACKPLUGININTERFACE_H
#define LIGHTPACKPLUGININTERFACE_H

#include <QObject>
#include "ApiServer.hpp"


class LightpackPluginInterface : public QObject
{
    Q_OBJECT
public:
    explicit LightpackPluginInterface(QObject *parent = 0);
    
signals:
     void updateDeviceLockStatus(Api::DeviceLockStatus status);
     void updateLedsColors(const QList<QRgb> & colors);

public slots:
     bool Lock();
     bool UnLock();

     void setColors(int r, int g, int b);
     void setColor(int ind,int r, int g, int b);

     // Settings
     void setSettingProfile(QString key, QVariant value);
     QVariant getSettingProfile(QString key);
     void setSettingMain(QString key, QVariant value);
     QVariant getSettingMain(QString key);

private:
     bool m_locked;
     QList<QRgb> m_colors;
     void initColors(int numberOfLeds);
};

#endif // LIGHTPACKPLUGININTERFACE_H
