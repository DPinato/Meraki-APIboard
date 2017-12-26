#include "apihelper.h"

APIHelper::APIHelper(QString key, MainWindow *p) {
	parent = p;
	apiKey = key;

	manager = new QNetworkAccessManager();
	manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
	QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), parent, SLOT(replyFinished(QNetworkReply*)));

	// TODO: think about a better arrangement for these, read from file?
	orgQueryURL = QUrl("https://api.meraki.com/api/v0/organizations");
	networkQueryURL = QUrl("https://api.meraki.com/api/v0/organizations/[organizationId]/networks");
	licenseQueryURL = QUrl("https://api.meraki.com/api/v0/organizations/[organizationId]/licenseState");

	tmpURL = networkQueryURL.toString();


}

APIHelper::~APIHelper() {
	delete manager;
}

void APIHelper::processQuery(QNetworkReply *r) {
	// this function is supposed to be able to figure out which query was made
	// and call the appropriate processing function
	qDebug() << "APIHelper::processQuery(...)";

	QByteArray response = r->readAll();
	qDebug() << r->url();
	qDebug() << response;

	QJsonDocument jDoc = QJsonDocument::fromJson(response);



	// figure out what kind of query was made
	QString tmp = orgQueryURL.toString();
	qDebug() << tmp.right(tmp.length()-tmp.indexOf(QString("/organizations")));
	qDebug() << r->url().toString().endsWith(
					tmp.right(tmp.length()-tmp.indexOf(QString("/organizations"))));


	if (r->url().toString().endsWith(
			orgQueryURL.toString().right(
				orgQueryURL.toString().length()-orgQueryURL.toString().indexOf(QString("/organizations"))))) {
		processOrgQuery(jDoc);
	}

	if (r->url().toString().endsWith(
			tmpURL.right(
				tmpURL.length()-tmpURL.indexOf(QString("/networks"))))) {
		processNetworkQuery(jDoc);
	}

	if (r->url().toString().endsWith(
			tmpURL.right(
				tmpURL.length()-tmpURL.indexOf(QString("/licenseState"))))) {
		int id = getOrgIDFromURL(r->url()).toInt();
		processLicenseQuery(jDoc, findOrg(id));
	}



	// tell MainWindow to update the GUI
	parent->updateUI();

}

void APIHelper::runOrgQuery() {
	// queries for all the organizations the user has access to
	qDebug() << "APIHelper::runOrgQuery()";

	QNetworkRequest request;
	request.setUrl(orgQueryURL);
	request.setRawHeader("X-Cisco-Meraki-API-Key", QByteArray(apiKey.toStdString().c_str(), apiKey.length()));
	request.setRawHeader("Content-Type", "application/json");

	manager->get(request);

}

void APIHelper::runOrgQuery(int index) {
	// get all networks for the org at the index
	qDebug() << "APIHelper::runNetworkQuery(int)";


	// build the URL containing the org ID
	QString url = networkQueryURL.toString();
	int i = url.indexOf(QString("[organizationId]"));
	tmpURL = url.remove(i, QString("[organizationId]").length());

	int index2 = tmpURL.lastIndexOf(QString("//"))+1;
	tmpURL.insert(index2, parent->orgList.at(index)->getOrgID());
	qDebug() << tmpURL;


	// do the query
	QNetworkRequest request;
	request.setUrl(tmpURL);
	request.setRawHeader("X-Cisco-Meraki-API-Key", QByteArray(apiKey.toStdString().c_str(), apiKey.length()));
	request.setRawHeader("Content-Type", "application/json");

	manager->get(request);

}

void APIHelper::runNetworkQuery(QModelIndex &index) {
	// return list of networks associated with organization
	// use index to figure out what to get information for
	qDebug() << "APIHelper::runNetworkQuery(QModelIndex)";

	int tmpOrgIndex;

	if (index.parent().data() == QVariant::Invalid) {
		// an organization was selected in the tree view
		tmpOrgIndex = index.row();
	}  else {
		return;
	}


	// build the URL containing the org ID
	QString url = networkQueryURL.toString();
	int i = url.indexOf(QString("[organizationId]"));
	tmpURL = url.remove(i, QString("[organizationId]").length());

	int index2 = tmpURL.lastIndexOf(QString("//"))+1;
	tmpURL.insert(index2, parent->orgList.at(tmpOrgIndex)->getOrgID());
	qDebug() << tmpURL;


	// do the query
	QNetworkRequest request;
	request.setUrl(tmpURL);
	request.setRawHeader("X-Cisco-Meraki-API-Key", QByteArray(apiKey.toStdString().c_str(), apiKey.length()));
	request.setRawHeader("Content-Type", "application/json");

	manager->get(request);

}

void APIHelper::runLicenseQuery(int index) {
	// get licensing state for org
	QString url = licenseQueryURL.toString();
	int i = url.indexOf(QString("[organizationId]"));
	tmpURL = url.remove(i, QString("[organizationId]").length());

	int index2 = tmpURL.lastIndexOf(QString("//"))+1;
	tmpURL.insert(index2, parent->orgList.at(index)->getOrgID());
	qDebug() << tmpURL;


	// do the query
	QNetworkRequest request;
	request.setUrl(tmpURL);
	request.setRawHeader("X-Cisco-Meraki-API-Key", QByteArray(apiKey.toStdString().c_str(), apiKey.length()));
	request.setRawHeader("Content-Type", "application/json");

	manager->get(request);

}

bool APIHelper::processOrgQuery(QJsonDocument doc) {
	// call this after querying list of organizations
	// https://api.meraki.com/api/v0/organizations
	qDebug() << "APIHelper::processOrgQuery()";

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgQuery(...)";
		return false;
	}

//	qDebug() << jDoc.toJson(QJsonDocument::Indented);


	// get organization data
	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	parent->orgList.resize(jArray.size());

	for (int index = 0; index < jArray.size(); index++) {
		QJsonObject jObj = jArray.at(index).toObject();
		parent->orgList[index] = new MOrganization();

		parent->orgList[index]->setOrgID(jObj["id"].toVariant().toString());
//		qDebug() << orgList[index]->getOrgID();

		parent->orgList[index]->setOrgName(jObj["name"].toString());
//		qDebug() << orgList[index]->getOrgName();


		// these are not necessarily be sent with the response
		if (jObj["samlConsumerUrl"] != QJsonValue::Undefined) {
			parent->orgList[index]->setSamlURL(jObj["samlConsumerUrl"].toString());
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


	qDebug() << "ORGS: " << parent->orgList.size();

	// do not do this, what if there are a lot of orgs with a lot of networks
	for (int i = 0; i < parent->orgList.size(); i++) {
		runOrgQuery(i);
	}

	parent->updateUI();

	return true;	// everything OK

}

void APIHelper::processNetworkQuery(QJsonDocument doc) {
	qDebug() << "\nAPIHelper::processNetworkQuery()";

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

//		qDebug() << tmpNetVar.netID;
//		qDebug() << tmpNetVar.netName;
//		qDebug() << tmpNetVar.orgID;
//		qDebug() << tmpNetVar.netTimezone;
//		qDebug() << tmpNetVar.netType;
//		qDebug() << tmpNetVar.netTags;
//		qDebug() << "\n";


		// do this only once for the first network
		// the other networks will be in the same organization
		if (orgIndex == -1) {
			for (int l = 0; l < parent->orgList.size(); l++) {
				if (parent->orgList.at(l)->getOrgID().compare(tmpNetVar.orgID) == 0) {
					orgIndex = l;
					break;
				}
			}
		}

//		qDebug() << "orgIndex: " << orgIndex;
		// increase number of networks in org
		parent->orgList[orgIndex]->setNetworksNum(parent->orgList.at(orgIndex)->getNetworksNum()+1);
		parent->orgList[orgIndex]->setNetwork(tmpNetVar, i);


	}

	parent->updateUI();

}

bool APIHelper::processLicenseQuery(QJsonDocument doc, int orgIndex) {
	qDebug() << "\nAPIHelper::processLicenseQuery()";

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID";
		return false;
	}


	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size() << "\n";

	licensesPerDevice tmpVar;

	parent->orgList[orgIndex]->licS


//		QJsonObject jObj = jArray.at(i).toObject();
//		tmpVar.netID = jObj["id"].toString();
//		tmpVar.netName = jObj["name"].toString();
//		tmpVar.orgID = jObj["organizationId"].toVariant().toString();
//		tmpVar.netTimezone = jObj["timeZone"].toString();
//		tmpVar.netType = jObj["type"].toString();
//		tmpVar.netTags = (jObj["tags"].toString());

//		qDebug() << tmpVar.netID;
//		qDebug() << tmpVar.netName;
//		qDebug() << tmpVar.orgID;
//		qDebug() << tmpVar.netTimezone;
//		qDebug() << tmpVar.netType;
//		qDebug() << tmpVar.netTags;
//		qDebug() << "\n";





	parent->updateUI();



	return true;		// everything ok
}

void APIHelper::setApiKey(QString key) {
	apiKey = key;
}

void APIHelper::setOrgQueryURL(QUrl u) {
	orgQueryURL = u;
}

void APIHelper::setNetworkQueryURL(QUrl u) {
	networkQueryURL = u;
}

QString APIHelper::getApiKey() {
	return apiKey;
}

QUrl APIHelper::getOrgQueryURL() {
	return orgQueryURL;
}

QUrl APIHelper::getNetworkQueryURL() {
	return networkQueryURL;
}

QString APIHelper::getOrgIDFromURL(QUrl u) {
	// get org ID from URL, should be right after .../organizations/<id>
	int i1 = u.toString().indexOf(QString("/organizations")) + QString("/organizations").length();
	int i2 = u.toString().indexOf("/", i1+1);
	QString test = u.toString().toStdString().substr(i1+1, i2-i1-1).c_str();
//	qDebug() << "i1: " << i1 << "\ti2: " << i2 << "\ttest: " << test;

	return test;
}

int APIHelper::findOrg(int orgID) {
	// given the org ID, return the index the org is in the vector
	QString tmpID = QString::number(orgID);
	for (int i = 0; i < parent->orgList.size(); i++) {
		if (parent->orgList.at(i)->getOrgID() == tmpID) { return i; }
	}

	return -1;	// could not find org with that org ID
}

void APIHelper::replyFinished(QNetworkReply *reply) {
	// called when the QNetworkAccessManager gets a reply from the query
	// TODO: figure out how to use this one, instead of the one in MainWindow

	qDebug() << reply->readAll();

}
