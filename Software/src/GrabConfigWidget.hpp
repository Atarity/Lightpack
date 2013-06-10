/*
 * BubbleWidget.hpp
 *
 *  Created on: 16.01.2012
 *     Project: Lightpack
 *
 *  Copyright (c) 2012 Mike Shatohin (MikeShatohin [at] gmail.com)
 *
 *  Lightpack a USB content-driving ambient lighting system
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

#include <QtWidgets/QWidget>
#include <QTimer>

namespace Ui {
class GrabConfigWidget;
}

class GrabConfigWidget : public QWidget
{
    Q_OBJECT
        
public:
    explicit GrabConfigWidget(QWidget *parent = 0);
    ~GrabConfigWidget();

    void showConfigFor(QRect widgetGeometry, int buttonCenter);
    void setCoefs(double red, double green, double blue);
    void setIsAreaEnabled(bool isAreaEnabled);
    bool isAreaEnabled();

signals:
    void isAreaEnabled_Toggled(bool state);
    void coefRed_ValueChanged(double value);
    void coefGreen_ValueChanged(double value);
    void coefBlue_ValueChanged(double value);

private:
    enum ArrowSide {
        LeftSide,
        RightSide,
        TopSide,
        BottomSide
    };

private:
    void setArrow(ArrowSide arrowSide);
    void paintArrow(QPainter *p, ArrowSide side);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void changeEvent(QEvent *);
    
private slots:
    void onIsAreaEnabled_Toggled(bool state);
    void onCoefRed_ValueChanged(int value);
    void onCoefGreen_ValueChanged(int value);
    void onCoefBlue_ValueChanged(int value);

private:
    Ui::GrabConfigWidget *ui;

    ArrowSide m_arrowSide;

    static const unsigned MarginArrow;
    static const unsigned Margin;
};
