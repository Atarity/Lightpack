#pragma once

#include <QtGui>
#include <QObject>
#include "enums.hpp"

class LightpackPluginInterface : public QObject
{
    Q_OBJECT
public:
    LightpackPluginInterface(QObject *parent = 0);
    ~LightpackPluginInterface();

 public slots:
// Plugin section
     QString GetSessionKey();
     int CheckLock(QString sessionKey);
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

     bool SetLeds(QString sessionKey, QList<QRect> leds);
     bool NewProfile(QString sessionKey, QString profile);
     bool DeleteProfile(QString sessionKey, QString profile);
     bool SetBacklight(QString sessionKey, int backlight);

// no LOCK
     QString Version();
     int GetCountLeds();
     int GetStatus();
     bool GetStatusAPI();
     QStringList GetProfiles();
     QString GetProfile();
     QList<QRect> GetLeds();
     QList<QRgb> GetColors();
     double GetFPS();
     QRect GetScreenSize();
     int GetBacklight();

// Settings
     void SetSettingProfile(QString key, QVariant value);
     QVariant GetSettingProfile(QString key);
     void SetSettingMain(QString key, QVariant value);
     QVariant GetSettingMain(QString key);

signals:
     void ChangeProfile(QString profile);
     void ChangeStatus(int status);
     void ChangeLockStatus(bool lock);

//end Plugin section

signals:
     void requestBacklightStatus();
     void updateDeviceLockStatus(DeviceLocked::DeviceLockStatus status);
     void updateLedsColors(const QList<QRgb> & colors);
     void updateGamma(double value);
     void updateBrightness(int value);
     void updateSmooth(int value);
     void updateProfile(QString profileName);
     void updateStatus(Backlight::Status status);
     void updateBacklight(Lightpack::Mode status);


public slots:
     void setNumberOfLeds(int numberOfLeds);
     void resultBacklightStatus(Backlight::Status status);
     void changeProfile(QString profile);
     void refreshAmbilightEvaluated(double updateResultMs);
     void refreshScreenRect(QRect rect);
     void updateColors(const QList<QRgb> & colors);

private slots:
      void timeoutLock();

private:
      bool lockAlive;

      static const int SignalWaitTimeoutMs;
      QTime m_time;
      bool m_isRequestBacklightStatusDone;
      Backlight::Status m_backlightStatusResult;

      double hz;
      QRect screen;

     QString lockSessionKey;
     QList<QRgb> m_colors;
     QTimer *m_timerLock;
     void initColors(int numberOfLeds);

};


