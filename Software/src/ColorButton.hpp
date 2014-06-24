
//#pragma once
#ifndef CB_H
#define CB_H

#include <QtWidgets/QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    ColorButton(QWidget *parent);
    ~ColorButton();
    QColor getColor();
    void setColor(QColor color);

signals:
    void colorChanged(QColor);

protected slots:
    void click();
    void currentColorChanged(QColor color);

protected:
    QColor m_color;
};

#endif //CB_H
