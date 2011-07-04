#ifndef QTGRABBER_HPP
#define QTGRABBER_HPP

#include "IGrabber.hpp"
class QtGrabber : public IGrabber
{
//    Q_OBJECT

public:
    QtGrabber();
    ~QtGrabber();
    virtual const char * getName();
    virtual void updateGrabScreenFromWidget( QWidget * widget );
    virtual QList<QRgb> grabWidgetsColors(QList<MoveMeWidget *> &widgets);

private:
    QRgb getColor(QPixmap pixmap, const QWidget * grabme);
    QRgb getColor(QPixmap pixmap, int x, int y, int width, int height);

    QRect screenres;
    int screen;
};

#endif // QTGRABBER_HPP
