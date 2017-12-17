#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow) {
	ui->setupUi(this);


	// I am doing this so I do not have to leave my API key in this code
	apiKey = getAPIkeyFromFile(QString("/home/davide/Desktop/apikey.txt"));
	qDebug() << apiKey;


	manager = new QNetworkAccessManager(this);
	manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));




	QNetworkRequest request;
	request.setUrl(QUrl("https://api.meraki.com/api/v0/organizations"));
	request.setRawHeader("X-Cisco-Meraki-API-Key", QByteArray(apiKey.toStdString().c_str(), apiKey.length()));
	request.setRawHeader("Content-Type", "application/json");


	QNetworkReply *reply = manager->get(request);




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



void MainWindow::replyFinished(QNetworkReply *reply) {
	QByteArray response = reply->readAll();
	qDebug() << response;

//	jDoc.fromRawData(reply->readAll().toStdString().c_str(), reply->readAll().toStdString().size());
	jDoc = QJsonDocument::fromJson(response);

	if (jDoc.isNull()) {
		qDebug() << "JSON IS NOT VALID";
	} else {
		qDebug() << "JSON IS VALID";
	}


//	qDebug() << jDoc.toJson(QJsonDocument::Indented);


	QJsonArray jArray = jDoc.array();
//	qDebug() << jArray << "\t" << jArray.size();

	orgIDs.resize(jArray.size());
	orgName.resize(jArray.size());
	samlURL.resize(jArray.size());
	samlURLs.resize(jArray.size());


	for (int index = 0; index < jArray.size(); index++) {
		QJsonObject jObj = jArray.at(index).toObject();

		orgIDs[index] = jObj["id"].toVariant().toDouble();
		qDebug() << jObj["id"].toVariant().toDouble();
		orgName[index] = jObj["name"].toString();
		qDebug() << jObj["name"].toString();


		// these are not necessarily be sent with the response
		if (jObj["samlConsumerUrl"] != QJsonValue::Undefined) {
			samlURL[index] = jObj["samlConsumerUrl"].toString();
			qDebug() << jObj["samlConsumerUrl"].toString();
		}


		if (jObj["samlConsumerUrls"] != QJsonValue::Array) {
			QJsonArray tmpJArray = jObj["samlConsumerUrls"].toArray();

			QList<QString> tmpStrList;
			for (int i = 0; i < tmpJArray.size(); i++) {
				tmpStrList.append(tmpJArray.at(i).toString());
				qDebug() << tmpJArray.at(i).toString();
			}

			samlURLs[index] = tmpStrList;
		}


		qDebug() << "\n";

	}






	for (int i = 0; i < orgIDs.length(); i++) {
		qDebug() << orgIDs.at(i);
		qDebug() << "\t" << orgName.at(i);
		qDebug() << "\t" << samlURL.at(i);

		for (int l = 0; l < samlURLs.length(); l++) {
			qDebug() << "\t\t" << samlURLs.at(l);
		}

	}



}



