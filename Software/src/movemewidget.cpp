/*
 * movemewidget.cpp
 *
 *  Created on: 29.01.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: Lightpack
 *
 *  Lightpack is very simple implementation of the backlight for a laptop
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <QtGui>
#include "movemewidget.h"
#include "ui_movemewidget.h"
#include "desktop.h"
#include "settings.h"

#include "debug.h"

// Colors changes when middle button clicked
const QColor MoveMeWidget::colors[MoveMeWidget::ColorsCount][2] = {
    { Qt::red,        Qt::black }, /* LED1 */
    { Qt::green,      Qt::black }, /* LED2 */
    { Qt::blue,       Qt::white }, /* LED3 */
    { Qt::yellow,     Qt::black }, /* LED4 */
    { Qt::darkRed,    Qt::white }, /* LED5 */
    { Qt::darkGreen,  Qt::white }, /* LED6 */
    { Qt::darkBlue,   Qt::white }, /* LED7 */
    { Qt::darkYellow, Qt::white }, /* LED8 */
    { Qt::black,      Qt::white },
    { Qt::magenta,    Qt::black },
    { Qt::cyan,       Qt::black },
    { Qt::white,      Qt::black }, // ColorIndexWhite == 11
};


MoveMeWidget::MoveMeWidget(int id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoveMeWidget)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << id;

    ui->setupUi(this);

    this->selfId = id;

    this->setCursorOnAll(Qt::OpenHandCursor);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    this->setFocusPolicy(Qt::NoFocus);
    ui->checkBox_SelfId->setText(QString::number(this->selfId + 1));

    this->setMouseTracking(true);

    this->resize(100, 100);

    setColors( selfId );

    connect(ui->checkBox_SelfId, SIGNAL(toggled(bool)), this, SLOT(checkBoxSelfId_Toggled(bool)));
}

MoveMeWidget::~MoveMeWidget()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete ui;
}

void MoveMeWidget::saveSizeAndPosition()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Settings::setValue("LED_" + QString::number(selfId+1) + "/Position", pos() );
    Settings::setValue("LED_" + QString::number(selfId+1) + "/Size", size() );
}

void MoveMeWidget::settingsProfileChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << selfId;

    coefRed = loadCoefWithCheck("CoefRed");
    coefGreen = loadCoefWithCheck("CoefGreen");
    coefBlue = loadCoefWithCheck("CoefBlue");

    this->move( Settings::value("LED_" + QString::number(selfId+1) + "/Position").toPoint() );
    this->resize( Settings::value("LED_" + QString::number(selfId+1) + "/Size").toSize() );

    ui->checkBox_SelfId->setChecked( Settings::value("LED_" + QString::number(selfId+1) + "/IsEnabled").toBool() );
}


void MoveMeWidget::setColors(int index)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << index;

    if(index < ColorsCount){
        colorIndex = index;
    }

    if(ui->checkBox_SelfId->isChecked()){
        this->setBackgroundColor(colors[colorIndex][0]);
        this->setTextColor(colors[colorIndex][1]);
    }else{
        this->setBackgroundColor(Qt::gray);
        this->setTextColor(Qt::darkGray);
    }
}

void MoveMeWidget::setBackgroundColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << color.rgb();

    QPalette pal = this->palette();
    pal.setBrush(this->backgroundRole(), QBrush(color));
    this->setPalette(pal);
}

void MoveMeWidget::setTextColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << color.rgb();

    QPalette pal = this->palette();
    pal.setBrush(QPalette::WindowText, QBrush(color));
    ui->checkBox_SelfId->setPalette(pal);
    ui->labelWidthHeight->setPalette(pal);
}

// private
void MoveMeWidget::setCursorOnAll(Qt::CursorShape cursor)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << cursor;

    this->setCursor(cursor);
    ui->checkBox_SelfId->setCursor(cursor);
    ui->labelWidthHeight->setCursor(cursor);
}

// private
double MoveMeWidget::loadCoefWithCheck(QString coefStr)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << coefStr;

    bool ok = false;
    double coef = Settings::value("LED_" + QString::number(selfId + 1) + "/" + coefStr).toDouble(&ok);
    if(ok == false){
        qWarning() << "LedWidget:" << "Settings bad value" << "[LED_" + QString::number(selfId + 1) + "]" << coefStr << "Convert to double error. Set it to default value" << coefStr << "= 1";
        coef = 1;
        Settings::setValue("LED_" + QString::number(selfId + 1) + "/" + coefStr, coef);
    }else if(coef < 0.1 || coef > 3){
        qWarning() << "LedWidget:" << "Settings bad value" << "[LED_" + QString::number(selfId + 1) + "]" << coefStr << "=" << coef << "Set it to default value" << coefStr << "= 1";
        coef = 1;
        Settings::setValue("LED_" + QString::number(selfId + 1) + "/" + coefStr, coef);
    }
    return coef;
}


void MoveMeWidget::mousePressEvent(QMouseEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << pe->pos();

    mousePressPosition = pe->pos();
    mousePressGlobalPosition = pe->globalPos();
    mousePressDiffFromBorder.setWidth(this->width() - pe->x());
    mousePressDiffFromBorder.setHeight(this->height() - pe->y());

    if(pe->buttons() == Qt::RightButton){
        // Send signal RightButtonClicked to main window for grouping widgets
        emit mouseRightButtonClicked(selfId);

    }else if(pe->buttons() == Qt::LeftButton){
        // First check corners
        if(pe->x() < BorderWidth && pe->y() < BorderWidth){
            cmd = RESIZE_LEFT_UP;
            this->setCursorOnAll(Qt::SizeFDiagCursor);
        }else if(pe->x() < BorderWidth && (this->height() - pe->y()) < BorderWidth){
            cmd = RESIZE_LEFT_DOWN;
            this->setCursorOnAll(Qt::SizeBDiagCursor);
        }else if(pe->y() < BorderWidth && (this->width() - pe->x()) < BorderWidth){
            cmd = RESIZE_RIGHT_UP;
            this->setCursorOnAll(Qt::SizeBDiagCursor);
        }else if((this->height() - pe->y()) < BorderWidth && (this->width() - pe->x()) < BorderWidth){
            cmd = RESIZE_RIGHT_DOWN;
            this->setCursorOnAll(Qt::SizeFDiagCursor);
        }
        // Next check sides
        else if(pe->x() < BorderWidth){
            cmd = RESIZE_HOR_LEFT;
            this->setCursorOnAll(Qt::SizeHorCursor);
        }else if((this->width() - pe->x()) < BorderWidth){
            cmd = RESIZE_HOR_RIGHT;
            this->setCursorOnAll(Qt::SizeHorCursor);
        }else if(pe->y() < BorderWidth){
            cmd = RESIZE_VER_UP;
            this->setCursorOnAll(Qt::SizeVerCursor);
        }else if((this->height() - pe->y()) < BorderWidth){
            cmd = RESIZE_VER_DOWN;
            this->setCursorOnAll(Qt::SizeVerCursor);
        }
        // Click on center, just move it
        else{
            cmd = MOVE;
            this->setCursorOnAll(Qt::ClosedHandCursor);
        }

        emit resizeOrMoveStarted();

    }else{
        cmd = NOP;
    }
}

void MoveMeWidget::mouseMoveEvent(QMouseEvent *pe)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << "pe->pos() =" << pe->pos();

    int newWidth, newHeight, newX, newY;
    QPoint moveHere;

    switch(cmd){
    case MOVE:
        moveHere = pe->globalPos() - mousePressPosition;
        this->move(moveHere);
        break;

    case RESIZE_HOR_RIGHT:
        newWidth = pe->x() + mousePressDiffFromBorder.width();
        this->resize((newWidth <= MinimumWidth) ? MinimumWidth : newWidth, this->height());
        break;

    case RESIZE_VER_DOWN:
        newHeight = pe->y() + mousePressDiffFromBorder.height();
        this->resize(this->width(), (newHeight <= MinimumHeight) ? MinimumHeight : newHeight);
        break;

    case RESIZE_HOR_LEFT:
        newY = this->pos().y();
        newHeight = this->height();

        newWidth = mousePressGlobalPosition.x() - pe->globalPos().x() + mousePressPosition.x() + mousePressDiffFromBorder.width();

        if(newWidth < MinimumWidth){
            newWidth = MinimumWidth;
            newX = mousePressGlobalPosition.x() + mousePressDiffFromBorder.width() - MinimumWidth;
        }else{
            newX = pe->globalPos().x() - mousePressPosition.x();
        }
        this->resize(newWidth, newHeight);
        this->move(newX, newY);
        break;

    case RESIZE_VER_UP:
        newX = this->pos().x();
        newWidth = this->width();

        newHeight = mousePressGlobalPosition.y() - pe->globalPos().y() + mousePressPosition.y() + mousePressDiffFromBorder.height();

        if(newHeight < MinimumHeight){
            newHeight = MinimumHeight;
            newY = mousePressGlobalPosition.y() + mousePressDiffFromBorder.height() - MinimumHeight;
        }else{
            newY = pe->globalPos().y() - mousePressPosition.y();
        }
        this->resize(newWidth, newHeight);
        this->move(newX, newY);
        break;


    case RESIZE_RIGHT_DOWN:
        newWidth = pe->x() + mousePressDiffFromBorder.width();
        newHeight = pe->y() + mousePressDiffFromBorder.height();
        this->resize((newWidth <= MinimumWidth) ? MinimumWidth : newWidth, (newHeight <= MinimumHeight) ? MinimumHeight : newHeight);
        break;

    case RESIZE_RIGHT_UP:
        newWidth = pe->x() + mousePressDiffFromBorder.width();
        if(newWidth < MinimumWidth) newWidth = MinimumWidth;
        newX = this->pos().x();

        newHeight = mousePressGlobalPosition.y() - pe->globalPos().y() + mousePressPosition.y() + mousePressDiffFromBorder.height();

        if(newHeight < MinimumHeight){
            newHeight = MinimumHeight;
            newY = mousePressGlobalPosition.y() + mousePressDiffFromBorder.height() - MinimumHeight;
        }else{
            newY = pe->globalPos().y() - mousePressPosition.y();
        }
        this->resize(newWidth, newHeight);
        this->move(newX, newY);
        break;

    case RESIZE_LEFT_DOWN:
        newHeight = pe->y() + mousePressDiffFromBorder.height();
        if(newHeight < MinimumHeight) newHeight = MinimumHeight;
        newY = this->pos().y();

        newWidth = mousePressGlobalPosition.x() - pe->globalPos().x() + mousePressPosition.x() + mousePressDiffFromBorder.width();

        if(newWidth < MinimumWidth){
            newWidth = MinimumWidth;
            newX = mousePressGlobalPosition.x() + mousePressDiffFromBorder.width() - MinimumWidth;
        }else{
            newX = pe->globalPos().x() - mousePressPosition.x();
        }
        this->resize(newWidth, newHeight);
        this->move(newX, newY);
        break;

    case RESIZE_LEFT_UP:
        newWidth = mousePressGlobalPosition.x() - pe->globalPos().x() + mousePressPosition.x() + mousePressDiffFromBorder.width();

        if(newWidth < MinimumWidth){
            newWidth = MinimumWidth;
            newX = mousePressGlobalPosition.x() + mousePressDiffFromBorder.width() - MinimumWidth;
        }else{
            newX = pe->globalPos().x() - mousePressPosition.x();
        }

        newHeight = mousePressGlobalPosition.y() - pe->globalPos().y() + mousePressPosition.y() + mousePressDiffFromBorder.height();

        if(newHeight < MinimumHeight){
            newHeight = MinimumHeight;
            newY = mousePressGlobalPosition.y() + mousePressDiffFromBorder.height() - MinimumHeight;
        }else{
            newY = pe->globalPos().y() - mousePressPosition.y();
        }
        this->resize(newWidth, newHeight);
        this->move(newX, newY);
        break;

    case NOP:
    default:
        checkAndSetCursors(pe);
        break;
    }

    resizeEvent(NULL);
}

void MoveMeWidget::mouseReleaseEvent(QMouseEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    checkAndSetCursors(pe);
    cmd = NOP;

    saveSizeAndPosition();

    emit resizeOrMoveCompleted(selfId);
}


void MoveMeWidget::wheelEvent(QWheelEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if(ui->checkBox_SelfId->isChecked() == false){
        return;
    }

    if(pe->delta() > 0) colorIndex++;
    if(pe->delta() < 0) colorIndex--;

    if(colorIndex >= ColorsCount){
        colorIndex = 0;
    }else if(colorIndex < 0) {
        colorIndex = ColorsCount - 1;
    }
    setColors(colorIndex);
}

void MoveMeWidget::resizeEvent(QResizeEvent *)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    ui->labelWidthHeight->setText( QString::number(this->width()) + "x" + QString::number(this->height()) );
}

void MoveMeWidget::paintEvent(QPaintEvent *)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    QPainter painter(this);
    painter.setPen(QColor(0x77, 0x77, 0x77));
    painter.drawRect(0, 0, width() - 1, height() - 1);
}


double MoveMeWidget::getCoefRed()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return coefRed;
}

double MoveMeWidget::getCoefGreen()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return coefGreen;
}

double MoveMeWidget::getCoefBlue()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return coefBlue;
}

bool MoveMeWidget::isGrabEnabled()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return ui->checkBox_SelfId->isChecked();
}


void MoveMeWidget::checkAndSetCursors(QMouseEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if(pe->x() < BorderWidth && pe->y() < BorderWidth){
        this->setCursorOnAll(Qt::SizeFDiagCursor);
    }else if(pe->x() < BorderWidth && (this->height() - pe->y()) < BorderWidth){
        this->setCursorOnAll(Qt::SizeBDiagCursor);
    }else if(pe->y() < BorderWidth && (this->width() - pe->x()) < BorderWidth){
        this->setCursorOnAll(Qt::SizeBDiagCursor);
    }else if((this->height() - pe->y()) < BorderWidth && (this->width() - pe->x()) < BorderWidth){
        this->setCursorOnAll(Qt::SizeFDiagCursor);
    }else if(pe->x() < BorderWidth){
        this->setCursorOnAll(Qt::SizeHorCursor);
    }else if((this->width() - pe->x()) < BorderWidth){
        this->setCursorOnAll(Qt::SizeHorCursor);
    }else if(pe->y() < BorderWidth){
        this->setCursorOnAll(Qt::SizeVerCursor);
    }else if((this->height() - pe->y()) < BorderWidth){
        this->setCursorOnAll(Qt::SizeVerCursor);
    }else{
        this->setCursorOnAll(Qt::OpenHandCursor);
    }
}

void MoveMeWidget::checkBoxSelfId_Toggled(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    setColors(colorIndex); // just update color

    Settings::setValue("LED_" + QString::number(selfId+1) + "/IsEnabled", state);
}
