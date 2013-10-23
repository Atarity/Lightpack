#-------------------------------------------------
#
# Project created by QtCreator 2013-10-22T15:58:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wizard
TEMPLATE = app

INCLUDEPATH += ../src ../math/include ../src/hidapi


SOURCES += main.cpp\
    Wizard.cpp \
    ../src/Settings.cpp \
    ../src/LedDeviceLightpack.cpp \
    ../src/AbstractLedDevice.cpp \
    ../math/PrismatikMath.cpp \
    LightpacksDiscoveryPage.cpp \
    MonitorsConfigurationPage.cpp \
    MonitorIdForm.cpp

HEADERS  += Wizard.hpp \
    ../src/Settings.hpp \
    ../src/LedDeviceLightpack.hpp \
    ../src/AbstractLedDevice.hpp \
    LightpacksDiscoveryPage.hpp \
    SettingsAwareTrait.hpp \
    MonitorsConfigurationPage.hpp \
    MonitorIdForm.hpp

FORMS    += \
    Wizard.ui \
    LightpacksDiscoveryPage.ui \
    MonitorsConfigurationPage.ui \
    MonitorIdForm.ui

unix:!macx{
    # Linux version using libusb and hidapi codes
    SOURCES += ../src/hidapi/linux/hid-libusb.c
}

win32{
    LIBS    += -lhid -lusbcamd -lsetupapi
    SOURCES += ../src/hidapi/windows/hid.c
}

macx{
    SOURCES += ../src/hidapi/mac/hid.c
}
