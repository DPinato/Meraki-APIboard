#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {

	ui->setupUi(this);
	QMainWindow::setWindowTitle("Meraki-APIboard");

	// initialise UI
	ui->snmp3Check->setChecked(false);
	on_snmp3Check_clicked(false);
	ui->nonMVPNCheck->setChecked(false);
	on_nonMVPNCheck_clicked(false);
	prepareModelObj();			// initialise QStandardItemModel objects
	prepareColumnHeaders();		// show columns headers for table views in the GUI


	// I am doing this so I do not have to leave my API key in this code
	//	apiKey = getAPIkeyFromFile(QString("D:\\Programming\\meraki_api_key.txt"));
	apiKey = getAPIkeyFromFile(QString("C:\\Users\\Davide\\Documents\\meraki_api_key.txt"));
	qDebug() << apiKey;

	//	urlListFile = QString("D:\\Programming\\Qt\\Meraki-APIboard\\URL_list.txt");
	urlListFile = QString("C:\\Users\\Davide\\Documents\\Meraki-APIboard\\URL_list.txt");
	qDebug() << urlListFile;


	apiHelpObj = new APIHelper(apiKey, this);
	apiHelpObj->readURLListFromFile(urlListFile);

	currOrgIndex = -1;



	// start by getting the list of organizations associated with the license key
	on_refreshOrgsButton_clicked();



}

MainWindow::~MainWindow() {
	delete ui;
}

QString MainWindow::getAPIkeyFromFile(QString file) {
	// the file should only contain the API key
	// it might end in a newline character
	QString tmp;

	QFile readFile(file);
	if (!readFile.open(QIODevice::ReadOnly)) { return QString(""); }

	tmp = readFile.readLine();
	if (tmp.at(tmp.length()-1) == QChar('\n')) { tmp.remove(tmp.length()-1, 1); }

	return tmp;

}

void MainWindow::prepareModelObj() {
	// use this function to initialise the QStandardItemModel aobjects
	// do not give them the number of rows here, the individual display functions will do so

	adminListModel[0] = new QStandardItemModel(this);
	adminListModel[0]->setColumnCount(4);
	ui->adminsTableView->setModel(adminListModel[0]);
	adminListModel[1] = new QStandardItemModel(this);
	adminListModel[1]->setColumnCount(2);
	ui->adminsTableView_2->setModel(adminListModel[1]);
	adminListModel[2] = new QStandardItemModel(this);
	adminListModel[2]->setColumnCount(2);
	ui->adminsTableView_3->setModel(adminListModel[2]);

	//	inventoryModel = new QStandardItemModel(orgList.at(orgIndex)->getOrgInventorySize(), 6, this);
	inventoryModel = new QStandardItemModel(this);
	inventoryModel->setColumnCount(6);
	ui->orgInventoryView->setModel(inventoryModel);

	//	msListModel = new QStandardItemModel(count, 5, this);
	msListModel = new QStandardItemModel(this);
	msListModel->setColumnCount(5);
	ui->msSwitchesTable->setModel(msListModel);

	//	msPortListModel = new QStandardItemModel(portCount, 13, this);
	msPortListModel = new QStandardItemModel(this);
	msPortListModel->setColumnCount(13);
	ui->msSwitchPortsTable->setModel(msPortListModel);

	//	mxListModel = new QStandardItemModel(count, 5, this);
	mxListModel = new QStandardItemModel(this);
	mxListModel->setColumnCount(5);
	ui->mxDeviceTable->setModel(mxListModel);

	//	smDeviceListModel = new QStandardItemModel(count, 49, this);
	smDeviceListModel = new QStandardItemModel(this);
	smDeviceListModel->setColumnCount(49);
	ui->smDevicesTable->setModel(smDeviceListModel);

	//	mxL3RulesModel = new QStandardItemModel(rulesCount, 8, this);
	mxL3RulesModel = new QStandardItemModel(this);
	mxL3RulesModel->setColumnCount(8);
	ui->mxL3FirewallTable->setModel(mxL3RulesModel);



}

void MainWindow::prepareColumnHeaders() {
	// show columns header and things in the UI, when the program first starts
	// this should be called after prepareUI()
	// consider reading all the column headers from file?

	// columns are | name | email | org_privilege | admin_id | networks | tags |
	adminListModel[0]->setHeaderData(0, Qt::Horizontal, QString("Name"));
	adminListModel[0]->setHeaderData(1, Qt::Horizontal, QString("E-mail"));
	adminListModel[0]->setHeaderData(2, Qt::Horizontal, QString("Org Privilege"));
	adminListModel[0]->setHeaderData(3, Qt::Horizontal, QString("ID"));

	adminListModel[2]->setHeaderData(0, Qt::Horizontal, QString("Tag"));
	adminListModel[2]->setHeaderData(1, Qt::Horizontal, QString("Access"));


	// columns are | model | mac | serial | publicIp | claimedAt | networkId |
	inventoryModel->setHeaderData(0, Qt::Horizontal, QString("model"));
	inventoryModel->setHeaderData(1, Qt::Horizontal, QString("mac"));
	inventoryModel->setHeaderData(2, Qt::Horizontal, QString("serial"));
	inventoryModel->setHeaderData(3, Qt::Horizontal, QString("publicIp"));
	inventoryModel->setHeaderData(4, Qt::Horizontal, QString("claimedAt"));
	inventoryModel->setHeaderData(5, Qt::Horizontal, QString("networkId"));


	// columns are | MAC | Model | Serial | public IP | network |
	msListModel->setHeaderData(0, Qt::Horizontal, QString("MAC"));
	msListModel->setHeaderData(1, Qt::Horizontal, QString("Model"));
	msListModel->setHeaderData(2, Qt::Horizontal, QString("Serial"));
	msListModel->setHeaderData(3, Qt::Horizontal, QString("Public IP"));
	msListModel->setHeaderData(4, Qt::Horizontal, QString("Network"));


	// columns are | number | name | tags | enabled | poeEnabled | type | vlan | voiceVlan | allowedVlans |
	// | isolationEnabled | rstpEnabled | stpGuard | accessPolicyNumber
	msPortListModel->setHeaderData(0, Qt::Horizontal, QString("number"));
	msPortListModel->setHeaderData(1, Qt::Horizontal, QString("name"));
	msPortListModel->setHeaderData(2, Qt::Horizontal, QString("tags"));
	msPortListModel->setHeaderData(3, Qt::Horizontal, QString("enabled"));
	msPortListModel->setHeaderData(4, Qt::Horizontal, QString("poeEnabled"));
	msPortListModel->setHeaderData(5, Qt::Horizontal, QString("type"));
	msPortListModel->setHeaderData(6, Qt::Horizontal, QString("vlan"));
	msPortListModel->setHeaderData(7, Qt::Horizontal, QString("voiceVlan"));
	msPortListModel->setHeaderData(8, Qt::Horizontal, QString("allowedVlans"));
	msPortListModel->setHeaderData(9, Qt::Horizontal, QString("isolationEnabled"));
	msPortListModel->setHeaderData(10, Qt::Horizontal, QString("rstpEnabled"));
	msPortListModel->setHeaderData(11, Qt::Horizontal, QString("stpGuard"));
	msPortListModel->setHeaderData(12, Qt::Horizontal, QString("accessPolicyNumber"));


	// columns are | MAC | Model | Serial | public IP | network |
	mxListModel->setHeaderData(0, Qt::Horizontal, QString("MAC"));
	mxListModel->setHeaderData(1, Qt::Horizontal, QString("Model"));
	mxListModel->setHeaderData(2, Qt::Horizontal, QString("Serial"));
	mxListModel->setHeaderData(3, Qt::Horizontal, QString("Public IP"));
	mxListModel->setHeaderData(4, Qt::Horizontal, QString("Network"));


	// columns are | id | name | tags | ssid | wifiMac | osName | systemModel | uuid |
	// | serialNumber | ip | systemType | availableDeviceCapacity | kioskAppName |
	// | biosVersion | lastConnected | missingAppsCount | userSuppliedAddress |
	// | location | lastUser | publicIp | phoneNumber | diskInfoJson | deviceCapacity |
	// | isManaged | hadMdm | isSupervised | meid | imei | iccid | simCarrierNetwork |
	// | cellularDataUsed | isHotspotEnabled | createdAt | batteryEstCharge | quarantined |
	// | avName | avRunning | asName | fwName | isRooted | loginRequired | screenLockEnabled |
	// | autoLoginDisabled | hasMdm | hasDesktopAgent | diskEncryptionEnabled |
	// | hardwareEncryptionCaps | passCodeLock
	// 49 columns, I missed one above, the full list is in the smDevice struct, wow
	smDeviceListModel->setHeaderData(0, Qt::Horizontal, QString("id"));
	smDeviceListModel->setHeaderData(1, Qt::Horizontal, QString("name"));
	smDeviceListModel->setHeaderData(2, Qt::Horizontal, QString("tags"));
	smDeviceListModel->setHeaderData(3, Qt::Horizontal, QString("ssid"));
	smDeviceListModel->setHeaderData(4, Qt::Horizontal, QString("wifiMac"));
	smDeviceListModel->setHeaderData(5, Qt::Horizontal, QString("osName"));
	smDeviceListModel->setHeaderData(6, Qt::Horizontal, QString("systemModel"));
	smDeviceListModel->setHeaderData(7, Qt::Horizontal, QString("uuid"));
	smDeviceListModel->setHeaderData(8, Qt::Horizontal, QString("serialNumber"));
	smDeviceListModel->setHeaderData(9, Qt::Horizontal, QString("ip"));
	smDeviceListModel->setHeaderData(10, Qt::Horizontal, QString("systemType"));
	smDeviceListModel->setHeaderData(11, Qt::Horizontal, QString("availableDeviceCapacity"));
	smDeviceListModel->setHeaderData(12, Qt::Horizontal, QString("kioskAppName"));
	smDeviceListModel->setHeaderData(13, Qt::Horizontal, QString("biosVersion"));
	smDeviceListModel->setHeaderData(14, Qt::Horizontal, QString("lastConnected"));
	smDeviceListModel->setHeaderData(15, Qt::Horizontal, QString("missingAppsCount"));
	smDeviceListModel->setHeaderData(16, Qt::Horizontal, QString("userSuppliedAddress"));
	smDeviceListModel->setHeaderData(17, Qt::Horizontal, QString("location"));
	smDeviceListModel->setHeaderData(18, Qt::Horizontal, QString("lastUser"));
	smDeviceListModel->setHeaderData(19, Qt::Horizontal, QString("publicIp"));
	smDeviceListModel->setHeaderData(20, Qt::Horizontal, QString("phoneNumber"));
	smDeviceListModel->setHeaderData(21, Qt::Horizontal, QString("diskInfoJson"));
	smDeviceListModel->setHeaderData(22, Qt::Horizontal, QString("deviceCapacity"));
	smDeviceListModel->setHeaderData(23, Qt::Horizontal, QString("isManaged"));
	smDeviceListModel->setHeaderData(24, Qt::Horizontal, QString("hadMdm"));
	smDeviceListModel->setHeaderData(25, Qt::Horizontal, QString("isSupervised"));
	smDeviceListModel->setHeaderData(26, Qt::Horizontal, QString("meid"));
	smDeviceListModel->setHeaderData(27, Qt::Horizontal, QString("imei"));
	smDeviceListModel->setHeaderData(28, Qt::Horizontal, QString("iccid"));
	smDeviceListModel->setHeaderData(29, Qt::Horizontal, QString("simCarrierNetwork"));
	smDeviceListModel->setHeaderData(30, Qt::Horizontal, QString("cellularDataUsed"));
	smDeviceListModel->setHeaderData(31, Qt::Horizontal, QString("isHotspotEnabled"));
	smDeviceListModel->setHeaderData(32, Qt::Horizontal, QString("createdAt"));
	smDeviceListModel->setHeaderData(33, Qt::Horizontal, QString("batteryEstCharge"));
	smDeviceListModel->setHeaderData(34, Qt::Horizontal, QString("quarantined"));
	smDeviceListModel->setHeaderData(35, Qt::Horizontal, QString("avName"));
	smDeviceListModel->setHeaderData(36, Qt::Horizontal, QString("avRunning"));
	smDeviceListModel->setHeaderData(37, Qt::Horizontal, QString("asName"));
	smDeviceListModel->setHeaderData(38, Qt::Horizontal, QString("fwName"));
	smDeviceListModel->setHeaderData(39, Qt::Horizontal, QString("isRooted"));
	smDeviceListModel->setHeaderData(40, Qt::Horizontal, QString("loginRequired"));
	smDeviceListModel->setHeaderData(41, Qt::Horizontal, QString("screenLockEnabled"));
	smDeviceListModel->setHeaderData(42, Qt::Horizontal, QString("screenLockDelay"));
	smDeviceListModel->setHeaderData(43, Qt::Horizontal, QString("autoLoginDisabled"));
	smDeviceListModel->setHeaderData(44, Qt::Horizontal, QString("hasMdm"));
	smDeviceListModel->setHeaderData(45, Qt::Horizontal, QString("hasDesktopAgent"));
	smDeviceListModel->setHeaderData(46, Qt::Horizontal, QString("diskEncryptionEnabled"));
	smDeviceListModel->setHeaderData(47, Qt::Horizontal, QString("hardwareEncryptionCaps"));
	smDeviceListModel->setHeaderData(48, Qt::Horizontal, QString("passCodeLock"));


	// columns are | policy | protocol | srcCidr | srcPort | dstCidr | dstPort | syslogEnabled | comment |
	mxL3RulesModel->setHeaderData(0, Qt::Horizontal, QString("policy"));
	mxL3RulesModel->setHeaderData(1, Qt::Horizontal, QString("protocol"));
	mxL3RulesModel->setHeaderData(2, Qt::Horizontal, QString("srcCidr"));
	mxL3RulesModel->setHeaderData(3, Qt::Horizontal, QString("srcPort"));
	mxL3RulesModel->setHeaderData(4, Qt::Horizontal, QString("destCidr"));
	mxL3RulesModel->setHeaderData(5, Qt::Horizontal, QString("destPort"));
	mxL3RulesModel->setHeaderData(6, Qt::Horizontal, QString("syslogEnabled"));
	mxL3RulesModel->setHeaderData(7, Qt::Horizontal, QString("comment"));



}

void MainWindow::updateOrgUI(int orgIndex) {
	// for the organization/network tree view
	// put the stuff in the UI after an API reply
	qDebug() << "MainWindow::updateUI(), orgIndex: " << orgIndex;
	if (orgIndex == -1) {
		// data for more than 1 org was queried
		// without this, the tree view would close all the time

		// TODO: this declaration of testTree should probably be changed
		QStandardItemModel *testTree = new QStandardItemModel(orgList.size(), 1, this);
		testTree->setHeaderData(0, Qt::Horizontal, QString(""));

		for (int i = 0; i < orgList.size(); i++) {
			// format the name of the org to look like "<org_name> (<org_id>)"
			QString parent = orgList.at(i)->getOrgName() + " (" + orgList.at(i)->getOrgID() + ")";
			testTree->setItem(i, new QStandardItem(parent));
			testTree->item(i)->setFlags(Qt::ItemIsEditable);	// make items not editable

			// show all networks in the org as child
			for (int n = 0; n < orgList.at(i)->getNetworksNum(); n++) {
				testTree->item(i)->setChild(n, new QStandardItem(orgList.at(i)->getNetwork(n).netName));
				testTree->item(i)->child(n)->setFlags(Qt::ItemIsEditable);

			}

		}

		ui->treeView->setModel(testTree);

		return;
	}


	// show things in the GUI
	ui->orgNameEdit->setText(orgList.at(orgIndex)->getOrgName());
	ui->networkNameEdit->setText("");

	currOrgIndex = orgIndex;
	displayAdminStuff(orgIndex);
	displayLicenseInfo(orgIndex);
	displayInventory(orgIndex);


}

void MainWindow::updateNetworkUI(QModelIndex &index) {
	// show information about the network/organization
	qDebug() << index;
	qDebug() << index.data() << "\t" << index.row();
	qDebug() << index.parent().data() << "\t" << index.parent().row();

	int tmpOrgIndex, tmpNetIndex;

	if (index.parent().data() == QVariant::Invalid) {
		// an organization was selected in the tree view
		tmpOrgIndex = index.row();
		tmpNetIndex = -1;
	} else {
		// a network was selected in the tree view
		tmpOrgIndex = index.parent().row();
		tmpNetIndex = index.row();
	}
	qDebug() << "tmpOrgIndex: " << tmpOrgIndex << "\ttmpNetIndex: " << tmpNetIndex;
	qDebug() << "\n";


	// show info about the organization
	ui->orgNameEdit->setText(orgList.at(tmpOrgIndex)->getOrgName());


	if (tmpNetIndex != -1) {
		// show info about network
		ui->networkNameEdit->setEnabled(true);
		ui->networkNameEdit->setText(orgList.at(tmpOrgIndex)->getNetwork(tmpNetIndex).netName);
	} else {
		ui->networkNameEdit->setEnabled(false);
		ui->networkNameEdit->setText("");
	}


}

void MainWindow::displayAdminStuff(int orgIndex) {
	// show administrators in the table view

	adminListModel[0]->setRowCount(orgList.at(orgIndex)->getAdminListSize());

	for (int i = 0; i < orgList.at(orgIndex)->getAdminListSize(); i++) {
		adminStruct tmpAdmin = orgList.at(orgIndex)->getAdmin(i);

		adminListModel[0]->setItem(i, 0, new QStandardItem(tmpAdmin.name));
		adminListModel[0]->setItem(i, 1, new QStandardItem(tmpAdmin.email));
		adminListModel[0]->setItem(i, 2, new QStandardItem(tmpAdmin.orgAccess));
		adminListModel[0]->setItem(i, 3, new QStandardItem(tmpAdmin.id));


		// display list of networks admin has access to, if this is a network admin
		// also display privileges of camera-only administrators
		// since there could be either 2 or 3 columns, leave the setHeaderData here
		adminListModel[1]->setRowCount(orgList.at(orgIndex)->getAdmin(i).nets.size());
		adminListModel[1]->setHeaderData(0, Qt::Horizontal, QString("Net ID"));
		adminListModel[1]->setHeaderData(1, Qt::Horizontal, QString("Access"));

		int j = 0;	// I think that a network admin can be a network admin and camera-only admin in different networks
		for (j = 0; j < tmpAdmin.nets.size(); j++) {
			adminNetPermission tmpPerm = tmpAdmin.nets.at(j);
			adminListModel[1]->setItem(j, 0, new QStandardItem(tmpPerm.netID));
			adminListModel[1]->setItem(j, 1, new QStandardItem(tmpPerm.accessLevel));
		}

		// for camera-only admins
		if (tmpAdmin.cNets.size() > 0) {
			adminListModel[1]->setColumnCount(3);
			adminListModel[1]->setHeaderData(2, Qt::Horizontal, QString("network_type"));

			for (j = j; j < tmpAdmin.cNets.size(); j++) {
				adminNetPermission tmpCPerm = tmpAdmin.cNets.at(j);
				adminListModel[1]->setItem(j, 0, new QStandardItem(tmpCPerm.netID));
				adminListModel[1]->setItem(j, 1, new QStandardItem(tmpCPerm.accessLevel));
				adminListModel[1]->setItem(j, 2, new QStandardItem(tmpCPerm.networkType));
			}
		}

		ui->adminsTableView_2->resizeColumnsToContents();
		ui->adminsTableView_2->resizeRowsToContents();



		// display list of tags assigned to administrator
		adminListModel[2]->setRowCount(orgList.at(orgIndex)->getAdmin(i).tags.size());

		for (int j = 0; j < tmpAdmin.tags.size(); j++) {
			adminTag tmpTag = tmpAdmin.tags.at(j);
			adminListModel[2]->setItem(j, 0, new QStandardItem(tmpTag.tag));
			adminListModel[2]->setItem(j, 1, new QStandardItem(tmpTag.adminAccessLevel));
		}

		ui->adminsTableView_3->resizeColumnsToContents();
		ui->adminsTableView_3->resizeRowsToContents();

	}

	ui->adminsTableView->resizeColumnsToContents();
	ui->adminsTableView->resizeRowsToContents();

}

void MainWindow::displayLicenseInfo(int orgIndex) {
	// display licensing of the organization in the GUI
	ui->licenseStateLabel->setText(orgList.at(orgIndex)->getLicenseStatus());
	ui->licenseExpDateLabel->setText(orgList.at(orgIndex)->getLicenseExpireDate());
	QString licText;
	for (int i = 0; i < orgList.at(orgIndex)->getLicenseListSize(); i++) {
		licText.append(orgList.at(orgIndex)->getLicensePerDevice(i).deviceType);
		licText.append(" - ");
		licText.append(QString::number(orgList.at(orgIndex)->getLicensePerDevice(i).count));
		licText.append("\n");

	}

	ui->licenseDevicesBrowser->setText(licText);

}

void MainWindow::displayInventory(int orgIndex) {
	// show devices in the inventory table
	inventoryModel->setRowCount(orgList.at(orgIndex)->getOrgInventorySize());

	// show stuff
	for (int i = 0; i < orgList.at(orgIndex)->getOrgInventorySize(); i++) {
		deviceInInventory tmpDevice = orgList.at(orgIndex)->getOrgInventoryDevice(i);

		inventoryModel->setItem(i, 0, new QStandardItem(tmpDevice.model));
		inventoryModel->setItem(i, 1, new QStandardItem(tmpDevice.mac));
		inventoryModel->setItem(i, 2, new QStandardItem(tmpDevice.serial));
		inventoryModel->setItem(i, 3, new QStandardItem(tmpDevice.publicIP));
		inventoryModel->setItem(i, 4, new QStandardItem(tmpDevice.claimedAt));
		inventoryModel->setItem(i, 5, new QStandardItem(tmpDevice.netID));

	}

	ui->orgInventoryView->resizeColumnsToContents();
	ui->orgInventoryView->resizeRowsToContents();

}

void MainWindow::displayOrgSNMP(int orgIndex) {
	// display SNMP configuration in the GUI
	// some parameters are only returned if the appropriate version of SNMP is enabled
	orgSNMP tmpSNMP = orgList.at(orgIndex)->getOrgSNMPSettings();


	// stuff that is always returned
	ui->snmpHostnameLabel->setText(tmpSNMP.hostname);
	ui->snmpPortLabel->setText(tmpSNMP.port);

	int index = 0;
	if (tmpSNMP.snmpAuthMode == "MD5") { index = 0; }
	if (tmpSNMP.snmpAuthMode == "SHA") { index = 1; }
	ui->snmpAuthMenu->setCurrentIndex(index);

	if (tmpSNMP.snmpPrivMode == "DES") { index = 0; }
	if (tmpSNMP.snmpPrivMode == "AES128") { index = 1; }
	ui->snmpPrivMenu->setCurrentIndex(index);

	QString tmpStr;
	for (int i = 0; i < tmpSNMP.snmpPeerIPs.size(); i++) {
		tmpStr.append(tmpSNMP.snmpPeerIPs.at(i)).append("\n");
	}
	ui->snmpPeerIPListBrowser->setText(tmpStr);


	// SNMPv2c stuff
	ui->snmp2cCheck->setChecked(tmpSNMP.snmp2cOrgEnabled);
	if (tmpSNMP.snmp2cOrgEnabled) {
		ui->snmpCommStringLabel->setText(tmpSNMP.v2CommString);
	} else {
		ui->snmpCommStringLabel->setText("");
	}


	// SNMPv3 stuff
	ui->snmp3Check->setChecked(tmpSNMP.snmp3OrgEnabled);
	on_snmp3Check_clicked(tmpSNMP.snmp3OrgEnabled);
	if (tmpSNMP.snmp3OrgEnabled) {
		ui->snmpUserLabel->setText(tmpSNMP.v3User);
		ui->snmpAuthPassEdit->setText("");
		ui->snmpPrivPassEdit->setText("");
	} else {
		ui->snmpUserLabel->setText("");
	}


}

void MainWindow::displayOrgVPN(int orgIndex) {
	// display configuration for non-Meraki site-to-site VPN peers
	if (orgList.at(orgIndex)->getOrgVPNPeerNum() > 0) {
		// there are non-Meraki site-to-site VPN peers, show them
		ui->nonMVPNCheck->setChecked(true);
		on_nonMVPNCheck_clicked(true);

		qDebug() << orgList.at(orgIndex)->getOrgVPNPeerNum()
				 << "\t" << ui->nonMVPNPeersMenu->count();

		ui->nonMVPNPeersMenu->clear();	// remove all the items from QComboBox

		// TODO: for some reason this makes QComboBox go out of bounds
		//		for (int i = 0; i < orgList.at(orgIndex)->getOrgVPNPeerNum(); i++) {
		//			qDebug() << "i: " << i;
		//			ui->nonMVPNPeersMenu->addItem(orgList.at(orgIndex)->getOrgVPNPeer(i).peerName);
		//		}


		on_nonMVPNPeersMenu_currentIndexChanged(0);		// show the first item and update all the GUI


	} else {
		ui->nonMVPNCheck->setChecked(false);
		on_nonMVPNCheck_clicked(false);
		ui->nonMVPNPeersMenu->clear();

	}

}

void MainWindow::displayMSInfo(int orgIndex, int netIndex) {
	// show switches in the MS GUI tab
	// if netIndex is -1, show all the switches in the organization
	// otherwise only show switches in that network
	qDebug() << "\nMainWindow::displayMSInfo, orgIndex: " << orgIndex << "\tnetIndex: " << netIndex;

	int count = 0;	// count switches

	for (int i = 0; i < orgList.at(orgIndex)->getOrgInventorySize(); i++) {
		deviceInInventory tmp = orgList.at(orgIndex)->getOrgInventoryDevice(i);

		if (tmp.model.contains("MS")) {
			if (netIndex != -1 && orgList.at(orgIndex)->getNetwork(netIndex).netID == tmp.netID) {
				// check if the MS is in the appropriate network
				// the first condition will also avoid getting the element at index -1
				count++;
			} else if (netIndex == -1) {
				count++;
			}
		}
	}

	qDebug() << "count: " << count;
	if (count == 0) { return; }


	msListModel->setRowCount(count);


	// show stuff
	count = 0;
	for (int i = 0; i < orgList.at(orgIndex)->getOrgInventorySize(); i++) {
		deviceInInventory tmp = orgList.at(orgIndex)->getOrgInventoryDevice(i);

		if (tmp.model.contains("MS")) {
			if (netIndex != -1 && orgList.at(orgIndex)->getNetwork(netIndex).netID == tmp.netID) {
				msListModel->setItem(count, 0, new QStandardItem(tmp.mac));
				msListModel->setItem(count, 1, new QStandardItem(tmp.model));
				msListModel->setItem(count, 2, new QStandardItem(tmp.serial));
				msListModel->setItem(count, 3, new QStandardItem(tmp.publicIP));
				msListModel->setItem(count, 4, new QStandardItem(tmp.netID));
				count++;

			} else if (netIndex == -1) {
				msListModel->setItem(count, 0, new QStandardItem(tmp.mac));
				msListModel->setItem(count, 1, new QStandardItem(tmp.model));
				msListModel->setItem(count, 2, new QStandardItem(tmp.serial));
				msListModel->setItem(count, 3, new QStandardItem(tmp.publicIP));
				msListModel->setItem(count, 4, new QStandardItem(tmp.netID));
				count++;

			}
		}
	}

	ui->msSwitchesTable->resizeColumnsToContents();
	ui->msSwitchesTable->resizeRowsToContents();

}

void MainWindow::displayMSPort(int devIndex, int orgIndex) {
	// show switch port status of the device at devIndex
	qDebug() << "\nMainWindow::displayMSPort, orgIndex: " << orgIndex << "\tdevIndex: " << devIndex;

	int portCount = orgList.at(orgIndex)->getOrgInventoryDevice(devIndex).ports.size();
	msPortListModel->setRowCount(portCount);

	// show port info
	for (int i = 0; i < portCount; i++) {
		deviceInInventory tmpDevice = orgList.at(orgIndex)->getOrgInventoryDevice(devIndex);

		msPortListModel->setItem(i, 0, new QStandardItem(QString::number(tmpDevice.ports.at(i).number)));
		msPortListModel->setItem(i, 1, new QStandardItem(tmpDevice.ports.at(i).name));
		msPortListModel->setItem(i, 2, new QStandardItem(tmpDevice.ports.at(i).tags));
		msPortListModel->setItem(i, 3, new QStandardItem(QString::number(tmpDevice.ports.at(i).enabled)));
		msPortListModel->setItem(i, 4, new QStandardItem(QString::number(tmpDevice.ports.at(i).poeEnabled)));
		msPortListModel->setItem(i, 5, new QStandardItem(tmpDevice.ports.at(i).type));
		msPortListModel->setItem(i, 6, new QStandardItem(QString::number(tmpDevice.ports.at(i).nativeVlan)));
		msPortListModel->setItem(i, 7, new QStandardItem(QString::number(tmpDevice.ports.at(i).voiceVlan)));
		msPortListModel->setItem(i, 8, new QStandardItem(tmpDevice.ports.at(i).allowedVLANs));
		msPortListModel->setItem(i, 9, new QStandardItem(QString::number(tmpDevice.ports.at(i).isolationEnabled)));
		msPortListModel->setItem(i, 10, new QStandardItem(QString::number(tmpDevice.ports.at(i).rstpEnabled)));
		msPortListModel->setItem(i, 11, new QStandardItem(tmpDevice.ports.at(i).stpGuard));
		msPortListModel->setItem(i, 12, new QStandardItem(tmpDevice.ports.at(i).accessPolicyNumber));

	}

	ui->msSwitchPortsTable->resizeColumnsToContents();
	ui->msSwitchPortsTable->resizeRowsToContents();

}

void MainWindow::displayMXInfo(int orgIndex, int netIndex) {
	// show MXs in the organization, if a netIndex is selected, only show MXs in that network
	qDebug() << "\nMainWindow::displayMXInfo, orgIndex: " << orgIndex << "\tnetIndex: " << netIndex;

	int count = 0;	// count MXs

	for (int i = 0; i < orgList.at(orgIndex)->getOrgInventorySize(); i++) {
		deviceInInventory tmp = orgList.at(orgIndex)->getOrgInventoryDevice(i);

		if (tmp.model.contains("MX")) {
			if (netIndex != -1 && orgList.at(orgIndex)->getNetwork(netIndex).netID == tmp.netID) {
				// check if the MS is in the appropriate network
				// the first condition will also avoid getting the element at index -1
				count++;
			} else if (netIndex == -1) {
				count++;
			}
		}
	}

	qDebug() << "count: " << count;
	if (count == 0) { return; }


	mxListModel->setRowCount(count);

	// show stuff
	count = 0;
	for (int i = 0; i < orgList.at(orgIndex)->getOrgInventorySize(); i++) {
		deviceInInventory tmp = orgList.at(orgIndex)->getOrgInventoryDevice(i);

		if (tmp.model.contains("MX")) {
			if (netIndex != -1 && orgList.at(orgIndex)->getNetwork(netIndex).netID == tmp.netID) {
				mxListModel->setItem(count, 0, new QStandardItem(tmp.mac));
				mxListModel->setItem(count, 1, new QStandardItem(tmp.model));
				mxListModel->setItem(count, 2, new QStandardItem(tmp.serial));
				mxListModel->setItem(count, 3, new QStandardItem(tmp.publicIP));
				mxListModel->setItem(count, 4, new QStandardItem(tmp.netID));
				count++;

			} else if (netIndex == -1) {
				mxListModel->setItem(count, 0, new QStandardItem(tmp.mac));
				mxListModel->setItem(count, 1, new QStandardItem(tmp.model));
				mxListModel->setItem(count, 2, new QStandardItem(tmp.serial));
				mxListModel->setItem(count, 3, new QStandardItem(tmp.publicIP));
				mxListModel->setItem(count, 4, new QStandardItem(tmp.netID));
				count++;

			}
		}
	}

	ui->mxDeviceTable->resizeColumnsToContents();
	ui->mxDeviceTable->resizeRowsToContents();

}

void MainWindow::displayMXL3Rules(int devIndex, int orgIndex) {
	// show L3 firewall rules of this MX
	qDebug() << "\nMainWindow::displayMXL3Rules, orgIndex: " << orgIndex << "\tdevIndex: " << devIndex;

	int rulesCount = orgList.at(orgIndex)->getOrgInventoryDevice(devIndex).rules.size();
	mxL3RulesModel->setRowCount(rulesCount);

	// show rule info
	for (int i = 0; i < rulesCount; i++) {
		deviceInInventory tmpDevice = orgList.at(orgIndex)->getOrgInventoryDevice(devIndex);

		mxL3RulesModel->setItem(i, 0, new QStandardItem(tmpDevice.rules.at(i).policy));
		mxL3RulesModel->setItem(i, 1, new QStandardItem(tmpDevice.rules.at(i).protocol));
		mxL3RulesModel->setItem(i, 2, new QStandardItem(tmpDevice.rules.at(i).srcCidr));
		mxL3RulesModel->setItem(i, 3, new QStandardItem(tmpDevice.rules.at(i).srcPort));
		mxL3RulesModel->setItem(i, 4, new QStandardItem(tmpDevice.rules.at(i).destCidr));
		mxL3RulesModel->setItem(i, 5, new QStandardItem(tmpDevice.rules.at(i).destPort));
		mxL3RulesModel->setItem(i, 6, new QStandardItem(QString::number(tmpDevice.rules.at(i).syslogEnabled)));
		mxL3RulesModel->setItem(i, 7, new QStandardItem(tmpDevice.rules.at(i).comment));

	}

	ui->mxL3FirewallTable->resizeColumnsToContents();
	ui->mxL3FirewallTable->resizeRowsToContents();

}

void MainWindow::displaySMDevices(int orgIndex, int netIndex) {
	// show SM devices in the organization
	// if a netIndex is selected, only show SM devices in that network
	qDebug() << "\nMainWindow::displaySMDevices, orgIndex: " << orgIndex << "\tnetIndex: " << netIndex;

	int count = 0;	// count devices that need to be shown

	// if netIndex is -1, go through all the combined or systems manager network
	// in the organization
	if (netIndex == -1) {
		for (int i = 0; i < orgList.at(orgIndex)->getNetworksNum(); i++) {
			if (orgList.at(orgIndex)->getNetwork(i).netType == "systems manager"
					|| orgList.at(orgIndex)->getNetwork(i).netType == "combined") {
				count += orgList.at(orgIndex)->getNetwork(i).smDevices.size();

			}
		}

	} else {
		// only get device count in the particular network
		count = orgList.at(orgIndex)->getNetwork(netIndex).smDevices.size();
	}

	qDebug() << "count: " << count;
	if (count == 0) { return; }

	smDeviceListModel->setRowCount(count);


	// show SM devices
	count = 0;
	if (netIndex == -1) {
		for (int i = 0; i < orgList.at(orgIndex)->getNetworksNum(); i++) {
			if (orgList.at(orgIndex)->getNetwork(i).netType == "systems manager"
					|| orgList.at(orgIndex)->getNetwork(i).netType == "combined") {

				for (int j = 0; j < orgList.at(orgIndex)->getNetwork(i).smDevices.size(); j++) {
					// go through every SM device in the network
					smDevice tmpSMDevice = orgList.at(orgIndex)->getNetwork(i).smDevices.at(j);

					smDeviceListModel->setItem(count, 0, new QStandardItem(tmpSMDevice.id));
					smDeviceListModel->setItem(count, 1, new QStandardItem(tmpSMDevice.name));

					QString tmpTags;
					for (int t = 0; t < tmpSMDevice.tags.size(); t++) {
						tmpTags.append(tmpSMDevice.tags.at(t));
						if (t < tmpSMDevice.tags.size()-1) { tmpTags.append(", ");}
					}
					smDeviceListModel->setItem(count, 2, new QStandardItem(tmpTags));

					smDeviceListModel->setItem(count, 3, new QStandardItem(tmpSMDevice.ssid));
					smDeviceListModel->setItem(count, 4, new QStandardItem(tmpSMDevice.wifiMac));
					smDeviceListModel->setItem(count, 5, new QStandardItem(tmpSMDevice.osName));
					smDeviceListModel->setItem(count, 6, new QStandardItem(tmpSMDevice.systemModel));
					smDeviceListModel->setItem(count, 7, new QStandardItem(tmpSMDevice.uuid));
					smDeviceListModel->setItem(count, 8, new QStandardItem(tmpSMDevice.serialNumber));
					smDeviceListModel->setItem(count, 9, new QStandardItem(tmpSMDevice.ip));
					smDeviceListModel->setItem(count, 10, new QStandardItem(tmpSMDevice.systemType));
					smDeviceListModel->setItem(count, 11, new QStandardItem(QString::number(tmpSMDevice.availableDeviceCapacity)));
					smDeviceListModel->setItem(count, 12, new QStandardItem(tmpSMDevice.kioskAppName));
					smDeviceListModel->setItem(count, 13, new QStandardItem(tmpSMDevice.biosVersion));
					smDeviceListModel->setItem(count, 14, new QStandardItem(QString::number(tmpSMDevice.lastConnected)));
					smDeviceListModel->setItem(count, 15, new QStandardItem(QString::number(tmpSMDevice.missingAppsCount)));
					smDeviceListModel->setItem(count, 16, new QStandardItem(tmpSMDevice.userSuppliedAddress));
					smDeviceListModel->setItem(count, 17, new QStandardItem(tmpSMDevice.location));
					smDeviceListModel->setItem(count, 18, new QStandardItem(tmpSMDevice.lastUser));
					smDeviceListModel->setItem(count, 19, new QStandardItem(tmpSMDevice.publicIp));
					smDeviceListModel->setItem(count, 20, new QStandardItem(tmpSMDevice.phoneNumber));
					smDeviceListModel->setItem(count, 21, new QStandardItem(tmpSMDevice.diskInfoJson));
					smDeviceListModel->setItem(count, 22, new QStandardItem(QString::number(tmpSMDevice.deviceCapacity)));
					smDeviceListModel->setItem(count, 23, new QStandardItem(QString::number(tmpSMDevice.isManaged)));
					smDeviceListModel->setItem(count, 24, new QStandardItem(QString::number(tmpSMDevice.hadMdm)));
					smDeviceListModel->setItem(count, 25, new QStandardItem(QString::number(tmpSMDevice.isSupervised)));
					smDeviceListModel->setItem(count, 26, new QStandardItem(tmpSMDevice.meid));
					smDeviceListModel->setItem(count, 27, new QStandardItem(tmpSMDevice.imei));
					smDeviceListModel->setItem(count, 28, new QStandardItem(tmpSMDevice.iccid));
					smDeviceListModel->setItem(count, 29, new QStandardItem(tmpSMDevice.simCarrierNetwork));
					smDeviceListModel->setItem(count, 30, new QStandardItem(QString::number(tmpSMDevice.cellularDataUsed)));
					smDeviceListModel->setItem(count, 31, new QStandardItem(QString::number(tmpSMDevice.isHotspotEnabled)));
					smDeviceListModel->setItem(count, 32, new QStandardItem(QString::number(tmpSMDevice.createdAt)));
					smDeviceListModel->setItem(count, 33, new QStandardItem(tmpSMDevice.batteryEstCharge));
					smDeviceListModel->setItem(count, 34, new QStandardItem(QString::number(tmpSMDevice.quarantined)));
					smDeviceListModel->setItem(count, 35, new QStandardItem(tmpSMDevice.avName));
					smDeviceListModel->setItem(count, 36, new QStandardItem(tmpSMDevice.avRunning));
					smDeviceListModel->setItem(count, 37, new QStandardItem(tmpSMDevice.asName));
					smDeviceListModel->setItem(count, 38, new QStandardItem(tmpSMDevice.fwName));
					smDeviceListModel->setItem(count, 39, new QStandardItem(QString::number(tmpSMDevice.isRooted)));
					smDeviceListModel->setItem(count, 40, new QStandardItem(QString::number(tmpSMDevice.loginRequired)));
					smDeviceListModel->setItem(count, 41, new QStandardItem(QString::number(tmpSMDevice.screenLockEnabled)));
					smDeviceListModel->setItem(count, 42, new QStandardItem(tmpSMDevice.screenLockDelay));
					smDeviceListModel->setItem(count, 43, new QStandardItem(QString::number(tmpSMDevice.autoLoginDisabled)));
					smDeviceListModel->setItem(count, 44, new QStandardItem(QString::number(tmpSMDevice.hasMdm)));
					smDeviceListModel->setItem(count, 45, new QStandardItem(QString::number(tmpSMDevice.hasDesktopAgent)));
					smDeviceListModel->setItem(count, 46, new QStandardItem(QString::number(tmpSMDevice.diskEncryptionEnabled)));
					smDeviceListModel->setItem(count, 47, new QStandardItem(tmpSMDevice.hardwareEncryptionCaps));
					smDeviceListModel->setItem(count, 48, new QStandardItem(QString::number(tmpSMDevice.passCodeLock)));

					count++;

				}
			}
		}

	} else {
		// only show SM devices for the particular network
		for (int j = 0; j < orgList.at(orgIndex)->getNetwork(netIndex).smDevices.size(); j++) {
			// go through every SM device in the network
			smDevice tmpSMDevice = orgList.at(orgIndex)->getNetwork(netIndex).smDevices.at(j);

			smDeviceListModel->setItem(count, 0, new QStandardItem(tmpSMDevice.id));
			smDeviceListModel->setItem(count, 1, new QStandardItem(tmpSMDevice.name));

			QString tmpTags;
			for (int t = 0; t < tmpSMDevice.tags.size(); t++) {
				tmpTags.append(tmpSMDevice.tags.at(t));
				if (t < tmpSMDevice.tags.size()-1) { tmpTags.append(", ");}
			}
			smDeviceListModel->setItem(count, 2, new QStandardItem(tmpTags));
			smDeviceListModel->setItem(count, 3, new QStandardItem(tmpSMDevice.ssid));
			smDeviceListModel->setItem(count, 4, new QStandardItem(tmpSMDevice.wifiMac));
			smDeviceListModel->setItem(count, 5, new QStandardItem(tmpSMDevice.osName));
			smDeviceListModel->setItem(count, 6, new QStandardItem(tmpSMDevice.systemModel));
			smDeviceListModel->setItem(count, 7, new QStandardItem(tmpSMDevice.uuid));
			smDeviceListModel->setItem(count, 8, new QStandardItem(tmpSMDevice.serialNumber));
			smDeviceListModel->setItem(count, 9, new QStandardItem(tmpSMDevice.ip));
			smDeviceListModel->setItem(count, 10, new QStandardItem(tmpSMDevice.systemType));
			smDeviceListModel->setItem(count, 11, new QStandardItem(QString::number(tmpSMDevice.availableDeviceCapacity)));
			smDeviceListModel->setItem(count, 12, new QStandardItem(tmpSMDevice.kioskAppName));
			smDeviceListModel->setItem(count, 13, new QStandardItem(tmpSMDevice.biosVersion));
			smDeviceListModel->setItem(count, 14, new QStandardItem(QString::number(tmpSMDevice.lastConnected)));
			smDeviceListModel->setItem(count, 15, new QStandardItem(QString::number(tmpSMDevice.missingAppsCount)));
			smDeviceListModel->setItem(count, 16, new QStandardItem(tmpSMDevice.userSuppliedAddress));
			smDeviceListModel->setItem(count, 17, new QStandardItem(tmpSMDevice.location));
			smDeviceListModel->setItem(count, 18, new QStandardItem(tmpSMDevice.lastUser));
			smDeviceListModel->setItem(count, 19, new QStandardItem(tmpSMDevice.publicIp));
			smDeviceListModel->setItem(count, 20, new QStandardItem(tmpSMDevice.phoneNumber));
			smDeviceListModel->setItem(count, 21, new QStandardItem(tmpSMDevice.diskInfoJson));
			smDeviceListModel->setItem(count, 22, new QStandardItem(QString::number(tmpSMDevice.deviceCapacity)));
			smDeviceListModel->setItem(count, 23, new QStandardItem(QString::number(tmpSMDevice.isManaged)));
			smDeviceListModel->setItem(count, 24, new QStandardItem(QString::number(tmpSMDevice.hadMdm)));
			smDeviceListModel->setItem(count, 25, new QStandardItem(QString::number(tmpSMDevice.isSupervised)));
			smDeviceListModel->setItem(count, 26, new QStandardItem(tmpSMDevice.meid));
			smDeviceListModel->setItem(count, 27, new QStandardItem(tmpSMDevice.imei));
			smDeviceListModel->setItem(count, 28, new QStandardItem(tmpSMDevice.iccid));
			smDeviceListModel->setItem(count, 29, new QStandardItem(tmpSMDevice.simCarrierNetwork));
			smDeviceListModel->setItem(count, 30, new QStandardItem(QString::number(tmpSMDevice.cellularDataUsed)));
			smDeviceListModel->setItem(count, 31, new QStandardItem(QString::number(tmpSMDevice.isHotspotEnabled)));
			smDeviceListModel->setItem(count, 32, new QStandardItem(QString::number(tmpSMDevice.createdAt)));
			smDeviceListModel->setItem(count, 33, new QStandardItem(tmpSMDevice.batteryEstCharge));
			smDeviceListModel->setItem(count, 34, new QStandardItem(QString::number(tmpSMDevice.quarantined)));
			smDeviceListModel->setItem(count, 35, new QStandardItem(tmpSMDevice.avName));
			smDeviceListModel->setItem(count, 36, new QStandardItem(tmpSMDevice.avRunning));
			smDeviceListModel->setItem(count, 37, new QStandardItem(tmpSMDevice.asName));
			smDeviceListModel->setItem(count, 38, new QStandardItem(tmpSMDevice.fwName));
			smDeviceListModel->setItem(count, 39, new QStandardItem(QString::number(tmpSMDevice.isRooted)));
			smDeviceListModel->setItem(count, 40, new QStandardItem(QString::number(tmpSMDevice.loginRequired)));
			smDeviceListModel->setItem(count, 41, new QStandardItem(QString::number(tmpSMDevice.screenLockEnabled)));
			smDeviceListModel->setItem(count, 42, new QStandardItem(tmpSMDevice.screenLockDelay));
			smDeviceListModel->setItem(count, 43, new QStandardItem(QString::number(tmpSMDevice.autoLoginDisabled)));
			smDeviceListModel->setItem(count, 44, new QStandardItem(QString::number(tmpSMDevice.hasMdm)));
			smDeviceListModel->setItem(count, 45, new QStandardItem(QString::number(tmpSMDevice.hasDesktopAgent)));
			smDeviceListModel->setItem(count, 46, new QStandardItem(QString::number(tmpSMDevice.diskEncryptionEnabled)));
			smDeviceListModel->setItem(count, 47, new QStandardItem(tmpSMDevice.hardwareEncryptionCaps));
			smDeviceListModel->setItem(count, 48, new QStandardItem(QString::number(tmpSMDevice.passCodeLock)));

			count++;

		}
	}

	ui->smDevicesTable->resizeColumnsToContents();
	ui->smDevicesTable->resizeRowsToContents();

}




void MainWindow::replyFinished(QNetworkReply *reply) {
	qDebug() << "\nMainWindow::replyFinished(...)";
	apiHelpObj->processQuery(reply);

	// do next event in the queue, if there are more events in queue
	if (apiHelpObj->getEventQueueSize() > apiHelpObj->getEventIndex()) {
		eventRequest tmp = apiHelpObj->getNextEvent();
		apiHelpObj->runQuery(tmp);
	}

}



void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
	// distinguish on whether an organization or a network was double clicked
	// run query for that organization / network
	//	updateNetworkUI(QModelIndex(index));
	eventRequest tmp;
	if (index.parent().data() == QVariant::Invalid) {
		// an organization was selected in the tree view
		tmp.orgIndex = index.row();
		tmp.netIndex = -1;
		tmp.urlListIndex = 27;	// get networks in the org
		apiHelpObj->putEventInQueue(tmp);	// get networks in the org


		//		apiHelpObj->runQuery(27, tmpOrgIndex, tmpNetIndex);	// get networks in the org

		updateOrgUI(tmp.orgIndex);

	} else {
		// a network was selected in the tree view
		tmp.orgIndex = index.parent().row();
		tmp.netIndex = index.row();
		//		updateNetworkUI(QModelIndex(index));
	}


}

void MainWindow::on_nonMVPNCheck_clicked(bool checked) {
	// make non-Meraki VPN related stuff non-editable
	ui->nonMVPNPeersMenu->setEnabled(checked);
	ui->nonMVPNAddPeerButton->setEnabled(checked);
	ui->nonMVPNPeerNameEdit->setEnabled(checked);
	ui->nonMVPNPeerIPEdit->setEnabled(checked);
	ui->nonMVPNSecretEdit->setEnabled(checked);
	ui->nonMVPNSecretCheck->setEnabled(checked);
	ui->nonMVPNSubnetBrowser->setEnabled(checked);
	ui->nonMVPNTagsBrowser->setEnabled(checked);

}

void MainWindow::on_debugButton_clicked() {
	eventRequest tmp;
	tmp.urlListIndex = 49;
	tmp.orgIndex = 0;
	tmp.netIndex = -1;
	apiHelpObj->runQuery(tmp);	// GET /organizations/[organizationId]/inventory

}

void MainWindow::on_treeView_clicked(const QModelIndex &index) {
	// update info shown for the org

}

void MainWindow::on_snmp3Check_clicked(bool checked) {
	// make SNMP related stuff editable or not
	ui->snmpAuthMenu->setEnabled(checked);
	ui->snmpPrivMenu->setEnabled(checked);
	ui->snmpAuthPassEdit->setEnabled(checked);
	ui->snmpAuthPassCheck->setEnabled(checked);
	ui->snmpPrivPassEdit->setEnabled(checked);
	ui->snmpPrivPassCheck->setEnabled(checked);
	ui->snmpPeerIPListBrowser->setEnabled(checked);

}

void MainWindow::on_refreshOrgsButton_clicked() {
	// query for all orgs associated with this API key
	eventRequest tmp;
	tmp.urlListIndex = 41;
	tmp.orgIndex = -1;
	tmp.netIndex = -1;
	apiHelpObj->putEventInQueue(tmp, true);	// GET /organizations
}

void MainWindow::on_tabWidget_currentChanged(int index) {
	// run the appropriate queries to show things in the GUI
	qDebug() << "tab: " << index << "\t" << ui->tabWidget->tabText(index);

	if (currOrgIndex == -1) {
		// this happens usually if the program has just started, or something bad has happened
		qDebug() << "currOrgIndex: " << currOrgIndex;
		return;
	}

	eventRequest tmp;
	tmp.orgIndex = currOrgIndex;

	switch (index) {
		case 0: {
			// Administration tab
			tmp.urlListIndex = 0;	// GET /organizations/[organizationId]/admins
			apiHelpObj->putEventInQueue(tmp);

			break;
		}

		case 1: {
			// Organization Settings tab
			tmp.urlListIndex = 49;	// GET /organizations/[organizationId]/snmp
			apiHelpObj->putEventInQueue(tmp);

			tmp.urlListIndex = 51;	// GET /organizations/[organizationId]/thirdPartyVPNPeers
			apiHelpObj->putEventInQueue(tmp);


			break;
		}

		case 2: {
			// Licensing / Inventory tab
			tmp.urlListIndex = 47;	// GET /organizations/[organizationId]/licenseState
			apiHelpObj->putEventInQueue(tmp);

			tmp.urlListIndex = 48;	// GET /organizations/[organizationId]/inventory
			apiHelpObj->putEventInQueue(tmp);
			break;
		}

		case 3: {
			// MS tab, show switches in the inventory
			if (orgList.at(currOrgIndex)->getOrgInventorySize() == 0) {
				// org inventory is empty, fetch it
				tmp.urlListIndex = 48;	// GET /organizations/[organizationId]/inventory
				apiHelpObj->putEventInQueue(tmp);
			}

			displayMSInfo(currOrgIndex);
			break;
		}

		case 4: {
			// MX tab, show MXs in inventory and MX L3 firewall rules
			if (orgList.at(currOrgIndex)->getOrgInventorySize() == 0) {
				// org inventory is empty, fetch it
				tmp.urlListIndex = 48;	// GET /organizations/[organizationId]/inventory
				apiHelpObj->putEventInQueue(tmp);
			}

			displayMXInfo(currOrgIndex);
			break;
		}

		case 8: {
			// SM tab, show full list of SM devices
			if (orgList.at(currOrgIndex)->getOrgInventorySize() == 0) {
				// org inventory is empty, fetch it
				tmp.urlListIndex = 48;	// GET /organizations/[organizationId]/inventory
				apiHelpObj->putEventInQueue(tmp);
			}

			// get SM devices in the network/org
			// only check "systems manager" and "combined" network types
			for (int i = 0; i < orgList.at(currOrgIndex)->getNetworksNum(); i++) {
				if (orgList.at(currOrgIndex)->getNetwork(i).netType == "systems manager"
						|| orgList.at(currOrgIndex)->getNetwork(i).netType == "combined") {
					tmp.urlListIndex = 82;	// GET /networks/[networkId]/sm/devices
					tmp.netIndex = i;
					apiHelpObj->putEventInQueue(tmp);
				}
			}

			displaySMDevices(currOrgIndex);
			break;
		}

	}

}

void MainWindow::on_adminsTableView_clicked(const QModelIndex &index) {
	// show details of the administrator clicked in the access and tag table views
	qDebug() << "on_adminsTableView_clicked, row: " << index.row();

	adminStruct tmpAdmin = orgList.at(currOrgIndex)->getAdmin(index.row());


	// display list of networks admin has access to, if this is a network admin
	// also display privileges of camera-only administrators
	QStandardItemModel *adminListModel2 = new QStandardItemModel(tmpAdmin.nets.size(), 2, this);
	adminListModel2->setHeaderData(0, Qt::Horizontal, QString("Net ID"));
	adminListModel2->setHeaderData(1, Qt::Horizontal, QString("Access"));

	int j = 0;
	for (int j = 0; j < tmpAdmin.nets.size(); j++) {
		adminNetPermission tmpPerm = tmpAdmin.nets.at(j);
		adminListModel2->setItem(j, 0, new QStandardItem(tmpPerm.netID));
		adminListModel2->setItem(j, 1, new QStandardItem(tmpPerm.accessLevel));
	}

	// for camera-only admins
	if (tmpAdmin.cNets.size() > 0) {
		adminListModel2->setColumnCount(3);
		adminListModel2->setHeaderData(2, Qt::Horizontal, QString("network_type"));

		for (j = j; j < tmpAdmin.cNets.size(); j++) {
			adminNetPermission tmpCPerm = tmpAdmin.cNets.at(j);
			adminListModel2->setItem(j, 0, new QStandardItem(tmpCPerm.netID));
			adminListModel2->setItem(j, 1, new QStandardItem(tmpCPerm.accessLevel));
			adminListModel2->setItem(j, 2, new QStandardItem(tmpCPerm.networkType));
		}
	}


	ui->adminsTableView_2->setModel(adminListModel2);
	ui->adminsTableView_2->resizeColumnsToContents();
	ui->adminsTableView_2->resizeRowsToContents();



	// display list of tags assigned to administrator
	QStandardItemModel *adminListModel3 = new QStandardItemModel(tmpAdmin.tags.size(), 2, this);
	adminListModel3->setHeaderData(0, Qt::Horizontal, QString("Tag"));
	adminListModel3->setHeaderData(1, Qt::Horizontal, QString("Access"));

	for (int j = 0; j < tmpAdmin.tags.size(); j++) {
		adminTag tmpTag = tmpAdmin.tags.at(j);
		adminListModel3->setItem(j, 0, new QStandardItem(tmpTag.tag));
		adminListModel3->setItem(j, 1, new QStandardItem(tmpTag.adminAccessLevel));
	}

	ui->adminsTableView_3->setModel(adminListModel3);
	ui->adminsTableView_3->resizeColumnsToContents();
	ui->adminsTableView_3->resizeRowsToContents();


	// DEBUG
	qDebug() << tmpAdmin.name;
	for (int j = 0; j < tmpAdmin.nets.size(); j++) {
		qDebug() << tmpAdmin.nets.at(j).netID << "\t" << tmpAdmin.nets.at(j).accessLevel;
	}
	for (int j = 0; j < tmpAdmin.tags.size(); j++) {
		qDebug() << tmpAdmin.tags.at(j).tag << "\t" << tmpAdmin.tags.at(j).adminAccessLevel;
	}


}

void MainWindow::on_nonMVPNPeersMenu_currentIndexChanged(int index) {
	// show data from the appropriate VPN peer
	nonMerakiVPNPeer tmpPeer = orgList.at(currOrgIndex)->getOrgVPNPeer(index);

	ui->nonMVPNPeerNameEdit->setText(tmpPeer.peerName);
	ui->nonMVPNPeerIPEdit->setText(tmpPeer.peerPublicIP);
	ui->nonMVPNSecretEdit->setText(tmpPeer.secret);

	// private subnets
	QString tmpStr;
	for (int i = 0; i < tmpPeer.privateSubnets.size(); i++) {
		tmpStr.append(tmpPeer.privateSubnets.at(i)).append("\n");
	}
	ui->nonMVPNSubnetBrowser->setText(tmpStr);

	// tags
	tmpStr = "";
	for (int i = 0; i < tmpPeer.tags.size()-1; i++) {
		tmpStr.append(tmpPeer.tags.at(i)).append(", ");
	}
	tmpStr.append(tmpPeer.tags.at(tmpPeer.tags.size()-1));
	ui->nonMVPNTagsBrowser->setText(tmpStr);

}

void MainWindow::on_nonMVPNSecretCheck_clicked(bool checked) {
	if (checked) {
		ui->nonMVPNSecretEdit->setEchoMode(QLineEdit::Normal);
	} else {
		ui->nonMVPNSecretEdit->setEchoMode(QLineEdit::Password);
	}
}

void MainWindow::on_snmpAuthPassCheck_clicked(bool checked) {
	if (checked) {
		ui->snmpAuthPassEdit->setEchoMode(QLineEdit::Normal);
	} else {
		ui->snmpAuthPassEdit->setEchoMode(QLineEdit::Password);
	}
}

void MainWindow::on_snmpPrivPassCheck_clicked(bool checked) {
	if (checked) {
		ui->snmpPrivPassEdit->setEchoMode(QLineEdit::Normal);
	} else {
		ui->snmpPrivPassEdit->setEchoMode(QLineEdit::Password);
	}
}

void MainWindow::on_msSwitchesTable_clicked(const QModelIndex &index) {
	// display switch port information of the appropriate switch
	qDebug() << "\nMainWindow::on_msSwitchesTable_clicked(), row: " << index.row();
	qDebug() << msListModel->item(index.row(), 2)->text();


	deviceInInventory tmpDev = orgList.at(currOrgIndex)->getOrgDeviceFromSerial(msListModel->item(index.row(), 2)->text());

	// run query to get info for switch ports
	eventRequest tmp;
	tmp.orgIndex = currOrgIndex;
	tmp.urlListIndex = 93;	// GET /devices/[serial]/switchPorts
	tmp.deviceSerial = tmpDev.serial;
	apiHelpObj->putEventInQueue(tmp);

}

void MainWindow::on_mxDeviceTable_clicked(const QModelIndex &index) {
	qDebug() << "\nMainWindow::on_mxDeviceTable_clicked(), row: " << index.row();
	qDebug() << mxListModel->item(index.row(), 2)->text();

	deviceInInventory tmpDev = orgList.at(currOrgIndex)->getOrgDeviceFromSerial(mxListModel->item(index.row(), 2)->text());

	// run query to get L3 firewall rule for this MX
	eventRequest tmp;
	tmp.orgIndex = currOrgIndex;
	tmp.urlListIndex = 20;	// GET /networks/[networkId]/l3FirewallRules
	tmp.netIndex = orgList.at(currOrgIndex)->getIndexOfNetwork(tmpDev.netID);
	tmp.deviceSerial = tmpDev.serial;
	apiHelpObj->putEventInQueue(tmp);

}
