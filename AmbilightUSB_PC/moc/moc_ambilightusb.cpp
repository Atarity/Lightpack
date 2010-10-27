/****************************************************************************
** Meta object code from reading C++ file 'ambilightusb.h'
**
** Created: Wed Oct 27 21:01:14 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../inc/ambilightusb.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ambilightusb.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AmbilightUsb[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      32,   13,   13,   13, 0x05,
      52,   13,   13,   13, 0x05,
      79,   13,   13,   13, 0x05,
     108,   13,   13,   13, 0x05,
     136,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     173,  166,   13,   13, 0x0a,
     233,  206,   13,   13, 0x0a,
     303,  265,   13,   13, 0x0a,
     339,  328,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AmbilightUsb[] = {
    "AmbilightUsb\0\0openDeviceError()\0"
    "openDeviceSuccess()\0writeBufferToDeviceError()\0"
    "writeBufferToDeviceSuccess()\0"
    "readBufferFromDeviceError()\0"
    "readBufferFromDeviceSuccess()\0colors\0"
    "updateColors(int[LEDS_COUNT][3])\0"
    "usb_send_data_timeout_secs\0"
    "setUsbSendDataTimeoutMs(double)\0"
    "prescallerIndex,outputCompareRegValue\0"
    "setTimerOptions(int,int)\0colorDepth\0"
    "setColorDepth(int)\0"
};

const QMetaObject AmbilightUsb::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AmbilightUsb,
      qt_meta_data_AmbilightUsb, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AmbilightUsb::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AmbilightUsb::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AmbilightUsb::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AmbilightUsb))
        return static_cast<void*>(const_cast< AmbilightUsb*>(this));
    return QObject::qt_metacast(_clname);
}

int AmbilightUsb::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: openDeviceError(); break;
        case 1: openDeviceSuccess(); break;
        case 2: writeBufferToDeviceError(); break;
        case 3: writeBufferToDeviceSuccess(); break;
        case 4: readBufferFromDeviceError(); break;
        case 5: readBufferFromDeviceSuccess(); break;
        case 6: updateColors((*reinterpret_cast< int(*)[LEDS_COUNT][3]>(_a[1]))); break;
        case 7: setUsbSendDataTimeoutMs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: setTimerOptions((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: setColorDepth((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void AmbilightUsb::openDeviceError()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void AmbilightUsb::openDeviceSuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void AmbilightUsb::writeBufferToDeviceError()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void AmbilightUsb::writeBufferToDeviceSuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void AmbilightUsb::readBufferFromDeviceError()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void AmbilightUsb::readBufferFromDeviceSuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
