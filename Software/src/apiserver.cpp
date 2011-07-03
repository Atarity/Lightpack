#include <QtNetwork>
#include <stdlib.h>

#include "apiserver.h"
#include "../inc/mainwindow.h"

// get
// getstatus - on off
// getstatusapi - work idle
// getprofiles - list name profiles
// getprofile - current name profile

// commands
// lock - begin work with api (disable capture,backlight)
// unlock - end work with api (enable capture,backlight)
// setcolor:1-r,g,b;5-r,g,b;
// setprofile:<name>
// setstatus: - on, off

ApiServer::ApiServer(QObject *parent)
    : QTcpServer(parent)
{
    activeClient = NULL;
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colorsNew     << StructRGB();
    }
}

void ApiServer::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clients.insert(client);

    client->write(QString("version:%1\n").arg(VERSION_API).toUtf8());

    qDebug() << "New client from:" << client->peerAddress().toString();

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void ApiServer::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    while(client->canReadLine())
    {
        QString line = QString::fromUtf8(client->readLine()).trimmed();
        qDebug() << "Read line:" << line;

        MainWindow *mw = (MainWindow*)parent();
        try{
            if (line=="getstatus")
            {
                QString status = "off";
                if (mw->isAmbilightOn) status="on" ;
                client->write(QString("status:%1\n").arg(status).toUtf8());
            }else if (line.left(9)=="setstatus") {
                    QString status = line.split(":")[1];
                    if  (status=="on") mw->ambilightOn();
                    if  (status=="off") mw->ambilightOff();
                    client->write(QString("ok\n").toUtf8());
            }else if (line=="getstatusapi") {
                QString statusApi = "busy";
                if (this->activeClient==NULL) statusApi="idle" ;
                client->write(QString("status:%1\n").arg(statusApi).toUtf8());
            }else if (line=="getprofile") {
                QString profile = Settings::fileName();
                client->write(QString("profile:%1\n").arg(profile).toUtf8());
            }else if (line=="getprofiles") {
                QString profiles;
                QStringList settingsFiles = mw->profilesFindAll();
                for(int i=0; i<settingsFiles.count(); i++){
                    profiles +=settingsFiles.at(i)+";";
                }
                client->write(QString("profiles:%1\n").arg(profiles).toUtf8());
            }else if (line.left(10)=="setprofile") {
                QString profile = line.split(":")[1];
                // load profile
                mw->profileSwitch(profile);
                client->write(QString("profile:%1\n").arg(profile).toUtf8());
            }else if (line=="lock") {
                QString ret = "error";
                if (activeClient==NULL)
                {
                    activeClient = client;
                    ret="success";
                    //todo disable capture
                    mw->grabManager->setAmbilightOn(false,true);

                }
                if(activeClient == client) ret = "success";
                client->write(QString("lock:%1\n").arg(ret).toUtf8());
            }else if (line=="unlock") {
                QString ret = "error";
                if (activeClient == NULL) ret = "success";
                if (activeClient==client)
                {
                    activeClient = NULL;
                    ret="success";
                    //todo enable capture
                    mw->grabManager->setAmbilightOn(true,true);

                }
                client->write(QString("unlock:%1\n").arg(ret).toUtf8());
            }else if (line.left(8)=="setcolor") {
                //todo
                QString str = line.split(":")[1];
                qDebug() << "Colors line:" << str;
                QStringList colors = str.split(";");
                for (int i = 0; i < colors.size(); ++i)
                {
                    QString color = colors.at(i);
                    if (color!="")
                    {
                        qDebug() << "Color:" << color;
                        QString num = color.split("-")[0];
                        QStringList rgb = color.split("-")[1].split(",");
                        colorsNew[num.toInt()].rgb = qRgb(rgb[0].toInt(),rgb[1].toInt(),rgb[2].toInt());
                    }
                }
                emit updateLedsColors( colorsNew );

                client->write(QString("blabla\n").toUtf8());
            }else{
                client->write(QString("unknow\n").toUtf8());
            }
        }
        catch(...)
        {
            client->write(QString("error\n").toUtf8());
        }

    }
}

void ApiServer::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();
    if (activeClient==client)
    {
        activeClient = NULL;
        MainWindow *mw = (MainWindow*)parent();
        mw->grabManager->setAmbilightOn(true,true);
    }
    clients.remove(client);
}
