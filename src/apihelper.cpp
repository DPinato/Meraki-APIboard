#include "apihelper.h"

APIHelper::APIHelper(QString key, MainWindow *p) {
	parent = p;
	apiKey = key;
	eventIndex = 0;

	manager = new QNetworkAccessManager();
	manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
	QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), parent, SLOT(replyFinished(QNetworkReply*)));

	// TODO: think about a better arrangement for these, read from file?
	baseURL = QString("");
	orgQueryURL = QUrl("https://api.meraki.com/api/v0/organizations");
	networkQueryURL = QUrl("https://api.meraki.com/api/v0/organizations/[organizationId]/networks");
	licenseQueryURL = QUrl("https://api.meraki.com/api/v0/organizations/[organizationId]/licenseState");

	tmpURL = networkQueryURL.toString();


}

APIHelper::~APIHelper() {
	delete manager;
}

bool APIHelper::readURLListFromFile(QString urlFile) {
	// get all the URLs and the HTTP method used from file
	QString tmp;
	int index;
	int l = 0;
	urlRequest tmpReq;


	QFile readFile(urlFile);
	if (!readFile.open(QIODevice::ReadOnly)) { return false; }

	while(!readFile.atEnd()) {
		// a line starting with # can be considered as a comment
		tmp = readFile.readLine();

		// first line will contain baseURL
		if (baseURL.isEmpty()) {
			index = tmp.indexOf(" ")+1;
			l = tmp.length()-index;
			baseURL = tmp.right(l).left(l-2);

		} else {

			if (tmp.length() > 2 && tmp.at(0) != '#') {
				// get the HTTP method
				index = tmp.indexOf(" ");

				if (tmp.left(index) == "GET") {
					tmpReq.reqType = 1;
				} else if (tmp.left(index) == "PUT") {
					tmpReq.reqType = 2;
				} else if (tmp.left(index) == "POST") {
					tmpReq.reqType = 3;
				} else if (tmp.left(index) == "DELETE") {
					tmpReq.reqType = 4;
				}

				// get URL for HTTP method
				l = tmp.length()-index-1;
				tmpReq.url = tmp.right(l).left(l-2);
				urlList.append(tmpReq);

			}
		}

	}


	// debug
	for (int i = 0; i < urlList.size(); i++) {
		qDebug() << i << "\t" << urlList.at(i).reqType << "\t" << urlList.at(i).url;
	}


	return true;	// everything went OK

}

void APIHelper::runQuery(eventRequest e) {
	// run the query corresponding to the query ID
	// data is needed for POST and PUT
	// this function can only be used for queries that need org ID and network ID, no others
	qDebug() << "\nAPIHelper::runQuery(), queryID: " << e.urlListIndex;
	QString queryURL;

	if (e.orgIndex != -1) {
		// put the organization ID in the URL
		queryURL = urlList.at(e.urlListIndex).url;
		qDebug() << queryURL.indexOf(QString("[organizationId]"));
		queryURL = queryURL.replace(queryURL.indexOf(QString("[organizationId]"))
									, QString("[organizationId]").length()
									, parent->orgList.at(e.orgIndex)->getOrgID());

		queryURL.insert(0, baseURL);
	}

	if (e.netIndex != -1) {
		// put network ID in the URL

	}

	if (e.orgIndex == -1 && e.netIndex == -1) {
		queryURL = QString(baseURL.left(baseURL.length()-1) + urlList.at(e.urlListIndex).url);
	}

	qDebug() << queryURL;


	// do HTTP query
	QNetworkRequest request;
	request.setUrl(queryURL);
	request.setRawHeader("X-Cisco-Meraki-API-Key", QByteArray(apiKey.toStdString().c_str(), apiKey.length()));
	request.setRawHeader("Content-Type", "application/json");


	if (urlList.at(e.urlListIndex).reqType == 1) {
		manager->get(request);
	} else if (urlList.at(e.urlListIndex).reqType == 2) {
		manager->put(request, e.data);
	} else if (urlList.at(e.urlListIndex).reqType == 3) {
		manager->post(request, e.data);
	} else if (urlList.at(e.urlListIndex).reqType == 4) {
		manager->deleteResource(request);
	}



}

void APIHelper::processQuery(QNetworkReply *r) {
	// this function is supposed to be able to figure out which query was made
	// and call the appropriate processing function
	qDebug() << "\nAPIHelper::processQuery(...)";
//	queueEventRequests[eventIndex].responseReceived = true;		// update the event in the queue

	QByteArray response = r->readAll();
	qDebug() << r->url();
	qDebug() << r->error();
	qDebug() << "Response: " << response;

	QJsonDocument jDoc = QJsonDocument::fromJson(response);


	// figure out what kind of query was made
//	int queryIndex = getQueryIndex(r);
	int queryIndex = queueEventRequests.at(eventIndex).urlListIndex;
	qDebug() << "eventIndex: " << eventIndex << "\tqueryIndex: " << queryIndex;

	switch (queryIndex) {

		case 27: {
			// GET /organizations/[organizationId]/networks
			processNetworkQuery(jDoc);
			break;
		}

		case 41: {
			// GET /organizations
			processOrgQuery(jDoc);
			break;
		}


	}


/*
	QString tmp = orgQueryURL.toString();
	qDebug() << tmp.right(tmp.length()-tmp.indexOf(QString("/organizations")));
	qDebug() << r->url().toString().endsWith(
					tmp.right(tmp.length()-tmp.indexOf(QString("/organizations"))));

	// query 41
	if (r->url().toString().endsWith(
			orgQueryURL.toString().right(
				orgQueryURL.toString().length()-orgQueryURL.toString().indexOf(QString("/organizations"))))) {
		processOrgQuery(jDoc);

		// get all the information regarding the organizations
		// 0, 9, 22, 27, (42), 47, 48, 49, 51, 63, 64
		eventRequest tmp;
		tmp.urlListIndex = 0;
		tmp.orgIndex = 0;
		tmp.netIndex = -1;
		tmp.data = QByteArray(0);

		putEventInQueue(tmp);	// GET /organizations/[organizationId]/snmp

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

*/


	queueEventRequests[eventIndex].responseProcessed = true;		// update the event in the queue
	eventIndex++;

}

void APIHelper::putEventInQueue(eventRequest e) {
	// if the queue is empty, run it immediately
	// otherwise it will be run when it is time

	// if the organization list is empty, do not do anything
	if (parent->orgList.size() == 0) { return; }


	queueEventRequests.append(e);
	qDebug() << "\nAPIHelper::putEventInQueue(...)\t" << eventIndex << "\t" << queueEventRequests.size();

//	if (queueEventRequests.size() == 0) {
	if (eventIndex == queueEventRequests.size()-1) {
		qDebug() << "Running query immediately, eventIndex: " << eventIndex
				 << "\tqueueEventReq size: " << queueEventRequests.size();
		runQuery(e);
	}

//	qDebug() << "Queueing query, eventIndex: " << eventIndex
//			 << "\tqueueEventReq size: " << queueEventRequests.size();
//	queueEventRequests.append(e);

}

bool APIHelper::processOrgQuery(QJsonDocument doc) {
	// call this after querying list of organizations
	// https://api.meraki.com/api/v0/organizations
	qDebug() << "\nAPIHelper::processOrgQuery(...)";

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

//	// do not do this, what if there are a lot of orgs with a lot of networks
//	for (int i = 0; i < parent->orgList.size(); i++) {
//		runQuery(i);
//	}

	parent->updateOrgUI(-1);

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

	parent->updateOrgUI(-1);
	parent->updateOrgUI(orgIndex);

}

bool APIHelper::processLicenseQuery(QJsonDocument doc, int orgIndex) {
	qDebug() << "\nAPIHelper::processLicenseQuery()";

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID";
		return false;
	}

	licensesPerDevice tmpVar;
	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size() << "\n";


	// save info for licensing status of organization
	parent->orgList[orgIndex]->setLicenseExpDate(jObj["expirationDate"].toString());
	parent->orgList[orgIndex]->setLicenseStatus(jObj["status"].toString());

	// get list of licensedDeviceCounts
	QJsonObject jObjCounts = jObj["licensedDeviceCounts"].toObject();	// makes it easier to work with it
	parent->orgList[orgIndex]->setLicenseDeviceNum(jObjCounts.keys().size());

	for (int i = 0; i < jObjCounts.size(); i++) {
		tmpVar.deviceType = jObjCounts.keys().at(i);
		tmpVar.count = jObjCounts[jObjCounts.keys().at(i)].toInt();
		parent->orgList[orgIndex]->setLicensePerDevice(tmpVar, i);
	}

	parent->updateOrgUI(orgIndex);

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

int APIHelper::getEventQueueSize() {
	return queueEventRequests.size();
}

eventRequest APIHelper::getNextEvent() {
	return queueEventRequests.at(0);
}

int APIHelper::getEventIndex() {
	return eventIndex;
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
