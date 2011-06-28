#ifndef QCOLORBUTTON_H
#define QCOLORBUTTON_H

#include<QtGui>

class QColorButton : public QPushButton
{
    Q_OBJECT
public:
    QColorButton(QWidget *parent);
    ~QColorButton();
protected:
    void updateColorLabelSize();
    virtual void resizeEvent(QResizeEvent *);

protected slots:
    void click();
signals:
    void colorChanged(QColor);



//protected slots:
//    void onQColorButtonClicked();

protected:
    QLabel * colorLabel;
};

#endif // QCOLORBUTTON_H
