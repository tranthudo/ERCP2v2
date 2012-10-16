/****************************************************************************
** Meta object code from reading C++ file 'openglpanel.h'
**
** Created: Tue Oct 16 22:00:16 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../openglpanel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'openglpanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OpenglPanel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      40,   38,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_OpenglPanel[] = {
    "OpenglPanel\0\0startManualCalibration()\0"
    "n\0setNumberOfPoints(int)\0"
};

void OpenglPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        OpenglPanel *_t = static_cast<OpenglPanel *>(_o);
        switch (_id) {
        case 0: _t->startManualCalibration(); break;
        case 1: _t->setNumberOfPoints((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData OpenglPanel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject OpenglPanel::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_OpenglPanel,
      qt_meta_data_OpenglPanel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OpenglPanel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OpenglPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OpenglPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OpenglPanel))
        return static_cast<void*>(const_cast< OpenglPanel*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int OpenglPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
