/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[21];
    char stringdata0[440];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 13), // "replyFinished"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 14), // "QNetworkReply*"
QT_MOC_LITERAL(4, 41, 5), // "reply"
QT_MOC_LITERAL(5, 47, 25), // "on_treeView_doubleClicked"
QT_MOC_LITERAL(6, 73, 5), // "index"
QT_MOC_LITERAL(7, 79, 23), // "on_nonMVPNCheck_clicked"
QT_MOC_LITERAL(8, 103, 7), // "checked"
QT_MOC_LITERAL(9, 111, 22), // "on_debugButton_clicked"
QT_MOC_LITERAL(10, 134, 19), // "on_treeView_clicked"
QT_MOC_LITERAL(11, 154, 21), // "on_snmp3Check_clicked"
QT_MOC_LITERAL(12, 176, 28), // "on_refreshOrgsButton_clicked"
QT_MOC_LITERAL(13, 205, 27), // "on_tabWidget_currentChanged"
QT_MOC_LITERAL(14, 233, 26), // "on_adminsTableView_clicked"
QT_MOC_LITERAL(15, 260, 39), // "on_nonMVPNPeersMenu_currentIn..."
QT_MOC_LITERAL(16, 300, 29), // "on_nonMVPNSecretCheck_clicked"
QT_MOC_LITERAL(17, 330, 28), // "on_snmpAuthPassCheck_clicked"
QT_MOC_LITERAL(18, 359, 28), // "on_snmpPrivPassCheck_clicked"
QT_MOC_LITERAL(19, 388, 26), // "on_msSwitchesTable_clicked"
QT_MOC_LITERAL(20, 415, 24) // "on_mxDeviceTable_clicked"

    },
    "MainWindow\0replyFinished\0\0QNetworkReply*\0"
    "reply\0on_treeView_doubleClicked\0index\0"
    "on_nonMVPNCheck_clicked\0checked\0"
    "on_debugButton_clicked\0on_treeView_clicked\0"
    "on_snmp3Check_clicked\0"
    "on_refreshOrgsButton_clicked\0"
    "on_tabWidget_currentChanged\0"
    "on_adminsTableView_clicked\0"
    "on_nonMVPNPeersMenu_currentIndexChanged\0"
    "on_nonMVPNSecretCheck_clicked\0"
    "on_snmpAuthPassCheck_clicked\0"
    "on_snmpPrivPassCheck_clicked\0"
    "on_msSwitchesTable_clicked\0"
    "on_mxDeviceTable_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x0a /* Public */,
       5,    1,   92,    2, 0x08 /* Private */,
       7,    1,   95,    2, 0x08 /* Private */,
       9,    0,   98,    2, 0x08 /* Private */,
      10,    1,   99,    2, 0x08 /* Private */,
      11,    1,  102,    2, 0x08 /* Private */,
      12,    0,  105,    2, 0x08 /* Private */,
      13,    1,  106,    2, 0x08 /* Private */,
      14,    1,  109,    2, 0x08 /* Private */,
      15,    1,  112,    2, 0x08 /* Private */,
      16,    1,  115,    2, 0x08 /* Private */,
      17,    1,  118,    2, 0x08 /* Private */,
      18,    1,  121,    2, 0x08 /* Private */,
      19,    1,  124,    2, 0x08 /* Private */,
      20,    1,  127,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QModelIndex,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::QModelIndex,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::QModelIndex,    6,
    QMetaType::Void, QMetaType::QModelIndex,    6,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 1: _t->on_treeView_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->on_nonMVPNCheck_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->on_debugButton_clicked(); break;
        case 4: _t->on_treeView_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: _t->on_snmp3Check_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_refreshOrgsButton_clicked(); break;
        case 7: _t->on_tabWidget_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_adminsTableView_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 9: _t->on_nonMVPNPeersMenu_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->on_nonMVPNSecretCheck_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->on_snmpAuthPassCheck_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->on_snmpPrivPassCheck_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->on_msSwitchesTable_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 14: _t->on_mxDeviceTable_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
