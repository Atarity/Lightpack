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
// setcolor:1-r,g,b;5-r,g,b;   numbering starts with 1
// setgamma:2.00 - set gamma for setcolor
// setsmooth:100 -set smooth in device
// setprofile:<name> - set profile
// setstatus:on - set status (on, off)

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
    ClientSettings cs;
    cs.gamma = 2;
    cs.smooth = 100;
    clients.insert(client,cs);

    client->write(QString("version:%1\n").arg(VERSION_API).toUtf8());

    DEBUG_LOW_LEVEL << "New client from:" << client->peerAddress().toString();

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

QString ApiServer::getCommand(QString str)
{
    QString command = "empty";
    if (str.length()>0)
    {
        if (str.indexOf(":")>0)
            command = str.split(":")[0];
        else
            command=str;
    }
    return command;
 }

QString ApiServer::getArg(QString str)
{
    QString arg = "";
    if(str.indexOf(":")>0)
        arg = str.split(":")[1];
    return arg;
}

void ApiServer::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    while(client->canReadLine())
    {
        QString line = QString::fromUtf8(client->readLine()).trimmed();
        DEBUG_LOW_LEVEL << "Read line:" << line;

        QString command = getCommand(line);

        MainWindow *mw = (MainWindow*)parent();
        try{
            if (command=="getstatus")
            {
                QString status = "off";
                if (mw->isAmbilightOn) status="on" ;
                client->write(QString("status:%1\n").arg(status).toUtf8());
            }else if (command=="setstatus") {
                    QString status = getArg(line);
                    QString tmp = "error";
                    if  (status=="on") {mw->ambilightOn();tmp = "ok";}
                    if  (status=="off") {mw->ambilightOff();tmp="ok";}
                    client->write(QString("setstatus:%1\n").arg(tmp).toUtf8());
            }else if (command=="getstatusapi") {
                QString statusApi = "busy";
                if (this->activeClient==NULL) statusApi="idle" ;
                client->write(QString("statusapi:%1\n").arg(statusApi).toUtf8());
            }else if (command=="getprofile") {
                QString profile =  QFileInfo(Settings::fileName()).completeBaseName();
                client->write(QString("profile:%1\n").arg(profile).toUtf8());
            }else if (command=="getprofiles") {
                QString profiles;
                QStringList settingsFiles = mw->profilesFindAll();
                for(int i=0; i<settingsFiles.count(); i++){
                    profiles +=settingsFiles.at(i)+";";
                }
                client->write(QString("profiles:%1\n").arg(profiles).toUtf8());
            }else if (command=="setprofile") {
                QString profile = getArg(line);
                QString tmp = "unknow";
                QStringList settingsFiles = mw->profilesFindAll();
                for(int i=0; i<settingsFiles.count(); i++){
                    if (profile==settingsFiles.at(i)){
                    //load profile
                    mw->profileSwitchCombobox(profile);
                    tmp = "set";
                    break;
                    }
                }
                client->write(QString("setprofile:%1 %2\n").arg(tmp,profile).toUtf8());
            }else if (command=="lock") {
                QString ret = "error";
                if (activeClient==NULL)
                {
                    activeClient = client;
                    ret="success";
                    //disable capture
                    mw->grabManager->setAmbilightOn(false,true);
                    // mw->ledDevice->setSmoothSlowdown(cs.smooth);
                }
                if(activeClient == client) ret = "success";
                client->write(QString("lock:%1\n").arg(ret).toUtf8());
            }else if (command=="unlock") {
                QString ret = "error";
                if (activeClient == NULL) ret = "success";
                if (activeClient==client)
                {
                    activeClient = NULL;
                    ret="success";
                    //enable capture
                    mw->grabManager->setAmbilightOn(true,true);
                }
                client->write(QString("unlock:%1\n").arg(ret).toUtf8());
            }else if(command=="setgamma"){
                QString str = getArg(line);
                bool ok;
                 QString ret = "error";
                double gamma = str.toDouble(&ok);
                if (ok)
                {
                     ret = "ok";
                    if (gamma<0.01) {gamma=0.01;ret="ok(0.01)";};
                    if (gamma>3) {gamma=3;ret="ok(3.00)";};
                     ClientSettings cs = clients.value(client);
                     cs.gamma = gamma;
                     clients.remove(client);
                     clients.insert(client,cs);
                }
                client->write(QString("setgamma:%1\n").arg(ret).toUtf8());
            }else if(command=="setsmooth"){
            QString str = getArg(line);
            bool ok;
             QString ret = "error";
            double smooth = str.toDouble(&ok);
            if (ok)
            {
                ret = "ok";
               if (smooth<0) {smooth=0;ret="ok(0)";};
               if (smooth>255) {smooth=255;ret="ok(255)";};
                 ClientSettings cs = clients.value(client);
                 cs.smooth = smooth;
                 clients.remove(client);
                 clients.insert(client,cs);
                 mw->ledDevice->setSmoothSlowdown(cs.smooth);
            }
            client->write(QString("setsmooth:%1\n").arg(ret).toUtf8());
        }else if (command=="setcolor") {
                if (client == activeClient)
                {
                    QString str = getArg(line);
                    DEBUG_HIGH_LEVEL << "Colors line:" << str;
                    // parse colors
                    QString tmp="ok";
                    if (str=="") tmp = "error";
                    QStringList colors = str.split(";");
                    for (int i = 0; i < colors.size(); ++i)
                    {
                        QString color = colors.at(i);
                        if (color!="")
                        {
                            qDebug() << "Color:" << color;
                            bool ok;
                            int num=0,r=0,g=0,b=0;
                            if (color.indexOf("-")>0)
                            {
                                   num= color.split("-")[0].toInt(&ok);
                                   if (ok)
                                   {
                                       QStringList rgb = color.split("-")[1].split(",");
                                       if (rgb.count()>0) r = rgb[0].toInt(&ok);
                                       if (rgb.count()>1) g = rgb[1].toInt(&ok);
                                       if (rgb.count()>2) b = rgb[2].toInt(&ok);
                                       if (r<0)r=0; if (r>255)r=255;
                                       if (g<0)g=0; if (g>255)g=255;
                                       if (b<0)b=0; if (b>255)b=255;
                                   }
                            }
                            if ((ok)&&(num>0)&&(num<LEDS_COUNT+1))
                            {
                                ClientSettings cs = clients.value(client);
                                qDebug() << "gamma "<< cs.gamma;

                                mw->ledDevice->setSmoothSlowdown(cs.smooth);

                                r = 256.0 * pow( r  / 256.0, cs.gamma );
                                g = 256.0 * pow( g / 256.0, cs.gamma );
                                b = 256.0 * pow( b  / 256.0, cs.gamma );

                                if(r > 0xff) r = 0xff;
                                if(g > 0xff) g = 0xff;
                                if(b > 0xff) b = 0xff;
                                colorsNew[num-1].rgb = qRgb(r,g,b);
                            }
                            else
                                tmp = "error";
                        }
                    }
                    emit updateLedsColors( colorsNew );
                    mw->updateGrabbedColors(colorsNew);
                    client->write(QString("setcolor:%1\n").arg(tmp).toUtf8());
                }
                else
                {
                    if (activeClient!=NULL)
                        client->write(QString("setcolor:busy\n").toUtf8());
                    else
                        client->write(QString("setcolor:need lock\n").toUtf8());
                }
            }else{
                client->write(QString("unknow command\n").toUtf8());
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
    DEBUG_LOW_LEVEL << "Client disconnected:" << client->peerAddress().toString();
    if (activeClient==client)
    {
        activeClient = NULL;
        MainWindow *mw = (MainWindow*)parent();
        mw->grabManager->setAmbilightOn(true,true);
    }
    clients.remove(client);
}
