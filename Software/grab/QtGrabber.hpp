#ifndef QTGRABBER_HPP
#define QTGRABBER_HPP

#include "IGrabber.hpp"
class QtGrabber : public IGrabber
{
    Q_OBJECT

public:
    QtGrabber(QObject * parent);
    ~QtGrabber();
    virtual const char * getName();
    virtual void updateGrabScreenFromWidget( QWidget * widget );
    virtual void grabWidgets(QList<MoveMeWidget *> &widgets, QList<StructRGB> &colors, int widgets_count);

private:
    QRgb getColor(QPixmap pixmap, const QWidget * grabme);
    QRgb getColor(QPixmap pixmap, int x, int y, int width, int height);

    QRect screenres;
    int screen;
};

#endif // QTGRABBER_HPP
