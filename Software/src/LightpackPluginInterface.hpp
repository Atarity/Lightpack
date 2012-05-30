#ifndef LIGHTPACKPLUGININTERFACE_H
#define LIGHTPACKPLUGININTERFACE_H

#include <QObject>
#include "ApiServer.hpp"


class LightpackPluginInterface : public QObject
{
    Q_OBJECT
public:
    explicit LightpackPluginInterface(QObject *parent = 0);
    ~LightpackPluginInterface();
    void setNumberOfLeds(int numberOfLeds);

    
signals:
     void updateDeviceLockStatus(DeviceLocked::DeviceLockStatus status);
     void updateLedsColors(const QList<QRgb> & colors);

public slots:
     void setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus status);
     QString getSessionKey();
     bool Lock(QString sessionKey);
     bool UnLock(QString sessionKey);

     int numberOfLeds();
     bool setColors(QString sessionKey,int r, int g, int b);
     bool setColor(QString sessionKey,int ind,int r, int g, int b);

     // Settings
     void setSettingProfile(QString key, QVariant value);
     QVariant getSettingProfile(QString key);
     void setSettingMain(QString key, QVariant value);
     QVariant getSettingMain(QString key);

private slots:
      void timeoutLock();

private:
      bool lockAlive;
     QString lockSessionKey;
     QList<QRgb> m_colors;
     QTimer *m_timerLock;
     void initColors(int numberOfLeds);

};

#endif // LIGHTPACKPLUGININTERFACE_H
