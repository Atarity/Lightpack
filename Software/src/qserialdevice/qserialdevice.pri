#
# QSerialDevice library project file
#

unix:include(unix/ttylocker.pri)
include(qserialdevice/qserialdevice.pri)
include(qserialdeviceenumerator/qserialdeviceenumerator.pri)

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
    TARGET = qserialdeviced
} else {
    DESTDIR = build/release
    TARGET = qserialdevice
}

win32 {
    LIBS += -lsetupapi -luuid -ladvapi32
}
unix:!macx {
    LIBS += -ludev
}
macx {
    LIBS += -framework IOKit -framework CoreFoundation
}
