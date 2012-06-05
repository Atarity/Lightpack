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
     void requestBacklightStatus();
     void updateDeviceLockStatus(DeviceLocked::DeviceLockStatus status);
     void updateLedsColors(const QList<QRgb> & colors);
     void updateGamma(double value);
     void updateBrightness(int value);
     void updateSmooth(int value);
     void updateProfile(QString profileName);
     void updateStatus(Backlight::Status status);


public slots:
     void setDeviceLockViaAPI(DeviceLocked::DeviceLockStatus status);
     void resultBacklightStatus(Backlight::Status status);
     void changeProfile(QString profile);


private slots:
      void timeoutLock();

private:
      bool lockAlive;

      static const int SignalWaitTimeoutMs;
      QTime m_time;
      bool m_isRequestBacklightStatusDone;
      Backlight::Status m_backlightStatusResult;

     QString lockSessionKey;
     QList<QRgb> m_colors;
     QTimer *m_timerLock;
     void initColors(int numberOfLeds);

// Plugin section
signals:
     void ChangeProfile(QString profile);
     void ChangeStatus(int status);
     void ChangeLockStatus(bool lock);

public slots:
     QString GetSessionKey();
     bool Lock(QString sessionKey);

// need LOCK
     bool UnLock(QString sessionKey);
     bool SetStatus(QString sessionKey, int status);
     bool SetColors(QString sessionKey, int r, int g, int b);
     bool SetColor(QString sessionKey, int ind,int r, int g, int b);
     bool SetGamma(QString sessionKey, int gamma);
     bool SetBrightness(QString sessionKey, int brightness);
     bool SetSmooth(QString sessionKey, int smooth);
     bool SetProfile(QString sessionKey, QString profile);

// no LOCK
     int GetCountLeds();
     int GetStatus();
     bool GetStatusAPI();
     QStringList GetProfiles();
     QString GetProfile();

// Settings
     void SetSettingProfile(QString key, QVariant value);
     QVariant GetSettingProfile(QString key);
     void SetSettingMain(QString key, QVariant value);
     QVariant GetSettingMain(QString key);
//end Plugin section

};

#endif // LIGHTPACKPLUGININTERFACE_H
