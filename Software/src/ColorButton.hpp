
//#pragma once
#ifndef CB_H
#define CB_H

#include <QtGui>

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    ColorButton(QWidget *parent);
    ~ColorButton();
    QColor getColor();
    void setColor(QColor color);
protected:
    void updateColorLabelSize();
    virtual void resizeEvent(QResizeEvent *);

protected slots:
    void click();
    void currentColorChanged(QColor color);
signals:
    void colorChanged(QColor);



//protected slots:
//    void onQColorButtonClicked();

protected:
    QLabel * colorLabel;
};

#endif //CB_H
