#ifndef SYSTRAYICON_UNITY_P_HPP
#define SYSTRAYICON_UNITY_P_HPP

#include "SysTrayIcon_p.hpp"
#include "Settings.hpp"
#undef signals
#include <libappindicator/app-indicator.h>
#include <QObject>
#include <QUrl>
#include <libnotify/notify.h>

static void action_switch_on (GtkAction *action, SysTrayIcon *q);
static void action_switch_off (GtkAction *action, SysTrayIcon *q);
static void action_show_settings (GtkAction *action, SysTrayIcon *q);
static void action_quit (GtkAction *action, SysTrayIcon *q);

static GtkActionEntry entries[] = {
 /*  Name, 		stock_id, 	label, 		accelerator, 	tooltip, 	callback  */
    { "On",       NULL,     "On",        NULL,           NULL,       G_CALLBACK (action_switch_on) },
    { "Off",      NULL,     "Off",       NULL,           NULL,       G_CALLBACK (action_switch_off) },
    { "Settings", NULL,     "Settings",  NULL,           NULL,       G_CALLBACK (action_show_settings) },
    { "Quit",     NULL,     "Quit",      NULL,           NULL,       G_CALLBACK (action_quit) },
};

static guint n_entries = G_N_ELEMENTS (entries);

static const gchar *ui_info =
"<ui>"
"  <popup name='IndicatorPopup'>"
"    <menuitem action='On' />"
"    <menuitem action='Off' />"
"    <menuitem action='Settings' />"
"    <separator />"
"    <menuitem action='Quit' />"
"  </popup>"
"</ui>";

static void action_switch_on (GtkAction *action, SysTrayIcon *q)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    q->backlightOn();
}

static void action_switch_off (GtkAction *action, SysTrayIcon *q)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    q->backlightOff();
}


static void action_show_settings (GtkAction *action, SysTrayIcon *q)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    q->showSettings();
}

static void action_quit (GtkAction *action, SysTrayIcon *q)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
    q->quit();
}

class SysTrayIconPrivate : public QObject, public SysTrayIconPrivateData
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(SysTrayIcon)

public:
    SysTrayIconPrivate(SysTrayIcon * q)
        : SysTrayIconPrivateData(q)
    {
        GtkWidget *indicator_menu;
        GtkActionGroup *action_group;
        GtkUIManager *uim;
        AppIndicator *indicator;
        GError *error = NULL;

        notify_init (tr("Prismatik").toUtf8().constData());

        /* Menus */
        action_group = gtk_action_group_new ("AppActions");
        gtk_action_group_add_actions (action_group, entries, n_entries, q_ptr);

        uim = gtk_ui_manager_new ();
        gtk_ui_manager_insert_action_group (uim, action_group, 0);

        if (!gtk_ui_manager_add_ui_from_string (uim, ui_info, -1, &error))
        {
          DEBUG_LOW_LEVEL << "Failed to build menus:"<< error->message;
          g_message ("Failed to build menus: %s\n", error->message);
          g_error_free (error);
          error = NULL;
        }
        gtk_ui_manager_ensure_update(uim);

        /* Indicator */
        indicator = app_indicator_new ("prismatik-indicator",
                    "prismatik-on",
                    APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

        indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");

        app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
        //app_indicator_set_attention_icon (indicator, "prismatik-on");

        app_indicator_set_menu (indicator, GTK_MENU (indicator_menu));
        gtk_widget_show_all(indicator_menu);
    }

    virtual ~SysTrayIconPrivate()
    {
    notify_uninit();
    return;
    }

    void init()
    {
        connect(&_updatesProcessor, SIGNAL(readyRead()), this, SLOT(onCheckUpdate_Finished()));
    }

    bool isVisible() const
    {
        return true;
    }

    void hide()
    {

    }

    void show()
    {

    }

    void showMessage(const QString &title, const QString &text)
    {

    }

    void showMessage(const SysTrayIcon::Message msg)
    {

    }

    void retranslateUi()
    {

    }

    void setStatus(const SysTrayIcon::Status status, const QString *arg = NULL)
    {

    }

    QString toolTip() const
    {
        return "";
    }

    void updateProfiles()
    {

    }

    virtual void checkUpdate()
    {
        _updatesProcessor.requestUpdates();
    }

private slots:
    void onCheckUpdate_Finished()
    {
        using namespace SettingsScope;
        QList<UpdateInfo> updates = _updatesProcessor.readUpdates(Settings::getLastReadUpdateId());
        if (updates.size() > 0) {
            if(updates.size() > 1) {
                NotifyNotification * updates = notify_notification_new ("Updates are available", "please visit http://lightpack.tv","Prismatik");
                notify_notification_show (updates, NULL);
                g_object_unref(G_OBJECT(updates));
            } else {
                UpdateInfo updateInfo = updates.last();
                if (!updateInfo.url.isEmpty())
                    updateInfo.text = updateInfo.text.append("\n please visit ").append(updateInfo.url);
                NotifyNotification * update = notify_notification_new (updateInfo.title.toUtf8().constData(), updateInfo.text.toUtf8().constData(), "Prismatik");
                notify_notification_show (update, NULL);
                g_object_unref(G_OBJECT(update));
            }
            Settings::setLastReadUpdateId(updates.last().id);
        }
    }
};

#endif // SYSTRAYICON_UNITY_P_HPP
