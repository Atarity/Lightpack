//#ifndef __PY_PLUGIN_H__
//#define __PY_PLUGIN_H__

#include <PythonQt.h>
#include <QObject>

class PyPlugin : public QObject{

	Q_OBJECT

	public:
        PyPlugin(PythonQtObjectPtr plugin_, QObject* parent_ = 0);
		~PyPlugin();
		
		QString getName() const;
		QString getDescription() const;
		QString getAuthor() const;
		QString getVersion() const;
	
        bool isEnabled();

        void getSettings();
	
	public slots:
        void setEnabled(bool enable);
        void execute();
	
	signals:
		void aboutToExecute();
		void executed();

    private:
		PythonQtObjectPtr _plugin;
	
		QString _name;
		QString _description;
		QString _author;
		QString _version;
};

//#endif // __PLUGIN_ENGINE_H__
