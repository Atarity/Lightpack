#ifndef APISERVER_H
#define APISERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>

#include "struct_rgb.h"

#define VERSION_API      "1.1"

class ClientSettings
{
    public:
        int smooth;
        double gamma;
        bool auth;
};

class ApiServer : public QTcpServer
{
    Q_OBJECT

    public:
        bool enableAuth;
        QString ApiKey;
        ApiServer(QObject *parent=0);


    signals:
        void updateLedsColors(const QList<StructRGB> & colorsNew);

    private slots:
        void readyRead();
        void disconnected();

    protected:
        void incomingConnection(int socketfd);

    private:
        //QSet<QTcpSocket*> clients;
        QMap<QTcpSocket*,ClientSettings>clients;
        QTcpSocket* activeClient;
        QList<StructRGB> colorsNew;

        QString getCommand(QString str);
        QString getArg(QString str);

};


#endif // APISERVER_H
