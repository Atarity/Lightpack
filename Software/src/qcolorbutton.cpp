#include"defs.h"
#include"qcolorbutton.hpp"
#include"debug.h"
#define COLOR_LABEL_SPACING 5

QColorButton::QColorButton(QWidget * parent) : QPushButton(parent)
{
    this->setText("");
    colorLabel = new QLabel(this);
    colorLabel->setText("");
    colorLabel->setStyleSheet("background: rgb(127,127,127); border: solid 1px #000000;");
    updateColorLabelSize();
    connect(this, SIGNAL(clicked()), this, SLOT(click()));
}

void QColorButton::resizeEvent(QResizeEvent * /*event*/)
{
    updateColorLabelSize();
}

QColorButton::~QColorButton()
{
    delete colorLabel;
}

void QColorButton::updateColorLabelSize()
{
#ifdef MAC_OS
    colorLabel->move(COLOR_LABEL_SPACING*2, COLOR_LABEL_SPACING+3);
    colorLabel->resize(this->width()-(COLOR_LABEL_SPACING * 4),this->height()-(COLOR_LABEL_SPACING * 2));
#else
    colorLabel->move(COLOR_LABEL_SPACING, COLOR_LABEL_SPACING);
    colorLabel->resize(this->width()-(COLOR_LABEL_SPACING * 2),this->height()-(COLOR_LABEL_SPACING * 2));
#endif
}

void QColorButton::setColor(QColor color)
{
    colorLabel->setStyleSheet("background: rgb("
                                           +QString::number(color.red())+ ", "
                                           +QString::number(color.green())+ ", "
                                           +QString::number(color.blue())+ ");"
                              +"border: solid 1px #000000;");
    emit colorChanged(color);
}

QColor QColorButton::getColor()
{
    return colorLabel->palette().color(QPalette::Background);
}

void QColorButton::currentColorChanged(QColor color)
{
    this->setColor(color);
}

void QColorButton::click()
{
    QColorDialog * dialog = new QColorDialog(this);
    QColor savedColor = getColor();
    connect(dialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(currentColorChanged(QColor)));
    dialog->setCurrentColor(getColor());
    if (dialog->exec() != QDialog::Accepted)
        setColor(savedColor);
    delete dialog;
}

