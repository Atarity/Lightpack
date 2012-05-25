/****************************************************************************
**
** Copyright (C) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Script Generator project on Qt Labs.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
qs.script.importExtension("qt");
qs.script.importExtension("qt.core");
qs.script.importExtension("qt.gui");


function tr(s) { return s; }

function Screenshot(parent) {
    QWidget.call(this, parent);

    this.screenshotLabel = new QLabel();
    this.screenshotLabel.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding);
    this.screenshotLabel.alignment = Qt.Alignment(Qt.AlignCenter);
    this.screenshotLabel.setMinimumSize(240, 160);

    this.createOptionsGroupBox();
    this.createButtonsLayout();

    this.mainLayout = new QVBoxLayout();
    this.mainLayout.addWidget(this.screenshotLabel,0 ,0);
    this.mainLayout.addWidget(this.optionsGroupBox,0 ,0);
    this.mainLayout.addLayout(this.buttonsLayout);
    this.setLayout(this.mainLayout);

    this.shootScreen();
    this.delaySpinBox.setValue(5);

    this.windowIcon = new QIcon("classpath:com/trolltech/images/qt-logo.png");
    this.windowTitle = tr("Screenshot");
    this.resize(300, 200);
}

Screenshot.prototype = new QWidget();

Screenshot.prototype.resizeEvent = function(event) {
    var scaledSize = this.originalPixmap.size();
    scaledSize.scale(this.screenshotLabel.size, Qt.AspectRatioMode.KeepAspectRatio);
    if (this.screenshotLabel.pixmap != null
        || scaledSize != this.screenshotLabel.pixmap.size())
        this.updateScreenshotLabel();
}

Screenshot.prototype.newScreenshot = function() {
    if ( this.hideThisWindowCheckBox.checked)
        this.hide();
    this.newScreenshotButton.setDisabled(true);

    GrabManager.start(0);
    LedDeviceFactory.offLeds();

    //FIXME
    // QTimer.singleShot(this.delaySpinBox.value * 1000,
    //                      this, this.shootScreen);
    var singleShot = new QTimer();
    singleShot.singleShot = true;
    singleShot.timeout.connect(this, this.shootScreen);
    singleShot.start(this.delaySpinBox.value * 1000);
}

Screenshot.prototype.saveScreenshot = function() {
    var format = "png";
    var initialPath = QDir.currentPath() + tr("/untitled.") + format;
    var filter = tr(format.toUpperCase() + " Files (*." + format + ");;All Files (*)");
    var fileName = QFileDialog.getSaveFileName(this, tr("Save As"), initialPath, filter, null, null);
    // new QFileDialog.Option.Filter(filter)); //FIXME

    if (fileName != "")
        this.originalPixmap.save(fileName); //, format); //FIXME
}

Screenshot.prototype.shootScreen = function() {
    if ( this.delaySpinBox.value != 0)
        QApplication.beep();

    this.originalPixmap = null;

    this.originalPixmap = QPixmap.grabWindow(
        QApplication.desktop().winId());
    this.updateScreenshotLabel();

    this.newScreenshotButton.enabled = true;
    if (this.hideThisWindowCheckBox.checked)
        this.show();
}

Screenshot.prototype.updateCheckBox = function() {
    if (this.delaySpinBox.value)
        this.hideThisWindowCheckBox.setDisabled(true);
    else
        this.hideThisWindowCheckBox.setDisabled(false);
}

Screenshot.prototype.createOptionsGroupBox = function() {
    this.optionsGroupBox = new QGroupBox(tr("Options"));

    this.delaySpinBox = new QSpinBox();
    this.delaySpinBox.suffix = tr(" s");
    this.delaySpinBox.maximum = 60;
    this.delaySpinBox['valueChanged(int)'].connect(this, this.updateCheckBox);

    this.delaySpinBoxLabel = new QLabel(tr("Screenshot Delay:"));

    this.hideThisWindowCheckBox = new QCheckBox(tr("Hide This Window"));

    this.optionsGroupBoxLayout = new QGridLayout();
    this.optionsGroupBoxLayout.addWidget(this.delaySpinBoxLabel, 0, 0);
    this.optionsGroupBoxLayout.addWidget(this.delaySpinBox, 0, 1);
    this.optionsGroupBoxLayout.addWidget(this.hideThisWindowCheckBox, 1, 0, 1, 2);
    this.optionsGroupBox.setLayout(this.optionsGroupBoxLayout);
}

Screenshot.prototype.createButtonsLayout = function() {
    this.newScreenshotButton = this.createButton(tr("New Screenshot"), this,
                                            this.newScreenshot);

    this.saveScreenshotButton = this.createButton(tr("Save Screenshot"), this,
                                            this.saveScreenshot);

    this.quitScreenshotButton = this.createButton(tr("Quit"), this, this.close);

    this.buttonsLayout = new QHBoxLayout();
    this.buttonsLayout.addStretch();
    this.buttonsLayout.addWidget(this.newScreenshotButton, 0, 0);
    this.buttonsLayout.addWidget(this.saveScreenshotButton, 0, 0);
    this.buttonsLayout.addWidget(this.quitScreenshotButton, 0, 0);
}

Screenshot.prototype.createButton = function(text, receiver, member) {
    var button = new QPushButton(text);
    button.clicked.connect(receiver, member);
    return button;
}

Screenshot.prototype.updateScreenshotLabel = function() {
    this.screenshotLabel.setPixmap(this.originalPixmap.scaled(this.screenshotLabel.size,
                                                              Qt.KeepAspectRatio,
                                                              Qt.SmoothTransformation));
}

var screenshot = new Screenshot(null);

screenshot.show();
QCoreApplication.exec();
