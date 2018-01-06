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


	orgQueryURL = QUrl("https://api.meraki.com/api/v0/organizations");
	networkQueryURL = QUrl("https://api.meraki.com/api/v0/organizations/[organizationId]/networks");

	apiHelpObj = new APIHelper(apiKey, this);
	apiHelpObj->readURLListFromFile(urlListFile);

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
	// show stuff in the "Administration" tab
	ui->orgNameEdit->setText(orgList.at(orgIndex)->getOrgName());
	ui->networkNameEdit->setText("");

	// show administrators in the table view


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
	apiHelpObj->putEventInQueue(tmp);	// GET /organizations
}

void MainWindow::on_tabWidget_currentChanged(int index) {
	qDebug() << "tab: " << index << "\t" << ui->tabWidget->tabText(index);

}
