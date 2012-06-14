//#ifndef __PY_PLUGIN_H__
//#define __PY_PLUGIN_H__

#include <PythonQt.h>
#include <QObject>

class PyPlugin : public QObject{

	Q_OBJECT

	public:
        PyPlugin(PythonQtObjectPtr plugin_, QObject* parent_ = 0);
		~PyPlugin();
		
        QString getSessionKey() const;
        QString getName() const;
		QString getDescription() const;
		QString getAuthor() const;
		QString getVersion() const;
        QIcon getIcon() const;
	
        bool isEnabled();
        int getPriority();

        void getSettings();
	
	public slots:
        void init();
        void setEnabled(bool enable);
        void setPriority(int priority);
        void execute();
        void stop();

	signals:
		void aboutToExecute();
		void executed();

    private:
		PythonQtObjectPtr _plugin;
	
        bool runing;
        QString _sessionKey;
		QString _name;
		QString _description;
		QString _author;
		QString _version;
        QString _icon;
};

//#endif // __PLUGIN_ENGINE_H__
