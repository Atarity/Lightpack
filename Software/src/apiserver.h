#ifndef APISERVER_H
#define APISERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>

#include "struct_rgb.h"

#define VERSION_API      "1.0"


class ApiServer : public QTcpServer
{
    Q_OBJECT

    public:
        ApiServer(QObject *parent=0);

    signals:
        void updateLedsColors(const QList<StructRGB> & colorsNew);

    private slots:
        void readyRead();
        void disconnected();

    protected:
        void incomingConnection(int socketfd);

    private:
        QSet<QTcpSocket*> clients;
        QTcpSocket* activeClient;
        QList<StructRGB> colorsNew;


};


#endif // APISERVER_H
