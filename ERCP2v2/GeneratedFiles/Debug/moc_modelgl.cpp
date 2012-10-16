/****************************************************************************
** Meta object code from reading C++ file 'modelgl.h'
**
** Created: Tue Oct 16 20:55:06 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../modelgl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modelgl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ModelGL[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,    9,    8,    8, 0x05,
      31,   29,    8,    8, 0x05,
      54,   50,    8,    8, 0x05,
      86,   84,    8,    8, 0x05,
     116,  114,    8,    8, 0x05,
     146,  144,    8,    8, 0x05,
     177,  174,    8,    8, 0x05,
     205,  202,    8,    8, 0x05,
     233,  230,    8,    8, 0x05,
     265,  258,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     292,  174,    8,    8, 0x0a,
     316,  202,    8,    8, 0x0a,
     340,  230,    8,    8, 0x0a,
     364,   84,    8,    8, 0x0a,
     391,  114,    8,    8, 0x0a,
     418,  144,    8,    8, 0x0a,
     445,   84,    8,    8, 0x0a,
     469,  114,    8,    8, 0x0a,
     493,  144,    8,    8, 0x0a,
     517,  174,    8,    8, 0x0a,
     538,  202,    8,    8, 0x0a,
     559,  230,    8,    8, 0x0a,
     585,  580,    8,    8, 0x0a,
     609,  607,    8,    8, 0x0a,
     633,  631,    8,    8, 0x0a,
     660,  654,    8,    8, 0x0a,
     681,  674,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ModelGL[] = {
    "ModelGL\0\0w\0widthChanged(int)\0h\0"
    "heightChanged(int)\0pos\0"
    "camPositionChanged(glm::vec3)\0x\0"
    "camPositionXChanged(double)\0y\0"
    "camPositionYChanged(double)\0z\0"
    "camPositionZChanged(double)\0rx\0"
    "camAngleXChanged(double)\0ry\0"
    "camAngleYChanged(double)\0rz\0"
    "camAngleZChanged(double)\0angles\0"
    "objAngleChanged(glm::vec3)\0"
    "setCameraAngleX(double)\0setCameraAngleY(double)\0"
    "setCameraAngleZ(double)\0"
    "setCameraPositionX(double)\0"
    "setCameraPositionY(double)\0"
    "setCameraPositionZ(double)\0"
    "setObjPositionX(double)\0setObjPositionY(double)\0"
    "setObjPositionZ(double)\0setObjAngleX(double)\0"
    "setObjAngleY(double)\0setObjAngleZ(double)\0"
    "fovy\0setCameraFovy(double)\0n\0"
    "setCameraNear(double)\0f\0setCameraFar(double)\0"
    "width\0setWidth(int)\0height\0setHeight(int)\0"
};

void ModelGL::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ModelGL *_t = static_cast<ModelGL *>(_o);
        switch (_id) {
        case 0: _t->widthChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->heightChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->camPositionChanged((*reinterpret_cast< glm::vec3(*)>(_a[1]))); break;
        case 3: _t->camPositionXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->camPositionYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->camPositionZChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->camAngleXChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->camAngleYChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->camAngleZChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->objAngleChanged((*reinterpret_cast< glm::vec3(*)>(_a[1]))); break;
        case 10: _t->setCameraAngleX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->setCameraAngleY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->setCameraAngleZ((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->setCameraPositionX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->setCameraPositionY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->setCameraPositionZ((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 16: _t->setObjPositionX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->setObjPositionY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->setObjPositionZ((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 19: _t->setObjAngleX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 20: _t->setObjAngleY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 21: _t->setObjAngleZ((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 22: _t->setCameraFovy((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 23: _t->setCameraNear((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 24: _t->setCameraFar((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 25: _t->setWidth((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: _t->setHeight((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ModelGL::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ModelGL::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ModelGL,
      qt_meta_data_ModelGL, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ModelGL::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ModelGL::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ModelGL::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModelGL))
        return static_cast<void*>(const_cast< ModelGL*>(this));
    return QObject::qt_metacast(_clname);
}

int ModelGL::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void ModelGL::widthChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ModelGL::heightChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ModelGL::camPositionChanged(glm::vec3 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ModelGL::camPositionXChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ModelGL::camPositionYChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ModelGL::camPositionZChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ModelGL::camAngleXChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ModelGL::camAngleYChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ModelGL::camAngleZChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ModelGL::objAngleChanged(glm::vec3 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE
