/********************************************************************************
** Form generated from reading UI file 'ercp2v2.ui'
**
** Created: Thu Oct 25 22:35:32 2012
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERCP2V2_H
#define UI_ERCP2V2_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include "openglpanel.h"

QT_BEGIN_NAMESPACE

class Ui_ERCP2v2Class
{
public:
    QWidget *centralWidget;
    OpenglPanel *widget;
    QGroupBox *groupBox;
    QDoubleSpinBox *doubleSpinBoxCamPosX;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxCamPosY;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxCamPosZ;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxCamAngX;
    QLabel *label_4;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxCamAngY;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxCamAngZ;
    QGroupBox *groupBox_2;
    QDoubleSpinBox *doubleSpinBoxObjPosX;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBoxObjPosY;
    QLabel *label_8;
    QDoubleSpinBox *doubleSpinBoxObjPosZ;
    QLabel *label_9;
    QDoubleSpinBox *doubleSpinBoxObjAngX;
    QLabel *label_10;
    QLabel *label_11;
    QDoubleSpinBox *doubleSpinBoxObjAngY;
    QLabel *label_12;
    QDoubleSpinBox *doubleSpinBoxObjAngZ;
    QGroupBox *groupBox_3;
    QDoubleSpinBox *doubleSpinBoxFovy;
    QLabel *label_13;
    QDoubleSpinBox *doubleSpinBoxNear;
    QLabel *label_14;
    QDoubleSpinBox *doubleSpinBoxFar;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QSpinBox *spinBoxWidth;
    QSpinBox *spinBoxHeight;
    QPushButton *pushButton;
    QGroupBox *groupBox_4;
    QPushButton *pushButtonResetCamera;
    QGroupBox *groupBox_5;
    QPushButton *pushButtonResetModelMatrix;
    QGroupBox *groupBox_6;
    QPushButton *pushButtonInitialization;
    QPushButton *pushButtonStartTracking;
    QPushButton *pushButtonReference;
    QSpinBox *spinBox;
    QPushButton *pushButtonResetTracking;
    QGroupBox *groupBox_7;
    QPushButton *pushButtonCalibration;
    QSpinBox *spinBoxNpointsCalibration;
    QLabel *label_18;
    QPushButton *pushButtonCalculateCalibration;
    QTextEdit *textEditCommand;
    QPushButton *pushButtonTestOptimization;
    QPushButton *pushButtonTestMI;
    QPushButton *pushButtonTestManualTracking;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ERCP2v2Class)
    {
        if (ERCP2v2Class->objectName().isEmpty())
            ERCP2v2Class->setObjectName(QString::fromUtf8("ERCP2v2Class"));
        ERCP2v2Class->resize(1387, 881);
        QIcon icon;
        icon.addFile(QString::fromUtf8("data/n2t.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        ERCP2v2Class->setWindowIcon(icon);
        centralWidget = new QWidget(ERCP2v2Class);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        widget = new OpenglPanel(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(30, 20, 626, 626));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(820, 30, 171, 241));
        doubleSpinBoxCamPosX = new QDoubleSpinBox(groupBox);
        doubleSpinBoxCamPosX->setObjectName(QString::fromUtf8("doubleSpinBoxCamPosX"));
        doubleSpinBoxCamPosX->setGeometry(QRect(50, 30, 91, 22));
        doubleSpinBoxCamPosX->setDecimals(5);
        doubleSpinBoxCamPosX->setMinimum(-10000);
        doubleSpinBoxCamPosX->setMaximum(10000);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 21, 16));
        doubleSpinBoxCamPosY = new QDoubleSpinBox(groupBox);
        doubleSpinBoxCamPosY->setObjectName(QString::fromUtf8("doubleSpinBoxCamPosY"));
        doubleSpinBoxCamPosY->setGeometry(QRect(50, 60, 91, 22));
        doubleSpinBoxCamPosY->setDecimals(5);
        doubleSpinBoxCamPosY->setMinimum(-10000);
        doubleSpinBoxCamPosY->setMaximum(10000);
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 60, 21, 16));
        doubleSpinBoxCamPosZ = new QDoubleSpinBox(groupBox);
        doubleSpinBoxCamPosZ->setObjectName(QString::fromUtf8("doubleSpinBoxCamPosZ"));
        doubleSpinBoxCamPosZ->setGeometry(QRect(50, 90, 91, 22));
        doubleSpinBoxCamPosZ->setDecimals(5);
        doubleSpinBoxCamPosZ->setMinimum(-10000);
        doubleSpinBoxCamPosZ->setMaximum(10000);
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 90, 21, 16));
        doubleSpinBoxCamAngX = new QDoubleSpinBox(groupBox);
        doubleSpinBoxCamAngX->setObjectName(QString::fromUtf8("doubleSpinBoxCamAngX"));
        doubleSpinBoxCamAngX->setGeometry(QRect(50, 130, 91, 22));
        doubleSpinBoxCamAngX->setDecimals(5);
        doubleSpinBoxCamAngX->setMinimum(-10000);
        doubleSpinBoxCamAngX->setMaximum(10000);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 130, 21, 20));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(20, 170, 21, 20));
        doubleSpinBoxCamAngY = new QDoubleSpinBox(groupBox);
        doubleSpinBoxCamAngY->setObjectName(QString::fromUtf8("doubleSpinBoxCamAngY"));
        doubleSpinBoxCamAngY->setGeometry(QRect(50, 170, 91, 22));
        doubleSpinBoxCamAngY->setDecimals(5);
        doubleSpinBoxCamAngY->setMinimum(-10000);
        doubleSpinBoxCamAngY->setMaximum(10000);
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 200, 21, 20));
        doubleSpinBoxCamAngZ = new QDoubleSpinBox(groupBox);
        doubleSpinBoxCamAngZ->setObjectName(QString::fromUtf8("doubleSpinBoxCamAngZ"));
        doubleSpinBoxCamAngZ->setGeometry(QRect(50, 200, 91, 22));
        doubleSpinBoxCamAngZ->setDecimals(5);
        doubleSpinBoxCamAngZ->setMinimum(-10000);
        doubleSpinBoxCamAngZ->setMaximum(10000);
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(820, 290, 171, 241));
        doubleSpinBoxObjPosX = new QDoubleSpinBox(groupBox_2);
        doubleSpinBoxObjPosX->setObjectName(QString::fromUtf8("doubleSpinBoxObjPosX"));
        doubleSpinBoxObjPosX->setGeometry(QRect(50, 30, 91, 22));
        doubleSpinBoxObjPosX->setDecimals(5);
        doubleSpinBoxObjPosX->setMinimum(-10000);
        doubleSpinBoxObjPosX->setMaximum(10000);
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(20, 30, 21, 16));
        doubleSpinBoxObjPosY = new QDoubleSpinBox(groupBox_2);
        doubleSpinBoxObjPosY->setObjectName(QString::fromUtf8("doubleSpinBoxObjPosY"));
        doubleSpinBoxObjPosY->setGeometry(QRect(50, 60, 91, 22));
        doubleSpinBoxObjPosY->setDecimals(5);
        doubleSpinBoxObjPosY->setMinimum(-10000);
        doubleSpinBoxObjPosY->setMaximum(10000);
        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 60, 21, 16));
        doubleSpinBoxObjPosZ = new QDoubleSpinBox(groupBox_2);
        doubleSpinBoxObjPosZ->setObjectName(QString::fromUtf8("doubleSpinBoxObjPosZ"));
        doubleSpinBoxObjPosZ->setGeometry(QRect(50, 90, 91, 22));
        doubleSpinBoxObjPosZ->setDecimals(5);
        doubleSpinBoxObjPosZ->setMinimum(-10000);
        doubleSpinBoxObjPosZ->setMaximum(10000);
        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(20, 90, 21, 16));
        doubleSpinBoxObjAngX = new QDoubleSpinBox(groupBox_2);
        doubleSpinBoxObjAngX->setObjectName(QString::fromUtf8("doubleSpinBoxObjAngX"));
        doubleSpinBoxObjAngX->setGeometry(QRect(50, 130, 91, 22));
        doubleSpinBoxObjAngX->setDecimals(5);
        doubleSpinBoxObjAngX->setMinimum(-10000);
        doubleSpinBoxObjAngX->setMaximum(10000);
        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(20, 130, 21, 20));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(20, 170, 21, 20));
        doubleSpinBoxObjAngY = new QDoubleSpinBox(groupBox_2);
        doubleSpinBoxObjAngY->setObjectName(QString::fromUtf8("doubleSpinBoxObjAngY"));
        doubleSpinBoxObjAngY->setGeometry(QRect(50, 170, 91, 22));
        doubleSpinBoxObjAngY->setDecimals(5);
        doubleSpinBoxObjAngY->setMinimum(-10000);
        doubleSpinBoxObjAngY->setMaximum(10000);
        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(20, 200, 21, 20));
        doubleSpinBoxObjAngZ = new QDoubleSpinBox(groupBox_2);
        doubleSpinBoxObjAngZ->setObjectName(QString::fromUtf8("doubleSpinBoxObjAngZ"));
        doubleSpinBoxObjAngZ->setGeometry(QRect(50, 200, 91, 22));
        doubleSpinBoxObjAngZ->setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
        doubleSpinBoxObjAngZ->setDecimals(5);
        doubleSpinBoxObjAngZ->setMinimum(-10000);
        doubleSpinBoxObjAngZ->setMaximum(10000);
        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(820, 550, 171, 211));
        doubleSpinBoxFovy = new QDoubleSpinBox(groupBox_3);
        doubleSpinBoxFovy->setObjectName(QString::fromUtf8("doubleSpinBoxFovy"));
        doubleSpinBoxFovy->setGeometry(QRect(50, 30, 91, 22));
        label_13 = new QLabel(groupBox_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(10, 30, 31, 16));
        doubleSpinBoxNear = new QDoubleSpinBox(groupBox_3);
        doubleSpinBoxNear->setObjectName(QString::fromUtf8("doubleSpinBoxNear"));
        doubleSpinBoxNear->setGeometry(QRect(50, 60, 91, 22));
        doubleSpinBoxNear->setMinimum(0.01);
        label_14 = new QLabel(groupBox_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(10, 60, 21, 16));
        doubleSpinBoxFar = new QDoubleSpinBox(groupBox_3);
        doubleSpinBoxFar->setObjectName(QString::fromUtf8("doubleSpinBoxFar"));
        doubleSpinBoxFar->setGeometry(QRect(50, 90, 91, 22));
        doubleSpinBoxFar->setMaximum(10000);
        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(10, 90, 21, 16));
        label_16 = new QLabel(groupBox_3);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(10, 130, 31, 20));
        label_17 = new QLabel(groupBox_3);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(10, 170, 31, 20));
        spinBoxWidth = new QSpinBox(groupBox_3);
        spinBoxWidth->setObjectName(QString::fromUtf8("spinBoxWidth"));
        spinBoxWidth->setGeometry(QRect(50, 130, 91, 22));
        spinBoxWidth->setMaximum(3000);
        spinBoxHeight = new QSpinBox(groupBox_3);
        spinBoxHeight->setObjectName(QString::fromUtf8("spinBoxHeight"));
        spinBoxHeight->setGeometry(QRect(50, 170, 91, 22));
        spinBoxHeight->setMaximum(3000);
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(1190, 790, 75, 23));
        groupBox_4 = new QGroupBox(centralWidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(1010, 30, 101, 231));
        pushButtonResetCamera = new QPushButton(groupBox_4);
        pushButtonResetCamera->setObjectName(QString::fromUtf8("pushButtonResetCamera"));
        pushButtonResetCamera->setGeometry(QRect(10, 20, 81, 23));
        groupBox_5 = new QGroupBox(centralWidget);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(1010, 290, 101, 211));
        pushButtonResetModelMatrix = new QPushButton(groupBox_5);
        pushButtonResetModelMatrix->setObjectName(QString::fromUtf8("pushButtonResetModelMatrix"));
        pushButtonResetModelMatrix->setGeometry(QRect(10, 20, 75, 23));
        groupBox_6 = new QGroupBox(centralWidget);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(1020, 550, 151, 191));
        pushButtonInitialization = new QPushButton(groupBox_6);
        pushButtonInitialization->setObjectName(QString::fromUtf8("pushButtonInitialization"));
        pushButtonInitialization->setGeometry(QRect(10, 70, 121, 23));
        pushButtonStartTracking = new QPushButton(groupBox_6);
        pushButtonStartTracking->setObjectName(QString::fromUtf8("pushButtonStartTracking"));
        pushButtonStartTracking->setGeometry(QRect(10, 110, 121, 23));
        pushButtonReference = new QPushButton(groupBox_6);
        pushButtonReference->setObjectName(QString::fromUtf8("pushButtonReference"));
        pushButtonReference->setGeometry(QRect(10, 30, 75, 23));
        spinBox = new QSpinBox(groupBox_6);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(100, 30, 42, 22));
        spinBox->setMinimum(1);
        spinBox->setMaximum(3);
        pushButtonResetTracking = new QPushButton(groupBox_6);
        pushButtonResetTracking->setObjectName(QString::fromUtf8("pushButtonResetTracking"));
        pushButtonResetTracking->setGeometry(QRect(10, 150, 121, 23));
        groupBox_7 = new QGroupBox(centralWidget);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(1190, 560, 161, 181));
        pushButtonCalibration = new QPushButton(groupBox_7);
        pushButtonCalibration->setObjectName(QString::fromUtf8("pushButtonCalibration"));
        pushButtonCalibration->setGeometry(QRect(10, 20, 141, 23));
        spinBoxNpointsCalibration = new QSpinBox(groupBox_7);
        spinBoxNpointsCalibration->setObjectName(QString::fromUtf8("spinBoxNpointsCalibration"));
        spinBoxNpointsCalibration->setGeometry(QRect(91, 50, 51, 22));
        spinBoxNpointsCalibration->setMinimum(5);
        spinBoxNpointsCalibration->setSingleStep(10);
        spinBoxNpointsCalibration->setValue(10);
        label_18 = new QLabel(groupBox_7);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(20, 50, 46, 13));
        pushButtonCalculateCalibration = new QPushButton(groupBox_7);
        pushButtonCalculateCalibration->setObjectName(QString::fromUtf8("pushButtonCalculateCalibration"));
        pushButtonCalculateCalibration->setEnabled(false);
        pushButtonCalculateCalibration->setGeometry(QRect(10, 80, 141, 23));
        textEditCommand = new QTextEdit(centralWidget);
        textEditCommand->setObjectName(QString::fromUtf8("textEditCommand"));
        textEditCommand->setGeometry(QRect(1140, 30, 211, 231));
        pushButtonTestOptimization = new QPushButton(centralWidget);
        pushButtonTestOptimization->setObjectName(QString::fromUtf8("pushButtonTestOptimization"));
        pushButtonTestOptimization->setGeometry(QRect(1150, 300, 201, 23));
        pushButtonTestMI = new QPushButton(centralWidget);
        pushButtonTestMI->setObjectName(QString::fromUtf8("pushButtonTestMI"));
        pushButtonTestMI->setGeometry(QRect(1150, 330, 201, 23));
        pushButtonTestManualTracking = new QPushButton(centralWidget);
        pushButtonTestManualTracking->setObjectName(QString::fromUtf8("pushButtonTestManualTracking"));
        pushButtonTestManualTracking->setGeometry(QRect(1150, 360, 201, 23));
        ERCP2v2Class->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ERCP2v2Class);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1387, 21));
        ERCP2v2Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ERCP2v2Class);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ERCP2v2Class->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ERCP2v2Class);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ERCP2v2Class->setStatusBar(statusBar);

        retranslateUi(ERCP2v2Class);
        QObject::connect(pushButtonResetCamera, SIGNAL(clicked()), ERCP2v2Class, SLOT(resetCamera()));
        QObject::connect(pushButtonResetModelMatrix, SIGNAL(clicked()), ERCP2v2Class, SLOT(resetModel()));
        QObject::connect(pushButton, SIGNAL(clicked()), ERCP2v2Class, SLOT(close()));
        QObject::connect(pushButtonCalibration, SIGNAL(clicked()), widget, SLOT(startManualCalibration()));
        QObject::connect(spinBoxNpointsCalibration, SIGNAL(valueChanged(int)), widget, SLOT(setNumberOfPoints(int)));
        QObject::connect(widget, SIGNAL(finishSelectingPoints()), ERCP2v2Class, SLOT(enableCaculateCalibration()));
        QObject::connect(pushButtonCalculateCalibration, SIGNAL(clicked()), ERCP2v2Class, SLOT(pushButtonCalculateCalibrationClicked()));
        QObject::connect(pushButtonCalculateCalibration, SIGNAL(clicked()), widget, SLOT(calculateCalibration()));
        QObject::connect(ERCP2v2Class, SIGNAL(saveOpenGLImage()), widget, SLOT(repaint()));
        QObject::connect(pushButtonTestOptimization, SIGNAL(clicked()), widget, SLOT(testOptimization()));
        QObject::connect(pushButtonTestMI, SIGNAL(released()), widget, SLOT(testNewMutualInformation()));
        QObject::connect(pushButtonTestManualTracking, SIGNAL(clicked()), widget, SLOT(testManualTracking()));

        QMetaObject::connectSlotsByName(ERCP2v2Class);
    } // setupUi

    void retranslateUi(QMainWindow *ERCP2v2Class)
    {
        ERCP2v2Class->setWindowTitle(QApplication::translate("ERCP2v2Class", "ERCP2v2", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ERCP2v2Class", "Camera Transform", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ERCP2v2Class", "X", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ERCP2v2Class", "Y", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ERCP2v2Class", "Z", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ERCP2v2Class", "RX", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ERCP2v2Class", "RY", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("ERCP2v2Class", "RZ", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("ERCP2v2Class", "Object Transform", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("ERCP2v2Class", "X", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("ERCP2v2Class", "Y", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("ERCP2v2Class", "Z", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("ERCP2v2Class", "RX", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("ERCP2v2Class", "RY", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("ERCP2v2Class", "RZ", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("ERCP2v2Class", "View Port", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("ERCP2v2Class", "Fovy", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("ERCP2v2Class", "Near", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("ERCP2v2Class", "Far", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("ERCP2v2Class", "width", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("ERCP2v2Class", "height", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("ERCP2v2Class", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("ERCP2v2Class", "Camera Control", 0, QApplication::UnicodeUTF8));
        pushButtonResetCamera->setText(QApplication::translate("ERCP2v2Class", "Reset Camera", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("ERCP2v2Class", "Object Control", 0, QApplication::UnicodeUTF8));
        pushButtonResetModelMatrix->setText(QApplication::translate("ERCP2v2Class", "Reset Model", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("ERCP2v2Class", "Main Control", 0, QApplication::UnicodeUTF8));
        pushButtonInitialization->setText(QApplication::translate("ERCP2v2Class", "Initialization", 0, QApplication::UnicodeUTF8));
        pushButtonStartTracking->setText(QApplication::translate("ERCP2v2Class", "Start Tracking", 0, QApplication::UnicodeUTF8));
        pushButtonReference->setText(QApplication::translate("ERCP2v2Class", "Reference", 0, QApplication::UnicodeUTF8));
        pushButtonResetTracking->setText(QApplication::translate("ERCP2v2Class", "Reset Tracking", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("ERCP2v2Class", "Ultilities", 0, QApplication::UnicodeUTF8));
        pushButtonCalibration->setText(QApplication::translate("ERCP2v2Class", "Camera Calibration", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("ERCP2v2Class", "N Points", 0, QApplication::UnicodeUTF8));
        pushButtonCalculateCalibration->setText(QApplication::translate("ERCP2v2Class", "Calculate", 0, QApplication::UnicodeUTF8));
        pushButtonTestOptimization->setText(QApplication::translate("ERCP2v2Class", "Test Optimization", 0, QApplication::UnicodeUTF8));
        pushButtonTestMI->setText(QApplication::translate("ERCP2v2Class", "Test New Mutual Information", 0, QApplication::UnicodeUTF8));
        pushButtonTestManualTracking->setText(QApplication::translate("ERCP2v2Class", "Test Manual Tracking", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ERCP2v2Class: public Ui_ERCP2v2Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ERCP2V2_H
