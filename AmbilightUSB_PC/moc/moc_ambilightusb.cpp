/****************************************************************************
** Meta object code from reading C++ file 'ambilightusb.h'
**
** Created: Wed Oct 27 23:08:20 2010
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
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   14,   13,   13, 0x05,
      48,   14,   13,   13, 0x05,
      81,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     122,  115,   13,   13, 0x0a,
     173,  146,   13,   13, 0x0a,
     243,  205,   13,   13, 0x0a,
     279,  268,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AmbilightUsb[] = {
    "AmbilightUsb\0\0isSuccess\0openDeviceSuccess(bool)\0"
    "writeBufferToDeviceSuccess(bool)\0"
    "readBufferFromDeviceSuccess(bool)\0"
    "colors\0updateColors(LedColors)\0"
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
        case 0: openDeviceSuccess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: writeBufferToDeviceSuccess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: readBufferFromDeviceSuccess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: updateColors((*reinterpret_cast< LedColors(*)>(_a[1]))); break;
        case 4: setUsbSendDataTimeoutMs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: setTimerOptions((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: setColorDepth((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void AmbilightUsb::openDeviceSuccess(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AmbilightUsb::writeBufferToDeviceSuccess(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AmbilightUsb::readBufferFromDeviceSuccess(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
