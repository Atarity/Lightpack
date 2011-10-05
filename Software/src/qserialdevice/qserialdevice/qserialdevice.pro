# PROJECT = Serial Device Liblary
TEMPLATE = lib
CONFIG += staticlib
# CONFIG += dll
QT -= gui
OBJECTS_DIR = obj
MOC_DIR = moc

# TODO: here in future replace
contains( CONFIG, dll ):win32:DEFINES += QSERIALDEVICE_SHARED

unix:include(../unix/ttylocker.pri)
include(qserialdevice.pri)

CONFIG(debug, debug|release) { 
    DESTDIR = debug
    TARGET = qserialdeviced
} else { 
    DESTDIR = release
    TARGET = qserialdevice
}
