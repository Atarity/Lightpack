#ifndef QCOLORBUTTON_H
#define QCOLORBUTTON_H

#include<QtGui>

class QColorButton : public QPushButton
{
    Q_OBJECT
public:
    QColorButton(QWidget *parent);
    ~QColorButton();
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

#endif // QCOLORBUTTON_H
