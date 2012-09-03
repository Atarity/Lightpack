#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T13:03:08
#
#-------------------------------------------------

QT       -= core gui

TARGET = libraryinjector
TEMPLATE = lib
LIBS += -luuid -lwsock32 -lole32 -ladvapi32
QMAKE_LFLAGS +=-Wl,--kill-at

CONFIG(debug, debug | release) {
    QMAKE_POST_LINK = cp -f debug/libraryinjector.dll ../bin
} else {
    QMAKE_POST_LINK = cp -f release/libraryinjector.dll ../bin
}
DEFINES += LIBRARYINJECTOR_LIBRARY


SOURCES += \
    LibraryInjector.c

HEADERS += \
    ILibraryInjector.h \
    LibraryInjector.h
