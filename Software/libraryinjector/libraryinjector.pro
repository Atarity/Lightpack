#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T13:03:08
#
#-------------------------------------------------

QT -= core gui

DESTDIR  = ../lib
TARGET   = libraryinjector
TEMPLATE = lib
LIBS += -luuid -lole32 -ladvapi32 -luser32
QMAKE_LFLAGS +=-Wl,--kill-at

DEFINES += LIBRARYINJECTOR_LIBRARY


SOURCES += \
    LibraryInjector.c \
    dllmain.c

HEADERS += \
    ILibraryInjector.h \
    LibraryInjector.h
