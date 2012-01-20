/*
 * BubbleWidget.cpp
 *
 *  Created on: 16.01.2012
 *     Project: Lightpack
 *
 *  Copyright (c) 2012 brunql
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

#include "GrabConfigWidget.hpp"
#include "ui_GrabConfigWidget.h"
#include <QPainter>
#include <QBitmap>
#include "debug.h"

const unsigned GrabConfigWidget::MarginArrow = 20;
const unsigned GrabConfigWidget::Margin = 10;

GrabConfigWidget::GrabConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GrabConfigWidget),
    m_arrowSide(LeftSide)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);

    adjustSize();

    connect(ui->checkBox_IsAreaEnabled, SIGNAL(toggled(bool)), this, SLOT(onIsAreaEnabled_Toggled(bool)));
    connect(ui->spinBox_Red,    SIGNAL(valueChanged(int)), this, SLOT(onCoefRed_ValueChanged(int)));
    connect(ui->spinBox_Green,  SIGNAL(valueChanged(int)), this, SLOT(onCoefGreen_ValueChanged(int)));
    connect(ui->spinBox_Blue,   SIGNAL(valueChanged(int)), this, SLOT(onCoefBlue_ValueChanged(int)));
}

GrabConfigWidget::~GrabConfigWidget()
{
    delete ui;
}

void GrabConfigWidget::showConfigFor(QPoint p, QRect r, int buttonCenter)
{
    move(p.x() + r.right(), p.y() - height() / 2 + buttonCenter);
    show();
}

void GrabConfigWidget::setIsAreaEnabled(bool isAreaEnabled)
{
    ui->checkBox_IsAreaEnabled->setChecked(isAreaEnabled);
}

void GrabConfigWidget::paintArrow(QPainter *p, ArrowSide side)
{
    QMargins margins(Margin, Margin, Margin, Margin);
    QRect mainRect(0, 0, width() - 1, height() - 1);
    QPainterPath pathArrow;
    QLine lineWhiteArrow;

    switch(side)
    {
    case LeftSide:
        margins.setLeft(MarginArrow);
        mainRect.setLeft(Margin);

        pathArrow.moveTo(Margin, height() / 2 - Margin);
        pathArrow.lineTo(0,      height() / 2);
        pathArrow.lineTo(Margin, height() / 2 + Margin);

        lineWhiteArrow = QLine(pathArrow.elementAt(0).x,
                               pathArrow.elementAt(0).y + 1,
                               pathArrow.elementAt(2).x,
                               pathArrow.elementAt(2).y - 1);
        break;
    case RightSide:
        margins.setRight(MarginArrow);
        mainRect.setRight(width() - Margin - 1);

        pathArrow.moveTo(width() - Margin,  height() / 2 - Margin);
        pathArrow.lineTo(width(),           height() / 2);
        pathArrow.lineTo(width() - Margin,  height() / 2 + Margin);

        lineWhiteArrow = QLine(pathArrow.elementAt(0).x,
                               pathArrow.elementAt(0).y + 1,
                               pathArrow.elementAt(2).x,
                               pathArrow.elementAt(2).y - 1);
        break;
    case TopSide:
        margins.setTop(MarginArrow);
        mainRect.setTop(Margin);

        pathArrow.moveTo(width() / 2 - Margin,  Margin);
        pathArrow.lineTo(width() / 2,           0);
        pathArrow.lineTo(width() / 2 + Margin,  Margin);

        lineWhiteArrow = QLine(pathArrow.elementAt(0).x + 1,
                               pathArrow.elementAt(0).y,
                               pathArrow.elementAt(2).x - 1,
                               pathArrow.elementAt(2).y);

        break;
    case BottomSide:
        margins.setBottom(MarginArrow);
        mainRect.setBottom(height() - Margin - 2);

        pathArrow.moveTo(width() / 2 - Margin,  height() - Margin - 1);
        pathArrow.lineTo(width() / 2,           height() - 1);
        pathArrow.lineTo(width() / 2 + Margin,  height() - Margin - 1);

        lineWhiteArrow = QLine(pathArrow.elementAt(0).x + 1,
                               pathArrow.elementAt(0).y,
                               pathArrow.elementAt(2).x - 1,
                               pathArrow.elementAt(2).y);

        break;
    }

    ui->gridLayout->setContentsMargins(margins);

    p->setPen(QColor(0x77, 0x77, 0x77));
    p->fillRect(mainRect, Qt::white);
    p->drawRect(mainRect);

    p->fillPath(pathArrow, Qt::white);
    p->drawPath(pathArrow);

    p->setPen(Qt::white);
    p->drawLine(lineWhiteArrow);
}

void GrabConfigWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    paintArrow(&painter, m_arrowSide);
}

void GrabConfigWidget::onIsAreaEnabled_Toggled(bool state)
{
    ui->spinBox_Red->setEnabled(state);
    ui->spinBox_Green->setEnabled(state);
    ui->spinBox_Blue->setEnabled(state);

    emit isAreaEnabled_Toggled(state);
}

void GrabConfigWidget::onCoefRed_ValueChanged(int value)
{
    emit coefRed_ValueChanged(value / 100.0);
}

void GrabConfigWidget::onCoefGreen_ValueChanged(int value)
{
    emit coefGreen_ValueChanged(value / 100.0);
}

void GrabConfigWidget::onCoefBlue_ValueChanged(int value)
{
    emit coefBlue_ValueChanged(value / 100.0);
}
