/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QPushButton *refreshOrgsButton;
    QTabWidget *tabWidget;
    QWidget *tab;
    QLabel *label_2;
    QLineEdit *orgNameEdit;
    QPushButton *adPushUpdatesButton;
    QPushButton *adSnapshotButton;
    QLabel *label_19;
    QLineEdit *networkNameEdit;
    QPushButton *debugButton;
    QTabWidget *adminTabWidget;
    QWidget *tab_27;
    QTableView *adminsTableView_3;
    QTableView *adminsTableView_2;
    QTableView *adminsTableView;
    QWidget *tab_28;
    QWidget *tab_17;
    QLineEdit *nonMVPNPeerNameEdit;
    QLineEdit *snmpPrivPassEdit;
    QCheckBox *snmpPrivPassCheck;
    QLineEdit *snmpAuthPassEdit;
    QCheckBox *snmp3Check;
    QCheckBox *snmp2cCheck;
    QLabel *label_18;
    QLineEdit *nonMVPNPeerIPEdit;
    QPushButton *nonMVPNAddPeerButton;
    QComboBox *nonMVPNPeersMenu;
    QComboBox *snmpAuthMenu;
    QTextBrowser *snmpPeerIPListBrowser;
    QLabel *label_6;
    QLineEdit *nonMVPNSecretEdit;
    QCheckBox *snmpAuthPassCheck;
    QLabel *label_7;
    QCheckBox *nonMVPNSecretCheck;
    QLabel *label_8;
    QLabel *label_17;
    QLabel *label_16;
    QTextBrowser *nonMVPNSubnetBrowser;
    QCheckBox *nonMVPNCheck;
    QComboBox *snmpPrivMenu;
    QLabel *label_26;
    QLabel *snmpHostnameLabel;
    QLabel *label_40;
    QLabel *label_41;
    QLabel *snmpPortLabel;
    QLabel *snmpUserLabel;
    QLabel *snmpCommStringLabel;
    QLabel *label_42;
    QLabel *label_43;
    QTextBrowser *nonMVPNTagsBrowser;
    QWidget *tab_15;
    QLabel *licenseExpDateLabel;
    QLabel *label_4;
    QTextBrowser *licenseDevicesBrowser;
    QLabel *label_5;
    QPushButton *nonMVPNAddPeerButton_2;
    QLineEdit *nonMVPNPeerNameEdit_2;
    QLabel *licenseStateLabel;
    QLabel *label_3;
    QTableView *orgInventoryView;
    QLabel *label_20;
    QLineEdit *orgNameEdit_2;
    QPushButton *nonMVPNAddPeerButton_3;
    QPushButton *refreshOrgsButton_2;
    QLabel *label_44;
    QTextBrowser *missingLicensesBrowser;
    QWidget *tab_3;
    QLabel *label_45;
    QLineEdit *msNetworkEdit;
    QTableView *msSwitchesTable;
    QTableView *msSwitchPortsTable;
    QPushButton *msRefreshButton;
    QWidget *tab_2;
    QWidget *tab_4;
    QWidget *tab_5;
    QWidget *tab_6;
    QWidget *tab_7;
    QTreeView *treeView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuAbout;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(900, 564);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 3, 121, 20));
        refreshOrgsButton = new QPushButton(centralWidget);
        refreshOrgsButton->setObjectName(QStringLiteral("refreshOrgsButton"));
        refreshOrgsButton->setGeometry(QRect(170, 3, 51, 21));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(230, 3, 656, 501));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 10, 101, 21));
        orgNameEdit = new QLineEdit(tab);
        orgNameEdit->setObjectName(QStringLiteral("orgNameEdit"));
        orgNameEdit->setGeometry(QRect(120, 10, 191, 21));
        orgNameEdit->setMaxLength(30);
        adPushUpdatesButton = new QPushButton(tab);
        adPushUpdatesButton->setObjectName(QStringLiteral("adPushUpdatesButton"));
        adPushUpdatesButton->setGeometry(QRect(10, 430, 91, 31));
        adSnapshotButton = new QPushButton(tab);
        adSnapshotButton->setObjectName(QStringLiteral("adSnapshotButton"));
        adSnapshotButton->setGeometry(QRect(110, 430, 91, 31));
        label_19 = new QLabel(tab);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(10, 40, 101, 21));
        networkNameEdit = new QLineEdit(tab);
        networkNameEdit->setObjectName(QStringLiteral("networkNameEdit"));
        networkNameEdit->setGeometry(QRect(120, 40, 191, 21));
        networkNameEdit->setMaxLength(30);
        debugButton = new QPushButton(tab);
        debugButton->setObjectName(QStringLiteral("debugButton"));
        debugButton->setGeometry(QRect(210, 430, 91, 31));
        adminTabWidget = new QTabWidget(tab);
        adminTabWidget->setObjectName(QStringLiteral("adminTabWidget"));
        adminTabWidget->setGeometry(QRect(10, 80, 631, 341));
        tab_27 = new QWidget();
        tab_27->setObjectName(QStringLiteral("tab_27"));
        adminsTableView_3 = new QTableView(tab_27);
        adminsTableView_3->setObjectName(QStringLiteral("adminsTableView_3"));
        adminsTableView_3->setGeometry(QRect(320, 160, 301, 141));
        adminsTableView_2 = new QTableView(tab_27);
        adminsTableView_2->setObjectName(QStringLiteral("adminsTableView_2"));
        adminsTableView_2->setGeometry(QRect(10, 160, 301, 141));
        adminsTableView = new QTableView(tab_27);
        adminsTableView->setObjectName(QStringLiteral("adminsTableView"));
        adminsTableView->setGeometry(QRect(10, 10, 611, 141));
        adminTabWidget->addTab(tab_27, QString());
        tab_28 = new QWidget();
        tab_28->setObjectName(QStringLiteral("tab_28"));
        adminTabWidget->addTab(tab_28, QString());
        tabWidget->addTab(tab, QString());
        tab_17 = new QWidget();
        tab_17->setObjectName(QStringLiteral("tab_17"));
        nonMVPNPeerNameEdit = new QLineEdit(tab_17);
        nonMVPNPeerNameEdit->setObjectName(QStringLiteral("nonMVPNPeerNameEdit"));
        nonMVPNPeerNameEdit->setGeometry(QRect(170, 280, 161, 21));
        nonMVPNPeerNameEdit->setMaxLength(30);
        snmpPrivPassEdit = new QLineEdit(tab_17);
        snmpPrivPassEdit->setObjectName(QStringLiteral("snmpPrivPassEdit"));
        snmpPrivPassEdit->setGeometry(QRect(360, 140, 231, 21));
        snmpPrivPassEdit->setMaxLength(30);
        snmpPrivPassEdit->setEchoMode(QLineEdit::Password);
        snmpPrivPassCheck = new QCheckBox(tab_17);
        snmpPrivPassCheck->setObjectName(QStringLiteral("snmpPrivPassCheck"));
        snmpPrivPassCheck->setGeometry(QRect(600, 140, 51, 21));
        snmpAuthPassEdit = new QLineEdit(tab_17);
        snmpAuthPassEdit->setObjectName(QStringLiteral("snmpAuthPassEdit"));
        snmpAuthPassEdit->setGeometry(QRect(360, 110, 231, 21));
        snmpAuthPassEdit->setMaxLength(30);
        snmpAuthPassEdit->setEchoMode(QLineEdit::Password);
        snmp3Check = new QCheckBox(tab_17);
        snmp3Check->setObjectName(QStringLiteral("snmp3Check"));
        snmp3Check->setGeometry(QRect(10, 80, 111, 21));
        snmp2cCheck = new QCheckBox(tab_17);
        snmp2cCheck->setObjectName(QStringLiteral("snmp2cCheck"));
        snmp2cCheck->setGeometry(QRect(10, 10, 111, 21));
        label_18 = new QLabel(tab_17);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(80, 310, 91, 21));
        nonMVPNPeerIPEdit = new QLineEdit(tab_17);
        nonMVPNPeerIPEdit->setObjectName(QStringLiteral("nonMVPNPeerIPEdit"));
        nonMVPNPeerIPEdit->setGeometry(QRect(400, 250, 101, 21));
        nonMVPNPeerIPEdit->setMaxLength(15);
        nonMVPNAddPeerButton = new QPushButton(tab_17);
        nonMVPNAddPeerButton->setObjectName(QStringLiteral("nonMVPNAddPeerButton"));
        nonMVPNAddPeerButton->setGeometry(QRect(100, 280, 61, 21));
        nonMVPNPeersMenu = new QComboBox(tab_17);
        nonMVPNPeersMenu->setObjectName(QStringLiteral("nonMVPNPeersMenu"));
        nonMVPNPeersMenu->setGeometry(QRect(170, 250, 161, 21));
        snmpAuthMenu = new QComboBox(tab_17);
        snmpAuthMenu->addItem(QString());
        snmpAuthMenu->addItem(QString());
        snmpAuthMenu->setObjectName(QStringLiteral("snmpAuthMenu"));
        snmpAuthMenu->setGeometry(QRect(130, 80, 51, 21));
        snmpPeerIPListBrowser = new QTextBrowser(tab_17);
        snmpPeerIPListBrowser->setObjectName(QStringLiteral("snmpPeerIPListBrowser"));
        snmpPeerIPListBrowser->setGeometry(QRect(130, 110, 111, 61));
        label_6 = new QLabel(tab_17);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(280, 110, 81, 21));
        nonMVPNSecretEdit = new QLineEdit(tab_17);
        nonMVPNSecretEdit->setObjectName(QStringLiteral("nonMVPNSecretEdit"));
        nonMVPNSecretEdit->setGeometry(QRect(400, 280, 191, 21));
        nonMVPNSecretEdit->setMaxLength(25);
        nonMVPNSecretEdit->setEchoMode(QLineEdit::Password);
        snmpAuthPassCheck = new QCheckBox(tab_17);
        snmpAuthPassCheck->setObjectName(QStringLiteral("snmpAuthPassCheck"));
        snmpAuthPassCheck->setGeometry(QRect(600, 110, 51, 21));
        label_7 = new QLabel(tab_17);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(280, 140, 81, 21));
        nonMVPNSecretCheck = new QCheckBox(tab_17);
        nonMVPNSecretCheck->setObjectName(QStringLiteral("nonMVPNSecretCheck"));
        nonMVPNSecretCheck->setGeometry(QRect(600, 280, 51, 21));
        label_8 = new QLabel(tab_17);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(30, 110, 101, 21));
        label_17 = new QLabel(tab_17);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(350, 280, 41, 21));
        label_16 = new QLabel(tab_17);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(350, 250, 51, 21));
        nonMVPNSubnetBrowser = new QTextBrowser(tab_17);
        nonMVPNSubnetBrowser->setObjectName(QStringLiteral("nonMVPNSubnetBrowser"));
        nonMVPNSubnetBrowser->setGeometry(QRect(170, 320, 111, 61));
        nonMVPNCheck = new QCheckBox(tab_17);
        nonMVPNCheck->setObjectName(QStringLiteral("nonMVPNCheck"));
        nonMVPNCheck->setGeometry(QRect(10, 250, 161, 21));
        snmpPrivMenu = new QComboBox(tab_17);
        snmpPrivMenu->addItem(QString());
        snmpPrivMenu->addItem(QString());
        snmpPrivMenu->setObjectName(QStringLiteral("snmpPrivMenu"));
        snmpPrivMenu->setGeometry(QRect(190, 80, 71, 21));
        label_26 = new QLabel(tab_17);
        label_26->setObjectName(QStringLiteral("label_26"));
        label_26->setGeometry(QRect(160, 10, 61, 21));
        snmpHostnameLabel = new QLabel(tab_17);
        snmpHostnameLabel->setObjectName(QStringLiteral("snmpHostnameLabel"));
        snmpHostnameLabel->setGeometry(QRect(220, 10, 171, 21));
        label_40 = new QLabel(tab_17);
        label_40->setObjectName(QStringLiteral("label_40"));
        label_40->setGeometry(QRect(280, 80, 41, 21));
        label_41 = new QLabel(tab_17);
        label_41->setObjectName(QStringLiteral("label_41"));
        label_41->setGeometry(QRect(400, 10, 31, 21));
        snmpPortLabel = new QLabel(tab_17);
        snmpPortLabel->setObjectName(QStringLiteral("snmpPortLabel"));
        snmpPortLabel->setGeometry(QRect(430, 10, 51, 21));
        snmpUserLabel = new QLabel(tab_17);
        snmpUserLabel->setObjectName(QStringLiteral("snmpUserLabel"));
        snmpUserLabel->setGeometry(QRect(340, 80, 131, 21));
        snmpCommStringLabel = new QLabel(tab_17);
        snmpCommStringLabel->setObjectName(QStringLiteral("snmpCommStringLabel"));
        snmpCommStringLabel->setGeometry(QRect(260, 40, 141, 21));
        label_42 = new QLabel(tab_17);
        label_42->setObjectName(QStringLiteral("label_42"));
        label_42->setGeometry(QRect(160, 40, 91, 21));
        label_43 = new QLabel(tab_17);
        label_43->setObjectName(QStringLiteral("label_43"));
        label_43->setGeometry(QRect(350, 320, 41, 21));
        nonMVPNTagsBrowser = new QTextBrowser(tab_17);
        nonMVPNTagsBrowser->setObjectName(QStringLiteral("nonMVPNTagsBrowser"));
        nonMVPNTagsBrowser->setGeometry(QRect(390, 320, 201, 61));
        tabWidget->addTab(tab_17, QString());
        tab_15 = new QWidget();
        tab_15->setObjectName(QStringLiteral("tab_15"));
        licenseExpDateLabel = new QLabel(tab_15);
        licenseExpDateLabel->setObjectName(QStringLiteral("licenseExpDateLabel"));
        licenseExpDateLabel->setGeometry(QRect(100, 40, 121, 21));
        label_4 = new QLabel(tab_15);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 40, 81, 21));
        licenseDevicesBrowser = new QTextBrowser(tab_15);
        licenseDevicesBrowser->setObjectName(QStringLiteral("licenseDevicesBrowser"));
        licenseDevicesBrowser->setGeometry(QRect(300, 10, 101, 101));
        label_5 = new QLabel(tab_15);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 110, 121, 16));
        nonMVPNAddPeerButton_2 = new QPushButton(tab_15);
        nonMVPNAddPeerButton_2->setObjectName(QStringLiteral("nonMVPNAddPeerButton_2"));
        nonMVPNAddPeerButton_2->setGeometry(QRect(140, 350, 61, 21));
        nonMVPNPeerNameEdit_2 = new QLineEdit(tab_15);
        nonMVPNPeerNameEdit_2->setObjectName(QStringLiteral("nonMVPNPeerNameEdit_2"));
        nonMVPNPeerNameEdit_2->setGeometry(QRect(10, 350, 121, 21));
        nonMVPNPeerNameEdit_2->setMaxLength(14);
        licenseStateLabel = new QLabel(tab_15);
        licenseStateLabel->setObjectName(QStringLiteral("licenseStateLabel"));
        licenseStateLabel->setGeometry(QRect(100, 10, 121, 21));
        label_3 = new QLabel(tab_15);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 10, 91, 21));
        orgInventoryView = new QTableView(tab_15);
        orgInventoryView->setObjectName(QStringLiteral("orgInventoryView"));
        orgInventoryView->setGeometry(QRect(10, 130, 631, 201));
        label_20 = new QLabel(tab_15);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(10, 390, 121, 16));
        orgNameEdit_2 = new QLineEdit(tab_15);
        orgNameEdit_2->setObjectName(QStringLiteral("orgNameEdit_2"));
        orgNameEdit_2->setGeometry(QRect(130, 390, 191, 21));
        orgNameEdit_2->setMaxLength(30);
        nonMVPNAddPeerButton_3 = new QPushButton(tab_15);
        nonMVPNAddPeerButton_3->setObjectName(QStringLiteral("nonMVPNAddPeerButton_3"));
        nonMVPNAddPeerButton_3->setGeometry(QRect(330, 390, 61, 21));
        refreshOrgsButton_2 = new QPushButton(tab_15);
        refreshOrgsButton_2->setObjectName(QStringLiteral("refreshOrgsButton_2"));
        refreshOrgsButton_2->setGeometry(QRect(230, 10, 51, 21));
        label_44 = new QLabel(tab_15);
        label_44->setObjectName(QStringLiteral("label_44"));
        label_44->setGeometry(QRect(440, 10, 81, 21));
        missingLicensesBrowser = new QTextBrowser(tab_15);
        missingLicensesBrowser->setObjectName(QStringLiteral("missingLicensesBrowser"));
        missingLicensesBrowser->setGeometry(QRect(530, 10, 101, 101));
        tabWidget->addTab(tab_15, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        label_45 = new QLabel(tab_3);
        label_45->setObjectName(QStringLiteral("label_45"));
        label_45->setGeometry(QRect(10, 10, 51, 21));
        msNetworkEdit = new QLineEdit(tab_3);
        msNetworkEdit->setObjectName(QStringLiteral("msNetworkEdit"));
        msNetworkEdit->setGeometry(QRect(60, 10, 191, 21));
        msNetworkEdit->setMaxLength(30);
        msSwitchesTable = new QTableView(tab_3);
        msSwitchesTable->setObjectName(QStringLiteral("msSwitchesTable"));
        msSwitchesTable->setGeometry(QRect(10, 40, 631, 141));
        msSwitchPortsTable = new QTableView(tab_3);
        msSwitchPortsTable->setObjectName(QStringLiteral("msSwitchPortsTable"));
        msSwitchPortsTable->setGeometry(QRect(10, 190, 631, 221));
        msRefreshButton = new QPushButton(tab_3);
        msRefreshButton->setObjectName(QStringLiteral("msRefreshButton"));
        msRefreshButton->setGeometry(QRect(590, 10, 51, 21));
        tabWidget->addTab(tab_3, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        tabWidget->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        tabWidget->addTab(tab_6, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QStringLiteral("tab_7"));
        tabWidget->addTab(tab_7, QString());
        treeView = new QTreeView(centralWidget);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setGeometry(QRect(10, 33, 211, 470));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 900, 20));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuAbout->menuAction());

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(3);
        adminTabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QApplication::translate("MainWindow", "Organizations/Networks", nullptr));
        refreshOrgsButton->setText(QApplication::translate("MainWindow", "Refresh", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Organization name:", nullptr));
        orgNameEdit->setText(QApplication::translate("MainWindow", "123456789012345678901234567890", nullptr));
        adPushUpdatesButton->setText(QApplication::translate("MainWindow", "Push Updates", nullptr));
        adSnapshotButton->setText(QApplication::translate("MainWindow", "Take Snapshot", nullptr));
        label_19->setText(QApplication::translate("MainWindow", "Network name:", nullptr));
        networkNameEdit->setText(QApplication::translate("MainWindow", "123456789012345678901234567890", nullptr));
        debugButton->setText(QApplication::translate("MainWindow", "DEBUG", nullptr));
        adminTabWidget->setTabText(adminTabWidget->indexOf(tab_27), QApplication::translate("MainWindow", "Administrators", nullptr));
        adminTabWidget->setTabText(adminTabWidget->indexOf(tab_28), QApplication::translate("MainWindow", "SAML", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Administration", nullptr));
        nonMVPNPeerNameEdit->setText(QString());
        nonMVPNPeerNameEdit->setPlaceholderText(QApplication::translate("MainWindow", "Non-Meraki VPN peer name", nullptr));
        snmpPrivPassEdit->setText(QApplication::translate("MainWindow", "123456789012345678901234567890", nullptr));
        snmpPrivPassCheck->setText(QApplication::translate("MainWindow", "show", nullptr));
        snmpAuthPassEdit->setText(QApplication::translate("MainWindow", "123456789012345678901234567890", nullptr));
        snmp3Check->setText(QApplication::translate("MainWindow", "SNMP v3 enabled", nullptr));
        snmp2cCheck->setText(QApplication::translate("MainWindow", "SNMP v2c enabled", nullptr));
        label_18->setText(QApplication::translate("MainWindow", "Private subnets:", nullptr));
        nonMVPNPeerIPEdit->setText(QApplication::translate("MainWindow", "100.100.100.100", nullptr));
        nonMVPNAddPeerButton->setText(QApplication::translate("MainWindow", "Add Peer", nullptr));
        snmpAuthMenu->setItemText(0, QApplication::translate("MainWindow", "MD5", nullptr));
        snmpAuthMenu->setItemText(1, QApplication::translate("MainWindow", "SHA", nullptr));

        snmpPeerIPListBrowser->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aaa.bbb.ccc.ddd</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aaa.bbb.ccc.ddd</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aaa.bbb.ccc.ddd</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aaa.bbb.ccc.ddd</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0;"
                        " text-indent:0px;\">aaa.bbb.ccc.ddd</p></body></html>", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Auth password:", nullptr));
        nonMVPNSecretEdit->setText(QApplication::translate("MainWindow", "1234567890123456789012345", nullptr));
        snmpAuthPassCheck->setText(QApplication::translate("MainWindow", "show", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "Priv password:", nullptr));
        nonMVPNSecretCheck->setText(QApplication::translate("MainWindow", "show", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "Peer IPs (allowed):", nullptr));
        label_17->setText(QApplication::translate("MainWindow", "Secret:", nullptr));
        label_16->setText(QApplication::translate("MainWindow", "Public IP:", nullptr));
        nonMVPNSubnetBrowser->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aaa.bbb.ccc.ddd</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aaa.bbb.ccc.ddd</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aaa.bbb.ccc.ddd</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aaa.bbb.ccc.ddd</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0;"
                        " text-indent:0px;\">aaa.bbb.ccc.ddd</p></body></html>", nullptr));
        nonMVPNCheck->setText(QApplication::translate("MainWindow", "Non-Meraki site-to-site VPN:", nullptr));
        snmpPrivMenu->setItemText(0, QApplication::translate("MainWindow", "DES", nullptr));
        snmpPrivMenu->setItemText(1, QApplication::translate("MainWindow", "AES128", nullptr));

        label_26->setText(QApplication::translate("MainWindow", "Hostname:", nullptr));
        snmpHostnameLabel->setText(QApplication::translate("MainWindow", "<hostname_label>", nullptr));
        label_40->setText(QApplication::translate("MainWindow", "User:", nullptr));
        label_41->setText(QApplication::translate("MainWindow", "Port:", nullptr));
        snmpPortLabel->setText(QApplication::translate("MainWindow", "<port>", nullptr));
        snmpUserLabel->setText(QApplication::translate("MainWindow", "<user>", nullptr));
        snmpCommStringLabel->setText(QApplication::translate("MainWindow", "<string>", nullptr));
        label_42->setText(QApplication::translate("MainWindow", "Community string:", nullptr));
        label_43->setText(QApplication::translate("MainWindow", "Tags:", nullptr));
        nonMVPNTagsBrowser->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">tag1, tag2, tag3, tag4</p></body></html>", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_17), QApplication::translate("MainWindow", "Organization Settings", nullptr));
        licenseExpDateLabel->setText(QApplication::translate("MainWindow", "<mmm dd, yyyy ttt>", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Expiration date:", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Organization Inventory:", nullptr));
        nonMVPNAddPeerButton_2->setText(QApplication::translate("MainWindow", "Claim", nullptr));
        nonMVPNPeerNameEdit_2->setText(QString());
        nonMVPNPeerNameEdit_2->setPlaceholderText(QApplication::translate("MainWindow", "Order/Serial/Key", nullptr));
        licenseStateLabel->setText(QApplication::translate("MainWindow", "<state>", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Licensing status:", nullptr));
        label_20->setText(QApplication::translate("MainWindow", "Clone this organization:", nullptr));
        orgNameEdit_2->setText(QString());
        orgNameEdit_2->setPlaceholderText(QApplication::translate("MainWindow", "New organization name", nullptr));
        nonMVPNAddPeerButton_3->setText(QApplication::translate("MainWindow", "Clone", nullptr));
        refreshOrgsButton_2->setText(QApplication::translate("MainWindow", "Refresh", nullptr));
        label_44->setText(QApplication::translate("MainWindow", "Missing licenses:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_15), QApplication::translate("MainWindow", "Licensing / Inventory", nullptr));
        label_45->setText(QApplication::translate("MainWindow", "Network", nullptr));
        msNetworkEdit->setText(QApplication::translate("MainWindow", "123456789012345678901234567890", nullptr));
        msRefreshButton->setText(QApplication::translate("MainWindow", "Refresh", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "MS", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "MX", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainWindow", "MR", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("MainWindow", "MV", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("MainWindow", "MC", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_7), QApplication::translate("MainWindow", "Systems Manager", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", nullptr));
        menuView->setTitle(QApplication::translate("MainWindow", "View", nullptr));
        menuAbout->setTitle(QApplication::translate("MainWindow", "About", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
