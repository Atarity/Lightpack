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
#include "GrabWidget.hpp"
#include "ui_GrabWidget.h"
#include "Settings.hpp"
#include "debug.h"

using namespace SettingsScope;

// Colors changes when middle button clicked
const QColor GrabWidget::colors[GrabWidget::ColorsCount][2] = {
    { Qt::red,         Qt::black }, /* LED1 */
    { Qt::green,       Qt::black }, /* LED2 */
    { Qt::blue,        Qt::white }, /* LED3 */
    { Qt::yellow,      Qt::black }, /* LED4 */
    { Qt::darkRed,     Qt::white }, /* LED5 */
    { Qt::darkGreen,   Qt::white }, /* LED6 */
    { Qt::darkBlue,    Qt::white }, /* LED7 */
    { Qt::darkYellow,  Qt::white }, /* LED8 */
    { qRgb(0,242,123), Qt::black },
    { Qt::magenta,     Qt::black },
    { Qt::cyan,        Qt::black },
    { Qt::white,       Qt::black }, // ColorIndexWhite == 11
};


GrabWidget::GrabWidget(int id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GrabWidget)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << id;

    ui->setupUi(this);

    this->m_selfId = id;

    this->setCursorOnAll(Qt::OpenHandCursor);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    this->setFocusPolicy(Qt::NoFocus);
    ui->checkBox_SelfId->setText(QString::number(this->m_selfId + 1));

    this->setMouseTracking(true);

    this->resize(100, 100);

    setColors( m_selfId );

    connect(ui->checkBox_SelfId, SIGNAL(toggled(bool)), this, SLOT(checkBoxSelfId_Toggled(bool)));
}

GrabWidget::~GrabWidget()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    delete ui;
}

void GrabWidget::closeEvent(QCloseEvent *event)
{
    qWarning() << Q_FUNC_INFO << "event->type():" << event->type() << "Id:" << m_selfId;

    event->ignore();
}


void GrabWidget::saveSizeAndPosition()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    Settings::setLedPosition(m_selfId, pos());
    Settings::setLedSize(m_selfId, size());
}

void GrabWidget::settingsProfileChanged()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << m_selfId;

    coefRed = Settings::getLedCoefRed(m_selfId);
    coefGreen = Settings::getLedCoefGreen(m_selfId);
    coefBlue = Settings::getLedCoefBlue(m_selfId);

    this->move(Settings::getLedPosition(m_selfId));
    this->resize(Settings::getLedSize(m_selfId));

    emit resizeOrMoveCompleted( m_selfId );

    ui->checkBox_SelfId->setChecked(Settings::isLedEnabled(m_selfId));
}


void GrabWidget::setColors(int index)
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

void GrabWidget::setBackgroundColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << color.rgb();

    QPalette pal = this->palette();
    pal.setBrush(this->backgroundRole(), QBrush(color));
    this->setPalette(pal);
}

void GrabWidget::setTextColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << color.rgb();

    QPalette pal = this->palette();
    pal.setBrush(QPalette::WindowText, QBrush(color));
    ui->checkBox_SelfId->setPalette(pal);
    ui->labelWidthHeight->setPalette(pal);
}

// private
void GrabWidget::setCursorOnAll(Qt::CursorShape cursor)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << cursor;

    ui->checkBox_SelfId->setCursor(Qt::ArrowCursor);
    ui->labelWidthHeight->setCursor(cursor);
    this->setCursor(cursor);
}




void GrabWidget::mousePressEvent(QMouseEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << pe->pos();

    mousePressPosition = pe->pos();
    mousePressGlobalPosition = pe->globalPos();
    mousePressDiffFromBorder.setWidth(this->width() - pe->x());
    mousePressDiffFromBorder.setHeight(this->height() - pe->y());

    if(pe->buttons() == Qt::RightButton){
        // Send signal RightButtonClicked to main window for grouping widgets
        emit mouseRightButtonClicked(m_selfId);

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
            // Force set cursor to ClosedHand
            this->grabMouse(Qt::ClosedHandCursor);
            this->releaseMouse();
            // And set it to this widget and labelWxH
            this->setCursorOnAll(Qt::ClosedHandCursor);
        }

        emit resizeOrMoveStarted();

    }else{
        cmd = NOP;
    }
}

void GrabWidget::mouseMoveEvent(QMouseEvent *pe)
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO << "pe->pos() =" << pe->pos();

    int newWidth, newHeight, newX, newY;
    QPoint moveHere;

    QRect screen = QApplication::desktop()->screenGeometry(this);

    int left, top, right, bottom;

    switch(cmd){
    case MOVE:
        moveHere = pe->globalPos() - mousePressPosition;

        left = moveHere.x();
        top = moveHere.y();

        right = moveHere.x() + this->width();
        bottom = moveHere.y() + this->height();

        if(left < screen.left() + StickyCloserPixels &&
           left > screen.left() - StickyCloserPixels)
            moveHere.setX( screen.left() );

        if(top < screen.top() + StickyCloserPixels &&
           top > screen.top() - StickyCloserPixels)
            moveHere.setY( screen.top() );

        if(right < screen.right() + StickyCloserPixels &&
           right > screen.right() - StickyCloserPixels)
            moveHere.setX(screen.right() - this->width() + 1);

        if(bottom < screen.bottom() + StickyCloserPixels &&
           bottom > screen.bottom() - StickyCloserPixels)
            moveHere.setY(screen.bottom() - this->height() + 1);

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

void GrabWidget::mouseReleaseEvent(QMouseEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    checkAndSetCursors(pe);

    cmd = NOP;

    // Force set cursor from widget to mouse
    this->grabMouse(this->cursor());
    this->releaseMouse();

    saveSizeAndPosition();

    emit resizeOrMoveCompleted(m_selfId);
}


void GrabWidget::wheelEvent(QWheelEvent *pe)
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

void GrabWidget::resizeEvent(QResizeEvent *)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    ui->labelWidthHeight->setText( QString::number(this->width()) + "x" + QString::number(this->height()) );
}

void GrabWidget::paintEvent(QPaintEvent *)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    QPainter painter(this);
    painter.setPen(QColor(0x77, 0x77, 0x77));
    painter.drawRect(0, 0, width() - 1, height() - 1);
}


double GrabWidget::getCoefRed()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return coefRed;
}

double GrabWidget::getCoefGreen()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return coefGreen;
}

double GrabWidget::getCoefBlue()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return coefBlue;
}

bool GrabWidget::isGrabEnabled()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return ui->checkBox_SelfId->isChecked();
}


void GrabWidget::checkAndSetCursors(QMouseEvent *pe)
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
        if(pe->buttons() & Qt::LeftButton){
            this->setCursorOnAll(Qt::ClosedHandCursor);
        }else{
            this->setCursorOnAll(Qt::OpenHandCursor);
        }
    }
}

void GrabWidget::checkBoxSelfId_Toggled(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    setColors(colorIndex); // just update color

    Settings::setLedEnabled(m_selfId, state);
}
