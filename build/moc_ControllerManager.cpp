/****************************************************************************
** Meta object code from reading C++ file 'ControllerManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../src/headers/ControllerManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ControllerManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ControllerManager_t {
    QByteArrayData data[14];
    char stringdata0[169];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ControllerManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ControllerManager_t qt_meta_stringdata_ControllerManager = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ControllerManager"
QT_MOC_LITERAL(1, 18, 19), // "controllerConnected"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 4), // "name"
QT_MOC_LITERAL(4, 44, 22), // "controllerDisconnected"
QT_MOC_LITERAL(5, 67, 13), // "buttonPressed"
QT_MOC_LITERAL(6, 81, 6), // "button"
QT_MOC_LITERAL(7, 88, 9), // "axisMoved"
QT_MOC_LITERAL(8, 98, 4), // "axis"
QT_MOC_LITERAL(9, 103, 5), // "value"
QT_MOC_LITERAL(10, 109, 14), // "buttonAPressed"
QT_MOC_LITERAL(11, 124, 14), // "buttonBPressed"
QT_MOC_LITERAL(12, 139, 14), // "buttonXPressed"
QT_MOC_LITERAL(13, 154, 14) // "buttonYPressed"

    },
    "ControllerManager\0controllerConnected\0"
    "\0name\0controllerDisconnected\0buttonPressed\0"
    "button\0axisMoved\0axis\0value\0buttonAPressed\0"
    "buttonBPressed\0buttonXPressed\0"
    "buttonYPressed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ControllerManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    0,   57,    2, 0x06 /* Public */,
       5,    1,   58,    2, 0x06 /* Public */,
       7,    2,   61,    2, 0x06 /* Public */,
      10,    0,   66,    2, 0x06 /* Public */,
      11,    0,   67,    2, 0x06 /* Public */,
      12,    0,   68,    2, 0x06 /* Public */,
      13,    0,   69,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ControllerManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ControllerManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->controllerConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->controllerDisconnected(); break;
        case 2: _t->buttonPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->axisMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->buttonAPressed(); break;
        case 5: _t->buttonBPressed(); break;
        case 6: _t->buttonXPressed(); break;
        case 7: _t->buttonYPressed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ControllerManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControllerManager::controllerConnected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ControllerManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControllerManager::controllerDisconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ControllerManager::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControllerManager::buttonPressed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ControllerManager::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControllerManager::axisMoved)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ControllerManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControllerManager::buttonAPressed)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ControllerManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControllerManager::buttonBPressed)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ControllerManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControllerManager::buttonXPressed)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ControllerManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ControllerManager::buttonYPressed)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ControllerManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ControllerManager.data,
    qt_meta_data_ControllerManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ControllerManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ControllerManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ControllerManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ControllerManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ControllerManager::controllerConnected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ControllerManager::controllerDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ControllerManager::buttonPressed(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ControllerManager::axisMoved(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ControllerManager::buttonAPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ControllerManager::buttonBPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ControllerManager::buttonXPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ControllerManager::buttonYPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
