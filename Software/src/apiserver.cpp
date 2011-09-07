#include <QtNetwork>
#include <stdlib.h>

#include "apiserver.h"
#include "../inc/mainwindow.h"

#include "settings.h"
#include "timeevaluations.h"

// get
// getstatus - on off
// getstatusapi - busy idle
// getprofiles - list name profiles
// getprofile - current name profile

// commands
// lock - begin work with api (disable capture,backlight)
// unlock - end work with api (enable capture,backlight)
// setcolor:1-r,g,b;5-r,g,b;   numbering starts with 1
// setgamma:2.00 - set gamma for setcolor
// setsmooth:100 - set smooth in device
// setprofile:<name> - set profile
// setstatus:on - set status (on, off)

ApiServer::ApiServer(QObject *parent)
    : QTcpServer(parent)
{
    activeClient = NULL;
    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        colorsNew     << 0;
    }
}

void ApiServer::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    ClientSettings cs;
    cs.gamma = GAMMA_CORRECTION_DEFAULT_VALUE;
    cs.smooth = FW_SMOOTH_SLOWDOWN_DEFAULT;
    cs.auth = false;
    clients.insert(client,cs);

    client->write(QString("version:%1\n").arg(VERSION_API).toUtf8());

    DEBUG_LOW_LEVEL << "New client from:" << client->peerName();// client->peerAddress().toString();

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

QString ApiServer::getCommand(const QString & str)
{
    QString cmd = str;
    int colonIndex = str.indexOf(":");
    if (colonIndex > 0)
    {
        cmd.truncate(colonIndex);
    }
    return cmd;
}

QString ApiServer::getArg(const QString &  str)
{
    QString arg = "";
    if(str.indexOf(":")>0)
        arg = str.split(":")[1];
    return arg;
}

// Convert ASCII char '5' to 5
static inline char getDigit(const char d)
{
    if (isdigit(d))
        return (d - '0');
    else
        return -1;
}


void ApiServer::readyRead_Buffer()
{
    // ignore

    DEBUG_LOW_LEVEL << Q_FUNC_INFO;
}


TimeEvaluations timeEval;
double allTime = 0;
int allTimeCount = 0;
#define ALL_TIME_MAX    1000

void ApiServer::readyRead()
{
#define API_DEBUG           0
#define API_SETCOLOR_NEW    2
#define API_TIME_EVAL       0

#if API_DEBUG
#   define API_DEBUG_OUT   qDebug()
#else
#   define API_DEBUG_OUT   if(0) qDebug()
#endif

    API_DEBUG_OUT << Q_FUNC_INFO << "ApiServer thread id:" << this->thread()->currentThreadId();


    QTcpSocket *client = (QTcpSocket*)sender();


    // Start magic
    disconnect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead_Buffer()));

    while (client->canReadLine())
    {

#if API_SETCOLOR_NEW == 2
#define TIME_NAME "setcolor NEW=2"

#if API_TIME_EVAL
        timeEval.howLongItStart();
#endif

        bool isReadFail = false;

        // setcolor:2-0,255,0;3-0,255,0;6-0,255,0;


        QByteArray buffer = client->readLine();

        API_DEBUG_OUT << "Read line:" << QString(buffer);
        if (buffer.startsWith("setcolor:"))
        {
            buffer.remove(0, buffer.indexOf(':') + 1);
            API_DEBUG_OUT << "setcolor-" << QString(buffer);

            // 2-0,255,0;3-0,255,0;6-0,255,0;

            while (buffer.isEmpty() == false)
            {
                // Read led number
                int ledNumber = getDigit(buffer[0]); // first char of ledNumber
                int ledNumber2 = getDigit(buffer[1]); // second char of ledNumber
                if (ledNumber > 0)
                {
                    if (buffer[1] == '-')
                    {
                        buffer.remove(0, 2); // remove "2-"
                    }
                    else if (ledNumber2 >= 0 && buffer[2] == '-')
                    {
                        ledNumber = ledNumber * 10 + ledNumber2; // 10,11,12..99
                        buffer.remove(0, 3); // remove "10-"
                    } else {
                        API_DEBUG_OUT << "lednumber fail:" << QString(buffer);
                        isReadFail = true;
                        break;
                    }
                } else {
                    API_DEBUG_OUT << "isdigit fail:" << QString(buffer);
                    isReadFail = true;
                    break;
                }

                API_DEBUG_OUT << "lednumber-" << ledNumber << "buff-" << QString(buffer);

                if (ledNumber <= 0 || ledNumber > LEDS_COUNT)
                {
                    isReadFail = true;
                    break;
                }

                // Convert for using in zero-based arrays
                ledNumber = ledNumber - 1;

                // Read led red, green and blue colors to buffer buffRgb[]
                int indexBuffer = 0;
                int indexRgb = 0;
                memset(buffRgb, 0, sizeof(buffRgb));

                for (indexBuffer = 0; indexBuffer < buffer.length(); indexBuffer++){
                    int d = getDigit(buffer[indexBuffer]);
                    if (d < 0) {
                        if (buffer[indexBuffer] != ',')
                        {
                            isReadFail = true;
                            break;
                        }
                        else if (buffer[indexBuffer] == ';')
                        {
                            break;
                        }
                        indexRgb++;
                    } else {
                        buffRgb[indexRgb] *= 10;
                        buffRgb[indexRgb] += d;
                    }
                }

                // Remove read colors
                buffer.remove(0, indexBuffer);

                // Save colors
                colorsNew[ledNumber] = qRgb(buffRgb[bRed], buffRgb[bGreen], buffRgb[bBlue]);

                API_DEBUG_OUT << "result color-" << buffRgb[bRed] << buffRgb[bGreen] << buffRgb[bBlue]
                              << "buff-" << QString(buffer);

                if (buffer[0] == ';')
                {
                    // Remove semicolon
                    buffer.remove(0, 1);
                } else {
                    // Buffer is empty
                    break;
                }
            }
        }


        // TODO: use isReadFail for check errors

#if API_TIME_EVAL
        allTime += timeEval.howLongItEnd();

        if (++allTimeCount >= ALL_TIME_MAX)
        {
            qDebug() << TIME_NAME << "time:" << allTime << "times:" << allTimeCount;
            allTimeCount = 0;
            allTime = 0;
        }
#endif

#endif

#if (API_SETCOLOR_NEW == 0) || (API_SETCOLOR_NEW == 1)
        QString line = QString(client->readLine().trimmed());
        QString command = getCommand(line);

        API_DEBUG_OUT << "Read line:" << line << "Cmd:" << command;

        if (command=="setcolor") {
#           if API_TIME_EVAL
            timeEval.howLongItStart();
#           endif

#endif

#if API_SETCOLOR_NEW == 1
#define TIME_NAME "setcolor NEW"

            // setcolor:2-0,255,0;3-0,255,0;6-0,255,0;7-0,255,0;8-0,255,0;9-0,255,0;10-0,255,0;4-0,255,0;5-0,255,0;1-0,255,0;
            line.remove(0, line.indexOf(':') + 1);
#if API_DEBUG
            API_DEBUG_OUT << "line-cmd: " << line;
#endif
            int number = line[0].digitValue();
            int num2 = line[1].digitValue();
            if (num2 != -1) {
                number = 10;
                line.remove(0,3);
            } else {
                line.remove(0,2);
            }
#if API_DEBUG
            API_DEBUG_OUT << "led num: " << number;
            API_DEBUG_OUT << "line-num: " << line;
#endif
            int rgb[3] = {};
            int rank = 1;
            int index = 0;

            for (int i=0; i<line.length(); i++){
                int d = line[i].digitValue();
                if (d < 0) {
                    index++;
                    rank = 1;
                } else {
                    rgb[index] *= rank;
                    rgb[index] += d;
                    rank = 10;
                }
            }
#if API_DEBUG
            API_DEBUG_OUT << "rgb:" << rgb[0] << rgb[1] << rgb[2];
#endif
            colorsNew[number-1] = qRgb(rgb[0],rgb[1],rgb[2]);


#elif API_SETCOLOR_NEW == 0
#define TIME_NAME "setcolor old"
            QString str = getArg(line);
            API_DEBUG_OUT << "Colors line:" << str;
            // parse colors
            QString tmp="ok";
            if (str=="") tmp = "error";
            QStringList colors = str.split(";");
            for (int i = 0; i < colors.size(); ++i)
            {
                QString color = colors.at(i);
                if (color!="")
                {
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

                        r = 256.0 * pow( r  / 256.0, cs.gamma );
                        g = 256.0 * pow( g / 256.0, cs.gamma );
                        b = 256.0 * pow( b  / 256.0, cs.gamma );

                        if(r > 0xff) r = 0xff;
                        if(g > 0xff) g = 0xff;
                        if(b > 0xff) b = 0xff;
                        colorsNew[num-1] = qRgb(r,g,b);
                    }
                    else
                        tmp = "error";
                }
            }

            API_DEBUG_OUT << hex << colorsNew[0] << colorsNew[1] << dec << tmp;
#endif

#if API_TIME_EVAL && (API_SETCOLOR_NEW != 2)
            allTime += timeEval.howLongItEnd();

            if (++allTimeCount >= ALL_TIME_MAX)
            {
                qDebug() << TIME_NAME << "time:" << allTime << "times:" << allTimeCount;
                allTimeCount = 0;
                allTime = 0;
            }
#endif
            emit updateLedsColors( colorsNew );
            client->write("nop");
        }
        // End magic
        QApplication::removePostedEvents(client);

        disconnect(client, SIGNAL(readyRead()), this, SLOT(readyRead_Buffer()));
        connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }
//}

void ApiServer::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    DEBUG_LOW_LEVEL << "Client disconnected:" << client->peerAddress().toString();
    if (activeClient==client)
    {
        activeClient = NULL;
        //        MainWindow *mw = (MainWindow*)parent();
        //        mw->m_grabManager->setAmbilightOn(mw->m_isAmbilightOn,true);
    }
    clients.remove(client);
}
