/****************************************************************************
** Meta object code from reading C++ file 'PythonQtTests.h'
**
** Created: Sat 5. May 08:31:17 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../PythonQtTests.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PythonQtTests.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PythonQtTestApi[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      32,   16,   16,   16, 0x08,
      43,   16,   16,   16, 0x08,
      59,   16,   16,   16, 0x08,
      74,   16,   16,   16, 0x08,
      89,   16,   16,   16, 0x08,
     112,   16,   16,   16, 0x08,
     130,   16,   16,   16, 0x08,
     145,   16,   16,   16, 0x08,
     162,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PythonQtTestApi[] = {
    "PythonQtTestApi\0\0initTestCase()\0"
    "testCall()\0testVariables()\0testRedirect()\0"
    "testImporter()\0testQColorDecorators()\0"
    "testQtNamespace()\0testConnects()\0"
    "testProperties()\0testDynamicProperties()\0"
};

const QMetaObject PythonQtTestApi::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PythonQtTestApi,
      qt_meta_data_PythonQtTestApi, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PythonQtTestApi::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PythonQtTestApi::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PythonQtTestApi::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQtTestApi))
        return static_cast<void*>(const_cast< PythonQtTestApi*>(this));
    return QObject::qt_metacast(_clname);
}

int PythonQtTestApi::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: initTestCase(); break;
        case 1: testCall(); break;
        case 2: testVariables(); break;
        case 3: testRedirect(); break;
        case 4: testImporter(); break;
        case 5: testQColorDecorators(); break;
        case 6: testQtNamespace(); break;
        case 7: testConnects(); break;
        case 8: testProperties(); break;
        case 9: testDynamicProperties(); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}
static const uint qt_meta_data_ClassD[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,    7,    8,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClassD[] = {
    "ClassD\0\0int\0getD()\0"
};

const QMetaObject ClassD::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ClassD,
      qt_meta_data_ClassD, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClassD::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClassD::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClassD::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClassD))
        return static_cast<void*>(const_cast< ClassD*>(this));
    if (!strcmp(_clname, "ClassA"))
        return static_cast< ClassA*>(const_cast< ClassD*>(this));
    if (!strcmp(_clname, "ClassB"))
        return static_cast< ClassB*>(const_cast< ClassD*>(this));
    return QObject::qt_metacast(_clname);
}

int ClassD::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { int _r = getD();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_ClassAWrapper[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   14,   15,   14, 0x0a,
      42,   40,   36,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClassAWrapper[] = {
    "ClassAWrapper\0\0ClassA*\0new_ClassA()\0"
    "int\0o\0getX(ClassA*)\0"
};

const QMetaObject ClassAWrapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ClassAWrapper,
      qt_meta_data_ClassAWrapper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClassAWrapper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClassAWrapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClassAWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClassAWrapper))
        return static_cast<void*>(const_cast< ClassAWrapper*>(this));
    return QObject::qt_metacast(_clname);
}

int ClassAWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { ClassA* _r = new_ClassA();
            if (_a[0]) *reinterpret_cast< ClassA**>(_a[0]) = _r; }  break;
        case 1: { int _r = getX((*reinterpret_cast< ClassA*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_ClassBWrapper[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   14,   15,   14, 0x0a,
      42,   40,   36,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClassBWrapper[] = {
    "ClassBWrapper\0\0ClassB*\0new_ClassB()\0"
    "int\0o\0getY(ClassB*)\0"
};

const QMetaObject ClassBWrapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ClassBWrapper,
      qt_meta_data_ClassBWrapper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClassBWrapper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClassBWrapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClassBWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClassBWrapper))
        return static_cast<void*>(const_cast< ClassBWrapper*>(this));
    return QObject::qt_metacast(_clname);
}

int ClassBWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { ClassB* _r = new_ClassB();
            if (_a[0]) *reinterpret_cast< ClassB**>(_a[0]) = _r; }  break;
        case 1: { int _r = getY((*reinterpret_cast< ClassB*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_ClassCWrapper[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   14,   15,   14, 0x0a,
      42,   40,   36,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClassCWrapper[] = {
    "ClassCWrapper\0\0ClassC*\0new_ClassC()\0"
    "int\0o\0getZ(ClassC*)\0"
};

const QMetaObject ClassCWrapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ClassCWrapper,
      qt_meta_data_ClassCWrapper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClassCWrapper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClassCWrapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClassCWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClassCWrapper))
        return static_cast<void*>(const_cast< ClassCWrapper*>(this));
    return QObject::qt_metacast(_clname);
}

int ClassCWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { ClassC* _r = new_ClassC();
            if (_a[0]) *reinterpret_cast< ClassC**>(_a[0]) = _r; }  break;
        case 1: { int _r = getZ((*reinterpret_cast< ClassC*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_ClassDWrapper[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   14,   15,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClassDWrapper[] = {
    "ClassDWrapper\0\0ClassD*\0new_ClassD()\0"
};

const QMetaObject ClassDWrapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ClassDWrapper,
      qt_meta_data_ClassDWrapper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClassDWrapper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClassDWrapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClassDWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClassDWrapper))
        return static_cast<void*>(const_cast< ClassDWrapper*>(this));
    return QObject::qt_metacast(_clname);
}

int ClassDWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { ClassD* _r = new_ClassD();
            if (_a[0]) *reinterpret_cast< ClassD**>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_PythonQtTestApiHelper[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x0a,
      35,   22,   22,   22, 0x0a,
      51,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PythonQtTestApiHelper[] = {
    "PythonQtTestApiHelper\0\0setPassed()\0"
    "stdOut(QString)\0stdErr(QString)\0"
};

const QMetaObject PythonQtTestApiHelper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PythonQtTestApiHelper,
      qt_meta_data_PythonQtTestApiHelper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PythonQtTestApiHelper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PythonQtTestApiHelper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PythonQtTestApiHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQtTestApiHelper))
        return static_cast<void*>(const_cast< PythonQtTestApiHelper*>(this));
    if (!strcmp(_clname, "PythonQtImportFileInterface"))
        return static_cast< PythonQtImportFileInterface*>(const_cast< PythonQtTestApiHelper*>(this));
    return QObject::qt_metacast(_clname);
}

int PythonQtTestApiHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setPassed(); break;
        case 1: stdOut((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: stdErr((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_PQCppObjectWrapper[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   19,   20,   19, 0x0a,
      38,   36,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PQCppObjectWrapper[] = {
    "PQCppObjectWrapper\0\0int\0getHeight()\0"
    "h\0setHeight(int)\0"
};

const QMetaObject PQCppObjectWrapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PQCppObjectWrapper,
      qt_meta_data_PQCppObjectWrapper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PQCppObjectWrapper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PQCppObjectWrapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PQCppObjectWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PQCppObjectWrapper))
        return static_cast<void*>(const_cast< PQCppObjectWrapper*>(this));
    return QObject::qt_metacast(_clname);
}

int PQCppObjectWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { int _r = getHeight();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: setHeight((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_PQCppObjectDecorator[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      30,   26,   22,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PQCppObjectDecorator[] = {
    "PQCppObjectDecorator\0\0int\0obj\0"
    "getH(PQCppObject*)\0"
};

const QMetaObject PQCppObjectDecorator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PQCppObjectDecorator,
      qt_meta_data_PQCppObjectDecorator, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PQCppObjectDecorator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PQCppObjectDecorator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PQCppObjectDecorator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PQCppObjectDecorator))
        return static_cast<void*>(const_cast< PQCppObjectDecorator*>(this));
    return QObject::qt_metacast(_clname);
}

int PQCppObjectDecorator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { int _r = getH((*reinterpret_cast< PQCppObject*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_PQCppObjectNoWrapDecorator[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      47,   27,   28,   27, 0x0a,
      77,   71,   28,   27, 0x0a,
     124,  118,   28,   27, 0x0a,
     162,  158,  154,   27, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PQCppObjectNoWrapDecorator[] = {
    "PQCppObjectNoWrapDecorator\0\0"
    "PQCppObjectNoWrap*\0new_PQCppObjectNoWrap()\0"
    "other\0new_PQCppObjectNoWrap(PQCppObjectNoWrap)\0"
    "value\0new_PQCppObjectNoWrap(double)\0"
    "int\0obj\0getH(PQCppObjectNoWrap*)\0"
};

const QMetaObject PQCppObjectNoWrapDecorator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PQCppObjectNoWrapDecorator,
      qt_meta_data_PQCppObjectNoWrapDecorator, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PQCppObjectNoWrapDecorator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PQCppObjectNoWrapDecorator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PQCppObjectNoWrapDecorator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PQCppObjectNoWrapDecorator))
        return static_cast<void*>(const_cast< PQCppObjectNoWrapDecorator*>(this));
    return QObject::qt_metacast(_clname);
}

int PQCppObjectNoWrapDecorator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { PQCppObjectNoWrap* _r = new_PQCppObjectNoWrap();
            if (_a[0]) *reinterpret_cast< PQCppObjectNoWrap**>(_a[0]) = _r; }  break;
        case 1: { PQCppObjectNoWrap* _r = new_PQCppObjectNoWrap((*reinterpret_cast< const PQCppObjectNoWrap(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PQCppObjectNoWrap**>(_a[0]) = _r; }  break;
        case 2: { PQCppObjectNoWrap* _r = new_PQCppObjectNoWrap((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PQCppObjectNoWrap**>(_a[0]) = _r; }  break;
        case 3: { int _r = getH((*reinterpret_cast< PQCppObjectNoWrap*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_PQCppObject2Decorator[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       2,   39, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      37,   22,   23,   22, 0x0a,
      78,   69,   56,   22, 0x0a,
     147,   69,  120,   22, 0x0a,
     203,   69,   56,   22, 0x0a,
     236,   69,   56,   22, 0x0a,

 // enums: name, flags, count, data
      56, 0x0,    2,   47,
     278, 0x1,    2,   51,

 // enum data: key, value
     287, uint(PQCppObject2Decorator::TestEnumValue1),
     302, uint(PQCppObject2Decorator::TestEnumValue2),
     287, uint(PQCppObject2Decorator::TestEnumValue1),
     302, uint(PQCppObject2Decorator::TestEnumValue2),

       0        // eod
};

static const char qt_meta_stringdata_PQCppObject2Decorator[] = {
    "PQCppObject2Decorator\0\0PQCppObject2*\0"
    "new_PQCppObject2()\0TestEnumFlag\0"
    "obj,flag\0testEnumFlag1(PQCppObject2*,TestEnumFlag)\0"
    "PQCppObject2::TestEnumFlag\0"
    "testEnumFlag2(PQCppObject2*,PQCppObject2::TestEnumFlag)\0"
    "testEnumFlag3(PQCppObject2*,int)\0"
    "testEnumFlag3(PQCppObject2*,TestEnumFlag)\0"
    "TestEnum\0TestEnumValue1\0TestEnumValue2\0"
};

const QMetaObject PQCppObject2Decorator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PQCppObject2Decorator,
      qt_meta_data_PQCppObject2Decorator, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PQCppObject2Decorator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PQCppObject2Decorator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PQCppObject2Decorator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PQCppObject2Decorator))
        return static_cast<void*>(const_cast< PQCppObject2Decorator*>(this));
    return QObject::qt_metacast(_clname);
}

int PQCppObject2Decorator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { PQCppObject2* _r = new_PQCppObject2();
            if (_a[0]) *reinterpret_cast< PQCppObject2**>(_a[0]) = _r; }  break;
        case 1: { TestEnumFlag _r = testEnumFlag1((*reinterpret_cast< PQCppObject2*(*)>(_a[1])),(*reinterpret_cast< TestEnumFlag(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< TestEnumFlag*>(_a[0]) = _r; }  break;
        case 2: { PQCppObject2::TestEnumFlag _r = testEnumFlag2((*reinterpret_cast< PQCppObject2*(*)>(_a[1])),(*reinterpret_cast< PQCppObject2::TestEnumFlag(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< PQCppObject2::TestEnumFlag*>(_a[0]) = _r; }  break;
        case 3: { TestEnumFlag _r = testEnumFlag3((*reinterpret_cast< PQCppObject2*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< TestEnumFlag*>(_a[0]) = _r; }  break;
        case 4: { TestEnumFlag _r = testEnumFlag3((*reinterpret_cast< PQCppObject2*(*)>(_a[1])),(*reinterpret_cast< TestEnumFlag(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< TestEnumFlag*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_PythonQtTestSlotCalling[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x08,
      40,   24,   24,   24, 0x08,
      47,   24,   24,   24, 0x08,
      67,   24,   24,   24, 0x08,
      86,   24,   24,   24, 0x08,
     105,   24,   24,   24, 0x08,
     129,   24,   24,   24, 0x08,
     151,   24,   24,   24, 0x08,
     175,   24,   24,   24, 0x08,
     198,   24,   24,   24, 0x08,
     219,   24,   24,   24, 0x08,
     236,   24,   24,   24, 0x08,
     254,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PythonQtTestSlotCalling[] = {
    "PythonQtTestSlotCalling\0\0initTestCase()\0"
    "init()\0testNoArgSlotCall()\0"
    "testPODSlotCalls()\0testCPPSlotCalls()\0"
    "testQVariantSlotCalls()\0testObjectSlotCalls()\0"
    "testMultiArgsSlotCall()\0testPyObjectSlotCall()\0"
    "testOverloadedCall()\0testCppFactory()\0"
    "testInheritance()\0testAutoConversion()\0"
};

const QMetaObject PythonQtTestSlotCalling::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PythonQtTestSlotCalling,
      qt_meta_data_PythonQtTestSlotCalling, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PythonQtTestSlotCalling::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PythonQtTestSlotCalling::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PythonQtTestSlotCalling::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQtTestSlotCalling))
        return static_cast<void*>(const_cast< PythonQtTestSlotCalling*>(this));
    return QObject::qt_metacast(_clname);
}

int PythonQtTestSlotCalling::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: initTestCase(); break;
        case 1: init(); break;
        case 2: testNoArgSlotCall(); break;
        case 3: testPODSlotCalls(); break;
        case 4: testCPPSlotCalls(); break;
        case 5: testQVariantSlotCalls(); break;
        case 6: testObjectSlotCalls(); break;
        case 7: testMultiArgsSlotCall(); break;
        case 8: testPyObjectSlotCall(); break;
        case 9: testOverloadedCall(); break;
        case 10: testCppFactory(); break;
        case 11: testInheritance(); break;
        case 12: testAutoConversion(); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}
static const uint qt_meta_data_PythonQtTestSlotCallingHelper[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      64,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      31,   30,   30,   30, 0x0a,
      43,   30,   30,   30, 0x0a,
      57,   55,   30,   30, 0x0a,
      72,   55,   30,   30, 0x0a,
      88,   55,   30,   30, 0x0a,
     106,  102,   30,   30, 0x0a,
     124,  102,   30,   30, 0x0a,
     146,  102,   30,   30, 0x0a,
     169,  165,   30,   30, 0x0a,
     193,   55,  189,   30, 0x0a,
     210,   55,  205,   30, 0x0a,
     229,   55,  224,   30, 0x0a,
     248,   55,  243,   30, 0x0a,
     276,   55,  262,   30, 0x0a,
     305,   55,  300,   30, 0x0a,
     325,   55,  319,   30, 0x0a,
     347,   55,  341,   30, 0x0a,
     378,   55,  363,   30, 0x0a,
     410,   55,  404,   30, 0x0a,
     433,   55,  426,   30, 0x0a,
     461,   55,  453,   30, 0x0a,
     492,  490,  483,   30, 0x0a,
     516,  490,  510,   30, 0x0a,
     542,  540,  532,   30, 0x0a,
     576,  574,  562,   30, 0x0a,
     617,  613,  604,   30, 0x0a,
     646,  613,  639,   30, 0x0a,
     665,  613,  639,   30, 0x0a,
     689,  685,  639,   30, 0x0a,
     715,  709,  639,   30, 0x0a,
     744,   30,  736,   30, 0x0a,
     771,  767,  757,   30, 0x0a,
     798,  794,  757,   30, 0x0a,
     831,  794,  604,   30, 0x0a,
     896,  767,  865,   30, 0x0a,
     951,  767,  942,   30, 0x0a,
     981,  767,  972,   30, 0x0a,
    1002,   30,  942,   30, 0x0a,
    1038, 1030, 1017,   30, 0x0a,
    1086, 1084, 1071,   30, 0x0a,
    1111, 1109, 1071,   30, 0x0a,
    1159, 1084, 1140,   30, 0x0a,
    1188, 1109, 1140,   30, 0x0a,
    1247,   30, 1229,   30, 0x0a,
    1311,   30, 1277,   30, 0x0a,
    1376,   30, 1355,   30, 0x0a,
    1442,   30, 1407,   30, 0x0a,
    1506,   30, 1484,   30, 0x0a,
    1545, 1543, 1535,   30, 0x0a,
    1575, 1543, 1567,   30, 0x0a,
    1605, 1543, 1597,   30, 0x0a,
    1635, 1543, 1627,   30, 0x0a,
    1657,   30, 1535,   30, 0x0a,
    1672,   30, 1567,   30, 0x0a,
    1687,   30, 1597,   30, 0x0a,
    1702,   30, 1627,   30, 0x0a,
    1717,   30, 1535,   30, 0x0a,
    1735,   30, 1567,   30, 0x0a,
    1753,   30, 1535,   30, 0x0a,
    1771,   30, 1567,   30, 0x0a,
    1789,  709,  639,   30, 0x0a,
    1830, 1824, 1817,   30, 0x0a,
    1867, 1863, 1858,   30, 0x0a,
    1906, 1899, 1891,   30, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PythonQtTestSlotCallingHelper[] = {
    "PythonQtTestSlotCallingHelper\0\0"
    "setPassed()\0testNoArg()\0a\0overload(bool)\0"
    "overload(float)\0overload(int)\0str\0"
    "overload(QString)\0overload(QStringList)\0"
    "overload(QObject*)\0a,b\0overload(float,int)\0"
    "int\0getInt(int)\0uint\0getUInt(uint)\0"
    "bool\0getBool(bool)\0char\0getChar(char)\0"
    "unsigned char\0getUChar(unsigned char)\0"
    "long\0getLong(long)\0ulong\0getULong(ulong)\0"
    "short\0getShort(short)\0unsigned short\0"
    "getUShort(unsigned short)\0QChar\0"
    "getQChar(QChar)\0qint64\0getLongLong(qint64)\0"
    "quint64\0getULongLong(quint64)\0double\0"
    "d\0getDouble(double)\0float\0getFloat(float)\0"
    "QString\0s\0getQString(QString)\0QStringList\0"
    "l\0getQStringList(QStringList)\0QVariant\0"
    "var\0getQVariant(QVariant)\0QColor\0"
    "getQColor1(QColor)\0getQColor2(QColor&)\0"
    "col\0getQColor3(QColor*)\0color\0"
    "getQColor4(QVariant)\0QColor*\0getQColor5()\0"
    "PyObject*\0obj\0getPyObject(PyObject*)\0"
    "val\0getPyObjectFromVariant(QVariant)\0"
    "getPyObjectFromVariant2(QVariant)\0"
    "PythonQtTestSlotCallingHelper*\0"
    "getTestObject(PythonQtTestSlotCallingHelper*)\0"
    "QObject*\0getQObject(QObject*)\0QWidget*\0"
    "getQWidget(QWidget*)\0getNewObject()\0"
    "QVariantList\0a,b,str\0"
    "getMultiArgs(int,double,QString)\0"
    "PQCppObject*\0h\0createPQCppObject(int)\0"
    "p\0getPQCppObject(PQCppObject*)\0"
    "PQCppObjectNoWrap*\0createPQCppObjectNoWrap(int)\0"
    "getPQCppObjectNoWrap(PQCppObjectNoWrap*)\0"
    "PQCppObjectNoWrap\0getPQCppObjectNoWrapAsValue()\0"
    "PQUnknownButRegisteredValueObject\0"
    "getUnknownButRegisteredValueObjectAsValue()\0"
    "PQUnknownValueObject\0"
    "getUnknownValueObjectAsValue()\0"
    "PQUnknownButRegisteredValueObject*\0"
    "getUnknownButRegisteredValueObjectAsPtr()\0"
    "PQUnknownValueObject*\0"
    "getUnknownValueObjectAsPtr()\0ClassA*\0"
    "o\0getClassAPtr(ClassA*)\0ClassB*\0"
    "getClassBPtr(ClassB*)\0ClassC*\0"
    "getClassCPtr(ClassC*)\0ClassD*\0"
    "getClassDPtr(ClassD*)\0createClassA()\0"
    "createClassB()\0createClassC()\0"
    "createClassD()\0createClassCAsA()\0"
    "createClassCAsB()\0createClassDAsA()\0"
    "createClassDAsB()\0setAutoConvertColor(QColor)\0"
    "QBrush\0brush\0setAutoConvertBrush(QBrush)\0"
    "QPen\0pen\0setAutoConvertPen(QPen)\0"
    "QCursor\0cursor\0setAutoConvertCursor(QCursor)\0"
};

const QMetaObject PythonQtTestSlotCallingHelper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PythonQtTestSlotCallingHelper,
      qt_meta_data_PythonQtTestSlotCallingHelper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PythonQtTestSlotCallingHelper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PythonQtTestSlotCallingHelper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PythonQtTestSlotCallingHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQtTestSlotCallingHelper))
        return static_cast<void*>(const_cast< PythonQtTestSlotCallingHelper*>(this));
    return QObject::qt_metacast(_clname);
}

int PythonQtTestSlotCallingHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setPassed(); break;
        case 1: testNoArg(); break;
        case 2: overload((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: overload((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: overload((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: overload((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: overload((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 7: overload((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 8: overload((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: { int _r = getInt((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 10: { uint _r = getUInt((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = _r; }  break;
        case 11: { bool _r = getBool((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: { char _r = getChar((*reinterpret_cast< char(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< char*>(_a[0]) = _r; }  break;
        case 13: { unsigned char _r = getUChar((*reinterpret_cast< unsigned char(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< unsigned char*>(_a[0]) = _r; }  break;
        case 14: { long _r = getLong((*reinterpret_cast< long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< long*>(_a[0]) = _r; }  break;
        case 15: { ulong _r = getULong((*reinterpret_cast< ulong(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< ulong*>(_a[0]) = _r; }  break;
        case 16: { short _r = getShort((*reinterpret_cast< short(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< short*>(_a[0]) = _r; }  break;
        case 17: { unsigned short _r = getUShort((*reinterpret_cast< unsigned short(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< unsigned short*>(_a[0]) = _r; }  break;
        case 18: { QChar _r = getQChar((*reinterpret_cast< QChar(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QChar*>(_a[0]) = _r; }  break;
        case 19: { qint64 _r = getLongLong((*reinterpret_cast< qint64(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = _r; }  break;
        case 20: { quint64 _r = getULongLong((*reinterpret_cast< quint64(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< quint64*>(_a[0]) = _r; }  break;
        case 21: { double _r = getDouble((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 22: { float _r = getFloat((*reinterpret_cast< float(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = _r; }  break;
        case 23: { QString _r = getQString((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 24: { QStringList _r = getQStringList((*reinterpret_cast< const QStringList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 25: { QVariant _r = getQVariant((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 26: { QColor _r = getQColor1((*reinterpret_cast< const QColor(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QColor*>(_a[0]) = _r; }  break;
        case 27: { QColor _r = getQColor2((*reinterpret_cast< QColor(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QColor*>(_a[0]) = _r; }  break;
        case 28: { QColor _r = getQColor3((*reinterpret_cast< QColor*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QColor*>(_a[0]) = _r; }  break;
        case 29: { QColor _r = getQColor4((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QColor*>(_a[0]) = _r; }  break;
        case 30: { QColor* _r = getQColor5();
            if (_a[0]) *reinterpret_cast< QColor**>(_a[0]) = _r; }  break;
        case 31: { PyObject* _r = getPyObject((*reinterpret_cast< PyObject*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PyObject**>(_a[0]) = _r; }  break;
        case 32: { PyObject* _r = getPyObjectFromVariant((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PyObject**>(_a[0]) = _r; }  break;
        case 33: { QVariant _r = getPyObjectFromVariant2((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 34: { PythonQtTestSlotCallingHelper* _r = getTestObject((*reinterpret_cast< PythonQtTestSlotCallingHelper*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PythonQtTestSlotCallingHelper**>(_a[0]) = _r; }  break;
        case 35: { QObject* _r = getQObject((*reinterpret_cast< QObject*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = _r; }  break;
        case 36: { QWidget* _r = getQWidget((*reinterpret_cast< QWidget*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = _r; }  break;
        case 37: { QObject* _r = getNewObject();
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = _r; }  break;
        case 38: { QVariantList _r = getMultiArgs((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 39: { PQCppObject* _r = createPQCppObject((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PQCppObject**>(_a[0]) = _r; }  break;
        case 40: { PQCppObject* _r = getPQCppObject((*reinterpret_cast< PQCppObject*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PQCppObject**>(_a[0]) = _r; }  break;
        case 41: { PQCppObjectNoWrap* _r = createPQCppObjectNoWrap((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PQCppObjectNoWrap**>(_a[0]) = _r; }  break;
        case 42: { PQCppObjectNoWrap* _r = getPQCppObjectNoWrap((*reinterpret_cast< PQCppObjectNoWrap*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PQCppObjectNoWrap**>(_a[0]) = _r; }  break;
        case 43: { PQCppObjectNoWrap _r = getPQCppObjectNoWrapAsValue();
            if (_a[0]) *reinterpret_cast< PQCppObjectNoWrap*>(_a[0]) = _r; }  break;
        case 44: { PQUnknownButRegisteredValueObject _r = getUnknownButRegisteredValueObjectAsValue();
            if (_a[0]) *reinterpret_cast< PQUnknownButRegisteredValueObject*>(_a[0]) = _r; }  break;
        case 45: { PQUnknownValueObject _r = getUnknownValueObjectAsValue();
            if (_a[0]) *reinterpret_cast< PQUnknownValueObject*>(_a[0]) = _r; }  break;
        case 46: { PQUnknownButRegisteredValueObject* _r = getUnknownButRegisteredValueObjectAsPtr();
            if (_a[0]) *reinterpret_cast< PQUnknownButRegisteredValueObject**>(_a[0]) = _r; }  break;
        case 47: { PQUnknownValueObject* _r = getUnknownValueObjectAsPtr();
            if (_a[0]) *reinterpret_cast< PQUnknownValueObject**>(_a[0]) = _r; }  break;
        case 48: { ClassA* _r = getClassAPtr((*reinterpret_cast< ClassA*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< ClassA**>(_a[0]) = _r; }  break;
        case 49: { ClassB* _r = getClassBPtr((*reinterpret_cast< ClassB*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< ClassB**>(_a[0]) = _r; }  break;
        case 50: { ClassC* _r = getClassCPtr((*reinterpret_cast< ClassC*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< ClassC**>(_a[0]) = _r; }  break;
        case 51: { ClassD* _r = getClassDPtr((*reinterpret_cast< ClassD*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< ClassD**>(_a[0]) = _r; }  break;
        case 52: { ClassA* _r = createClassA();
            if (_a[0]) *reinterpret_cast< ClassA**>(_a[0]) = _r; }  break;
        case 53: { ClassB* _r = createClassB();
            if (_a[0]) *reinterpret_cast< ClassB**>(_a[0]) = _r; }  break;
        case 54: { ClassC* _r = createClassC();
            if (_a[0]) *reinterpret_cast< ClassC**>(_a[0]) = _r; }  break;
        case 55: { ClassD* _r = createClassD();
            if (_a[0]) *reinterpret_cast< ClassD**>(_a[0]) = _r; }  break;
        case 56: { ClassA* _r = createClassCAsA();
            if (_a[0]) *reinterpret_cast< ClassA**>(_a[0]) = _r; }  break;
        case 57: { ClassB* _r = createClassCAsB();
            if (_a[0]) *reinterpret_cast< ClassB**>(_a[0]) = _r; }  break;
        case 58: { ClassA* _r = createClassDAsA();
            if (_a[0]) *reinterpret_cast< ClassA**>(_a[0]) = _r; }  break;
        case 59: { ClassB* _r = createClassDAsB();
            if (_a[0]) *reinterpret_cast< ClassB**>(_a[0]) = _r; }  break;
        case 60: { QColor _r = setAutoConvertColor((*reinterpret_cast< const QColor(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QColor*>(_a[0]) = _r; }  break;
        case 61: { QBrush _r = setAutoConvertBrush((*reinterpret_cast< const QBrush(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QBrush*>(_a[0]) = _r; }  break;
        case 62: { QPen _r = setAutoConvertPen((*reinterpret_cast< const QPen(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QPen*>(_a[0]) = _r; }  break;
        case 63: { QCursor _r = setAutoConvertCursor((*reinterpret_cast< const QCursor(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QCursor*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 64;
    }
    return _id;
}
static const uint qt_meta_data_PythonQtTestSignalHandler[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x08,
      42,   26,   26,   26, 0x08,
      62,   26,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PythonQtTestSignalHandler[] = {
    "PythonQtTestSignalHandler\0\0initTestCase()\0"
    "testSignalHandler()\0testRecursiveSignalHandler()\0"
};

const QMetaObject PythonQtTestSignalHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PythonQtTestSignalHandler,
      qt_meta_data_PythonQtTestSignalHandler, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PythonQtTestSignalHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PythonQtTestSignalHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PythonQtTestSignalHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQtTestSignalHandler))
        return static_cast<void*>(const_cast< PythonQtTestSignalHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int PythonQtTestSignalHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: initTestCase(); break;
        case 1: testSignalHandler(); break;
        case 2: testRecursiveSignalHandler(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_PythonQtTestSignalHandlerHelper[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x05,
      48,   32,   32,   32, 0x05,
      69,   67,   32,   32, 0x05,
     103,   93,   32,   32, 0x05,
     154,  149,   32,   32, 0x05,
     193,   32,   32,   32, 0x05,
     206,   32,   32,   32, 0x05,
     223,   32,   32,   32, 0x05,

 // slots: signature, parameters, type, tag, flags
     238,   32,   32,   32, 0x0a,
     257,  255,  250,   32, 0x0a,
     276,  255,  250,   32, 0x0a,
     299,  149,  250,   32, 0x0a,
     342,   67,  250,   32, 0x0a,
     379,   32,  370,   32, 0x0a,
     397,   67,   32,   32, 0x0a,
     426,   93,  250,   32, 0x0a,
     476,  255,  250,   32, 0x0a,
     495,  493,  250,   32, 0x0a,
     516,  255,  250,   32, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PythonQtTestSignalHandlerHelper[] = {
    "PythonQtTestSignalHandlerHelper\0\0"
    "intSignal(int)\0floatSignal(float)\0v\0"
    "variantSignal(QVariant)\0a,b,l,obj\0"
    "complexSignal(int,float,QStringList,QObject*)\0"
    "flag\0enumSignal(PQCppObject2::TestEnumFlag)\0"
    "signal1(int)\0signal2(QString)\0"
    "signal3(float)\0setPassed()\0bool\0a\0"
    "emitIntSignal(int)\0emitFloatSignal(float)\0"
    "emitEnumSignal(PQCppObject2::TestEnumFlag)\0"
    "emitVariantSignal(QVariant)\0QVariant\0"
    "expectedVariant()\0setExpectedVariant(QVariant)\0"
    "emitComplexSignal(int,float,QStringList,QObject*)\0"
    "emitSignal1(int)\0s\0emitSignal2(QString)\0"
    "emitSignal3(float)\0"
};

const QMetaObject PythonQtTestSignalHandlerHelper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PythonQtTestSignalHandlerHelper,
      qt_meta_data_PythonQtTestSignalHandlerHelper, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PythonQtTestSignalHandlerHelper::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PythonQtTestSignalHandlerHelper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PythonQtTestSignalHandlerHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQtTestSignalHandlerHelper))
        return static_cast<void*>(const_cast< PythonQtTestSignalHandlerHelper*>(this));
    return QObject::qt_metacast(_clname);
}

int PythonQtTestSignalHandlerHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: intSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: floatSignal((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: variantSignal((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 3: complexSignal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3])),(*reinterpret_cast< QObject*(*)>(_a[4]))); break;
        case 4: enumSignal((*reinterpret_cast< PQCppObject2::TestEnumFlag(*)>(_a[1]))); break;
        case 5: signal1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: signal2((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: signal3((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 8: setPassed(); break;
        case 9: { bool _r = emitIntSignal((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: { bool _r = emitFloatSignal((*reinterpret_cast< float(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: { bool _r = emitEnumSignal((*reinterpret_cast< PQCppObject2::TestEnumFlag(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 12: { bool _r = emitVariantSignal((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 13: { QVariant _r = expectedVariant();
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 14: setExpectedVariant((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 15: { bool _r = emitComplexSignal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3])),(*reinterpret_cast< QObject*(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 16: { bool _r = emitSignal1((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 17: { bool _r = emitSignal2((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 18: { bool _r = emitSignal3((*reinterpret_cast< float(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void PythonQtTestSignalHandlerHelper::intSignal(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PythonQtTestSignalHandlerHelper::floatSignal(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PythonQtTestSignalHandlerHelper::variantSignal(const QVariant & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PythonQtTestSignalHandlerHelper::complexSignal(int _t1, float _t2, const QStringList & _t3, QObject * _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PythonQtTestSignalHandlerHelper::enumSignal(PQCppObject2::TestEnumFlag _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void PythonQtTestSignalHandlerHelper::signal1(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void PythonQtTestSignalHandlerHelper::signal2(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void PythonQtTestSignalHandlerHelper::signal3(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
