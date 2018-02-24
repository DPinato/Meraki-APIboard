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
			processl3FirewallQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 21: {
			// PUT /networks/[networkId]/l3FirewallRules
			processl3FirewallQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 22: {
			// GET /organizations/[organizationId]/vpnFirewallRules
			processOrgVPNFirewallRulesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 23: {
			// PUT /organizations/[organizationId]/vpnFirewallRules
			processOrgVPNFirewallRulesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 24: {
			// GET /networks/[networkId]/ssids/[number]/l3FirewallRules
			processNetworkSSIDFirewallRulesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 25: {
			// PUT /networks/[networkId]/ssids/[number]/l3FirewallRules
			processNetworkSSIDFirewallRulesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 26: {
			// GET /networks/[networkId]/groupPolicies
			processNetworkGroupPolicyQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 27: {
			// GET /organizations/[organizationId]/networks
			processNetworkQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 28: {
			// GET /networks/[networkId]
			processNetworkQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 29: {
			// PUT /networks/[networkId]
			processNetworkQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 30: {
			// POST /organizations/[organizationId]/networks
			processNetworkQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 31: {
			// DELETE /networks/[networkId]
			processNetworkQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 32: {
			// POST /networks/[networkId]/bind
			processNetworkBindToTemplateQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 33: {
			// POST /networks/[networkId]/unbind
			processNetworkBindToTemplateQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 34: {
			// GET /networks/[networkId]/siteToSiteVpn
			processMerakiS2SVPNQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 35: {
			// PUT /networks/[networkId]/siteToSiteVpn
			processMerakiS2SVPNQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 36: {
			// GET /networks/[networkId]/traffic
			processNetworkTrafficQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 37: {
			// GET /networks/[networkId]/accessPolicies
			processNetworkAccessPoliciesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 38: {
			// GET /networks/[networkId]/airMarshal
			processNetworkAirMarshalQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 39: {
			// GET /networks/[networkId]/bluetoothSettings
			processNetworkBtoothSettingsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 40: {
			// GET /networks/[networkId]/bluetoothSettings
			processNetworkBtoothSettingsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 41: {
			// GET /organizations
			processOrgQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 42: {
			// GET /organizations/[organizationId]
			processOrgQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 43: {
			// PUT /organizations/[organizationId]
			processOrgQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 44: {
			// POST /organizations
			processOrgQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 45: {
			// POST /organizations/[organizationId]/clone
			processOrgQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 46: {
			// POST /organizations/[organizationId]/claim
			processSerialKeyOrderClaimQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 47: {
			// GET /organizations/[organizationId]/licenseState
			processLicenseQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 48: {
			// GET /organizations/[organizationId]/inventory
			processOrgInventoryQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 49: {
			// GET /organizations/[organizationId]/snmp
			processOrgSNMPQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 50: {
			// PUT /organizations/[organizationId]/snmp
			processOrgSNMPQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 51: {
			// GET /organizations/[organizationId]/thirdPartyVPNPeers
			processOrgVPNQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 52: {
			// PUT /organizations/[organizationId]/thirdPartyVPNPeers
			processOrgVPNQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 53: {
			// GET /networks/[networkId]/phoneAssignments
			processNetworkPhonesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 54: {
			// GET /networks/[networkId]/phoneAssignments/[serial]
			processNetworkPhonesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 55: {
			// PUT /networks/[networkId]/phoneAssignments/[serial]
			processNetworkPhonesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 56: {
			// DELETE /networks/[networkId]/phoneAssignments/[serial]
			processNetworkPhonesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 57: {
			// GET /networks/[networkId]/phoneCallgroups
			processNetworkPhoneCallgroupsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 58: {
			// GET /networks/[networkId]/phoneCallgroups/[id]
			processNetworkPhoneCallgroupsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 59: {
			// POST /networks/[networkId]/phoneCallgroups/
			processNetworkPhoneCallgroupsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 60: {
			// PUT /networks/[networkId]/phoneCallgroups/[id]
			processNetworkPhoneCallgroupsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 61: {
			// DELETE /networks/[networkId]/phoneCallgroups/[id]
			processNetworkPhoneCallgroupsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 62: {
			// GET /networks/[networkId]/phoneContacts
			processNetworkPhoneContactsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 63: {
			// POST /networks/[networkId]/phoneContacts
			processNetworkPhoneContactsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 64: {
			// PUT /networks/[networkId]/phoneContacts/[contactId]
			processNetworkPhoneContactsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 65: {
			// DELETE /networks/[networkId]/phoneContacts/[contactId]
			processNetworkPhoneContactsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 66: {
			// GET /networks/[networkId]/phoneNumbers
			processNetworkPhoneNumbersQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 67: {
			// GET /networks/[networkId]/phoneNumbers/available
			processNetworkAvailablePhoneNumbersQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 68: {
			// GET /organizations/[organizationId]/samlRoles
			processSamlRolesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 69: {
			// GET /organizations/[organizationId]/samlRoles/[id]
			processSamlRolesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 70: {
			// PUT /organizations/[organizationId]/samlRoles/[id]
			processSamlRolesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 71: {
			// POST /organizations/[organizationId]/samlRoles
			processSamlRolesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 72: {
			// DELETE /organizations/[organizationId]/samlRoles/[id]
			processSamlRolesQuery(jDoc, queueEventRequests.at(eventIndex));
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
			// TODO: I do not do the batchToken yet
			processNetworkSMDevicesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 88: {
			// PUT /networks/[networkId]/sm/devices/tags
			processNetworkSMTagsUpdateQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 89: {
			// PUT /networks/[networkId]/sm/device/fields
			processNetworkSMFieldsUpdateQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 90: {
			// PUT /networks/[networkId]/sm/devices/lock
			processNetworkSMDeviceLockQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 91: {
			// PUT /networks/[networkId]/sm/device/wipe
			processNetworkSMDeviceWipeQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 92: {
			// PUT /networks/[networkId]/sm/devices/checkin
			processNetworkSMDeviceCheckinQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 93: {
			// PUT /networks/[networkId]/sm/devices/move
			processNetworkSMDeviceMoveQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 94: {
			// GET /networks/[networkId]/sm/profiles
			processNetworkSMProfilesQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 95: {
			// GET /networks/[networkId]/ssids
			processNetworkSSIDsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 96: {
			// GET /networks/[networkId]/ssids/[id]
			processNetworkSSIDsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 97: {
			// PUT /networks/[networkId]/ssids/[id]
			processNetworkSSIDsQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 98: {
			// GET /devices/[serial]/switchPorts
			processSwitchPortQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 99: {
			// GET /devices/[serial]/switchPorts/[id]
			processSwitchPortQuery(jDoc, queueEventRequests.at(eventIndex));
			break;
		}

		case 100: {
			// PUT /devices/[serial]/switchPorts/[id]
			processSwitchPortQuery(jDoc, queueEventRequests.at(eventIndex));
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

bool APIHelper::processOrgQuery(QJsonDocument doc, eventRequest e) {
	// call this after querying list of organizations
	// if orgIndex is not -1, it means that a particular org was queried
	qDebug() << "\nAPIHelper::processOrgQuery(...), orgIndex: " << e.orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgQuery(...)";
		return false;
	}

	// get organization data
	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 41 returns all orgs for API key, GET
	// urlList 42 returns a single organization, GET
	// urlList 43 returns a single organization, PUT
	// urlList 44 returns a single organization, POST
	// urlList 45 returns a single organization, POST
	int i = e.orgIndex;
	int count = e.orgIndex+1;	// makes it so that the for-loop will do at least 1 iteration

	if (e.urlListIndex == 41) {
		if (e.orgIndex == -1) {
			i = 0;		// update all the orgs
			count = jArray.size();
			parent->orgList.resize(jArray.size());
		}

	} else if (e.urlListIndex == 42 || e.urlListIndex == 43) {
		// only get data for one organization, do nothing

	} else if (e.urlListIndex == 44 || e.urlListIndex == 45) {
		// a new org needs to be added
		i = parent->orgList.size();
		count = i+1;
		parent->orgList.resize(count);
	}


	for (i; i < count; i++) {
		qDebug() << i;
		QJsonObject jObj = jArray.at(i).toObject();
		parent->orgList[i] = new MOrganization();
		parent->orgList[i]->setOrgID(jObj["id"].toVariant().toString());
//		parent->orgList[i]->setOrgID(jObj["id"].toDouble());
		parent->orgList[i]->setOrgName(jObj["name"].toString());
		parent->orgList[i]->setOrgSamlUrl(jObj["samlConsumerUrl"].toString());

		QJsonArray jSaml = jObj["samlConsumerUrls"].toArray();
		parent->orgList[i]->setOrgSamlUrlsNum(jSaml.size());
		for (int j = 0; j < jSaml.size(); j++) {
			QString tmpUrl = jSaml.at(j).toString();
			parent->orgList[i]->setOrgSamlUrlEntry(tmpUrl, j);
		}
	}


	parent->updateOrgUI(e.orgIndex);

	return true;	// everything OK

}

bool APIHelper::processNetworkQuery(QJsonDocument doc, eventRequest e) {
	// if a netIndex is returned, the query was run for a single network in the organization
	qDebug() << "\nAPIHelper::processNetworkQuery(), netIndex: " << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 27 returns all networks in the org, GET
	// urlList 28 returns a single network in the org, GET
	// urlList 29 returns a single network in the org, PUT
	// urlList 30 returns a single network in the org, POST
	// urlList 31 returns nothing, DELETE
	int i = 0;
	int count = jArray.size();

	if (e.urlListIndex == 27) {
		parent->orgList[e.orgIndex]->setNetworksNum(count);

	} else if (e.urlListIndex == 28 || e.urlListIndex == 29) {
		i = e.netIndex;
		count = i+1;

	} else if (e.urlListIndex == 30) {
		// a new network needs to be created, increment the vector size and
		// put the new network at the end of the vector
		i = parent->orgList[e.orgIndex]->getNetworksNum();
		count = i+1;
		parent->orgList[e.orgIndex]->setNetworksNum(count);

	} else if (e.urlListIndex == 31) {
		return parent->orgList[e.orgIndex]->removeNetwork(e.netIndex);
	}


	for (i; i < count; i++) {
		// this loop assumes that the whole list of networks in an org was queried, before a single network
		QJsonObject jObj = jArray.at(i).toObject();
		networkVars tmpNetVar = parent->orgList[e.orgIndex]->getNetwork(i);

		tmpNetVar.netID = jObj["id"].toString();
		tmpNetVar.netName = jObj["name"].toString();
		tmpNetVar.orgID = jObj["organizationId"].toVariant().toString();
		tmpNetVar.netTimezone = jObj["timeZone"].toString();
		tmpNetVar.netType = jObj["type"].toString();
		tmpNetVar.netTags = (jObj["tags"].toString());

		parent->orgList[e.orgIndex]->setNetwork(tmpNetVar, i);

	}

	parent->updateOrgUI(-1);
	parent->updateOrgUI(e.orgIndex);

	return true;	// everything OK

}

bool APIHelper::processNetworkBindToTemplateQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkBindToTemplateQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex: " << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkBindToTemplateQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 32 returns nothing, POST
	// urlList 33 returns nothing, POST


	return true;	// everything OK

}

bool APIHelper::processSerialKeyOrderClaimQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processSerialKeyOrderClaimQuery(...), orgIndex: " << e.orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgAdminsQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 46 returns nothing, POST


	return true;	// everything OK

}

bool APIHelper::processLicenseQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processLicenseQuery(...), orgIndex: " << e.orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processLicenseQuery(...)";
		return false;
	}

	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size() << "\n";

	// urlList 47 returns JSON object with license info for org, GET
	// save info for licensing status of organization
	parent->orgList[e.orgIndex]->setLicenseExpDate(jObj["expirationDate"].toString());
	parent->orgList[e.orgIndex]->setLicenseStatus(jObj["status"].toString());

	// get list of licensedDeviceCounts
	QJsonObject jObjCounts = jObj["licensedDeviceCounts"].toObject();	// makes it easier to work with it
	parent->orgList[e.orgIndex]->setLicenseDeviceNum(jObjCounts.keys().size());

	for (int i = 0; i < jObjCounts.size(); i++) {
		licensesPerDevice tmpVar;

		tmpVar.deviceType = jObjCounts.keys().at(i);
		tmpVar.count = jObjCounts[jObjCounts.keys().at(i)].toInt();
		parent->orgList[e.orgIndex]->setLicensePerDevice(tmpVar, i);
	}

//	parent->updateOrgUI(orgIndex);
	parent->displayLicenseInfo(e.orgIndex);

	return true;		// everything ok

}

bool APIHelper::processOrgAdminsQuery(QJsonDocument doc, eventRequest e) {
	// process list of administrators in organization
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

	if (e.urlListIndex == 0) {
		parent->orgList[e.orgIndex]->setAdminsNum(count);

	} else if (e.urlListIndex == 1) {
		// a new administrator needs to be created, increment the vector size
		// and put the new admin at the end of the network
		i = parent->orgList[e.orgIndex]->getAdminListSize();
		count = i+1;
		parent->orgList[e.orgIndex]->setAdminsNum(count);

	} else if (e.urlListIndex == 2) {
		i = adminIndex;
		count = i+1;

	} else if (e.urlListIndex == 3) {
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

bool APIHelper::processSamlRolesQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processSamlRolesQuery(...), orgIndex: " << e.orgIndex
			 << "\tid: " << e.id;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processSamlRolesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 68 returns JSON array with list of all SAML roles for the org, GET
	// urlList 69 returns JSON object with SAML role, GET
	// urlList 70 returns JSON object with SAML role, PUT
	// urlList 71 returns JSON object with SAML role created, POST
	// urlList 72 nothing after deleting SAML role, DELETE
	int samlIndex = parent->orgList[e.orgIndex]->getIndexOfSamlRole(e.id);
	int i = 0;
	int count = jArray.size();

	if (e.urlListIndex == 68) {
		parent->orgList[e.orgIndex]->setOrgSamlRolesNum(jArray.size());
	} else if (e.urlListIndex == 69 || e.urlListIndex == 70) {
		i = samlIndex;
		count = i+1;
	} else if (e.urlListIndex == 71) {
		// create a new element in the vector
		i = parent->orgList[e.orgIndex]->getOrgSamlRolesNum();
		count = i+1;
		parent->orgList[e.orgIndex]->setOrgSamlRolesNum(count);
	} else if (e.urlListIndex == 72) {
		return parent->orgList[e.orgIndex]->removeOrgSamlAdminRole(samlIndex);
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


		parent->orgList[e.orgIndex]->setOrgSamlRole(tmpSamlRole, i);

	}


	return true;	// everything went well

}

bool APIHelper::processOrgInventoryQuery(QJsonDocument doc, eventRequest e) {
	// process inventory for an organization
	// GET /organizations/[organizationId]/inventory
	qDebug() << "\nAPIHelper::processOrgInventoryQuery(...), orgIndex: " << e.orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgInventoryQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 48 returns array with list of devices in org inventory, GET
	parent->orgList[e.orgIndex]->setOrgInventorySize(jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		deviceInInventory tmpDevice;
		QJsonObject jObj = jArray.at(i).toObject();

		tmpDevice.claimedAt = jObj["claimedAt"].toString();
		tmpDevice.mac = jObj["mac"].toString();
		tmpDevice.model = jObj["model"].toString();
		tmpDevice.netID = jObj["networkId"].toString();
		tmpDevice.publicIP = jObj["publicIp"].toString();
		tmpDevice.serial = jObj["serial"].toString();

		parent->orgList[e.orgIndex]->setOrgInventoryDevice(tmpDevice, i);

	}

	parent->displayInventory(e.orgIndex);

	return true;	// everything went well

}

bool APIHelper::processNetworkDevicesQuery(QJsonDocument doc, eventRequest e) {
	// if serial is present, it means that the query was run for a single device
	// if serial is empty, get all devices in the network
	// this should probably be run with a serial number only after it is run without it, to avoid
	// QVector out of bounds accesses
	// TODO: beaconIdParams are also returned
	qDebug() << "\nAPIHelper::processNetworkDevicesQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex << "\tserial: " << e.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkDevicesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// urlList 11 returns all the devices in a network, GET
	// urlList 12 returns a single device in the network, GET
	// urlList 14 returns a single device in the network, PUT
	int devIndex = parent->orgList[e.orgIndex]->getIndexOfInventoryDevice(e.deviceSerial);
	int i = 0;
	int count = jArray.size();

	if (e.urlListIndex == 11) {
		// process all serials in the network
		parent->orgList[e.orgIndex]->setNetworkDevicesNum(e.netIndex, jArray.size());
	} else if (e.urlListIndex == 12 || e.urlListIndex == 14) {
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

		parent->orgList[e.orgIndex]->setNetworkDevice(e.netIndex, tmpNetDev, i);

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

bool APIHelper::processOrgSNMPQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processOrgSNMPQuery(...), orgIndex: " << e.orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgSNMPQuery(...)";
		return false;
	}


	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size();

	// urlList 49 returns JSON object of SNMP configuration for the organisation, GET
	// urlList 50 returns JSON object of SNMP configuration for the organization, PUT

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

	parent->orgList.at(e.orgIndex)->setOrgSNMPSettings(tmpSNMP);
	parent->displayOrgSNMP(e.orgIndex);

	return true;	// everything went ok

}

bool APIHelper::processOrgVPNQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processOrgVPNQuery(...), orgIndex: " << e.orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgVPNQuery(...)";
		return false;
	}


	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 51 returns JSON array of 3rd party site-to-site VPN peers in org, GET
	// urlList 52 returns JSON array of 3rd party site-to-site VPN peers in org, PUT
	parent->orgList[e.orgIndex]->setOrgVPNPeerNum(jArray.size());


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

		parent->orgList.at(e.orgIndex)->setOrgVPNPeer(tmpVPN, i);

	}

	parent->displayOrgVPN(e.orgIndex);

	return true;	// everything went ok

}

bool APIHelper::processOrgVPNFirewallRulesQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processOrgVPNFirewallRulesQuery(...), orgIndex: " << e.orgIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgVPNFirewallRulesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 22 returns a list of all firewall rules for org site-to-site VPN, GET
	// urlList 23 returns a list of all firewall rules for org site-to-site VPN, PUT
	// no need for any special processing here, since an array will be returned anyway

	parent->orgList[e.orgIndex]->setOrgVPNFirewallRulesNum(jArray.size());


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

		parent->orgList.at(e.orgIndex)->setOrgVPNFirewallRule(tmpRule, i);

	}

	return true;	// everything went ok

}

bool APIHelper::processSwitchPortQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processSwitchPortQuery(...), orgIndex: " << e.orgIndex
			 << "\tdevSerial" << e.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processOrgVPNQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// get the index of the device in the organization inventory
	deviceInInventory tmpDevice = parent->orgList.at(e.orgIndex)->getOrgDeviceFromSerial(e.deviceSerial);
	int devIndex = parent->orgList.at(e.orgIndex)->getIndexOfInventoryDevice(tmpDevice.serial);

	if (devIndex == -1) {
		qDebug() << "Could not find " << e.deviceSerial << " in orgIndex " << e.orgIndex;
		return false;
	}

	qDebug() << e.deviceSerial << " is at index: " << devIndex;


	// urlList 98 returns a JSON array with config of all switch ports on a switch, GET
	// urlList 99 returns a JSON object with config of the switch port, GET
	// urlList 100 returns a JSON object with config of the switch port to be updated, PUT
	int i = 0;
	int count = jArray.size();
	if (e.urlListIndex == 98) {
		parent->orgList.at(e.orgIndex)->setSwitchPortNum(devIndex, jArray.size());
	} else if (e.urlListIndex == 99 || e.urlListIndex == 100) {
		i = e.portIndex;
		count = i+1;
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

		parent->orgList.at(e.orgIndex)->setSwitchPort(devIndex, tmpPort, i);

	}


	parent->displayMSPort(devIndex, e.orgIndex);	// display things in the table

	return true;	// everything went ok

}

bool APIHelper::processl3FirewallQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processl3FirewallQuery(...), orgIndex: "<< e.orgIndex
			 << "\tdeviceSerial" << e.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processl3FirewallQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 20 returns a list of all the MX firewall rules, GET
	// urlList 21 returns a list of all the MX firewall rules, PUT
	// no need for any special processing here, since an array will be returned anyway


	// get the index of the device in the organization inventory
	deviceInInventory tmpDevice = parent->orgList.at(e.orgIndex)->getOrgDeviceFromSerial(e.deviceSerial);
	int devIndex = parent->orgList.at(e.orgIndex)->getIndexOfInventoryDevice(tmpDevice.serial);
	parent->orgList.at(e.orgIndex)->setMXL3RulesNum(devIndex, jArray.size());


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

		parent->orgList.at(e.orgIndex)->setMXL3Rule(devIndex, tmpRule, i);

	}


	parent->displayMXL3Rules(devIndex, e.orgIndex);	// display things in the table

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

bool APIHelper::processNetworkSMDevicesQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSMDevicesQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMDevicesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.object()["devices"].toArray();	// the reply contains an array called devices[]
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList.at(e.orgIndex)->setNetworkSMDevicesNum(e.netIndex, jArray.size());

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

		parent->orgList.at(e.orgIndex)->setNetworkSMDevice(e.netIndex, tmpSMDevice, i);

	}

	parent->displaySMDevices(e.orgIndex);

	return true;	// everything went ok

}



bool APIHelper::processNetworkSMProfilesQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSMProfilesQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMProfilesQuery(...)";
		return false;
	}

	// for some reason, the response will contain an array called "profiles"
	QJsonArray jArray = doc.object()["profiles"].toArray();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[e.orgIndex]->setNetworkSMProfilesNum(e.netIndex, jArray.size());


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


		// use the smWifiPayload struct here
		QJsonArray jWifis = jObj["wifis"].toArray();
		tmpSMProfile.wifis.resize(jWifis.size());
		for (int j = 0; j < jWifis.size(); j++) {
			QJsonObject jWifi = jWifis.at(j).toObject();
			smWifiPayload tmpWifi;

			tmpWifi.id = jWifi["id"].toString();
			tmpWifi.configuration = jWifi["configuration"].toString();
			tmpWifi.isManualConfig = jWifi["is_manual_config"].toBool();
			tmpWifi.hiddenNetwork = jWifi["HIDDEN_NETWORK"].toBool();
			tmpWifi.encryptionType = jWifi["EncryptionType"].toString();
			tmpWifi.password = jWifi["Password"].toString();
			tmpWifi.ssidStr = jWifi["SSID_STR"].toString();
			tmpWifi.autoJoin = jWifi["AutoJoin"].toBool();
			tmpWifi.payloadUUID = jWifi["PayloadUUID"].toString();
			tmpWifi.proxyType = jWifi["ProxyType"].toString();
			tmpWifi.pccMobileConfigId = jWifi["pcc_mobile_config_id"].toString();
			tmpWifi.qosEnable = jWifi["QoSEnable"].toString();
			tmpWifi.useUsernameAsCn = jWifi["use_username_as_cn"].toBool();

			tmpSMProfile.wifis[j] = tmpWifi;

		}

		QJsonArray jPayloads = jObj["payload_types"].toArray();
		tmpSMProfile.payloadTypes.resize(jTags.size());
		for (int j = 0; j < jPayloads.size(); j++) {
			tmpSMProfile.payloadTypes[j] = jPayloads.at(j).toString();
		}

		parent->orgList[e.orgIndex]->setNetworkSMProfile(e.netIndex, tmpSMProfile, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkSMTagsUpdateQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSMTagsUpdateQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMTagsUpdateQuery(...)";
		return false;
	}

	// the response will contain an array called "success" or "errors"
	QJsonArray jArray = doc.object()["success"].toArray();
	qDebug() << jArray << "\t" << jArray.size();

	// all the devices matching the scope, serials or wifiMacs are returned
	for (int i = 0; i < jArray.size(); i++) {
		// get device index to modify tags
		int smDeviceIndex = parent->orgList[e.orgIndex]->getIndexOfNetworkSMDevice(e.netIndex, e.id);
		smDevice tmpSMDevice = parent->orgList[e.orgIndex]->getNetworkSMDevice(e.netIndex, smDeviceIndex);
		QJsonObject jObj = jArray.at(i).toObject();

		QJsonArray jTags = jObj["tags"].toArray();
		tmpSMDevice.tags.resize(jTags.size());
		for (int j = 0; j < jTags.size(); j++) {
			tmpSMDevice.tags[j] = jTags.at(j).toString();
		}


		parent->orgList[e.orgIndex]->setNetworkSMDevice(e.netIndex, tmpSMDevice, smDeviceIndex);

	}



	return true;	// everything went well

}

bool APIHelper::processNetworkSMFieldsUpdateQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSMFieldsUpdateQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMFieldsUpdateQuery(...)";
		return false;
	}

	// the response will contain an array called "success" or "errors"
	QJsonArray jArray = doc.object()["success"].toArray();
	qDebug() << jArray << "\t" << jArray.size();

	// all the devices matching the scope, serial or wifiMac are returned
	for (int i = 0; i < jArray.size(); i++) {
		int smDeviceIndex = parent->orgList[e.orgIndex]->getIndexOfNetworkSMDevice(e.netIndex, e.id);
		smDevice tmpSMDevice = parent->orgList[e.orgIndex]->getNetworkSMDevice(e.netIndex, smDeviceIndex);
		QJsonObject jObj = jArray.at(i).toObject();

		tmpSMDevice.name = jObj["name"].toString();
		tmpSMDevice.notes = jObj["notes"].toString();

		parent->orgList[e.orgIndex]->setNetworkSMDevice(e.netIndex, tmpSMDevice, smDeviceIndex);
	}

	return true;	// everything went well

}

bool APIHelper::processNetworkSMDeviceLockQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSMDeviceLockQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMDeviceLockQuery(...)";
		return false;
	}

	// the response will contain an object "success" indicating whether the lock command returned
	// true of false
	// TODO: figure out some sort of integration or some message to give to the user

	return true;	// everything went well

}

bool APIHelper::processNetworkSMDeviceWipeQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSMDeviceWipeQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMDeviceWipeQuery(...)";
		return false;
	}

	// the response will contain an object "success" indicating whether the lock command returned
	// true of false
	// TODO: figure out some sort of integration or some message to give to the user

	return true;	// everything went well

}

bool APIHelper::processNetworkSMDeviceCheckinQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSMDeviceCheckinQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMDeviceCheckinQuery(...)";
		return false;
	}

	// the response will contain an object "success" indicating whether the lock command returned
	// true of false
	// TODO: figure out some sort of integration or some message to give to the user

	return true;	// everything went well

}

bool APIHelper::processNetworkSMDeviceMoveQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSMDeviceMoveQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSMDeviceMoveQuery(...)";
		return false;
	}

	// the response will contain an object "success" indicating whether the lock command returned
	// true of false
	// TODO: figure out some sort of integration or some message to give to the user

	return true;	// everything went well
}

bool APIHelper::processNetworkGroupPolicyQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processGroupPolicyQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processGroupPolicyQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();	// the reply contains an array containing the group policies
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList.at(e.orgIndex)->setNetworkGroupPolicyNum(e.netIndex, jArray.size());


	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		networkGroupPolicy tmpGPolicy;

		tmpGPolicy.name = jObj["name"].toString();
		tmpGPolicy.groupPolicyId = jObj["groupPolicyId"].toInt();

		parent->orgList.at(e.orgIndex)->setNetworkGroupPolicy(e.netIndex, tmpGPolicy, i);

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
	tmpNetDev.devCDP.resize(MAX_SWITCH_PORTS);
	tmpNetDev.devLLDP.resize(MAX_SWITCH_PORTS);

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

bool APIHelper::processNetworkSSIDsQuery(QJsonDocument doc, eventRequest e) {
	// if ssidIndex is -1, a query for all the SSIDs in the network was made
	qDebug() << "\nAPIHelper::processNetworkSSIDsQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex << "\tssidIndex" << e.ssidIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSSIDsQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();


	// urlList 95 returns a JSON array with config of all the SSIDs in the network, GET
	// urlList 96 returns a JSON object with the SSID configuration, GET
	// urlList 97 returns a JSON object with the SSID configuration, PUT
	int i = 0;
	int count = jArray.size();
	if (e.urlListIndex == 95) {
//		count = MAX_SSID_NUM;		// there is a fixed number of SSIDs
	} else if (e.urlListIndex == 96 || e.urlListIndex == 97) {
		i = e.ssidIndex;
		count = e.ssidIndex+1;
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

		tmpSSID.walledGardenEnabled = jObj["walledGardenEnabled"].toBool();
		tmpSSID.walledGardenRanges = jObj["walledGardenRanges"].toString();

		parent->orgList[e.orgIndex]->setNetworkSSID(e.netIndex, tmpSSID, i);

	}

	return true;	// everything went well

}

bool APIHelper::processMerakiS2SVPNQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processMerakiS2SVPNQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processMerakiS2SVPNQuery(...)";
		return false;
	}

	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size();

	// urlList 34 returns Meraki site-to-site autoVPN settings for the network, GET
	// urlList 35 returns Meraki site-to-site autoVPN settings for the network, PUT
	// no need for any special processing here, since an single JSON object will be returned anyway
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


	parent->orgList[e.orgIndex]->setNetworkS2SVPN(e.netIndex, tmpVPN);

	return true;	// everything went well

}

bool APIHelper::processNetworkTrafficQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkTrafficQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkTrafficQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[e.orgIndex]->setNetworkTrafficFlowsNum(e.netIndex, jArray.size());


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

		parent->orgList[e.orgIndex]->setNetworkTrafficFlow(e.netIndex, tmpTraffic, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkAccessPoliciesQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkAccessPoliciesQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkAccessPoliciesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[e.orgIndex]->setNetworkAccessPoliciesNum(e.netIndex, jArray.size());


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

		parent->orgList[e.orgIndex]->setNetworkAccessPolicy(e.netIndex, tmpPolicy, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkAirMarshalQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkAirMarshalQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkAirMarshalQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	parent->orgList[e.orgIndex]->setNetworkAirMarshalEntriesNum(e.netIndex, jArray.size());


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

		parent->orgList[e.orgIndex]->setNetworkAirMarshalEntry(e.netIndex, tmpAirMarshal, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkBtoothSettingsQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkBtoothSettingsQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkBtoothSettingsQuery(...)";
		return false;
	}

	QJsonObject jObj = doc.object();
	qDebug() << jObj << "\t" << jObj.size();

	// urlList 39 returns bluetooth settings for the network, GET
	// urlList 40 returns bluetooth settings for the network, PUT
	// no need for any special processing here, since an single JSON object will be returned anyway
	netBtoothSettings tmpBtooth;
	tmpBtooth.id = jObj["id"].toString();	// this may not even be returned
	tmpBtooth.scanningEnabled = jObj["scanningEnabled"].toBool();
	tmpBtooth.advertisingEnabled = jObj["advertisingEnabled"].toBool();
	tmpBtooth.uuid = jObj["uuid"].toString();
	tmpBtooth.majorMinorAssignmentMode = jObj["majorMinorAssignmentMode"].toString();
	tmpBtooth.major = jObj["major"].toInt();
	tmpBtooth.minor = jObj["minor"].toInt();

	parent->orgList[e.orgIndex]->setNetworkBtoothSettings(e.netIndex, tmpBtooth);

	return true;	// everything went well

}

bool APIHelper::processNetworkPhonesQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkPhonesQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex << "\tserial: " << e.deviceSerial;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkPhonesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 53 returns JSON array of phones in a network and contact assignment, GET
	// urlList 54 returns JSON object of phone and its contact assignment, GET
	// urlList 55 returns JSON object of phone and its contact assignment, PUT
	// urlList 56 returns nothing, DELETE
	int phoneIndex = parent->orgList[e.orgIndex]->getIndexOfNetworkPhone(e.netIndex, e.deviceSerial);
	int i = 0;
	int count = jArray.size();

	if (e.urlListIndex == 53) {
		parent->orgList[e.orgIndex]->setNetworkPhoneNum(e.netIndex, jArray.size());

	} else if (e.urlListIndex == 54 || e.urlListIndex == 55) {
		i = phoneIndex;
		count = i+1;

	} else if (e.urlListIndex == 56) {
		return parent->orgList[e.orgIndex]->removeNetworkPhoneAssignment(e.netIndex, phoneIndex);
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

		parent->orgList[e.orgIndex]->setNetworkPhone(e.netIndex, tmpPhone, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkPhoneContactsQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkPhoneContactsQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkPhoneContactsQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 62 returns JSON array of phone contacts in network, GET
	// urlList 63 returns JSON object of phone contact just added, POST
	// urlList 64 returns JSON object of phone contact updated, PUT
	// urlList 65 returns nothing, DELETE
	int phoneContactIndex = parent->orgList[e.orgIndex]->getIndexOfPhoneContact(e.netIndex, e.id);
	int i = 0;
	int count = jArray.size();

	if (e.urlListIndex == 62) {
		parent->orgList[e.orgIndex]->setNetworkPhoneContactNum(e.netIndex, jArray.size());

	} else if (e.urlListIndex == 63) {
		// add element to the vector
		i = parent->orgList[e.orgIndex]->getNetworkPhoneContactNum(e.netIndex);
		count = i+1;
		parent->orgList[e.orgIndex]->setNetworkPhoneContactNum(e.netIndex, count);
	} else if (e.urlListIndex == 64) {
		i = phoneContactIndex;
		count = i+1;
	} else if (e.urlListIndex == 65) {
		return parent->orgList[e.orgIndex]->removeNetworkPhoneContact(e.netIndex, phoneContactIndex);
	}



	for (i = 0; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netPhoneContact tmpPhoneContact;

		tmpPhoneContact.id = jObj["id"].toInt();
		tmpPhoneContact.name = jObj["name"].toString();
		tmpPhoneContact.type = jObj["type"].toString();

		parent->orgList[e.orgIndex]->setNetworkPhoneContact(e.netIndex, tmpPhoneContact, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkPhoneCallgroupsQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkPhoneCallgroupsQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex << "\tid: " << e.id;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkPhoneCallgroupsQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 57 returns JSON array of the list of call groups in the network, GET
	// urlList 58 returns JSON object of call group in network, GET
	// urlList 59 returns JSON object of call group created in network, POST
	// urlList 60 returns JSON object of call group updated in network, PUT
	// urlList 61 returns nothing, DELETE
	int callGroupIndex = parent->orgList[e.orgIndex]->getIndexOfPhoneCallgroupId(e.netIndex, e.id);
	int i = 0;
	int count = jArray.size();

	if (e.urlListIndex == 57) {
		parent->orgList[e.orgIndex]->setNetworkPhoneCallgroupsNum(e.netIndex, jArray.size());

	} else if (e.urlListIndex == 58 || e.urlListIndex == 60) {
		// only do single id
		i = callGroupIndex;
		count = i+1;
	} else if (e.urlListIndex == 59) {
		// create a new element in the vector
		i = parent->orgList[e.orgIndex]->getNetworkPhoneCallgroupsNum(e.netIndex);
		count = i+1;
		parent->orgList[e.orgIndex]->setNetworkPhoneCallgroupsNum(e.netIndex, count);
	} else if (e.urlListIndex == 61) {
		return parent->orgList[e.orgIndex]->removeNetworkPhoneCallGroup(e.netIndex, callGroupIndex);
	}


	for (i; i < count; i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		netPhoneCallgroup tmpPhoneCGroup;

		tmpPhoneCGroup.id = jObj["id"].toString();
		tmpPhoneCGroup.name = jObj["name"].toString();
		tmpPhoneCGroup.publicNumber = jObj["publicNumber"].toString();
		tmpPhoneCGroup.ext = jObj["ext"].toString();

		parent->orgList[e.orgIndex]->setNetworkPhoneCallgroupEntry(e.netIndex, tmpPhoneCGroup, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkPhoneNumbersQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkPhoneNumbersQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkPhoneNumbersQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 66 returns JSON array of the list of all phone numbers in the network, GET
	// no special processing needed here
	parent->orgList[e.orgIndex]->setNetworkPublicNumbersNum(e.netIndex, jArray.size());

	for (int i = 0; i < jArray.size(); i++) {
		QString tmpPhoneNum;
		QJsonObject jObj = jArray.at(i).toObject();

		tmpPhoneNum = jObj["publicNumber"].toString();

		parent->orgList[e.orgIndex]->setNetworkPublicNumberEntry(e.netIndex, tmpPhoneNum, i);

	}

	return true;	// everything went well

}

bool APIHelper::processNetworkAvailablePhoneNumbersQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkAvailablePhoneNumbersQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex" << e.netIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkAvailablePhoneNumbersQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 67 according to the documentation, returns a single JSON object with the available number, GET
	// I will treat it as if an array was returned, since it does not make sense
	// no special processing needed here
	parent->orgList[e.orgIndex]->setNetworkAvailablePublicNumbersNum(e.netIndex, jArray.size());

	for (int i = 0; i < jArray.size(); i++) {
		QString tmpPhoneNum;
		QJsonObject jObj = jArray.at(i).toObject();

		tmpPhoneNum = jObj["publicNumber"].toString();

		parent->orgList[e.orgIndex]->setNetworkAvailablePublicNumberEntry(e.netIndex, tmpPhoneNum, i);

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

bool APIHelper::processNetworkSSIDFirewallRulesQuery(QJsonDocument doc, eventRequest e) {
	qDebug() << "\nAPIHelper::processNetworkSSIDFirewallRulesQuery(...), orgIndex: " << e.orgIndex
			 << "\tnetIndex: " << e.netIndex << "\tssidIndex: " << e.ssidIndex;

	if (doc.isNull()) {
		qDebug() << "JSON IS NOT VALID, APIHelper::processNetworkSSIDFirewallRulesQuery(...)";
		return false;
	}

	QJsonArray jArray = doc.array();
	qDebug() << jArray << "\t" << jArray.size();

	// urlList 24 returns a list of all firewall rules for a network SSID, GET
	// urlList 25 returns a list of all firewall rules for a network SSID, PUT
	// no need for any special processing here, since an array will be returned anyway

	parent->orgList[e.orgIndex]->setNetworkSSIDFwRulesNum(e.netIndex, e.ssidIndex, jArray.size());

	for (int i = 0; i < jArray.size(); i++) {
		QJsonObject jObj = jArray.at(i).toObject();
		l3Firewall tmpRule;

		tmpRule.comment = jObj["comment"].toString();
		tmpRule.policy = jObj["policy"].toString();
		tmpRule.protocol = jObj["protocol"].toString();
		tmpRule.destPort = jObj["destPort"].toString();
		tmpRule.destCidr = jObj["destCidr"].toString();

		parent->orgList.at(e.orgIndex)->setNetworkSSIDFwRule(e.netIndex, e.ssidIndex, tmpRule, i);

	}

	return true;	// everything went ok

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
