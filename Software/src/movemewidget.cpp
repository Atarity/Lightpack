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

// Colors changes when middle button clicked
const QColor MoveMeWidget::colors[ColorsCount][2] = {
    { Qt::red,      Qt::black },
    { Qt::green,    Qt::black },
    { Qt::blue,     Qt::white },
    { Qt::yellow,   Qt::black },
    { Qt::white,    Qt::black },
    { Qt::black,    Qt::white },
    { Qt::magenta,  Qt::black },
    { Qt::cyan,     Qt::black },
};


MoveMeWidget::MoveMeWidget(int id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoveMeWidget)
{
    ui->setupUi(this);

    this->selfId = id;

    this->setCursorOnAll(Qt::OpenHandCursor);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    this->setFocusPolicy(Qt::NoFocus);
    ui->labelSelfId->setText(QString::number(this->selfId + 1));

    this->setMouseTracking(true);

    this->resize(100, 100);

    colorIndex = 0;
    this->setBackgroundColor(colors[colorIndex][0]);
    this->setTextColor(colors[colorIndex][1]);

    qDebug() << "MoveMeWidget(" << (selfId + 1) << "): Load coef-s from settings";
    // Load coef-s from settings
    coefRed = loadCoefWithCheck("CoefRed");
    coefGreen = loadCoefWithCheck("CoefGreen");
    coefBlue = loadCoefWithCheck("CoefBlue");
}

MoveMeWidget::~MoveMeWidget()
{
    delete ui;
}

void MoveMeWidget::setBackgroundColor(QColor color)
{
    QPalette pal = this->palette();
    pal.setBrush(this->backgroundRole(), QBrush(color));
    this->setPalette(pal);
}

void MoveMeWidget::setTextColor(QColor color)
{
    QPalette pal = this->palette();
    pal.setBrush(this->foregroundRole(), QBrush(color));
    ui->labelSelfId->setPalette(pal);
    ui->labelWidthHeight->setPalette(pal);
}

void MoveMeWidget::setSizeAndPosition(int w, int h, int x, int y)
{
    this->move(x, y);
    this->resize(w, h);
}

// private
void MoveMeWidget::setCursorOnAll(Qt::CursorShape cursor)
{
    this->setCursor(cursor);
    ui->labelSelfId->setCursor(cursor);
    ui->labelWidthHeight->setCursor(cursor);
}

// private
double MoveMeWidget::loadCoefWithCheck(QString coefStr)
{
    bool ok = false;
    double coef = settings->value("LED_" + QString::number(selfId + 1) + "/" + coefStr).toDouble(&ok);
    if(ok == false){
        qWarning() << "LedWidget:" << "Settings bad value" << "[LED_" + QString::number(selfId + 1) + "]" << coefStr << "Convert to double error. Set it to default value" << coefStr << "= 1";
        coef = 1;
        settings->setValue("LED_" + QString::number(selfId + 1) + "/" + coefStr, coef);
    }else if(coef < 0.1 || coef > 3){
        qWarning() << "LedWidget:" << "Settings bad value" << "[LED_" + QString::number(selfId + 1) + "]" << coefStr << "=" << coef << "Set it to default value" << coefStr << "= 1";
        coef = 1;
        settings->setValue("LED_" + QString::number(selfId + 1) + "/" + coefStr, coef);
    }
    return coef;
}


void MoveMeWidget::mousePressEvent(QMouseEvent *pe)
{
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

        emit resizeStarted();

    }else{
        cmd = NOP;
    }
}

void MoveMeWidget::mouseMoveEvent(QMouseEvent *pe)
{
    int newWidth, newHeight, newX, newY;
    QPoint moveHere;

    switch(cmd){
    case MOVE:
        moveHere = pe->globalPos() - mousePressPosition;
        if(moveHere.x() < StickyCloserPixels) moveHere.setX(0);
        if(moveHere.y() < StickyCloserPixels) moveHere.setY(0);

        if(moveHere.x() + this->width() > Desktop::WidthAvailable - StickyCloserPixels)
            moveHere.setX(Desktop::WidthAvailable - this->width());
        if(moveHere.y() + this->height() > Desktop::HeightFull - StickyCloserPixels)
            moveHere.setY(Desktop::HeightFull - this->height());

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
}

void MoveMeWidget::mouseReleaseEvent(QMouseEvent *pe)
{
    checkAndSetCursors(pe);
    cmd = NOP;
    emit resizeCompleted(selfId);
}


void MoveMeWidget::wheelEvent(QWheelEvent *pe)
{
    if(pe->delta() > 0) colorIndex++;
    if(pe->delta() < 0) colorIndex--;

    if(colorIndex >= ColorsCount){
        colorIndex = 0;
    }else if(colorIndex < 0) {
        colorIndex = ColorsCount - 1;
    }
    this->setBackgroundColor(colors[colorIndex][0]);
    this->setTextColor(colors[colorIndex][1]);
}

void MoveMeWidget::resizeEvent(QResizeEvent *)
{
    ui->labelWidthHeight->setText( QString::number(this->width()) + "x" + QString::number(this->height()) );
}

void MoveMeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QColor(0x77, 0x77, 0x77));
    painter.drawRect(0, 0, width() - 1, height() - 1);
}


double MoveMeWidget::getCoefRed()
{
    return coefRed;
}

double MoveMeWidget::getCoefGreen()
{
    return coefGreen;
}

double MoveMeWidget::getCoefBlue()
{
    return coefBlue;
}


void MoveMeWidget::checkAndSetCursors(QMouseEvent *pe)
{
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
