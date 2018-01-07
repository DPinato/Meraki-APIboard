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

//	apiHelpObj->runQuery(41);	// GET /organizations
//	apiHelpObj->putEventInQueue(eventRequest{49, 0, -1, 0});	// GET /organizations/[organizationId]/snmp

//	updateUI(-1);		// show the networks in the tree

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

		tmp.urlListIndex = 0;	// get admins in the org
		apiHelpObj->putEventInQueue(tmp);


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
			break;
		}

		case 1: {
			// Organization Settings tab

			break;
		}

		case 2: {
			// Licensing / Inventory tab
			tmp.urlListIndex = 47;	// GET /organizations/[organizationId]/licenseState
			apiHelpObj->putEventInQueue(tmp);

			tmp.urlListIndex = 48;
			apiHelpObj->putEventInQueue(tmp);


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
