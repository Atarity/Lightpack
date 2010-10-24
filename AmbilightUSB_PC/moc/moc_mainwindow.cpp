/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon Oct 25 00:18:54 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../inc/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   12,   11,   11, 0x08,
      68,   11,   11,   11, 0x08,
      82,   11,   11,   11, 0x08,
      97,   11,   11,   11, 0x08,
     109,   11,   11,   11, 0x08,
     124,   11,   11,   11, 0x08,
     142,   11,   11,   11, 0x08,
     166,   11,   11,   11, 0x08,
     205,  199,   11,   11, 0x08,
     242,   11,   11,   11, 0x08,
     273,   11,   11,   11, 0x08,
     305,   11,   11,   11, 0x08,
     340,   11,   11,   11, 0x08,
     372,   11,   11,   11, 0x08,
     406,   11,   11,   11, 0x08,
     439,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0reason\0"
    "iconActivated(QSystemTrayIcon::ActivationReason)\0"
    "ambilightOn()\0ambilightOff()\0showAbout()\0"
    "showSettings()\0timerForUsbPoll()\0"
    "usbTimerDelayMsChange()\0"
    "usbTimerReconnectDelayMsChange()\0state\0"
    "settingsShowPixelsForAmbilight(bool)\0"
    "settingsWidthAmbilightChange()\0"
    "settingsHeightAmbilightChange()\0"
    "settingsUsbSendDataTimeoutChange()\0"
    "settingsWhiteBalanceRedChange()\0"
    "settingsWhiteBalanceGreenChange()\0"
    "settingsWhiteBalanceBlueChange()\0"
    "settingsHwTimerOptionsChange()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: iconActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 1: ambilightOn(); break;
        case 2: ambilightOff(); break;
        case 3: showAbout(); break;
        case 4: showSettings(); break;
        case 5: timerForUsbPoll(); break;
        case 6: usbTimerDelayMsChange(); break;
        case 7: usbTimerReconnectDelayMsChange(); break;
        case 8: settingsShowPixelsForAmbilight((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: settingsWidthAmbilightChange(); break;
        case 10: settingsHeightAmbilightChange(); break;
        case 11: settingsUsbSendDataTimeoutChange(); break;
        case 12: settingsWhiteBalanceRedChange(); break;
        case 13: settingsWhiteBalanceGreenChange(); break;
        case 14: settingsWhiteBalanceBlueChange(); break;
        case 15: settingsHwTimerOptionsChange(); break;
        default: ;
        }
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
