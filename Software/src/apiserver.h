#ifndef APISERVER_H
#define APISERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QRgb>

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
    QString ApiKey;
    ApiServer(QObject *parent = 0);


signals:
    void updateLedsColors(const QList<QRgb> & colorsNew);

private slots:
    void readyRead();
    void readyRead_Buffer();
    void disconnected();

protected:
    void incomingConnection(int socketfd);

private:
    //QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*,ClientSettings>clients;
    QTcpSocket* activeClient;
    QList<QRgb> colorsNew;

    QString getCommand(const QString &  str);
    QString getArg(const QString &  str);


private:
    enum BuffRgbIndexes{
        bRed, bGreen, bBlue, bSize
    };

    int buffRgb[bSize]; // buffer for store temp red, green and blue values

};


#endif // APISERVER_H
