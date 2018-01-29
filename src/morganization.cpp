#include "morganization.h"
#include "morganization.h"


MOrganization::MOrganization() {
	samlURL = QString("");
}

MOrganization::MOrganization(QString orgName, QString orgId) {
	id = orgId;
	name = orgName;
	samlURL = QString("");
}

MOrganization::~MOrganization() {
}

void MOrganization::setOrgID(QString orgId) {
	id = orgId;
}

void MOrganization::setOrgName(QString orgName) {
	name = orgName;
}

void MOrganization::setSamlURL(QString url) {
	samlURL = url;
}

void MOrganization::setNetworksNum(int n) {
	netList.resize(n);
}

void MOrganization::setNetwork(networkVars v, int index) {
	netList[index] = v;
}

void MOrganization::setLicenseStatus(QString s) {
	licStatus = s;
}

void MOrganization::setLicenseExpDate(QString d) {
	licExpireDate = d;
}

void MOrganization::setLicenseDeviceNum(int n) {
	licenseList.resize(n);
}

void MOrganization::setLicensePerDevice(licensesPerDevice a, int index) {
	licenseList[index] = a;
}

void MOrganization::setAdminsNum(int n) {
	adminList.resize(n);
}

void MOrganization::setAdmin(adminStruct a, int index) {
	adminList[index] = a;
}

void MOrganization::setOrgInventorySize(int n) {
	orgInventory.resize(n);
}

void MOrganization::setOrgInventoryDevice(deviceInInventory a, int index) {
	orgInventory[index] = a;
}

void MOrganization::setSwitchPortNum(int devIndex, int n) {
	orgInventory[devIndex].ports.resize(n);
}

void MOrganization::setSwitchPort(int devIndex, switchPort s, int index) {
	orgInventory[devIndex].ports[index] = s;
}

void MOrganization::setMXL3RulesNum(int devIndex, int n) {
	orgInventory[devIndex].rules.resize(n);
}

void MOrganization::setMXL3Rule(int devIndex, mxL3Firewall s, int index) {
	orgInventory[devIndex].rules[index] = s;
}

void MOrganization::setOrgSNMPSettings(orgSNMP s) {
	snmpSettings = s;
}

void MOrganization::setOrgVPNPeerNum(int n) {
	nonMerakiVPNs.resize(n);
}

void MOrganization::setOrgVPNPeer(nonMerakiVPNPeer p, int index) {
	nonMerakiVPNs[index] = p;
}

void MOrganization::setSMDevicesNum(int netIndex, int n) {
	netList[netIndex].smDevices.resize(n);
}

void MOrganization::setSMDevice(int netIndex, smDevice s, int index) {
	netList[netIndex].smDevices[index] = s;
}

void MOrganization::setNetworkGroupPolicyNum(int netIndex, int n) {
	netList[netIndex].gPolicies.resize(n);
}

void MOrganization::setNetworkGroupPolicy(int netIndex, networkGroupPolicy s, int index) {
	netList[netIndex].gPolicies[index] = s;
}

void MOrganization::setClientsConnectedNum(int devIndex, int n) {
	orgInventory[devIndex].clients.resize(n);
}

void MOrganization::setClientConnected(int devIndex, clientConnected s, int index) {
	orgInventory[devIndex].clients[index] = s;
}

void MOrganization::setNetworkDevicesNum(int netIndex, int n) {
	netList[netIndex].netDevices.resize(n);
}

void MOrganization::setNetworkDevice(int netIndex, deviceInNetwork s, int index) {
	netList[netIndex].netDevices[index] = s;
}





QString MOrganization::getOrgID() {
	return id;
}

QString MOrganization::getOrgName() {
	return name;
}

QString MOrganization::getSamlURL() {
	return samlURL;
}

int MOrganization::getNetworksNum() {
	return netList.size();
}

networkVars MOrganization::getNetwork(int index) {
	return netList.at(index);
}

QString MOrganization::getLicenseStatus() {
	return licStatus;
}

QString MOrganization::getLicenseExpireDate() {
	return licExpireDate;
}

int MOrganization::getLicenseListSize() {
	return licenseList.size();
}

licensesPerDevice MOrganization::getLicensePerDevice(int index) {
	return licenseList.at(index);
}

int MOrganization::getAdminListSize() {
	return adminList.size();
}

adminStruct MOrganization::getAdmin(int index) {
	return adminList.at(index);
}

int MOrganization::getOrgInventorySize() {
	return orgInventory.size();
}

deviceInInventory MOrganization::getOrgInventoryDevice(int index) {
	return orgInventory.at(index);
}

deviceInInventory MOrganization::getOrgDeviceFromSerial(QString serial) {
	// use the serial number to find a particular device in the org inventory
	// return a null deviceInInventory if no device was found
	for (int i = 0; i < orgInventory.size(); i++) {
		if (orgInventory.at(i).serial == serial) {
			return orgInventory.at(i);
		}
	}

	// no device was found
	return deviceInInventory{"-1","-1","-1","-1","-1","-1"};

}

int MOrganization::getSwitchPortNum(int devIndex) {
	return orgInventory.at(devIndex).ports.size();
}

switchPort MOrganization::getSwitchport(int devIndex, int index) {
	return orgInventory.at(devIndex).ports.at(index);
}

int MOrganization::getMXL3RulesNum(int devIndex) {
	return orgInventory.at(devIndex).rules.size();
}

mxL3Firewall MOrganization::getMXL3Rule(int devIndex, int index) {
	return orgInventory.at(devIndex).rules.at(index);
}

orgSNMP MOrganization::getOrgSNMPSettings() {
	return snmpSettings;
}

int MOrganization::getOrgVPNPeerNum() {
	return nonMerakiVPNs.size();
}

nonMerakiVPNPeer MOrganization::getOrgVPNPeer(int index) {
	return nonMerakiVPNs.at(index);
}

int MOrganization::getSMDevicesNum(int netIndex) {
	return netList.at(netIndex).smDevices.size();
}

smDevice MOrganization::getSMDevice(int netIndex, int index) {
	return netList.at(netIndex).smDevices.at(index);
}

int MOrganization::getNetworkGroupPolicyNum(int netIndex) {
	return netList.at(netIndex).gPolicies.size();
}

networkGroupPolicy MOrganization::getNetworkGroupPolicy(int netIndex, int index) {
	return netList.at(netIndex).gPolicies.at(index);
}

int MOrganization::getClientsConnectedNum(int devIndex) {
	return orgInventory.at(devIndex).clients.size();
}

clientConnected MOrganization::getClientConnected(int devIndex, int index) {
	return orgInventory.at(devIndex).clients.at(index);
}

int MOrganization::getNetworkDevicesNum(int netIndex) {
	return netList.at(netIndex).netDevices.size();
}

deviceInNetwork MOrganization::getNetworkDevice(int netIndex, int index) {
	return netList.at(netIndex).netDevices.at(index);
}

int MOrganization::getIndexOfInventoryDevice(QString serial) {
	// given the serial number of a device, returns the index of it in the inventory vector
	// returns -1 if it is unable to find it
	for (int i = 0; i < orgInventory.size(); i++) {
		if (orgInventory.at(i).serial == serial) {
			return i;
		}
	}


	return -1;	// no device was found

}

int MOrganization::getIndexOfClientConnected(QString netID, QString mac) {
	// given the network ID and MAC address of a connected client, returns the index of it
	// in the clients connected vector, this function should be used in conjunction with getIndexOfInventoryDevice(...)
	// returns -1 if it is unable to find it
	for (int i = 0; i < orgInventory.size(); i++) {
		if (netID == orgInventory.at(i).netID) {
			// check if MAC address can be found in this device

			for (int j = 0; j < orgInventory.at(i).clients.size(); j++) {
				clientConnected tmpClient = orgInventory.at(i).clients.at(j);

				if (tmpClient.mac == mac) {	return j; }
			}

		}

	}


	return -1;	// no device was found

}

int MOrganization::getIndexOfNetwork(QString netID) {
	// given the full network ID, i.e. N_584905001604768550, return the index of it in the organization
	// returns -1 if it is unable to find it
	for (int i = 0; i < netList.size(); i++) {
		if (netList.at(i).netID == netID) {
			return i;
		}
	}

	// no network was found
	return -1;
}

int MOrganization::getIndexOfNetworkDevice(QString netID, QString serial) {
	// given network ID and device serial number, return the index of the serial number in netDevices, i.e.
	// list of devices in this network
	// returns -1 if it is unable to find it
	int netIndex = getIndexOfNetwork(netID);
	for (int i = 0; i < netList.at(netIndex).netDevices.size(); i++) {
		if (netList.at(netIndex).netDevices.at(i).serial == serial) {
			return i;
		}
	}

	// no device was found
	return -1;
}





void MOrganization::showVariables() {
	// debug function, just show all the variables in the class
	qDebug() << "Org ID: " << id;
	qDebug() << "Org name: " << name;
	qDebug() << "SAML URL: " << samlURL;

	qDebug() << "SAML URLs: ";
	for (int i = 0; i < samlURLs.size(); i++) {
		qDebug() << "\t" << samlURLs.at(i);
	}


	// show networks
	qDebug() << "Networks in this organization: " << netList.size();
	for (int i = 0; i < netList.size(); i++) {
		qDebug() << "\tName: " << netList.at(i).netName << "\ttype: " << netList.at(i).netType;
		qDebug() << "\tnetID: " << netList.at(i).netID << "\torgID: " << netList.at(i).orgID;
		qDebug() << "\ttimezone: " << netList.at(i).netTimezone;
		qDebug() << "\ttags: " << netList.at(i).netTags;
	}

}


