#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {

	ui->setupUi(this);
	QMainWindow::setWindowTitle("Meraki-APIboard");


	// I am doing this so I do not have to leave my API key in this code
//	apiKey = getAPIkeyFromFile(QString("/home/davide/Desktop/apikey.txt"));
	apiKey = getAPIkeyFromFile(QString("D:\\Programming\\meraki_api_key.txt"));
	qDebug() << apiKey;

	orgQueryURL = QUrl("https://api.meraki.com/api/v0/organizations");
	networkQueryURL = QUrl("https://api.meraki.com/api/v0/organizations/[organizationId]/networks");


	manager = new QNetworkAccessManager(this);
	manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));


	connect(this, SIGNAL(orgQueryFinished()), this, SLOT(runNetworkQuery()));
	runOrgQuery();


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

void MainWindow::runOrgQuery() {
	qDebug() << "MainWindow::runOrgQuery()";

	QNetworkRequest request;
	request.setUrl(orgQueryURL);
	request.setRawHeader("X-Cisco-Meraki-API-Key", QByteArray(apiKey.toStdString().c_str(), apiKey.length()));
	request.setRawHeader("Content-Type", "application/json");

	manager->get(request);

}

void MainWindow::runNetworkQuery() {
	// query the API for list of networks for every organization retrieved previously
	// https://api.meraki.com/api/v0/organizations/[organizationId]/networks
	qDebug() << "MainWindow::runNetworkQuery()";

	for (int i = 0; i < orgList.size(); i++) {
		// build the URL containing the org ID
		QString url = networkQueryURL.toString();
		int index = url.indexOf(QString("[organizationId]"));
		tmpNetURL = url.remove(index, QString("[organizationId]").length());

		int index2 = tmpNetURL.lastIndexOf(QString("//"))+1;
		tmpNetURL.insert(index2, orgList.at(i)->getOrgID());
		qDebug() << tmpNetURL;


		// do the query
		QNetworkRequest request;
		request.setUrl(tmpNetURL);
		request.setRawHeader("X-Cisco-Meraki-API-Key", QByteArray(apiKey.toStdString().c_str(), apiKey.length()));
		request.setRawHeader("Content-Type", "application/json");

		manager->get(request);

	}


}

void MainWindow::processOrgQuery(QJsonDocument doc) {
	// call this after querying list of organizations
	// https://api.meraki.com/api/v0/organizations
	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID";
		return;
	}

//	qDebug() << jDoc.toJson(QJsonDocument::Indented);


	// get organization data
	QJsonArray jArray = doc.array();
//	qDebug() << jArray << "\t" << jArray.size();

	orgList.resize(jArray.size());

	for (int index = 0; index < jArray.size(); index++) {
		QJsonObject jObj = jArray.at(index).toObject();
		orgList[index] = new MOrganization();

		orgList[index]->setOrgID(jObj["id"].toVariant().toString());
//		qDebug() << orgList[index]->getOrgID();

		orgList[index]->setOrgName(jObj["name"].toString());
//		qDebug() << orgList[index]->getOrgName();


		// these are not necessarily be sent with the response
		if (jObj["samlConsumerUrl"] != QJsonValue::Undefined) {
			orgList[index]->setSamlURL(jObj["samlConsumerUrl"].toString());
//			qDebug() << orgList[index]->getSamlURL();
		}

//		if (jObj.contains("samlConsumerUrls")) {
//			// this not yet implemented
//			qDebug() << "YES, index: " << index;
//			QJsonArray tmpJArray = jObj["samlConsumerUrls"].toArray();

//			for (int i = 0; i < tmpJArray.size(); i++) {
//				qDebug() << tmpJArray.at(i).toString();
//			}
//		}


//		qDebug() << "\n";

	}



	for (int i = 0; i < orgList.length(); i++) {
		orgList.at(i)->showVariables();
		qDebug() << "\n";
	}

}

void MainWindow::processNetworkQuery(QJsonDocument doc) {
	// call this after querying for the list of networks
	qDebug() << "MainWindow::processNetworkQuery(...)";
	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID";
		return;
	}

	int orgIndex = -1;		// used to find in which organization these networks are

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size() << "\n";

	networkVars tmpNetVar;


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		tmpNetVar.netID = jObj["id"].toString();
		tmpNetVar.netName = jObj["name"].toString();
		tmpNetVar.orgID = jObj["organizationId"].toVariant().toString();
		tmpNetVar.netTimezone = jObj["timeZone"].toString();
		tmpNetVar.netType = jObj["type"].toString();
		tmpNetVar.netTags = (jObj["tags"].toString());

		qDebug() << tmpNetVar.netID;
		qDebug() << tmpNetVar.netName;
		qDebug() << tmpNetVar.orgID;
		qDebug() << tmpNetVar.netTimezone;
		qDebug() << tmpNetVar.netType;
		qDebug() << tmpNetVar.netTags;
		qDebug() << "\n";


		// do this only once for the first network
		// the other networks will be in the same organization
		if (orgIndex == -1) {
			for (int l = 0; l < orgList.size(); l++) {
				if (orgList.at(l)->getOrgID().compare(tmpNetVar.orgID) == 0) {
					orgIndex = l;
					break;
				}
			}
		}

		qDebug() << "orgIndex: " << orgIndex;
		orgList[orgIndex]->setNetworksNum(orgList.at(orgIndex)->getNetworksNum()+1);	// increase number of networks in org
		orgList[orgIndex]->setNetwork(tmpNetVar, i);

	}




}



void MainWindow::replyFinished(QNetworkReply *reply) {
	QByteArray response = reply->readAll();
	qDebug() << reply->url();

//	jDoc.fromRawData(reply->readAll().toStdString().c_str(), reply->readAll().toStdString().size());
	QJsonDocument jDoc = QJsonDocument::fromJson(response);


	// figure out what kind of query was made
	QString tmp = orgQueryURL.toString();
	qDebug() << reply->url().toString().endsWith(
					tmp.right(tmp.length()-tmp.indexOf(QString("/organizations"))));
	qDebug() << tmp.right(tmp.length()-tmp.indexOf(QString("/organizations")));

	tmp = tmpNetURL;
	qDebug() << reply->url().toString().endsWith(
					tmp.right(tmp.length()-tmp.indexOf(QString("/networks"))));
	qDebug() << tmp.right(tmp.length()-tmp.indexOf(QString("/networks")));
	qDebug() << "\n\n";

	if (reply->url().toString().endsWith(
			orgQueryURL.toString().right(
				orgQueryURL.toString().length()-orgQueryURL.toString().indexOf(QString("/organizations"))))) {
		processOrgQuery(jDoc);
		emit orgQueryFinished();
	}

	if (reply->url().toString().endsWith(
			tmpNetURL.right(
				tmpNetURL.length()-tmpNetURL.indexOf(QString("/networks"))))) {
		processNetworkQuery(jDoc);

	}

	updateUI();

}



