#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T13:03:08
#
#-------------------------------------------------

QT       -= core gui

DESTDIR  = bin
TARGET   = libraryinjector
TEMPLATE = lib
LIBS += -luuid -lwsock32 -lole32 -ladvapi32
QMAKE_LFLAGS +=-Wl,--kill-at

QMAKE_POST_LINK = cp -f \"$(dir $(DESTDIR))libraryinjector.dll\" ../src/bin

DEFINES += LIBRARYINJECTOR_LIBRARY


SOURCES += \
    LibraryInjector.c

HEADERS += \
    ILibraryInjector.h \
    LibraryInjector.h
