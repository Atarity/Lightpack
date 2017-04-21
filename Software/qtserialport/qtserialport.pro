CONFIG-=create_cmake

lessThan(QT_MAJOR_VERSION, 5) {
    TEMPLATE = subdirs
    SUBDIRS = src
    CONFIG += ordered

    !infile($$OUT_PWD/.qmake.cache, QTSERIALPORT_PROJECT_ROOT) {
        system("echo QTSERIALPORT_PROJECT_ROOT = $$PWD >> $$OUT_PWD/.qmake.cache")
        system("echo QTSERIALPORT_BUILD_ROOT = $$OUT_PWD >> $$OUT_PWD/.qmake.cache")
    }
} else {
    load(qt_parts)
}
