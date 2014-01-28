/*
 * GrabWidget.cpp
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
#include <QtWidgets/QDesktopWidget>
#include <QTextItem>
#include "GrabAreaWidget.hpp"
#include "ui_GrabAreaWidget.h"
#include "Settings.hpp"
#include "debug.h"

using namespace SettingsScope;

GrabAreaWidget::GrabAreaWidget(int id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GrabAreaWidget)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << id;

    ui->setupUi(this);

    m_selfId = id;
    m_selfIdString = QString::number(m_selfId + 1);
    cmd = NOP;

    setCursorOnAll(Qt::OpenHandCursor);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setFocusPolicy(Qt::NoFocus);


    setMouseTracking(true);
}

GrabAreaWidget::~GrabAreaWidget()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << m_selfId;

    delete ui;
}

void GrabAreaWidget::closeEvent(QCloseEvent *event)
{
    qWarning() << Q_FUNC_INFO << "event->type():" << event->type() << "Id:" << m_selfId;

    event->ignore();
}

void GrabAreaWidget::setCursorOnAll(Qt::CursorShape cursor)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << cursor;

    setCursor(cursor);
}

void GrabAreaWidget::mousePressEvent(QMouseEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << pe->pos();

    mousePressPosition = pe->pos();
    mousePressGlobalPosition = pe->globalPos();
    mousePressDiffFromBorder.setWidth(width() - pe->x());
    mousePressDiffFromBorder.setHeight(height() - pe->y());

    if (pe->buttons() == Qt::RightButton)
    {
        // Send signal RightButtonClicked to main window for grouping widgets
        emit mouseRightButtonClicked(m_selfId);
    }
    else if (pe->buttons() == Qt::LeftButton)
    {
        // First check corners
        if (pe->x() < BorderWidth && pe->y() < BorderWidth)
        {
            cmd = RESIZE_LEFT_UP;
            setCursorOnAll(Qt::SizeFDiagCursor);
        }
        else if (pe->x() < BorderWidth && (height() - pe->y()) < BorderWidth)
        {
            cmd = RESIZE_LEFT_DOWN;
            setCursorOnAll(Qt::SizeBDiagCursor);
        }
        else if (pe->y() < BorderWidth && (width() - pe->x()) < BorderWidth)
        {
            cmd = RESIZE_RIGHT_UP;
            setCursorOnAll(Qt::SizeBDiagCursor);
        }
        else if ((height() - pe->y()) < BorderWidth && (width() - pe->x()) < BorderWidth)
        {
            cmd = RESIZE_RIGHT_DOWN;
            setCursorOnAll(Qt::SizeFDiagCursor);
        }
        // Next check sides
        else if (pe->x() < BorderWidth)
        {
            cmd = RESIZE_HOR_LEFT;
            setCursorOnAll(Qt::SizeHorCursor);
        }
        else if ((width() - pe->x()) < BorderWidth)
        {
            cmd = RESIZE_HOR_RIGHT;
            setCursorOnAll(Qt::SizeHorCursor);
        }
        else if (pe->y() < BorderWidth)
        {
            cmd = RESIZE_VER_UP;
            setCursorOnAll(Qt::SizeVerCursor);
        }
        else if ((height() - pe->y()) < BorderWidth)
        {
            cmd = RESIZE_VER_DOWN;
            setCursorOnAll(Qt::SizeVerCursor);
        }
        // Click on center, just move it
        else
        {
            cmd = MOVE;
            setCursorOnAll(Qt::ClosedHandCursor);
        }
        emit resizeOrMoveStarted(m_selfId);
    }
    else
    {
        cmd = NOP;
    }
    repaint();
}

void GrabAreaWidget::mouseMoveEvent(QMouseEvent *pe)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << "pe->pos() =" << pe->pos();

    int newWidth, newHeight, newX, newY;
    QPoint moveHere;

    QRect screen = QApplication::desktop()->screenGeometry(this);

    int left, top, right, bottom;

    switch (cmd)
    {
    case MOVE:
        moveHere = pe->globalPos() - mousePressPosition;

        left = moveHere.x();
        top = moveHere.y();

        right = moveHere.x() + width();
        bottom = moveHere.y() + height();

        if (left < screen.left() + StickyCloserPixels &&
           left > screen.left() - StickyCloserPixels)
            moveHere.setX(screen.left());

        if (top < screen.top() + StickyCloserPixels &&
           top > screen.top() - StickyCloserPixels)
            moveHere.setY(screen.top());

        if (right < screen.right() + StickyCloserPixels &&
           right > screen.right() - StickyCloserPixels)
            moveHere.setX(screen.right() - width() + 1);

        if (bottom < screen.bottom() + StickyCloserPixels &&
           bottom > screen.bottom() - StickyCloserPixels)
            moveHere.setY(screen.bottom() - height() + 1);

        move(moveHere);
        break;

    case RESIZE_HOR_RIGHT:
        newWidth = pe->x() + mousePressDiffFromBorder.width();
        resize((newWidth <= MinimumWidth) ? MinimumWidth : newWidth, height());
        break;

    case RESIZE_VER_DOWN:
        newHeight = pe->y() + mousePressDiffFromBorder.height();
        resize(width(), (newHeight <= MinimumHeight) ? MinimumHeight : newHeight);
        break;

    case RESIZE_HOR_LEFT:
        newY = pos().y();
        newHeight = height();

        newWidth = mousePressGlobalPosition.x() - pe->globalPos().x() + mousePressPosition.x() + mousePressDiffFromBorder.width();

        if (newWidth < MinimumWidth)
        {
            newWidth = MinimumWidth;
            newX = mousePressGlobalPosition.x() + mousePressDiffFromBorder.width() - MinimumWidth;
        } else {
            newX = pe->globalPos().x() - mousePressPosition.x();
        }
        resize(newWidth, newHeight);
        move(newX, newY);
        break;

    case RESIZE_VER_UP:
        newX = pos().x();
        newWidth = width();

        newHeight = mousePressGlobalPosition.y() - pe->globalPos().y() + mousePressPosition.y() + mousePressDiffFromBorder.height();

        if (newHeight < MinimumHeight)
        {
            newHeight = MinimumHeight;
            newY = mousePressGlobalPosition.y() + mousePressDiffFromBorder.height() - MinimumHeight;
        } else {
            newY = pe->globalPos().y() - mousePressPosition.y();
        }
        resize(newWidth, newHeight);
        move(newX, newY);
        break;


    case RESIZE_RIGHT_DOWN:
        newWidth = pe->x() + mousePressDiffFromBorder.width();
        newHeight = pe->y() + mousePressDiffFromBorder.height();
        resize((newWidth <= MinimumWidth) ? MinimumWidth : newWidth, (newHeight <= MinimumHeight) ? MinimumHeight : newHeight);
        break;

    case RESIZE_RIGHT_UP:
        newWidth = pe->x() + mousePressDiffFromBorder.width();
        if (newWidth < MinimumWidth) newWidth = MinimumWidth;
        newX = pos().x();

        newHeight = mousePressGlobalPosition.y() - pe->globalPos().y() + mousePressPosition.y() + mousePressDiffFromBorder.height();

        if (newHeight < MinimumHeight)
        {
            newHeight = MinimumHeight;
            newY = mousePressGlobalPosition.y() + mousePressDiffFromBorder.height() - MinimumHeight;
        } else {
            newY = pe->globalPos().y() - mousePressPosition.y();
        }
        resize(newWidth, newHeight);
        move(newX, newY);
        break;

    case RESIZE_LEFT_DOWN:
        newHeight = pe->y() + mousePressDiffFromBorder.height();
        if (newHeight < MinimumHeight) newHeight = MinimumHeight;
        newY = pos().y();

        newWidth = mousePressGlobalPosition.x() - pe->globalPos().x() + mousePressPosition.x() + mousePressDiffFromBorder.width();

        if (newWidth < MinimumWidth)
        {
            newWidth = MinimumWidth;
            newX = mousePressGlobalPosition.x() + mousePressDiffFromBorder.width() - MinimumWidth;
        } else {
            newX = pe->globalPos().x() - mousePressPosition.x();
        }
        resize(newWidth, newHeight);
        move(newX, newY);
        break;

    case RESIZE_LEFT_UP:
        newWidth = mousePressGlobalPosition.x() - pe->globalPos().x() + mousePressPosition.x() + mousePressDiffFromBorder.width();

        if (newWidth < MinimumWidth)
        {
            newWidth = MinimumWidth;
            newX = mousePressGlobalPosition.x() + mousePressDiffFromBorder.width() - MinimumWidth;
        } else {
            newX = pe->globalPos().x() - mousePressPosition.x();
        }

        newHeight = mousePressGlobalPosition.y() - pe->globalPos().y() + mousePressPosition.y() + mousePressDiffFromBorder.height();

        if (newHeight < MinimumHeight)
        {
            newHeight = MinimumHeight;
            newY = mousePressGlobalPosition.y() + mousePressDiffFromBorder.height() - MinimumHeight;
        } else {
            newY = pe->globalPos().y() - mousePressPosition.y();
        }
        resize(newWidth, newHeight);
        move(newX, newY);
        break;

    case NOP:
    default:        
        checkAndSetCursors(pe);
        break;
    }
    resizeEvent(NULL);
}

void GrabAreaWidget::mouseReleaseEvent(QMouseEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    checkAndSetCursors(pe);

    cmd = NOP;

    emit resizeOrMoveCompleted(m_selfId);
    repaint();
}


void GrabAreaWidget::wheelEvent(QWheelEvent *pe)
{
    Q_UNUSED(pe);
    DEBUG_MID_LEVEL << Q_FUNC_INFO;
}

void GrabAreaWidget::resizeEvent(QResizeEvent *)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    m_widthHeight = QString::number(width()) + "x" + QString::number(height());
}

void GrabAreaWidget::paintEvent(QPaintEvent *)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    QPainter painter(this);
    painter.setBrush(QBrush(cmd == NOP ? QColor(Qt::darkGray) : QColor(Qt::white)));
    painter.setPen(QColor(0x77,0x77,0x77));
    painter.drawRect(0, 0, width() - 1, height() - 1);

//    // Icon 'resize' opacity
    painter.setOpacity(0.4);

    // Draw icon 12x12px with 3px padding from the bottom right corner
    if (m_textColor == Qt::white)
        painter.drawPixmap(width() - 18, height() - 18, 12, 12, QPixmap(":/icons/res_light.png"));
    else{
        //painter.setOpacity(0.5);
        painter.drawPixmap(width() - 18, height() - 18, 12, 12, QPixmap(":/icons/res_dark.png"));
    }

    // Self ID and size text opacity
    painter.setOpacity(0.25);

    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize((height() / 3 < width() / 3) ? height() / 3 : width() / 3);
    painter.setFont(font);

    painter.setPen(m_textColor);
    painter.setBrush(QBrush(m_textColor));
    painter.drawText(rect(), m_selfIdString, QTextOption(Qt::AlignCenter));

    font.setBold(false);
    font.setPointSize(10);
    painter.setFont(font);

    QRect rectWidthHeight = rect();
    rectWidthHeight.setBottom(rect().bottom() - 3);
    painter.drawText(rectWidthHeight, m_widthHeight, QTextOption(Qt::AlignHCenter | Qt::AlignBottom));
}

void GrabAreaWidget::checkAndSetCursors(QMouseEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (pe->x() < BorderWidth && pe->y() < BorderWidth)
    {
        setCursorOnAll(Qt::SizeFDiagCursor);
    }
    else if (pe->x() < BorderWidth && (height() - pe->y()) < BorderWidth)
    {
        setCursorOnAll(Qt::SizeBDiagCursor);
    }
    else if (pe->y() < BorderWidth && (width() - pe->x()) < BorderWidth)
    {
        setCursorOnAll(Qt::SizeBDiagCursor);
    }
    else if ((height() - pe->y()) < BorderWidth && (width() - pe->x()) < BorderWidth)
    {
        setCursorOnAll(Qt::SizeFDiagCursor);
    }
    else if (pe->x() < BorderWidth)
    {
        setCursorOnAll(Qt::SizeHorCursor);
    }
    else if ((width() - pe->x()) < BorderWidth)
    {
        setCursorOnAll(Qt::SizeHorCursor);
    }
    else if (pe->y() < BorderWidth)
    {
        setCursorOnAll(Qt::SizeVerCursor);
    }
    else if ((height() - pe->y()) < BorderWidth)
    {
        setCursorOnAll(Qt::SizeVerCursor);
    }
    else
    {
        if (pe->buttons() & Qt::LeftButton)
            setCursorOnAll(Qt::ClosedHandCursor);
        else
            setCursorOnAll(Qt::OpenHandCursor);
    }
}

void GrabAreaWidget::setBackgroundColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << color.rgb();

    QPalette pal = palette();

    pal.setBrush(backgroundRole(), QBrush(Qt::darkGray));
    setPalette(pal);
}

void GrabAreaWidget::setTextColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << color.rgb();

        m_textColor = Qt::gray;
}
