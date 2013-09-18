#pragma once

#include <QObject>
#include <QProcess>
#include "debug.h"

class Plugin : public QObject
{
    Q_OBJECT
public:
    Plugin(QString name, QString path,QObject *parent = 0);
    ~Plugin();

    void Start();
    void Stop();

    QString Name() ;
    QString Guid() ;
    QString Description() ;
    QString Author() ;
    QString Version() ;
    QIcon Icon() ;

    bool isEnabled();
    void setEnabled(bool enable);
    void setPriority(int priority);
    int getPriority();


signals:
    
public slots:
    
private:

    QString _guid;
    QString _name;
    QString _description;
    QString _author;
    QString _version;
    QString _icon;
    QString _exec;
    QString _arguments;
    QString _pathPlugin;
    QProcess *process;

};


