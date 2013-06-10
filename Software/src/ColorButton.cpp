#include "../common/defs.h"
#include "ColorButton.hpp"
#include "debug.h"
#include <QPainter>
#include <QColorDialog>

#define COLOR_LABEL_SPACING 5

ColorButton::ColorButton(QWidget * parent) : QPushButton(parent)
{
    this->setText("");
    connect(this, SIGNAL(clicked()), this, SLOT(click()));
}

ColorButton::~ColorButton()
{
}

void ColorButton::setColor(QColor color)
{
    QSize size = this->iconSize();
//    size.setHeight(size.height() * 2.0 / 3.0);

    QImage image(size, QImage::Format_ARGB32);
    QPainter paint(&image);
    paint.fillRect(QRect(0, 0, size.width(), size.height()), color);
//    paint.drawRect(QRect(0, 0, size.width()-1, size.height()-1));

    QIcon icon(QPixmap::fromImage(image));
    this->setIcon(icon);

    m_color = color;

    emit colorChanged(color);
}

QColor ColorButton::getColor()
{
    return m_color;
}

void ColorButton::currentColorChanged(QColor color)
{
    this->setColor(color);
}

void ColorButton::click()
{
    QColorDialog * dialog = new QColorDialog(this);
    dialog->setWindowFlags(Qt::Window
                          | Qt::WindowStaysOnTopHint
                          | Qt::CustomizeWindowHint
                          | Qt::WindowCloseButtonHint);

    QColor savedColor = getColor();
    connect(dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(currentColorChanged(QColor)));
    dialog->setCurrentColor(getColor());
    if (dialog->exec() != QDialog::Accepted)
        setColor(savedColor);
    delete dialog;
}

