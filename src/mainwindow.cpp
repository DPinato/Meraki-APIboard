#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {

	ui->setupUi(this);
	QMainWindow::setWindowTitle("Meraki-APIboard");

	// initialise UI
	ui->snmpCheck->setChecked(false);
	on_snmpCheck_clicked(false);
	ui->nonMVPNCheck->setChecked(false);
	on_nonMVPNCheck_clicked(false);


	// I am doing this so I do not have to leave my API key in this code

//	apiKey = getAPIkeyFromFile(QString("/home/davide/Desktop/apikey.txt"));
	apiKey = getAPIkeyFromFile(QString("D:\\Programming\\meraki_api_key.txt"));
	qDebug() << apiKey;

	orgQueryURL = QUrl("https://api.meraki.com/api/v0/organizations");
	networkQueryURL = QUrl("https://api.meraki.com/api/v0/organizations/[organizationId]/networks");

	apiHelpObj = new APIHelper(apiKey, this);
	apiHelpObj->runOrgQuery();

	updateUI();		// show the networks in the tree


//	manager = new QNetworkAccessManager(this);
//	manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
//	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

//	connect(this, SIGNAL(orgQueryFinished()), this, SLOT(runNetworkQuery()));


//	runOrgQuery();


//	MOrganization *tmp = new MOrganization("TEST", 123);
//	tmp->setOrgID(123321);
//	tmp->setOrgName(QString("HASKDJHKASJDH"));
//	tmp->showVariables();

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

void MainWindow::updateUI() {
	// for the organization/network tree view
	// put the stuff in the UI after an API reply
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


	// get licensing state
	apiHelpObj->runLicenseQuery(tmpOrgIndex);

}




void MainWindow::replyFinished(QNetworkReply *reply) {
	qDebug() << "MainWindow::replyFinished(...)";

	apiHelpObj->processQuery(reply);

}



void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {

	updateNetworkUI(QModelIndex(index));
}

void MainWindow::on_snmpCheck_clicked(bool checked) {
	// make SNMP related stuff non-editable
	ui->snmpVerMenu->setEnabled(checked);
	ui->snmpAuthMenu->setEnabled(checked);
	ui->snmpPrivMenu->setEnabled(checked);
	ui->snmpAuthPassEdit->setEnabled(checked);
	ui->snmpAuthPassCheck->setEnabled(checked);
	ui->snmpPrivPassEdit->setEnabled(checked);
	ui->snmpPrivPassCheck->setEnabled(checked);
	ui->snmpPeerIPListBrowser->setEnabled(checked);

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
