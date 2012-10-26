/****************************************************************************
** Meta object code from reading C++ file 'ercp2v2.h'
**
** Created: Fri Oct 26 20:25:05 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ercp2v2.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ercp2v2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ERCP2v2[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,    9,    8,    8, 0x0a,
      48,   44,    8,    8, 0x0a,
      77,    9,    8,    8, 0x0a,
     106,   44,    8,    8, 0x0a,
     137,    8,    8,    8, 0x0a,
     151,    8,    8,    8, 0x0a,
     164,    8,    8,    8, 0x0a,
     176,    8,    8,    8, 0x0a,
     193,    8,    8,    8, 0x0a,
     221,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ERCP2v2[] = {
    "ERCP2v2\0\0pos\0updateUICamPosition(glm::vec3)\0"
    "ang\0updateUIObjAngles(glm::vec3)\0"
    "updateUICamAngles(glm::vec3)\0"
    "updateUIObjPosition(glm::vec3)\0"
    "resetCamera()\0resetModel()\0objOrigin()\0"
    "resetThirdView()\0enableCaculateCalibration()\0"
    "pushButtonCalculateCalibrationClicked()\0"
};

void ERCP2v2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ERCP2v2 *_t = static_cast<ERCP2v2 *>(_o);
        switch (_id) {
        case 0: _t->updateUICamPosition((*reinterpret_cast< glm::vec3(*)>(_a[1]))); break;
        case 1: _t->updateUIObjAngles((*reinterpret_cast< glm::vec3(*)>(_a[1]))); break;
        case 2: _t->updateUICamAngles((*reinterpret_cast< glm::vec3(*)>(_a[1]))); break;
        case 3: _t->updateUIObjPosition((*reinterpret_cast< glm::vec3(*)>(_a[1]))); break;
        case 4: _t->resetCamera(); break;
        case 5: _t->resetModel(); break;
        case 6: _t->objOrigin(); break;
        case 7: _t->resetThirdView(); break;
        case 8: _t->enableCaculateCalibration(); break;
        case 9: _t->pushButtonCalculateCalibrationClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ERCP2v2::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ERCP2v2::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ERCP2v2,
      qt_meta_data_ERCP2v2, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ERCP2v2::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ERCP2v2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ERCP2v2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ERCP2v2))
        return static_cast<void*>(const_cast< ERCP2v2*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ERCP2v2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
