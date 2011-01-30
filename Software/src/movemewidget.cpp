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


#include "QtGui"
#include "movemewidget.h"

// Colors changes when middle button clicked
const QColor MoveMeWidget::colors[ColorsCount] = {
    Qt::red,
    Qt::green,
    Qt::blue,
    Qt::yellow,
    Qt::white,
    Qt::black,
    Qt::magenta,
    Qt::cyan,
};


MoveMeWidget::MoveMeWidget(int id, QWidget *parent) :
    QLabel(parent)
{
    this->selfId = id;

    this->setCursor(Qt::OpenHandCursor);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    this->setFocusPolicy(Qt::NoFocus);
    this->setText(QString::number(this->selfId + 1));
    this->setAlignment(Qt::AlignCenter);

    this->setMouseTracking(true);

    this->resize(100, 100);

    colorIndex = 0;
    this->setBackgroundColor(colors[colorIndex]);
}

void MoveMeWidget::setBackgroundColor(QColor color)
{
    QPalette pal = this->palette();
    pal.setBrush(this->backgroundRole(), QBrush(color));
    this->setPalette(pal);
}

void MoveMeWidget::setSizeAndPosition(int w, int h, int x, int y)
{
    this->move(x, y);
    this->resize(w, h);
}


void MoveMeWidget::mousePressEvent(QMouseEvent *pe)
{
    mousePressPosition = pe->pos();
    mousePressGlobalPosition = pe->globalPos();
    mousePressDiffFromBorder.setWidth(this->width() - pe->x());
    mousePressDiffFromBorder.setHeight(this->height() - pe->y());

    if(pe->buttons() == Qt::MidButton){
        if(++colorIndex >= ColorsCount){
            colorIndex = 0;
        }
        this->setBackgroundColor(colors[colorIndex]);

    }else if(pe->buttons() == Qt::RightButton){
        // Send signal RightButtonClicked to main window for grouping widgets
        emit mouseRightButtonClicked(selfId);

    }else if(pe->buttons() == Qt::LeftButton){
        // First check corners
        if(pe->x() < BorderWidth && pe->y() < BorderWidth){
            cmd = RESIZE_LEFT_UP;
            this->setCursor(Qt::SizeFDiagCursor);
        }else if(pe->x() < BorderWidth && (this->height() - pe->y()) < BorderWidth){
            cmd = RESIZE_LEFT_DOWN;
            this->setCursor(Qt::SizeBDiagCursor);
        }else if(pe->y() < BorderWidth && (this->width() - pe->x()) < BorderWidth){
            cmd = RESIZE_RIGHT_UP;
            this->setCursor(Qt::SizeBDiagCursor);
        }else if((this->height() - pe->y()) < BorderWidth && (this->width() - pe->x()) < BorderWidth){
            cmd = RESIZE_RIGHT_DOWN;
            this->setCursor(Qt::SizeFDiagCursor);
        }
        // Next check sides
        else if(pe->x() < BorderWidth){
            cmd = RESIZE_HOR_LEFT;
            this->setCursor(Qt::SizeHorCursor);
        }else if((this->width() - pe->x()) < BorderWidth){
            cmd = RESIZE_HOR_RIGHT;
            this->setCursor(Qt::SizeHorCursor);
        }else if(pe->y() < BorderWidth){
            cmd = RESIZE_VER_UP;
            this->setCursor(Qt::SizeVerCursor);
        }else if((this->height() - pe->y()) < BorderWidth){
            cmd = RESIZE_VER_DOWN;
            this->setCursor(Qt::SizeVerCursor);
        }
        // Click on center, just move it
        else{
            cmd = MOVE;
            this->setCursor(Qt::ClosedHandCursor);
        }

        emit resizeStarted();

    }else{
        cmd = NOP;
    }
}

void MoveMeWidget::mouseMoveEvent(QMouseEvent *pe)
{
    int newWidth, newHeight, newX, newY;

    switch(cmd){
    case MOVE:
        this->move(pe->globalPos() - mousePressPosition);
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

        if(pe->x() < BorderWidth && pe->y() < BorderWidth){
            this->setCursor(Qt::SizeFDiagCursor);
        }else if(pe->x() < BorderWidth && (this->height() - pe->y()) < BorderWidth){
            this->setCursor(Qt::SizeBDiagCursor);
        }else if(pe->y() < BorderWidth && (this->width() - pe->x()) < BorderWidth){
            this->setCursor(Qt::SizeBDiagCursor);
        }else if((this->height() - pe->y()) < BorderWidth && (this->width() - pe->x()) < BorderWidth){
            this->setCursor(Qt::SizeFDiagCursor);
        }else if(pe->x() < BorderWidth){
            this->setCursor(Qt::SizeHorCursor);
        }else if((this->width() - pe->x()) < BorderWidth){
            this->setCursor(Qt::SizeHorCursor);
        }else if(pe->y() < BorderWidth){
            this->setCursor(Qt::SizeVerCursor);
        }else if((this->height() - pe->y()) < BorderWidth){
            this->setCursor(Qt::SizeVerCursor);
        }else{
            this->setCursor(Qt::OpenHandCursor);
        }
        break;
    }
}

void MoveMeWidget::mouseReleaseEvent(QMouseEvent *pe)
{
    this->setCursor(Qt::OpenHandCursor);
    cmd = NOP;
    emit resizeCompleted();
}
