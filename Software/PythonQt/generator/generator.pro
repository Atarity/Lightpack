TARGET = pythonqt_generator
CONFIG -= debug
CONFIG += release
DESTDIR = .

include(generator.pri)
include(../../build-config.prf)

win32: {
        QMAKE_POST_LINK = cp -f \"$${QTDIR}/bin/QtCore4.dll\" \"$$PWD\" && \
                cp -f \"$${QTDIR}/bin/QtXml4.dll\" \"$$PWD\" && \
                cp -f \"$${MINGW_RUNTIME_DIR}/mingwm10.dll\" \"$$PWD\" && \
                cp -f \"$${MINGW_RUNTIME_DIR}/libgcc_s_dw2-1.dll\" \"$$PWD\" && \
                cp -f \"$(DESTDIR_TARGET)\" \"$$PWD\" && \
                cd \"$$PWD\" && \
                $(TARGET)
}


# Input
HEADERS += \
        generatorsetqtscript.h \
        metaqtscriptbuilder.h \
        metaqtscript.h \
        shellgenerator.h \
        shellimplgenerator.h \
        shellheadergenerator.h \
        setupgenerator.h
   
SOURCES += \
        generatorsetqtscript.cpp \
        metaqtscriptbuilder.cpp \
        metaqtscript.cpp \
        shellgenerator.cpp \
        shellimplgenerator.cpp \
        shellheadergenerator.cpp \
        setupgenerator.cpp
