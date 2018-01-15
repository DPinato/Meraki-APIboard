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


	// I am doing this so I do not have to leave my API key in this code

//	apiKey = getAPIkeyFromFile(QString("/home/davide/Desktop/apikey.txt"));
	apiKey = getAPIkeyFromFile(QString("D:\\Programming\\meraki_api_key.txt"));
	qDebug() << apiKey;

	urlListFile = QString("D:\\Programming\\Qt\\Meraki-APIboard\\URL_list.txt");
	qDebug() << urlListFile;


	apiHelpObj = new APIHelper(apiKey, this);
	apiHelpObj->readURLListFromFile(urlListFile);

	currOrgIndex = -1;



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
	// columns are <name | email | org_privilege | admin_id | networks | tags>
	QStandardItemModel *adminTree = new QStandardItemModel(orgList.at(orgIndex)->getAdminListSize(), 4, this);

	// set column headers
	adminTree->setHeaderData(0, Qt::Horizontal, QString("Name"));
	adminTree->setHeaderData(1, Qt::Horizontal, QString("E-mail"));
	adminTree->setHeaderData(2, Qt::Horizontal, QString("Org Privilege"));
	adminTree->setHeaderData(3, Qt::Horizontal, QString("ID"));


	for (int i = 0; i < orgList.at(orgIndex)->getAdminListSize(); i++) {
		adminStruct tmpAdmin = orgList.at(orgIndex)->getAdmin(i);

		adminTree->setItem(i, 0, new QStandardItem(tmpAdmin.name));
		adminTree->setItem(i, 1, new QStandardItem(tmpAdmin.email));
		adminTree->setItem(i, 2, new QStandardItem(tmpAdmin.orgAccess));
		adminTree->setItem(i, 3, new QStandardItem(tmpAdmin.id));


		// display list of networks admin has access to, if this is a network admin
		// also display privileges of camera-only administrators
			QStandardItemModel *adminTree2 = new QStandardItemModel(orgList.at(orgIndex)->getAdmin(i).nets.size(), 2, this);
			adminTree2->setHeaderData(0, Qt::Horizontal, QString("Net ID"));
			adminTree2->setHeaderData(1, Qt::Horizontal, QString("Access"));

			int j = 0;	// I think that a network admin can be a network admin and camera-only admin in different networks
			for (j = 0; j < tmpAdmin.nets.size(); j++) {
				adminNetPermission tmpPerm = tmpAdmin.nets.at(j);
				adminTree2->setItem(j, 0, new QStandardItem(tmpPerm.netID));
				adminTree2->setItem(j, 1, new QStandardItem(tmpPerm.accessLevel));
			}

			// for camera-only admins
			if (tmpAdmin.cNets.size() > 0) {
				adminTree2->setColumnCount(3);
				adminTree2->setHeaderData(2, Qt::Horizontal, QString("network_type"));

				for (j = j; j < tmpAdmin.cNets.size(); j++) {
					adminNetPermission tmpCPerm = tmpAdmin.cNets.at(j);
					adminTree2->setItem(j, 0, new QStandardItem(tmpCPerm.netID));
					adminTree2->setItem(j, 1, new QStandardItem(tmpCPerm.accessLevel));
					adminTree2->setItem(j, 2, new QStandardItem(tmpCPerm.networkType));
				}
			}


			ui->adminsTableView_2->setModel(adminTree2);
			ui->adminsTableView_2->resizeColumnsToContents();
			ui->adminsTableView_2->resizeRowsToContents();


		// display list of tags assigned to administrator
			QStandardItemModel *adminTree3 = new QStandardItemModel(orgList.at(orgIndex)->getAdmin(i).tags.size(), 2, this);
			adminTree3->setHeaderData(0, Qt::Horizontal, QString("Tag"));
			adminTree3->setHeaderData(1, Qt::Horizontal, QString("Access"));

			for (int j = 0; j < tmpAdmin.tags.size(); j++) {
				adminTag tmpTag = tmpAdmin.tags.at(j);
				adminTree3->setItem(j, 0, new QStandardItem(tmpTag.tag));
				adminTree3->setItem(j, 1, new QStandardItem(tmpTag.adminAccessLevel));
			}

			ui->adminsTableView_3->setModel(adminTree3);
			ui->adminsTableView_3->resizeColumnsToContents();
			ui->adminsTableView_3->resizeRowsToContents();


	}

	ui->adminsTableView->setModel(adminTree);
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
	QStandardItemModel *inventoryModel = new QStandardItemModel(orgList.at(orgIndex)->getOrgInventorySize(), 6, this);

	// columns are <model | mac | serial | publicIp | claimedAt | networkId>
	inventoryModel->setHeaderData(0, Qt::Horizontal, QString("model"));
	inventoryModel->setHeaderData(1, Qt::Horizontal, QString("mac"));
	inventoryModel->setHeaderData(2, Qt::Horizontal, QString("serial"));
	inventoryModel->setHeaderData(3, Qt::Horizontal, QString("publicIp"));
	inventoryModel->setHeaderData(4, Qt::Horizontal, QString("claimedAt"));
	inventoryModel->setHeaderData(5, Qt::Horizontal, QString("networkId"));


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


	ui->orgInventoryView->setModel(inventoryModel);
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


	msListModel = new QStandardItemModel(count, 5, this);

	// columns are | MAC | Model | Serial | public IP | network |
	msListModel->setHeaderData(0, Qt::Horizontal, QString("MAC"));
	msListModel->setHeaderData(1, Qt::Horizontal, QString("Model"));
	msListModel->setHeaderData(2, Qt::Horizontal, QString("Serial"));
	msListModel->setHeaderData(3, Qt::Horizontal, QString("Public IP"));
	msListModel->setHeaderData(4, Qt::Horizontal, QString("Network"));


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

	ui->msSwitchesTable->setModel(msListModel);
	ui->msSwitchesTable->resizeColumnsToContents();
	ui->msSwitchesTable->resizeRowsToContents();

}

void MainWindow::displayMSPort(int devIndex, int orgIndex) {
	// show switch port status of the device at devIndex
	qDebug() << "\nMainWindow::displayMSPort, orgIndex: " << orgIndex << "\tdevIndex: " << devIndex;

	int portCount = orgList.at(orgIndex)->getOrgInventoryDevice(devIndex).ports.size();
	msPortListModel = new QStandardItemModel(portCount, 13, this);

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


	ui->msSwitchPortsTable->setModel(msPortListModel);
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


	mxListModel = new QStandardItemModel(count, 5, this);

	// columns are | MAC | Model | Serial | public IP | network |
	mxListModel->setHeaderData(0, Qt::Horizontal, QString("MAC"));
	mxListModel->setHeaderData(1, Qt::Horizontal, QString("Model"));
	mxListModel->setHeaderData(2, Qt::Horizontal, QString("Serial"));
	mxListModel->setHeaderData(3, Qt::Horizontal, QString("Public IP"));
	mxListModel->setHeaderData(4, Qt::Horizontal, QString("Network"));


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


	ui->mxDeviceTable->setModel(mxListModel);
	ui->mxDeviceTable->resizeColumnsToContents();
	ui->mxDeviceTable->resizeRowsToContents();


}

void MainWindow::displayMXL3Rules(int devIndex, int orgIndex) {
	// show L3 firewall rules of this MX
	qDebug() << "\nMainWindow::displayMXL3Rules, orgIndex: " << orgIndex << "\tdevIndex: " << devIndex;

	int rulesCount = orgList.at(orgIndex)->getOrgInventoryDevice(devIndex).rules.size();
	mxL3RulesModel = new QStandardItemModel(rulesCount, 8, this);

	// columns are | policy | protocol | srcCidr | srcPort | dstCidr | dstPort | syslogEnabled | comment |
	mxL3RulesModel->setHeaderData(0, Qt::Horizontal, QString("policy"));
	mxL3RulesModel->setHeaderData(1, Qt::Horizontal, QString("protocol"));
	mxL3RulesModel->setHeaderData(2, Qt::Horizontal, QString("srcCidr"));
	mxL3RulesModel->setHeaderData(3, Qt::Horizontal, QString("srcPort"));
	mxL3RulesModel->setHeaderData(4, Qt::Horizontal, QString("destCidr"));
	mxL3RulesModel->setHeaderData(5, Qt::Horizontal, QString("destPort"));
	mxL3RulesModel->setHeaderData(6, Qt::Horizontal, QString("syslogEnabled"));
	mxL3RulesModel->setHeaderData(7, Qt::Horizontal, QString("comment"));

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

	ui->mxL3FirewallTable->setModel(mxL3RulesModel);
	ui->mxL3FirewallTable->resizeColumnsToContents();
	ui->mxL3FirewallTable->resizeRowsToContents();

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
		updateNetworkUI(QModelIndex(index));
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

	}

}

void MainWindow::on_adminsTableView_clicked(const QModelIndex &index) {
	// show details of the administrator clicked in the access and tag table views
	qDebug() << "on_adminsTableView_clicked, row: " << index.row();

	adminStruct tmpAdmin = orgList.at(currOrgIndex)->getAdmin(index.row());


	// display list of networks admin has access to, if this is a network admin
	// also display privileges of camera-only administrators
	QStandardItemModel *adminTree2 = new QStandardItemModel(tmpAdmin.nets.size(), 2, this);
	adminTree2->setHeaderData(0, Qt::Horizontal, QString("Net ID"));
	adminTree2->setHeaderData(1, Qt::Horizontal, QString("Access"));

	int j = 0;
	for (int j = 0; j < tmpAdmin.nets.size(); j++) {
		adminNetPermission tmpPerm = tmpAdmin.nets.at(j);
		adminTree2->setItem(j, 0, new QStandardItem(tmpPerm.netID));
		adminTree2->setItem(j, 1, new QStandardItem(tmpPerm.accessLevel));
	}

	// for camera-only admins
	if (tmpAdmin.cNets.size() > 0) {
		adminTree2->setColumnCount(3);
		adminTree2->setHeaderData(2, Qt::Horizontal, QString("network_type"));

		for (j = j; j < tmpAdmin.cNets.size(); j++) {
			adminNetPermission tmpCPerm = tmpAdmin.cNets.at(j);
			adminTree2->setItem(j, 0, new QStandardItem(tmpCPerm.netID));
			adminTree2->setItem(j, 1, new QStandardItem(tmpCPerm.accessLevel));
			adminTree2->setItem(j, 2, new QStandardItem(tmpCPerm.networkType));
		}
	}


	ui->adminsTableView_2->setModel(adminTree2);
	ui->adminsTableView_2->resizeColumnsToContents();
	ui->adminsTableView_2->resizeRowsToContents();



	// display list of tags assigned to administrator
	QStandardItemModel *adminTree3 = new QStandardItemModel(tmpAdmin.tags.size(), 2, this);
	adminTree3->setHeaderData(0, Qt::Horizontal, QString("Tag"));
	adminTree3->setHeaderData(1, Qt::Horizontal, QString("Access"));

	for (int j = 0; j < tmpAdmin.tags.size(); j++) {
		adminTag tmpTag = tmpAdmin.tags.at(j);
		adminTree3->setItem(j, 0, new QStandardItem(tmpTag.tag));
		adminTree3->setItem(j, 1, new QStandardItem(tmpTag.adminAccessLevel));
	}

	ui->adminsTableView_3->setModel(adminTree3);
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
