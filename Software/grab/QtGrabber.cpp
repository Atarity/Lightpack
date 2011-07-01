#include"QtGrabber.hpp"
#include"debug.h"
#include"QtGui"
QtGrabber::QtGrabber(QObject *parent) : IGrabber(parent)
{
//    capture = NULL;
    screen = 0;
}

QtGrabber::~QtGrabber()
{
}

const char * QtGrabber::getName()
{
    return "QtGrabber";
}

void QtGrabber::updateGrabScreenFromWidget(QWidget *widget)
{
    screen = QApplication::desktop()->screenNumber( widget );
    screenres = QApplication::desktop()->screenGeometry( screen );
}

void QtGrabber::grabWidgets(QList<MoveMeWidget *> &widgets, QList<StructRGB> &colors, int widgets_count)
{
    QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->screen(screen) ->winId(),
                                  screenres.x(), //!
                                  screenres.y(), //!
                                  screenres.width(),
                                  screenres.height());
    for(int i = 0; i < widgets_count; i++) {
        colors[i].rgb = getColor(pixmap, widgets[i]);
    }
}

QRgb QtGrabber::getColor(QPixmap pixmap, const QWidget * grabme)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return getColor(pixmap,
                    grabme->x(),
                    grabme->y(),
                    grabme->width(),
                    grabme->height());
}

QRgb QtGrabber::getColor(QPixmap pixmap, int x, int y, int width, int height)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO
                     << "x y w h:" << x << y << width << height;

    QPixmap pix = pixmap.copy(x,y, width, height);
    QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage im = scaledPix.toImage();
    QRgb result = im.pixel(0,0);

    DEBUG_HIGH_LEVEL << "QRgb result =" << hex << result;

    return result;
}
