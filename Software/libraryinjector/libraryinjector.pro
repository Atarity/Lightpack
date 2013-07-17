#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T13:03:08
#
#-------------------------------------------------

QT       -= core gui

DESTDIR  = ../lib
TARGET   = libraryinjector
TEMPLATE = lib
LIBS += -luuid -lwsock32 -lole32 -ladvapi32
QMAKE_LFLAGS +=-Wl,--kill-at

DEFINES += LIBRARYINJECTOR_LIBRARY


SOURCES += \
    LibraryInjector.c

HEADERS += \
    ILibraryInjector.h \
    LibraryInjector.h
