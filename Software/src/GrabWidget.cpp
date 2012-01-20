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
const QColor GrabWidget::m_colors[GrabWidget::ColorsCount][2] = {
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

    ui->button_OpenConfig->setFixedSize(25, 25);

    m_selfId = id;

    m_configWidget = new GrabConfigWidget();


    setCursorOnAll(Qt::OpenHandCursor);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setFocusPolicy(Qt::NoFocus);
    ui->label_ID->setText(QString::number(m_selfId + 1));

    setMouseTracking(true);

    resize(200, 200);

    fillBackgroundColored();

    connect(m_configWidget, SIGNAL(isAreaEnabled_Toggled(bool)), this, SLOT(onIsAreaEnabled_Toggled(bool)));
    connect(m_configWidget, SIGNAL(coefRed_ValueChanged(double)),   this, SLOT(onRedCoef_ValueChanged(double)));
    connect(m_configWidget, SIGNAL(coefGreen_ValueChanged(double)), this, SLOT(onGreenCoef_ValueChanged(double)));
    connect(m_configWidget, SIGNAL(coefBlue_ValueChanged(double)),  this, SLOT(onBlueCoef_ValueChanged(double)));
    connect(ui->button_OpenConfig, SIGNAL(clicked()), this, SLOT(onOpenConfigButton_Clicked()));
}

GrabWidget::~GrabWidget()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << m_selfId;

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

    m_coefRed = Settings::getLedCoefRed(m_selfId);
    m_coefGreen = Settings::getLedCoefGreen(m_selfId);
    m_coefBlue = Settings::getLedCoefBlue(m_selfId);

    this->move(Settings::getLedPosition(m_selfId));
    this->resize(Settings::getLedSize(m_selfId));

    emit resizeOrMoveCompleted( m_selfId );

    m_configWidget->setIsAreaEnabled(Settings::isLedEnabled(m_selfId));
}

void GrabWidget::setCursorOnAll(Qt::CursorShape cursor)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << cursor;

    ui->button_OpenConfig->setCursor(Qt::ArrowCursor);

    ui->label_ID->setCursor(cursor);
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

            // Grab all mouse input
            grabMouse(Qt::ClosedHandCursor);

            // And set it to this widget and labelWxH
            setCursorOnAll(Qt::ClosedHandCursor);
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

    // Release mouse after grab in mouse press event
    releaseMouse();

    saveSizeAndPosition();

    emit resizeOrMoveCompleted(m_selfId);
}


void GrabWidget::wheelEvent(QWheelEvent *pe)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO;

    if (Settings::isLedEnabled(m_selfId) == false){
        return;
    }

    if(pe->delta() > 0) m_colorIndex++;
    if(pe->delta() < 0) m_colorIndex--;

    if(m_colorIndex >= ColorsCount){
        m_colorIndex = 0;
    }else if(m_colorIndex < 0) {
        m_colorIndex = ColorsCount - 1;
    }
    fillBackground(m_colorIndex);
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

    return m_coefRed;
}

double GrabWidget::getCoefGreen()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return m_coefGreen;
}

double GrabWidget::getCoefBlue()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return m_coefBlue;
}

bool GrabWidget::isGrabEnabled()
{
    DEBUG_HIGH_LEVEL << Q_FUNC_INFO;

    return true;

    // TODO: test speed
    return Settings::isLedEnabled(m_selfId);
//    return ui->checkBox_SelfId->isChecked();
}

void GrabWidget::fillBackgroundWhite()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << m_selfId;

    setBackgroundColor(Qt::white);
    setTextColor(Qt::black);
}

void GrabWidget::fillBackgroundColored()
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << m_selfId;

    fillBackground(m_selfId);
}

void GrabWidget::fillBackground(int index)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << index;

    m_colorIndex = index % ColorsCount;

    setBackgroundColor(m_colors[m_colorIndex][0]);
    setTextColor(m_colors[m_colorIndex][1]);
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

void GrabWidget::onIsAreaEnabled_Toggled(bool state)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << state;

    Settings::setLedEnabled(m_selfId, state);

    fillBackgroundColored();    
}

void GrabWidget::onOpenConfigButton_Clicked()
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO;

    // Find y-coordinate for center of the button
    int buttonCenter = ui->button_OpenConfig->pos().y() + ui->button_OpenConfig->height() / 2;

    m_configWidget->showConfigFor(pos(), rect(), buttonCenter);
}

void GrabWidget::onRedCoef_ValueChanged(double value)
{
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << value;
    Settings::setLedCoefRed(m_selfId, value);
    m_coefRed = Settings::getLedCoefRed(m_selfId);
}

void GrabWidget::onGreenCoef_ValueChanged(double value)
{
    DEBUG_LOW_LEVEL << value;
    Settings::setLedCoefGreen(m_selfId, value);
    m_coefGreen = Settings::getLedCoefGreen(m_selfId);
}

void GrabWidget::onBlueCoef_ValueChanged(double value)
{
    DEBUG_LOW_LEVEL << value;
    Settings::setLedCoefBlue(m_selfId, value);
    m_coefBlue = Settings::getLedCoefBlue(m_selfId);
}

void GrabWidget::setBackgroundColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << color.rgb();

    QPalette pal = this->palette();

    if (Settings::isLedEnabled(m_selfId))
    {
        pal.setBrush(this->backgroundRole(), QBrush(color));
    } else {
        // Disabled widget
        pal.setBrush(this->backgroundRole(), QBrush(Qt::gray));
    }
    this->setPalette(pal);
}

void GrabWidget::setTextColor(QColor color)
{
    DEBUG_MID_LEVEL << Q_FUNC_INFO << hex << color.rgb();

    QPalette pal = this->palette();

    setOpenConfigButtonBackground(color);

    if (Settings::isLedEnabled(m_selfId))
    {
        pal.setBrush(QPalette::WindowText, QBrush(color));
    } else {
        // Disabled widget
        pal.setBrush(QPalette::WindowText, QBrush(Qt::darkGray));
    }
    ui->label_ID->setPalette(pal);
    ui->labelWidthHeight->setPalette(pal);
}

void GrabWidget::setOpenConfigButtonBackground(const QColor &color)
{
    QString image = (color == Qt::white && Settings::isLedEnabled(m_selfId)) ? "light" : "dark";

    ui->button_OpenConfig->setStyleSheet(
                "QPushButton         { border-image: url(:/buttons/arrow_right_" + image + "_25px.png) }"
                "QPushButton:hover   { border-image: url(:/buttons/arrow_right_" + image + "_25px_hover.png) }"
                "QPushButton:pressed { border-image: url(:/buttons/arrow_right_" + image + "_25px_pressed.png) }"
                );
}
