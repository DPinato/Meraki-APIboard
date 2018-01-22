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
	qDebug() << "orgID: " << e.orgIndex << "\tnetID: " << e.netIndex;
	QString queryURL = urlList.at(e.urlListIndex).url;


	// prepare the right part of the URL
	if (e.orgIndex != -1) {
		// put the organization ID in the URL
		qDebug() << queryURL.indexOf(QString("[organizationId]"));
		queryURL = queryURL.replace(queryURL.indexOf(QString("[organizationId]"))
									, QString("[organizationId]").length()
									, parent->orgList.at(e.orgIndex)->getOrgID());
	}

	if (e.netIndex != -1) {
		// put network ID in the URL
		qDebug() << queryURL.indexOf(QString("[networkId]"));
		queryURL = queryURL.replace(queryURL.indexOf(QString("[networkId]"))
									, QString("[networkId]").length()
									, parent->orgList.at(e.orgIndex)->getNetwork(e.netIndex).netID);
	}

	if (e.deviceSerial != "") {
		// put the device serial number in the URL
		qDebug() << queryURL.indexOf(QString("[serial]"));
		queryURL = queryURL.replace(queryURL.indexOf(QString("[serial]"))
									, QString("[serial]").length()
									, e.deviceSerial);

	}

	if (e.orgIndex == -1 && e.netIndex == -1) {
		// these kinds of queries will return info for multiple organizations at once, i.e. queryID 41
//		queryURL = QString(baseURL.left(baseURL.length()-1) + urlList.at(e.urlListIndex).url);
	}


	// insert the beginning of the URL
	// note that this will have 2 /, QUrl does not seem to care though
	queryURL.insert(0, baseURL);

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

		case 0: {
			// GET /organizations/[organizationId]/admins
			processOrgAdminsQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
			break;
		}

		case 4: {
			// GET /devices/[serial]/clients
			processClientsConnectedQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
										 , queueEventRequests.at(eventIndex).deviceSerial);
			break;
		}

		case 20: {
			// GET /networks/[networkId]/l3FirewallRules
			processMXL3FirewallQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									 , queueEventRequests.at(eventIndex).deviceSerial);
			break;
		}

		case 26: {
			// GET /networks/[networkId]/groupPolicies
			processGroupPolicyQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									, queueEventRequests.at(eventIndex).netIndex);
			break;
		}

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

		case 47: {
			// GET /organizations/[organizationId]/licenseState
			processLicenseQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
			break;
		}

		case 48: {
			// GET /organizations/[organizationId]/inventory
			processOrgInventoryQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
			break;
		}

		case 49: {
			// GET /organizations/[organizationId]/snmp
			processOrgSNMPQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
			break;
		}

		case 51: {
			// GET /organizations/[organizationId]/thirdPartyVPNPeers
			processOrgVPNQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
			break;
		}

		case 82: {
			// GET /networks/[networkId]/sm/devices
			processSMDevicesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								  , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 93: {
			// GET /devices/[serial]/switchPorts
			processSwitchPortQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								   , queueEventRequests.at(eventIndex).deviceSerial);
			break;
		}

	}


	// update event queue
	queueEventRequests[eventIndex].responseProcessed = true;		// update the event in the queue
	eventIndex++;

}

void APIHelper::putEventInQueue(eventRequest e, bool force) {
	// if the queue is empty, run it immediately
	// otherwise it will be run when it is time
	qDebug() << "\nAPIHelper::putEventInQueue(...), urlListIndex: " << e.urlListIndex << "\tforce: " << force;

	// if the organization list is empty, do not do anything
	if (!force) {
		if (parent->orgList.size() == 0) { return; }
	}


//	if (queueEventRequests.size() == 0) {
	if (eventIndex == queueEventRequests.size() || force) {
		queueEventRequests.append(e);

		qDebug() << "Running query immediately, eventIndex: " << eventIndex
				 << "\tqueueEventReq size: " << queueEventRequests.size();
		runQuery(e);

	} else {
		qDebug() << "Queueing query, eventIndex: " << eventIndex
				 << "\tqueueEventReq size: " << queueEventRequests.size();
		queueEventRequests.append(e);
	}


	// DEBUG
	for (int i = 0; i < queueEventRequests.size(); i++) {
		qDebug() << "Event " << i << "\t"
				 << queueEventRequests.at(i).urlListIndex
				 << "\torg: " << queueEventRequests.at(i).orgIndex
				 << "\tnet: " << queueEventRequests.at(i).netIndex;
	}

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
//	qDebug() << jArray << "\t" << jArray.size();


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
//	qDebug() << jArray << "\t" << jArray.size() << "\n";

	networkVars tmpNetVar;


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		tmpNetVar.netID = jObj["id"].toString();
		tmpNetVar.netName = jObj["name"].toString();
		tmpNetVar.orgID = jObj["organizationId"].toVariant().toString();
		tmpNetVar.netTimezone = jObj["timeZone"].toString();
		tmpNetVar.netType = jObj["type"].toString();
		tmpNetVar.netTags = (jObj["tags"].toString());


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
	qDebug() << "\nAPIHelper::processLicenseQuery(...), orgIndex: " << orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processLicenseQuery(...)";
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

//	parent->updateOrgUI(orgIndex);
	parent->displayLicenseInfo(orgIndex);

	return true;		// everything ok
}

bool APIHelper::processOrgAdminsQuery(QJsonDocument doc, int orgIndex) {
	// process list of administrators in organization obtained through
	// GET /organizations/[organizationId]/admins
	qDebug() << "\nAPIHelper::processOrgAdminsQuery(...), orgIndex: " << orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgAdminsQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	parent->orgList[orgIndex]->setAdminsNum(jArray.size());

	qDebug() << jArray << "\t" << jArray.size();


	for (int i = 0; i < jArray.size(); i++) {
		adminStruct tmpAdmin;	// this is here since it needs to completely reset at every iteration
		QJsonObject jObj = jArray.at(i).toObject();

		tmpAdmin.name = jObj["name"].toString();
		tmpAdmin.email = jObj["email"].toString();
		tmpAdmin.id = jObj["id"].toString();
		tmpAdmin.orgAccess = jObj["orgAccess"].toString();

		// get networks admin has access to
		QJsonArray jNets = jObj["networks"].toArray();
		for (int j = 0; j < jNets.size(); j++) {
			adminNetPermission tmpNet;
			tmpNet.netID = jNets.at(j).toObject()["id"].toString();
			tmpNet.accessLevel = jNets.at(j).toObject()["access"].toString();
			tmpAdmin.nets.append(tmpNet);
		}


		// get admin tags
		QJsonArray jTags = jObj["tags"].toArray();
		for (int j = 0; j < jTags.size(); j++) {
			adminTag tmpTag;
			tmpTag.tag = jTags.at(j).toObject()["tag"].toString();
			tmpTag.adminAccessLevel = jTags.at(j).toObject()["access"].toString();
			tmpAdmin.tags.append(tmpTag);
		}


		// check if admin has camera-level permissions
		QJsonArray jCamera = jObj["camera"].toArray();
		for (int j = 0; j < jCamera.size(); j++) {
			adminNetPermission tmpCamera;
			tmpCamera.netID = jCamera.at(j).toObject()["id"].toString();
			tmpCamera.accessLevel = jCamera.at(j).toObject()["access"].toString();
			tmpCamera.networkType = jCamera.at(j).toObject()["network_type"].toString();
			tmpAdmin.cNets.append(tmpCamera);
		}


		parent->orgList[orgIndex]->setAdmin(tmpAdmin, i);

	}


	parent->displayAdminStuff(orgIndex);

	return true;	// everything went well

}

bool APIHelper::processOrgInventoryQuery(QJsonDocument doc, int orgIndex) {
	// process inventory for an organization
	// GET /organizations/[organizationId]/inventory
	qDebug() << "\nAPIHelper::processOrgInventoryQuery(...), orgIndex: " << orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgInventoryQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	parent->orgList[orgIndex]->setOrgInventorySize(jArray.size());

	qDebug() << jArray << "\t" << jArray.size();


	for (int i = 0; i < jArray.size(); i++) {
		deviceInInventory tmpDevice;
		QJsonObject jObj = jArray.at(i).toObject();

		tmpDevice.claimedAt = jObj["claimedAt"].toString();
		tmpDevice.mac = jObj["mac"].toString();
		tmpDevice.model = jObj["model"].toString();
		tmpDevice.netID = jObj["networkId"].toString();
		tmpDevice.publicIP = jObj["publicIp"].toString();
		tmpDevice.serial = jObj["serial"].toString();

		parent->orgList[orgIndex]->setOrgInventoryDevice(tmpDevice, i);

	}

	parent->displayInventory(orgIndex);

	return true;	// everything went well

}

bool APIHelper::processOrgSNMPQuery(QJsonDocument doc, int orgIndex) {
	qDebug() << "\nAPIHelper::processOrgSNMPQuery(...), orgIndex: " << orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgSNMPQuery(...)";
		return false;
	}


	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size();

	orgSNMP tmpSNMP;

	tmpSNMP.snmp2cOrgEnabled = jObj["v2cEnabled"].toBool();
	tmpSNMP.hostname = jObj["hostname"].toString();			// always returned
	tmpSNMP.port = QString::number(jObj["port"].toInt());
	tmpSNMP.snmpAuthMode = jObj["v3AuthMode"].toString();
	tmpSNMP.snmpPrivMode = jObj["v3PrivMode"].toString();

	if (tmpSNMP.snmp2cOrgEnabled) {
		// stuff returned only if SNMPv2c is enabled
		tmpSNMP.v2CommString = jObj["v2CommunityString"].toString();
	}

	tmpSNMP.snmp3OrgEnabled = jObj["v3Enabled"].toBool();
	if (tmpSNMP.snmp3OrgEnabled) {
		// stuff returned only if SNMPv3 is enabled
		tmpSNMP.v3User = jObj["v3User"].toString();
	}

	// for some reason peerIps are not returned as an array
	// they are returned no matter what the settings
	QString tmp = jObj["peerIps"].toString();
	int h1 = 0;

	do {
		h1 = tmp.indexOf(';', 0);
		tmpSNMP.snmpPeerIPs.append(tmp.left(h1));
		tmp.remove(0, h1+1);

	} while (tmp.indexOf(';', 0) != -1);

	tmpSNMP.snmpPeerIPs.append(tmp);

	parent->orgList.at(orgIndex)->setOrgSNMPSettings(tmpSNMP);
	parent->displayOrgSNMP(orgIndex);

	return true;	// everything went ok

}

bool APIHelper::processOrgVPNQuery(QJsonDocument doc, int orgIndex) {
	qDebug() << "\nAPIHelper::processOrgVPNQuery(...), orgIndex: " << orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgVPNQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	parent->orgList[orgIndex]->setOrgVPNPeerNum(jArray.size());

	qDebug() << jArray << "\t" << jArray.size();


	for (int i = 0; i < jArray.size(); i++) {
		nonMerakiVPNPeer tmpVPN;
		QJsonObject jObj = jArray.at(i).toObject();

		tmpVPN.peerName = jObj["name"].toString();
		tmpVPN.peerPublicIP = jObj["publicIp"].toString();
		tmpVPN.secret = jObj["secret"].toString();

		QJsonArray jArr1 = jObj["privateSubnets"].toArray();
		for (int j = 0; j < jArr1.size(); j++) {
			tmpVPN.privateSubnets.append(jArr1.at(j).toString());
		}

		QJsonArray jArr2 = jObj["tags"].toArray();
		for (int j = 0; j < jArr2.size(); j++) {
			tmpVPN.tags.append(jArr2.at(j).toString());
		}

		parent->orgList.at(orgIndex)->setOrgVPNPeer(tmpVPN, i);

	}

	parent->displayOrgVPN(orgIndex);

	return true;	// everything went ok

}

bool APIHelper::processSwitchPortQuery(QJsonDocument doc, int orgIndex, QString devSerial) {
	qDebug() << "\nAPIHelper::processSwitchPortQuery(...), orgIndex: "
			 << orgIndex << "\tdevSerial" << devSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgVPNQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// get the index of the device in the organization inventory
	deviceInInventory tmpDevice = parent->orgList.at(orgIndex)->getOrgDeviceFromSerial(devSerial);
	int devIndex = parent->orgList.at(orgIndex)->getIndexOfInventoryDevice(tmpDevice.serial);

	if (devIndex == -1) {
		qDebug() << "Could not find " << devSerial << " in orgIndex " << orgIndex;
		return false;
	}

	qDebug() << devSerial << " is at index: " << devIndex;
	parent->orgList.at(orgIndex)->setSwitchPortNum(devIndex, jArray.size());


	// get switch ports info in the appropriate organization device
	for (int i = 0; i < jArray.size(); i++) {
		switchPort tmpPort;
		QJsonObject jObj = jArray.at(i).toObject();

		tmpPort.number = jObj["number"].toInt();
		tmpPort.name = jObj["name"].toString();
		tmpPort.tags = jObj["tags"].toString();
		tmpPort.enabled = jObj["enabled"].toBool();
		tmpPort.poeEnabled = jObj["poeEnabled"].toBool();
		tmpPort.type = jObj["type"].toString();
		tmpPort.nativeVlan = jObj["vlan"].toInt();
		tmpPort.voiceVlan = jObj["voiceVlan"].toInt();
		tmpPort.allowedVLANs = jObj["allowedVlans"].toString();
		tmpPort.isolationEnabled = jObj["isolationEnabled"].toBool();
		tmpPort.rstpEnabled = jObj["rstpEnabled"].toBool();
		tmpPort.stpGuard = jObj["stpGuard"].toString();
		tmpPort.accessPolicyNumber = jObj["accessPolicyNumber"].toString();

		parent->orgList.at(orgIndex)->setSwitchPort(devIndex, tmpPort, i);

	}


	parent->displayMSPort(devIndex, orgIndex);	// display things in the table

	return true;	// everything went ok

}

bool APIHelper::processMXL3FirewallQuery(QJsonDocument doc, int orgIndex, QString devSerial) {
	qDebug() << "\nAPIHelper::processMXL3FirewallQuery(...), orgIndex: "
			 << orgIndex << "\tdevSerial" << devSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processMXL3FirewallQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// get the index of the device in the organization inventory
	deviceInInventory tmpDevice = parent->orgList.at(orgIndex)->getOrgDeviceFromSerial(devSerial);
	int devIndex = parent->orgList.at(orgIndex)->getIndexOfInventoryDevice(tmpDevice.serial);
	parent->orgList.at(orgIndex)->setMXL3RulesNum(devIndex, jArray.size());


	// get MX L3 firewall rule info in the appropriate organization device
	for (int i = 0; i < jArray.size(); i++) {
		mxL3Firewall tmpRule;
		QJsonObject jObj = jArray.at(i).toObject();

		tmpRule.comment = jObj["comment"].toString();
		tmpRule.policy = jObj["policy"].toString();
		tmpRule.protocol = jObj["protocol"].toString();
		tmpRule.srcPort = jObj["srcPort"].toString();
		tmpRule.srcCidr = jObj["srcCidr"].toString();
		tmpRule.destPort = jObj["destPort"].toString();
		tmpRule.destCidr = jObj["destCidr"].toString();
		tmpRule.syslogEnabled = jObj["syslogEnabled"].toBool();

		parent->orgList.at(orgIndex)->setMXL3Rule(devIndex, tmpRule, i);

	}


	parent->displayMXL3Rules(devIndex, orgIndex);	// display things in the table

	return true;	// everything went ok

}

bool APIHelper::processSMDevicesQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processSMDevicesQuery(...), orgIndex: "
			 << orgIndex << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processSMDevicesQuery(...)";
		return false;
	}

	// for some reason, the reply contains an array called devices[]
	QJsonArray jArray = doc.object()["devices"].toArray();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList.at(orgIndex)->setSMDevicesNum(netIndex, jArray.size());

	for (int i = 0; i < jArray.size(); i++) {
		smDevice tmpSMDevice;
		QJsonObject jObj = jArray.at(i).toObject();

		// default fields
		tmpSMDevice.id = jObj["id"].toString();
		tmpSMDevice.name = jObj["name"].toString();

		QJsonArray tmpJArray = jObj["tags"].toArray();
		for (int j = 0; j < tmpJArray.size(); j++) {
			tmpSMDevice.tags.append(tmpJArray.at(j).toString());
		}

		tmpSMDevice.ssid = jObj["ssid"].toString();
		tmpSMDevice.wifiMac = jObj["wifiMac"].toString();
		tmpSMDevice.osName = jObj["osName"].toString();
		tmpSMDevice.systemModel = jObj["systemModel"].toString();
		tmpSMDevice.uuid = jObj["uuid"].toString();
		tmpSMDevice.serialNumber = jObj["serialNumber"].toString();


		// optional fields
		tmpSMDevice.ip = jObj["ip"].toString();
		tmpSMDevice.systemType = jObj["systemType"].toString();
		tmpSMDevice.availableDeviceCapacity = jObj["availableDeviceCapacity"].toDouble();
		tmpSMDevice.kioskAppName = jObj["kioskAppName"].toString();
		tmpSMDevice.biosVersion = jObj["biosVersion"].toString();
		tmpSMDevice.lastConnected = jObj["lastConnected"].toDouble();
		tmpSMDevice.missingAppsCount = jObj["missingAppsCount"].toDouble();
		tmpSMDevice.userSuppliedAddress = jObj["userSuppliedAddress"].toString();
		tmpSMDevice.location = jObj["location"].toString();
		tmpSMDevice.lastUser = jObj["lastUser"].toString();
		tmpSMDevice.publicIp = jObj["publicIp"].toString();
		tmpSMDevice.phoneNumber = jObj["phoneNumber"].toString();
		tmpSMDevice.diskInfoJson = jObj["diskInfoJson"].toString();
		tmpSMDevice.deviceCapacity = jObj["deviceCapacity"].toDouble();
		tmpSMDevice.isManaged = jObj["isManaged"].toBool();
		tmpSMDevice.hadMdm = jObj["hadMdm"].toBool();
		tmpSMDevice.isSupervised = jObj["isSupervised"].toBool();
		tmpSMDevice.meid = jObj["meid"].toString();
		tmpSMDevice.imei = jObj["imei"].toString();
		tmpSMDevice.iccid = jObj["iccid"].toString();
		tmpSMDevice.simCarrierNetwork = jObj["simCarrierNetwork"].toString();
		tmpSMDevice.cellularDataUsed = jObj["cellularDataUsed"].toDouble();
		tmpSMDevice.isHotspotEnabled = jObj["isHotspotEnabled"].toBool();
		tmpSMDevice.createdAt = jObj["createdAt"].toDouble();
		tmpSMDevice.batteryEstCharge = jObj["batteryEstCharge"].toString();
		tmpSMDevice.quarantined = jObj["quarantined"].toBool();
		tmpSMDevice.avName = jObj["avName"].toString();
		tmpSMDevice.avRunning = jObj["avRunning"].toString();
		tmpSMDevice.asName = jObj["asName"].toString();
		tmpSMDevice.fwName = jObj["fwName"].toString();
		tmpSMDevice.isRooted = jObj["isRooted"].toBool();
		tmpSMDevice.loginRequired = jObj["loginRequired"].toBool();
		tmpSMDevice.screenLockEnabled = jObj["screenLockEnabled"].toBool();
		tmpSMDevice.screenLockDelay = jObj["screenLockDelay"].toString();
		tmpSMDevice.autoLoginDisabled = jObj["autoLoginDisabled"].toBool();
		tmpSMDevice.hasMdm = jObj["hasMdm"].toBool();
		tmpSMDevice.hasDesktopAgent = jObj["hasDesktopAgent"].toBool();
		tmpSMDevice.diskEncryptionEnabled = jObj["diskEncryptionEnabled"].toBool();
		tmpSMDevice.hardwareEncryptionCaps = jObj["hardwareEncryptionCaps"].toString();
		tmpSMDevice.passCodeLock = jObj["passCodeLock"].toBool();

		parent->orgList.at(orgIndex)->setSMDevice(netIndex, tmpSMDevice, i);

	}

	parent->displaySMDevices(orgIndex);

	return true;	// everything went ok

}

bool APIHelper::processGroupPolicyQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processGroupPolicyQuery(...), orgIndex: "
			 << orgIndex << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processGroupPolicyQuery(...)";
		return false;
	}

	// the reply contains an array containing the group policies
	QJsonArray jArray = doc.toArray();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList.at(orgIndex)->setGroupPolicyNum(netIndex, jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		groupPolicy tmpGPolicy;

		tmpGPolicy.name = jObj["name"].toString();
		tmpGPolicy.groupPolicyId = jObj["groupPolicyId"].toInt();

		parent->orgList.at(orgIndex)->setGroupPolicy(netIndex, tmpGPolicy, i);

	}

	return true;	// everything went ok

}

bool APIHelper::processClientsConnectedQuery(QJsonDocument doc, int orgIndex, QString devSerial) {
	qDebug() << "\nAPIHelper::processClientsConnectedQuery(...), orgIndex: "
			 << orgIndex << "\tdevSerial" << devSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processClientsConnectedQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// get the index of the device in the organization inventory
	deviceInInventory tmpDevice = parent->orgList.at(orgIndex)->getOrgDeviceFromSerial(devSerial);
	int devIndex = parent->orgList.at(orgIndex)->getIndexOfInventoryDevice(tmpDevice.serial);

	if (devIndex == -1) {
		qDebug() << "Could not find " << devSerial << " in orgIndex " << orgIndex;
		return false;
	}

	qDebug() << devSerial << " is at index: " << devIndex;
	parent->orgList.at(orgIndex)->setClientsConnectedNum(devIndex, jArray.size());


	// get clients connected info in the appropriate organization device
	for (int i = 0; i < jArray.size(); i++) {
		clientConnected tmpClient;
		QJsonObject jObj = jArray.at(i).toObject();

		tmpClient.sent = jObj["usage"].toObject()["sent"].toDouble();
		tmpClient.recv = jObj["usage"].toObject()["recv"].toDouble();

		tmpClient.id = jObj["id"].toString();
		tmpClient.description = jObj["description"].toString();
		tmpClient.mdnsName = jObj["mdnsName"].toString();
		tmpClient.dhcpHostname = jObj["dhcpHostname"].toString();
		tmpClient.mac = jObj["mac"].toString();
		tmpClient.ip = jObj["ip"].toString();
		tmpClient.vlan = jObj["vlan"].toInt();
		tmpClient.switchport = jObj["switchport"].toString();

		parent->orgList.at(orgIndex)->setClientConnected(devIndex, tmpClient, i);

	}


	return true;	// everything went ok

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
	return queueEventRequests.at(eventIndex);
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
