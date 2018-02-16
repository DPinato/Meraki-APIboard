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

	if (queryURL.indexOf(QString("[mac]")) != -1) {
		// queryURL has a MAC address
		qDebug() << queryURL.indexOf(QString("[serial]"));
		int devIndex = parent->orgList.at(e.orgIndex)->getIndexOfInventoryDevice(e.deviceSerial);
		QString mac = parent->orgList.at(e.orgIndex)->getOrgInventoryDevice(devIndex).mac;
		queryURL = queryURL.replace(queryURL.indexOf(QString("[mac]"))
									, QString("[mac]").length()
									, mac);
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
			processOrgAdminsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 1: {
			// POST /organizations/[organizationId]/admins
			processOrgAdminsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 2: {
			// PUT /organizations/[organizationId]/admins/[id]
			processOrgAdminsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 3: {
			// DELETE /organizations/[organizationId]/admins/[id]
			processOrgAdminsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 4: {
			// GET /devices/[serial]/clients
			processClientsConnectedQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 5: {
			// GET /networks/[networkId]/clients/[mac]/policy, FINISH IT
			processClientGroupPolicyQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 6: {
			// PUT /networks/[networkId]/clients/[mac]/policy, FINISH IT
			processClientGroupPolicyQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}




		case 9: {
			// GET /organizations/[organizationId]/configTemplates
			processOrgConfigTemplatesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 10: {
			// DELETE /organizations/[organizationId]/configTemplates/[id]
			processOrgConfigTemplatesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 11: {
			// GET /networks/[networkId]/devices, Meraki devices in the network
			processNetworkDevicesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 12: {
			// GET /networks/[networkId]/devices/[serial]
			processNetworkDevicesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 14: {
			// PUT /networks/[networkId]/devices/[serial]
			processNetworkDevicesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 13: {
			 // GET /networks/[networkId]/devices/[serial]/uplink
			processNetworkDeviceUplinkQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 15: {
			// POST /networks/[networkId]/devices/claim
			processNetworkDeviceClaimed(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 16: {
			// POST /networks/[networkId]/devices/[serial]/remove
			processNetworkDeviceClaimed(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 17: {
			// GET /networks/[networkId]/devices/[serial]/lldp_cdp
			processDeviceLLDPCDPQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 18: {
			// GET /networks/[networkId]/cellularFirewallRules
			processNetworkCellularFirewallQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 19: {
			// PUT /networks/[networkId]/cellularFirewallRules
			processNetworkCellularFirewallQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}




		case 20: {
			// GET /networks/[networkId]/l3FirewallRules
			processl3FirewallQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									 , queueEventRequests.at(eventIndex).deviceSerial);
			break;
		}

		case 22: {
			// GET /organizations/[organizationId]/vpnFirewallRules
			processOrgVPNFirewallRulesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
			break;
		}

		case 26: {
			// GET /networks/[networkId]/groupPolicies
			processNetworkGroupPolicyQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									, queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 27: {
			// GET /organizations/[organizationId]/networks
			processNetworkQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
			break;
		}

		case 28: {
			// GET /networks/[networkId]
			processNetworkQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								, queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 34: {
			// GET /networks/[networkId]/siteToSiteVpn
			processMerakiS2SVPNQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									 , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 36: {
			// GET /networks/[networkId]/traffic
			processNetworkTrafficQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									   , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 37: {
			// GET /networks/[networkId]/accessPolicies
			processNetworkAccessPoliciesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
											  , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 38: {
			// GET /networks/[networkId]/airMarshal
			processNetworkAirMarshalQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
											  , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 39: {
			// GET /networks/[networkId]/bluetoothSettings
			processNetworkBtoothSettingsQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
											  , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 41: {
			// GET /organizations
			processOrgQuery(jDoc);
			break;
		}

		case 42: {
			// GET /organizations/[organizationId]
			processOrgQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
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

		case 53: {
			// GET /networks/[networkId]/phoneAssignments
			processNetworkPhonesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								  , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 54: {
			// GET /networks/[networkId]/phoneAssignments/[serial]
			processNetworkPhonesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									  , queueEventRequests.at(eventIndex).netIndex
									  , queueEventRequests.at(eventIndex).deviceSerial);
			break;
		}

		case 57: {
			// GET /networks/[networkId]/phoneCallgroups
			processNetworkPhoneCallgroupsQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
											   , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 58: {
			// GET /networks/[networkId]/phoneCallgroups/[id]
			processNetworkPhoneCallgroupsQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
											   , queueEventRequests.at(eventIndex).netIndex
											   , queueEventRequests.at(eventIndex).id);
			break;
		}

		case 61: {
			// GET /networks/[networkId]/phoneContacts
			processNetworkPhoneContactsQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								  , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 66: {
			// GET /networks/[networkId]/phoneNumbers
			// ???????
			break;
		}

		case 67: {
			// GET /networks/[networkId]/phoneNumbers/available
			// ???????
			break;
		}

		case 68: {
			// GET /organizations/[organizationId]/samlRoles
			processSamlRolesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex);
			break;
		}

		case 69: {
			// GET /organizations/[organizationId]/samlRoles/[id]
			processSamlRolesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								  , queueEventRequests.at(eventIndex).id);
			break;
		}

		case 76: {
			// GET /networks/[networkId]/sm/profile/clarity/[id]
			// figure out how to treat this, since an id needs to be presented
			// are there multiple clarity profiles in the network?
			// are the IDs of the profile the same ones as normal SM profiles?

			break;
		}

		case 81: {
			// GET /networks/[networkId]/sm/profile/umbrella/[id]

			break;
		}




		case 87: {
			// GET /networks/[networkId]/sm/devices
			processSMDevicesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								  , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 94: {
			// GET /networks/[networkId]/sm/profiles

			break;
		}



		case 95: {
			// GET /networks/[networkId]/ssids
			processNetworkSSIDsQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									 , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 96: {
			// GET /networks/[networkId]/ssids/[id]
			processNetworkSSIDsQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									 , queueEventRequests.at(eventIndex).netIndex
									 , queueEventRequests.at(eventIndex).ssidIndex);
			break;
		}

		case 98: {
			// GET /devices/[serial]/switchPorts
			processSwitchPortQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								   , queueEventRequests.at(eventIndex).deviceSerial);
			break;
		}

		case 99: {
			// GET /devices/[serial]/switchPorts/[id]
			processSwitchPortQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
								   , queueEventRequests.at(eventIndex).deviceSerial
								   , queueEventRequests.at(eventIndex).id);
			break;
		}

		case 101: {
			// GET /networks/[networkId]/staticRoutes
			processNetworkStaticRoutesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
											, queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 102: {
			// GET /networks/[networkId]/staticRoutes/[id]
			processNetworkStaticRoutesQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
											, queueEventRequests.at(eventIndex).netIndex
											, queueEventRequests.at(eventIndex).id);
			break;
		}


		case 106: {
			// GET /networks/[networkId]/vlans
			processNetworkVlansQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									 , queueEventRequests.at(eventIndex).netIndex);
			break;
		}

		case 107: {
			// GET /networks/[networkId]/vlans/[id]
			processNetworkVlansQuery(jDoc, queueEventRequests.at(eventIndex).orgIndex
									 , queueEventRequests.at(eventIndex).netIndex
									 , queueEventRequests.at(eventIndex).id);
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

bool APIHelper::processOrgQuery(QJsonDocument doc, int orgIndex) {
	// call this after querying list of organizations
	// if orgIndex is not -1, it means that a particular org was queried
	qDebug() << "\nAPIHelper::processOrgQuery(...)";

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgQuery(...)";
		return false;
	}

	// get organization data
	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	int i = orgIndex;
	int count = orgIndex+1;	// makes it so that the for-loop will do at least 1 iteration
	if (orgIndex == -1) {
		i = 0;		// update all the orgs
		count = jArray.size();

		// TODO: consider deleting all the MOrganization objects before this
		parent->orgList.resize(jArray.size());
	}


	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		parent->orgList[i] = new MOrganization();
		parent->orgList[i]->setOrgID(jObj["id"].toVariant().toString());
//		parent->orgList[i]->setOrgID(jObj["id"].toDouble());
		parent->orgList[i]->setOrgName(jObj["name"].toString());
		parent->orgList[i]->setOrgSamlUrl(jObj["samlConsumerUrl"].toString());

		QJsonArray jSaml = jObj["samlConsumerUrls"].toArray();
//		parent->orgList[i]->


	}




	parent->updateOrgUI(-1);

	return true;	// everything OK

}

bool APIHelper::processNetworkQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	// if a netIndex is returned, the query was run for a single network in the organization
	qDebug() << "\nAPIHelper::processNetworkQuery(), netIndex: " << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	int i = netIndex;
	int count = netIndex+1;	// makes it so that the for-loop will do at least 1 iteration
	if (netIndex == -1) {
		i = 0;		// update all the networks in the organization
		count = jArray.size();
		parent->orgList[orgIndex]->setNetworksNum(count);
	}


	for (i; i < count; i++) {
		// this loop assumes that the whole list of networks in an org was queried, before a single network
		QJsonObject jObj = jArray.at(i).toObject();
		networkVars tmpNetVar = parent->orgList[orgIndex]->getNetwork(i);

		tmpNetVar.netID = jObj["id"].toString();
		tmpNetVar.netName = jObj["name"].toString();
		tmpNetVar.orgID = jObj["organizationId"].toVariant().toString();
		tmpNetVar.netTimezone = jObj["timeZone"].toString();
		tmpNetVar.netType = jObj["type"].toString();
		tmpNetVar.netTags = (jObj["tags"].toString());


		// increase number of networks in org
		parent->orgList[orgIndex]->setNetwork(tmpNetVar, i);

	}


	parent->updateOrgUI(-1);
	parent->updateOrgUI(orgIndex);

	return true;	// everything OK

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

bool APIHelper::processOrgAdminsQuery(QJsonDocument doc, eventRequest e) {
	// process list of administrators in organization obtained through
	// GET /organizations/[organizationId]/admins
	qDebug() << "\nAPIHelper::processOrgAdminsQuery(...), orgIndex: " << e.orgIndex
			 << "\treqType: " << e.req.reqType << "\tid: " << e.id;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgAdminsQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 0 returns all the admins in the org, GET
	// urlList 1 returns the admin created, POST
	// urlList 2 returns the admin updated, PUT
	// urlList 3 returns nothing, DELETE
	int adminIndex = parent->orgList[e.orgIndex]->getIndexOfOrgAdmin(e.id);
	int i = 0;
	int count = jArray.size();

	if (e.req.reqType == 2 || e.req.reqType == 3) {
		// only do single id
		i = adminIndex;
		if (i == -1) {
			// a new administrator needs to be created, increment the vector size
			i = parent->orgList[e.orgIndex]->getAdminListSize();
			parent->orgList[e.orgIndex]->setAdminsNum(i+1);
		}

		count = i+1;
	} else if (e.req.reqType == 1) {
		parent->orgList[e.orgIndex]->setAdminsNum(jArray.size());
	} else if (e.req.reqType == 4) {
		return parent->orgList[e.orgIndex]->removeOrgAdmin(i);
	}


	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		adminStruct tmpAdmin;	// this is here since it needs to completely reset at every iteration

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


		parent->orgList[e.orgIndex]->setAdmin(tmpAdmin, i);

	}


	parent->displayAdminStuff(e.orgIndex);

	return true;	// everything went well

}

bool APIHelper::processOrgConfigTemplatesQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processOrgConfigTemplatesQuery(...), orgIndex: " << e.orgIndex
			 << "\tid: " << e.id;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgConfigTemplatesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// urlList 9 returns all the configuration templates in the org, GET
	// urlList 10 returns nothing, DELETE
	int templateIndex = parent->orgList[e.orgIndex]->getIndexOfConfigTemplate(e.id);
	if (e.urlListIndex == 9) {
		parent->orgList[e.orgIndex]->setOrgConfigTemplatesNum(jArray.size());

	} else if (e.urlListIndex == 10) {
		return parent->orgList[e.orgIndex]->removeOrgConfigTemplate(templateIndex);
	}


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		configTemplate tmpTemplate;

		tmpTemplate.id = jObj["id"].toString();
		tmpTemplate.name = jObj["name"].toString();

		parent->orgList[e.orgIndex]->setOrgConfigTemplate(tmpTemplate, i);

	}


	return true;	// everything went well

}

bool APIHelper::processSamlRolesQuery(QJsonDocument doc, int orgIndex, QString id) {
	qDebug() << "\nAPIHelper::processSamlRolesQuery(...), orgIndex: " << orgIndex
			 << "\tid: " << id;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processSamlRolesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	int i = 0;
	int count = jArray.size();
	if (id.length() > 0) {
		// only do single id
		i = parent->orgList[orgIndex]->getIndexOfSamlRole(id);
		count = i+1;
	} else {
		parent->orgList[orgIndex]->setOrgSamlRolesNum(jArray.size());
	}


	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		orgSamlRoles tmpSamlRole;

		tmpSamlRole.id = jObj["id"].toString();
		tmpSamlRole.role = jObj["role"].toString();
		tmpSamlRole.orgAccess = jObj["orgAccess"].toString();

		QJsonArray jPerm = jObj["networks"].toArray();		// networks
		tmpSamlRole.networks.resize(jPerm.size());
		for (int j = 0; j < jPerm.size(); j++) {
			adminNetPermission tmpPerm;
			tmpPerm.netID = jPerm.at(j).toObject()["id"].toString();
			tmpPerm.accessLevel = jPerm.at(j).toObject()["access"].toString();
			tmpSamlRole.networks[j] = tmpPerm;
		}

		QJsonArray jTags = jObj["tags"].toArray();			// tags
		tmpSamlRole.tags.resize(jTags.size());
		for (int j = 0; j < jTags.size(); j++) {
			adminTag tmpTag;
			tmpTag.tag = jTags.at(j).toObject()["tag"].toString();
			tmpTag.adminAccessLevel = jTags.at(j).toObject()["access"].toString();
			tmpSamlRole.tags[j] = tmpTag;
		}


		parent->orgList[orgIndex]->setOrgSamlRole(tmpSamlRole, i);

	}


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
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[orgIndex]->setOrgInventorySize(jArray.size());


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

bool APIHelper::processNetworkDevicesQuery(QJsonDocument doc, eventRequest event) {
	// if serial is present, it means that the query was run for a single device
	// if serial is empty, get all devices in the network
	// this should probably be run with a serial number only after it is run without it, to avoid
	// QVector out of bounds accesses
	// TODO: beaconIdParams are also returned
	qDebug() << "\nAPIHelper::processNetworkDevicesQuery(...), orgIndex: " << event.orgIndex
			 << "\tnetIndex" << event.netIndex << "\tserial: " << event.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkDevicesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// urlList 11 returns all the devices in a network, GET
	// urlList 12 returns a single device in the network, GET
	// urlList 14 returns a single device in the network, PUT
	int devIndex = parent->orgList[event.orgIndex]->getIndexOfInventoryDevice(event.deviceSerial);
	int i = 0;
	int count = jArray.size();

	if (event.urlListIndex == 11) {
		// process all serials in the network
		parent->orgList[event.orgIndex]->setNetworkDevicesNum(event.netIndex, jArray.size());
	} else if (event.urlListIndex == 12 || event.urlListIndex == 14) {
		// process data for single serial in network
		i = devIndex;
		count = i+1;
	}


	// this will make it run at least once
	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		deviceInNetwork tmpNetDev;

		tmpNetDev.lanIp = jObj["lanIp"].toString();
		tmpNetDev.serial = jObj["serial"].toString();
		tmpNetDev.mac = jObj["mac"].toString();
		tmpNetDev.lat = jObj["lat"].toDouble();
		tmpNetDev.lng = jObj["lng"].toDouble();
		tmpNetDev.address = jObj["address"].toString();
		tmpNetDev.name = jObj["name"].toString();
		tmpNetDev.model = jObj["model"].toString();
		tmpNetDev.networkId = jObj["networkId"].toString();
		tmpNetDev.wan1Ip = jObj["wan1Ip"].toString();
		tmpNetDev.wan2Ip = jObj["wan2Ip"].toString();

		parent->orgList[event.orgIndex]->setNetworkDevice(event.netIndex, tmpNetDev, i);

	}


	return true;	// everything went well

}

bool APIHelper::processNetworkDeviceUplinkQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkDeviceUplinkQuery(...), orgIndex: "
			 << e.orgIndex << "\tnetIndex" << e.netIndex << "\tdevSerial: " << e.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkDeviceUplinkQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// the idea here is that the deviceInNetwork in the networkVars, will be overwritten with the
	// uplink information added to it
	networkVars tmpNet = parent->orgList[e.orgIndex]->getNetwork(e.netIndex);
	int netDevIndex = parent->orgList.at(e.orgIndex)->getIndexOfNetworkDevice(tmpNet.netID, e.deviceSerial);
	deviceInNetwork tmpNetDev = parent->orgList.at(e.orgIndex)->getNetworkDevice(e.netIndex, netDevIndex);


	// read uplink info
	tmpNetDev.uplinkNum = jArray.size();

	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		deviceUplink tmpDevUplink;

		tmpDevUplink.interface = jObj["interface"].toString();
		tmpDevUplink.status = jObj["status"].toString();
		tmpDevUplink.ip = jObj["ip"].toString();
		tmpDevUplink.gateway = jObj["gateway"].toString();
		tmpDevUplink.publicIp = jObj["publicIp"].toString();
		tmpDevUplink.dns = jObj["dns"].toString();
		tmpDevUplink.usingStaticIp = jObj["usingStaticIp"].toBool();

		tmpNetDev.uplink[i] = tmpDevUplink;

	}


	// update networkVars netDevices element
	// this should only run if netDevices.size() > 0
	// so this function should probably be used only after
	parent->orgList[e.orgIndex]->setNetworkDevice(e.netIndex, tmpNetDev, netDevIndex);

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
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[orgIndex]->setOrgVPNPeerNum(jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		nonMerakiVPNPeer tmpVPN;

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

bool APIHelper::processOrgVPNFirewallRulesQuery(QJsonDocument doc, int orgIndex) {
	qDebug() << "\nAPIHelper::processOrgVPNFirewallRulesQuery(...), orgIndex: " << orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgVPNFirewallRulesQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[orgIndex]->setOrgVPNFirewallRulesNum(jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		l3Firewall tmpRule;

		tmpRule.comment = jObj["comment"].toString();
		tmpRule.policy = jObj["policy"].toString();
		tmpRule.protocol = jObj["protocol"].toString();
		tmpRule.srcPort = jObj["srcPort"].toString();
		tmpRule.srcCidr = jObj["srcCidr"].toString();
		tmpRule.destPort = jObj["destPort"].toString();
		tmpRule.destCidr = jObj["destCidr"].toString();
		tmpRule.syslogEnabled = jObj["syslogEnabled"].toBool();

		parent->orgList.at(orgIndex)->setOrgVPNFirewallRule(tmpRule, i);

	}

	return true;	// everything went ok

}

bool APIHelper::processSwitchPortQuery(QJsonDocument doc, int orgIndex, QString devSerial, QString id) {
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


	int i = 0;
	int count = jArray.size();
	if (id.length() > 0) {
		// only do single id
		count = i+1;
	} else {
		parent->orgList.at(orgIndex)->setSwitchPortNum(devIndex, jArray.size());
	}



	// get switch ports info in the appropriate organization device
	for (i; i < count; i++) {
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

bool APIHelper::processl3FirewallQuery(QJsonDocument doc, int orgIndex, QString devSerial) {
	qDebug() << "\nAPIHelper::processl3FirewallQuery(...), orgIndex: "
			 << orgIndex << "\tdevSerial" << devSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processl3FirewallQuery(...)";
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
		l3Firewall tmpRule;
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

bool APIHelper::processNetworkCellularFirewallQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkCellularFirewallQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkCellularFirewallQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 18 returns a list of all the cellular firewall rules, GET
	// urlList 19 returns a list of all the cellular firewall rules, PUT
	// no need for any special processing here, since an array will be returned anyway
	parent->orgList[e.orgIndex]->setNetworkCellularRulesNum(e.netIndex, jArray.size());

	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		l3Firewall tmpRule;

		tmpRule.comment = jObj["comment"].toString();
		tmpRule.policy = jObj["policy"].toString();
		tmpRule.protocol = jObj["protocol"].toString();
		tmpRule.destPort = jObj["destPort"].toString();
		tmpRule.destCidr = jObj["destCidr"].toString();
		tmpRule.srcPort = jObj["srcPort"].toString();
		tmpRule.srcCidr = jObj["srcCidr"].toString();
		tmpRule.syslogEnabled = jObj["syslogEnabled"].toBool();

		parent->orgList[e.orgIndex]->setNetworkCellularRule(e.netIndex, tmpRule, i);

	}


	return true;	// everything went ok

}

bool APIHelper::processSMDevicesQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processSMDevicesQuery(...), orgIndex: "
			 << orgIndex << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processSMDevicesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.object()["devices"].toArray();	// the reply contains an array called devices[]
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

bool APIHelper::processNetworkGroupPolicyQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processGroupPolicyQuery(...), orgIndex: "
			 << orgIndex << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processGroupPolicyQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();	// the reply contains an array containing the group policies
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList.at(orgIndex)->setNetworkGroupPolicyNum(netIndex, jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		networkGroupPolicy tmpGPolicy;

		tmpGPolicy.name = jObj["name"].toString();
		tmpGPolicy.groupPolicyId = jObj["groupPolicyId"].toInt();

		parent->orgList.at(orgIndex)->setNetworkGroupPolicy(netIndex, tmpGPolicy, i);

	}

	return true;	// everything went ok

}

bool APIHelper::processClientsConnectedQuery(QJsonDocument doc, eventRequest event) {
	qDebug() << "\nAPIHelper::processClientsConnectedQuery(...), orgIndex: " << event.orgIndex
			 << "\tdevSerial" << event.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processClientsConnectedQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// get the index of the device in the organization inventory
	deviceInInventory tmpDevice = parent->orgList.at(event.orgIndex)->getOrgDeviceFromSerial(event.deviceSerial);
	int devIndex = parent->orgList.at(event.orgIndex)->getIndexOfInventoryDevice(tmpDevice.serial);

	if (devIndex == -1) {
		qDebug() << "Could not find " << event.deviceSerial << " in orgIndex " << event.orgIndex;
		return false;
	}

	qDebug() << event.deviceSerial << " is at index: " << devIndex;
	parent->orgList.at(event.orgIndex)->setClientsConnectedNum(devIndex, jArray.size());


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

		parent->orgList.at(event.orgIndex)->setClientConnected(devIndex, tmpClient, i);

	}


	return true;	// everything went ok

}

bool APIHelper::processClientGroupPolicyQuery(QJsonDocument doc, eventRequest e) {
	// TODO: finish this, the tricky part is how to store the network ID and the MAC address
	// of the client queried
	qDebug() << "\nAPIHelper::processClientGroupPolicyQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex << "\tclientMac" << e.clientMac;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processClientGroupPolicyQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();




	return true;	// everything went ok

}

bool APIHelper::processDeviceLLDPCDPQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processDeviceLLDPCDPQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex << "\tdevSerial" << e.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processDeviceLLDPCDPQuery(...)";
		return false;
	}

	// an array is not returned anywhere in the JSON reply
	// try this anyway for ports, since a bunch of port values will be returned
	QJsonObject jObj = doc.object();
	jObj = jObj["ports"].toObject();
	qDebug() << jObj << "\t" << jObj.size();


	// the idea here is that the deviceInNetwork in the networkVars, will be overwritten with the
	// uplink information added to it
	networkVars tmpNet = parent->orgList[e.orgIndex]->getNetwork(e.netIndex);
	int netDevIndex = parent->orgList.at(e.orgIndex)->getIndexOfNetworkDevice(tmpNet.netID, e.deviceSerial);
	deviceInNetwork tmpNetDev = parent->orgList.at(e.orgIndex)->getNetworkDevice(e.netIndex, netDevIndex);

	int portCount = jObj.keys().size();

	// this is a workaround, since there are no devices with more than 54 ports
	// 48 LAN ports + 4 SFP + 2 stack ports
	tmpNetDev.devCDP.resize(54);
	tmpNetDev.devLLDP.resize(54);

	for (int i = 0; i < portCount; i++) {
		// this is not great since it will still go through portCount iterations
		// I do not think there is an alternative, as no array is returned here
		QJsonObject jObjPort = jObj[QString::number(i)].toObject();
//		QString portId = jObjPort.keys().at(i);

		// CDP
		tmpNetDev.devCDP[i].deviceId = jObjPort["deviceId"].toString();
		tmpNetDev.devCDP[i].portId = jObjPort["portId"].toString();
		tmpNetDev.devCDP[i].address = jObjPort["address"].toString();
		tmpNetDev.devCDP[i].sourcePort = jObjPort["sourcePort"].toString();

		// LLDP
		tmpNetDev.devLLDP[i].systemName = jObjPort["systemName"].toString();
		tmpNetDev.devLLDP[i].portId = jObjPort["portId"].toString();
		tmpNetDev.devLLDP[i].managementAddress = jObjPort["managementAddress"].toString();
		tmpNetDev.devLLDP[i].sourcePort = jObjPort["sourcePort"].toString();

	}

	// update the network device
	parent->orgList[e.orgIndex]->setNetworkDevice(e.netIndex, tmpNetDev, netDevIndex);


	return true;	// everything went well

}

bool APIHelper::processNetworkSSIDsQuery(QJsonDocument doc, int orgIndex, int netIndex, int ssidIndex) {
	// if ssidIndex is -1, a query for all the SSIDs in the network was made
	qDebug() << "\nAPIHelper::processNetworkSSIDsQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex << "\tssidIndex" << ssidIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSSIDsQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	int i = ssidIndex;
	int count = ssidIndex+1;	// makes it so that the for-loop will do at least 1 iteration
	if (ssidIndex == -1) {
		i = 0;		// update all the SSIDs in the network
		count = jArray.size();
	}

	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		ssid tmpSSID;

		tmpSSID.number = jObj["number"].toInt();
		tmpSSID.name = jObj["name"].toString();
		tmpSSID.enabled = jObj["enabled"].toBool();
		tmpSSID.splashPage = jObj["splashPage"].toString();
		tmpSSID.ssidAdminAccessible = jObj["ssidAdminAccessible"].toBool();
		tmpSSID.authMode = jObj["authMode"].toString();
		tmpSSID.psk = jObj["psk"].toString();
		tmpSSID.encryptionMode = jObj["encryptionMode"].toString();
		tmpSSID.wpaEncryptionMode = jObj["wpaEncryptionMode"].toString();

		QJsonArray jRadius = jObj["radiusServers"].toArray();
		tmpSSID.radiusServers.resize(jRadius.size());
		for (int j = 0; j < jRadius.size(); j++) {
			radiusServer tmpRadius;
			tmpRadius.host = jRadius.at(j).toObject()["host"].toString();
			tmpRadius.port = jRadius.at(j).toObject()["port"].toString();
			tmpSSID.radiusServers[i] = tmpRadius;
		}

		tmpSSID.ipAssignmentMode = jObj["ipAssignmentMode"].toString();
		tmpSSID.useVlanTagging = jObj["useVlanTagging"].toBool();
		tmpSSID.minBitrate = jObj["minBitrate"].toInt();
		tmpSSID.bandSelection = jObj["bandSelection"].toString();
		tmpSSID.perClientBandwidthLimitUp = jObj["perClientBandwidthLimitUp"].toDouble();
		tmpSSID.perClientBandwidthLimitDown = jObj["perClientBandwidthLimitDown"].toDouble();

		parent->orgList[orgIndex]->setNetworkSSID(netIndex, tmpSSID, i);

	}

	return true;	// everything went well

}

bool APIHelper::processMerakiS2SVPNQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processMerakiS2SVPNQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processMerakiS2SVPNQuery(...)";
		return false;
	}

	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size();


	merakiVPN tmpVPN;
	tmpVPN.mode = jObj["mode"].toString();

	QJsonArray jHubs = jObj["hubs"].toArray();
	tmpVPN.hubs.resize(jHubs.size());
	for (int i = 0; i < jHubs.size(); i++) {
		tmpVPN.hubs[i].hubId = jHubs.at(i).toObject()["hubId"].toString();
		tmpVPN.hubs[i].useDefaultRoute = jHubs.at(i).toObject()["useDefaultRoute"].toBool();
	}

	QJsonArray jSubnets = jObj["subnets"].toArray();
	tmpVPN.subnets.resize(jSubnets.size());
	for (int i = 0; i < jSubnets.size(); i++) {
		tmpVPN.subnets[i].localSubnet = jSubnets.at(i).toObject()["localSubnet"].toString();
		tmpVPN.subnets[i].useVpn = jSubnets.at(i).toObject()["useVpn"].toBool();
	}


	parent->orgList[orgIndex]->setNetworkS2SVPN(netIndex, tmpVPN);

	return true;	// everything went well

}

bool APIHelper::processNetworkTrafficQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processNetworkTrafficQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkTrafficQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[orgIndex]->setNetworkTrafficFlowsNum(netIndex, jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netTraffic tmpTraffic;

		tmpTraffic.application = jObj["application"].toString();
		tmpTraffic.destination = jObj["destination"].toString();
		tmpTraffic.protocol = jObj["protocol"].toString();
		tmpTraffic.port = jObj["port"].toInt();
		tmpTraffic.recv = jObj["recv"].toDouble();
		tmpTraffic.sent = jObj["sent"].toDouble();
		tmpTraffic.flows = jObj["flows"].toInt();
		tmpTraffic.activeTime = jObj["activeTime"].toDouble();
		tmpTraffic.numClients = jObj["numClients"].toInt();

		parent->orgList[orgIndex]->setNetworkTrafficFlow(netIndex, tmpTraffic, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkAccessPoliciesQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processNetworkAccessPoliciesQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkAccessPoliciesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[orgIndex]->setNetworkAccessPoliciesNum(netIndex, jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netAccessPolicy tmpPolicy;

		tmpPolicy.number = jObj["number"].toInt();
		tmpPolicy.name = jObj["name"].toString();
		tmpPolicy.accessType = jObj["accessType"].toString();
		tmpPolicy.guestVlan = jObj["guestVlan"].toInt();

		QJsonArray jRadius = jObj["radiusServers"].toArray();	// get RADIUS servers
		tmpPolicy.radiusServers.resize(jRadius.size());
		for (int j = 0; j < jRadius.size(); j++) {
			radiusServer tmpRadius;
			tmpRadius.host = jRadius.at(j).toObject()["ip"].toString();
			tmpRadius.port = jRadius.at(j).toObject()["port"].toInt();
			tmpPolicy.radiusServers[j] = tmpRadius;
		}

		parent->orgList[orgIndex]->setNetworkAccessPolicy(netIndex, tmpPolicy, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkAirMarshalQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processNetworkAirMarshalQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkAirMarshalQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[orgIndex]->setNetworkAirMarshalEntriesNum(netIndex, jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netAirMarshal tmpAirMarshal;

		tmpAirMarshal.ssid = jObj["ssid"].toString();

		QJsonArray jBssids = jObj["bssids"].toArray();		// get bssids
		tmpAirMarshal.bssids.resize(jBssids.size());
		for (int j = 0; j < jBssids.size(); j++) {
			// this will be a bit messed up, since there is another QVector in here
			QJsonObject jObjBssids = jBssids.at(j).toObject();
			bssidAirMarshal tmpBssid;

			tmpBssid.bssid = jObjBssids["bssid"].toString();
			tmpBssid.contained = jObjBssids["contained"].toBool();

			// get the list of who detected this BSSID
			QJsonArray jDetectedBy = jObjBssids["detectedBy"].toArray();
			tmpBssid.detectedBy.resize(jDetectedBy.size());
			for (int m = 0; m < jDetectedBy.size(); m++) {
				detectedAirMarshal tmpDetected;
				tmpDetected.device = jDetectedBy.at(m).toObject()["device"].toString();
				tmpDetected.rssi = jDetectedBy.at(m).toObject()["rssi"].toInt();
				tmpBssid.detectedBy[m] = tmpDetected;
			}

			tmpAirMarshal.bssids[j] = tmpBssid;
		}


		QJsonArray jChannels = jObj["channels"].toArray();	// get channels
		tmpAirMarshal.channels.resize(jChannels.size());
		for (int j = 0; j < jChannels.size(); j++) {
			tmpAirMarshal.channels[j] = jChannels.at(j).toInt();		// not 100% sure this is correct
		}


		tmpAirMarshal.firstSeen = jObj["firstSeen"].toDouble();
		tmpAirMarshal.lastSeen = jObj["lastSeen"].toDouble();


		QJsonArray jWiredMacs = jObj["wiredMacs"].toArray();
		tmpAirMarshal.wiredMacs.resize(jWiredMacs.size());
		for (int j = 0; j < jWiredMacs.size(); j++) {
			tmpAirMarshal.wiredMacs[j] = jWiredMacs.at(j).toString();	// not 100% sure this is correct
		}


		QJsonArray jWiredVlans = jObj["wiredVlans"].toArray();
		tmpAirMarshal.wiredVlans.resize(jWiredVlans.size());
		for (int j = 0; j < jWiredVlans.size(); j++) {
			tmpAirMarshal.wiredVlans[j] = jWiredVlans.at(j).toInt();	// not 100% sure this is correct
		}


		tmpAirMarshal.wiredLastSeen = jObj["wiredLastSeen"].toDouble();

		parent->orgList[orgIndex]->setNetworkAirMarshalEntry(netIndex, tmpAirMarshal, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkBtoothSettingsQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processNetworkBtoothSettingsQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkBtoothSettingsQuery(...)";
		return false;
	}

	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size();

	netBtoothSettings tmpBtooth;
	tmpBtooth.id = jObj["id"].toString();	// this may not even be returned
	tmpBtooth.scanningEnabled = jObj["scanningEnabled"].toBool();
	tmpBtooth.advertisingEnabled = jObj["advertisingEnabled"].toBool();
	tmpBtooth.uuid = jObj["uuid"].toString();
	tmpBtooth.majorMinorAssignmentMode = jObj["majorMinorAssignmentMode"].toString();
	tmpBtooth.major = jObj["major"].toInt();
	tmpBtooth.minor = jObj["minor"].toInt();

	return true;	// everything went well

}

bool APIHelper::processNetworkPhonesQuery(QJsonDocument doc, int orgIndex, int netIndex, QString serial) {
	qDebug() << "\nAPIHelper::processNetworkPhonesQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex << "\tserial: " << serial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkPhonesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	int i = 0;
	int count = jArray.size();
	if (serial.length() > 0) {
		// only do single serial
		i = parent->orgList[orgIndex]->getIndexOfInventoryDevice(serial);
		count = i+1;
	} else {
		parent->orgList[orgIndex]->setNetworkPhoneNum(netIndex, jArray.size());
	}


	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netPhone tmpPhone;

		tmpPhone.serial = jObj["serial"].toString();
		tmpPhone.contactId = jObj["contactId"].toString();
		tmpPhone.contactType = jObj["contactType"].toString();
		tmpPhone.ext = jObj["ext"].toString();

		QJsonArray jPublicNumber = jObj["publicNumber"].toArray();
		tmpPhone.publicNumber.resize(jPublicNumber.size());
		for (int j = 0; j < jPublicNumber.size(); j++) {
			tmpPhone.publicNumber[j] = jPublicNumber.at(j).toString();
		}

		parent->orgList[orgIndex]->setNetworkPhone(netIndex, tmpPhone, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkPhoneContactsQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processNetworkPhoneContactsQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkPhoneContactsQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[orgIndex]->setNetworkPhoneContactNum(netIndex, jArray.size());

	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netPhoneContact tmpPhoneContact;

		tmpPhoneContact.id = jObj["id"].toInt();
		tmpPhoneContact.name = jObj["name"].toString();
		tmpPhoneContact.type = jObj["type"].toString();

		parent->orgList[orgIndex]->setNetworkPhoneContact(netIndex, tmpPhoneContact, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkPhoneCallgroupsQuery(QJsonDocument doc, int orgIndex, int netIndex, QString id) {
	qDebug() << "\nAPIHelper::processNetworkPhoneCallgroupsQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex << "\tid: " << id;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkPhoneCallgroupsQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	int i = 0;
	int count = jArray.size();
	if (id.length() > 0) {
		// only do single id
		i = parent->orgList[orgIndex]->getIndexOfPhoneCallgroupId(netIndex, id);
		count = i+1;
	} else {
		parent->orgList[orgIndex]->setNetworkPhoneCallgroupsNum(netIndex, jArray.size());
	}


	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netPhoneCallgroup tmpPhoneCGroup;

		tmpPhoneCGroup.id = jObj["id"].toString();
		tmpPhoneCGroup.name = jObj["name"].toString();
		tmpPhoneCGroup.publicNumber = jObj["publicNumber"].toString();
		tmpPhoneCGroup.ext = jObj["ext"].toString();

		parent->orgList[orgIndex]->setNetworkPhoneCallgroupEntry(netIndex, tmpPhoneCGroup, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkStaticRoutesQuery(QJsonDocument doc, int orgIndex, int netIndex, QString id) {
	qDebug() << "\nAPIHelper::processNetworkStaticRoutesQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex << "\tid: " << id;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkStaticRoutesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	int i = 0;
	int count = jArray.size();
	if (id.length() > 0) {
		// only do single id
		i = parent->orgList[orgIndex]->getIndexOfNetworkStaticRoute(netIndex, id);
		count = i+1;
	} else {
		parent->orgList[orgIndex]->setNetworkStaticRoutesNum(netIndex, jArray.size());
	}


	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netStaticRoute tmpRoute;

		tmpRoute.id = jObj["id"].toString();
		tmpRoute.networkId = jObj["networkId"].toString();
		tmpRoute.name = jObj["name"].toString();
		tmpRoute.gatewayIp = jObj["gatewayIp"].toString();
		tmpRoute.subnet = jObj["subnet"].toString();

		parent->orgList[orgIndex]->setNetworkStaticRoute(netIndex, tmpRoute, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkVlansQuery(QJsonDocument doc, int orgIndex, int netIndex, QString id) {
	qDebug() << "\nAPIHelper::processNetworkVlansQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex << "\tid: " << id;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkVlansQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	int i = 0;
	int count = jArray.size();
	if (id.length() > 0) {
		// only do single id
		i = parent->orgList[orgIndex]->getIndexOfNetworkVlan(netIndex, id);
		count = i+1;
	} else {
		parent->orgList[orgIndex]->setNetworkVlansNum(netIndex, jArray.size());
	}


	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netVlan tmpVlan;

		tmpVlan.id = jObj["id"].toString();
		tmpVlan.networkId = jObj["networkId"].toString();
		tmpVlan.name = jObj["name"].toString();
		tmpVlan.applianceIp = jObj["applianceIp"].toString();
		tmpVlan.subnet = jObj["subnet"].toString();

		parent->orgList[orgIndex]->setNetworkVlan(netIndex, tmpVlan, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkSMProfilesQuery(QJsonDocument doc, int orgIndex, int netIndex) {
	qDebug() << "\nAPIHelper::processNetworkSMProfilesQuery(...), orgIndex: " << orgIndex
			 << "\tnetIndex" << netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMProfilesQuery(...)";
		return false;
	}

	// for some reason, the response will contain an array called "profiles"
	QJsonArray jArray = doc.object()["profiles"].toArray();
	qDebug() << jArray << "\t" << jArray.size();


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		smProfile tmpSMProfile;

		tmpSMProfile.id = jObj["id"].toString();
		tmpSMProfile.payloadDisplayName = jObj["payloadDisplayName"].toString();
		tmpSMProfile.payloadIdentifier = jObj["payloadIdentifier"].toString();
		tmpSMProfile.payloadDescription = jObj["payloadDescription"].toString();
		tmpSMProfile.scope = jObj["scope"].toString();

		QJsonArray jTags = jObj["tags"].toArray();
		tmpSMProfile.tags.resize(jTags.size());
		for (int j = 0; j < jTags.size(); j++) {
			tmpSMProfile.tags[j] = jTags.at(j).toString();
		}

		QJsonArray jWifis = jObj["wifis"].toArray();
		tmpSMProfile.wifis.resize(jTags.size());
		for (int j = 0; j < jWifis.size(); j++) {
			tmpSMProfile.wifis[j] = jWifis.at(j).toString();
		}

		QJsonArray jPayloads = jObj["payload_types"].toArray();
		tmpSMProfile.payloadTypes.resize(jTags.size());
		for (int j = 0; j < jPayloads.size(); j++) {
			tmpSMProfile.payloadTypes[j] = jPayloads.at(j).toString();
		}

		parent->orgList[orgIndex]->setNetworkSMProfile(netIndex, tmpSMProfile, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkDeviceClaimed(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkDeviceClaimed(...), netIndex: " << e.netIndex
			 << "\tserial: " << e.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkDeviceClaimed(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// urlList 15 returns nothing, POST
	// urlList 16 returns nothing, POST
	return true;	// everything went well

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
