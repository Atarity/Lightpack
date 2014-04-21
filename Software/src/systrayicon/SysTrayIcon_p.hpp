#ifndef SYSTRAYICON_P_HPP
#define SYSTRAYICON_P_HPP

#include "SysTrayIcon.hpp"
#include "UpdatesProcessor.hpp"

class SysTrayIconPrivateData
{

public:
    SysTrayIconPrivateData(SysTrayIcon * q)
        : q_ptr(q)
    {
    }

    virtual void init() = 0;

    virtual bool isVisible() const = 0;

    virtual void hide() = 0;

    virtual void show() = 0;

    virtual void showMessage(const QString &title, const QString &text) = 0;

    virtual void showMessage(const SysTrayIcon::Message msg) = 0;

    virtual void retranslateUi() = 0;

    virtual void setStatus(const SysTrayIcon::Status status, const QString *arg = NULL) = 0;

    virtual QString toolTip() const = 0;

    virtual void updateProfiles() = 0;

    virtual void checkUpdate() = 0;

protected:
    SysTrayIcon * const q_ptr;

    UpdatesProcessor _updatesProcessor;



};

#endif // SYSTRAYICON_P_HPP
