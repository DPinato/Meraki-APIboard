#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


#include "morganization.h"
#include "apihelper.h"

class MOrganization;
class APIHelper;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	QString getAPIkeyFromFile(QString file);
	void prepareModelObj();
	void prepareColumnHeaders();
	void updateOrgUI(int orgIndex);		// update the tree view of the organization/network
	void updateNetworkUI(int orgIndex, int netIndex);


	// functions to show different things in the GUI
	void displayAdminStuff(int orgIndex);
	void displayLicenseInfo(int orgIndex);
	void displayInventory(int orgIndex);
	void displayOrgSNMP(int orgIndex);
	void displayOrgVPN(int orgIndex);
	void displayMSInfo(int orgIndex, int netIndex = -1);
	void displayMSPort(int devIndex, int orgIndex);
	void displayMXInfo(int orgIndex, int netIndex = -1);
	void displayMXL3Rules(int devIndex, int orgIndex);
	void displaySMDevices(int orgIndex, int netIndex = -1);



	// TODO: I am sure there is a better arrangement than this
	QVector<MOrganization *> orgList;


public slots:
	void replyFinished(QNetworkReply *reply);

signals:

private slots:
	void on_treeView_doubleClicked(const QModelIndex &index);
	void on_nonMVPNCheck_clicked(bool checked);
	void on_debugButton_clicked();
	void on_treeView_clicked(const QModelIndex &index);
	void on_snmp3Check_clicked(bool checked);
	void on_refreshOrgsButton_clicked();
	void on_tabWidget_currentChanged(int index);
	void on_adminsTableView_clicked(const QModelIndex &index);
	void on_nonMVPNPeersMenu_currentIndexChanged(int index);
	void on_nonMVPNSecretCheck_clicked(bool checked);
	void on_snmpAuthPassCheck_clicked(bool checked);
	void on_snmpPrivPassCheck_clicked(bool checked);
	void on_msSwitchesTable_clicked(const QModelIndex &index);
	void on_mxDeviceTable_clicked(const QModelIndex &index);


private:
	Ui::MainWindow *ui;

	QString apiKey;			// file holding the API key being used
	QString urlListFile;	// file holding the list of URLs for the API
	QNetworkAccessManager *manager;


	APIHelper *apiHelpObj;

	int currOrgIndex;	// index of organization currently shown in the GUI

	// these objects are used to show list of things in the GUI
	QStandardItemModel *adminListModel[3];		// list of org administrators
	QStandardItemModel *inventoryModel;		// org inventory
	QStandardItemModel *msListModel;		// show list of switches
	QStandardItemModel *msPortListModel;	// show list of ports of a particular switch
	QStandardItemModel *mxListModel;		// show list of MXs
	QStandardItemModel *mxL3RulesModel;		// show list of L3 firewall rules for MX
	QStandardItemModel *smDeviceListModel;	// show list of SM devices

};

#endif // MAINWINDOW_H
