/*
 * GrabAreaWidget.hpp
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

#pragma once

//#include <QtWidgets/QWidget>
#include "GrabConfigWidget.hpp"

namespace Ui {
    class GrabAreaWidget;
}

class GrabAreaWidget : public QWidget
{
    Q_OBJECT
public:
    GrabAreaWidget(int id, QWidget *parent = 0);
    ~GrabAreaWidget();

signals:
    void resizeOrMoveStarted(int id);
    void resizeOrMoveCompleted(int id);
    void mouseRightButtonClicked(int selfId);
    void sizeAndPositionChanged(int w, int h, int x, int y);

private:
    virtual void closeEvent(QCloseEvent *event);
    void setCursorOnAll(Qt::CursorShape cursor);
    void checkAndSetCursors(QMouseEvent *pe);
    void setBackgroundColor(QColor color);
    void setTextColor(QColor color);

public:
    static const int ColorIndexWhite = 11;

private:
    enum {
        NOP,
        MOVE,
        RESIZE_HOR_RIGHT,
        RESIZE_HOR_LEFT,
        RESIZE_VER_UP,
        RESIZE_VER_DOWN,
        RESIZE_RIGHT_DOWN,
        RESIZE_RIGHT_UP,
        RESIZE_LEFT_DOWN,
        RESIZE_LEFT_UP
    } cmd;

    QPoint mousePressPosition;
    QPoint mousePressGlobalPosition;
    QSize mousePressDiffFromBorder;

    static const int MinimumWidth = 50;
    static const int MinimumHeight = 50;
    static const int BorderWidth = 10;
    static const int StickyCloserPixels = 10; // Sticky to screen when closer N pixels

    int m_selfId; // ID of this object

    Ui::GrabAreaWidget *ui;

    QColor m_textColor;
    QString m_selfIdString;
    QString m_widthHeight;

protected:
    virtual void mousePressEvent(QMouseEvent *pe);
    virtual void mouseMoveEvent(QMouseEvent *pe);
    virtual void mouseReleaseEvent(QMouseEvent *pe);
    virtual void wheelEvent(QWheelEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent(QPaintEvent *);
};

