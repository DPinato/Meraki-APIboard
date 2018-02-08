#include "morganization.h"
#include "morganization.h"


MOrganization::MOrganization() {
}

MOrganization::MOrganization(QString orgName, QString orgId) {
	id = orgId;
	name = orgName;
}

MOrganization::~MOrganization() {
}

void MOrganization::setOrgID(QString orgId) {
	id = orgId;
}

void MOrganization::setOrgName(QString orgName) {
	name = orgName;
}

void MOrganization::setOrgSamlUrl(QString url) {
	samlConsumerUrl = url;
}

void MOrganization::setOrgSamlUrlsNum(int num) {
	samlConsumerUrls.reserve(num);
}

void MOrganization::setOrgSamlUrlEntry(QString s, int index) {
	samlConsumerUrls[index] = s;
}

void MOrganization::setNetworksNum(int n) {
	netList.resize(n);
}

void MOrganization::setNetwork(networkVars v, int index) {
	netList[index] = v;
	netList[index].netSSIDs.resize(15);		// doing it here as this is a default constant value
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

void MOrganization::setOrgSamlRolesNum(int n) {
	samlRolesList.resize(n);
}

void MOrganization::setOrgSamlRole(orgSamlRoles s, int index) {
	samlRolesList[index] = s;
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

void MOrganization::setMXL3Rule(int devIndex, l3Firewall s, int index) {
	orgInventory[devIndex].rules[index] = s;
}

void MOrganization::setNetworkCellularRulesNum(int netIndex, int n) {
	netList[netIndex].cellularRules.resize(n);
}

void MOrganization::setNetworkCellularRule(int netIndex, l3Firewall s, int index) {
	netList[netIndex].cellularRules[index] = s;
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

void MOrganization::setOrgVPNFirewallRulesNum(int n) {
	vpnFirewallRules.resize(n);
}

void MOrganization::setOrgVPNFirewallRule(l3Firewall s, int index) {
	vpnFirewallRules[index] = s;
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

void MOrganization::setNetworkSSID(int netIndex, ssid s, int index) {
	netList[netIndex].netSSIDs[index] = s;
}

void MOrganization::setNetworkS2SVPN(int netIndex, merakiVPN s) {
	netList[netIndex].s2sMerakiVPN = s;
}

void MOrganization::setNetworkTrafficFlowsNum(int netIndex, int num) {
	netList[netIndex].netFlows.resize(num);
}

void MOrganization::setNetworkTrafficFlow(int netIndex, netTraffic s, int index) {
	netList[netIndex].netFlows[index] = s;
}

void MOrganization::setNetworkAccessPoliciesNum(int netIndex, int num) {
	netList[netIndex].accessPolicies.resize(num);
}

void MOrganization::setNetworkAccessPolicy(int netIndex, netAccessPolicy s, int index) {
	netList[netIndex].accessPolicies[index] = s;
}

void MOrganization::setNetworkAirMarshalEntriesNum(int netIndex, int num) {
	netList[netIndex].airMarshalEntries.resize(num);
}

void MOrganization::setNetworkAirMarshalEntry(int netIndex, netAirMarshal s, int index) {
	netList[netIndex].airMarshalEntries[index] = s;
}

void MOrganization::setNetworkBtoothSettings(int netIndex, netBtoothSettings s) {
	netList[netIndex].bToothSettings = s;
}

void MOrganization::setNetworkPhoneNum(int netIndex, int num) {
	netList[netIndex].netPhones.resize(num);
}

void MOrganization::setNetworkPhone(int netIndex, netPhone s, int index) {
	netList[netIndex].netPhones[index] = s;
}

void MOrganization::setNetworkPhoneContactNum(int netIndex, int num) {
	netList[netIndex].netPhoneContacts.resize(num);
}

void MOrganization::setNetworkPhoneContact(int netIndex, netPhoneContact s, int index) {
	netList[netIndex].netPhoneContacts[index] = s;
}

void MOrganization::setNetworkPhoneCallgroupsNum(int netIndex, int num) {
	netList[netIndex].netPhoneCallgroups.resize(num);
}

void MOrganization::setNetworkPhoneCallgroupEntry(int netIndex, netPhoneCallgroup s, int index) {
	netList[netIndex].netPhoneCallgroups[index] = s;
}





QString MOrganization::getOrgID() {
	return id;
}

QString MOrganization::getOrgName() {
	return name;
}

QString MOrganization::getOrgSamlUrl() {
	return samlConsumerUrl;
}

int MOrganization::getOrgSamlUrlsNum() {
	return samlConsumerUrls.size();
}

QString MOrganization::getOrgSamlUrlEntry(int index) {
	return samlConsumerUrls.at(index);
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

int MOrganization::getOrgSamlRolesNum() {
	return samlRolesList.size();
}

orgSamlRoles MOrganization::getOrgSamlRole(int index) {
	return samlRolesList.at(index);
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

l3Firewall MOrganization::getMXL3Rule(int devIndex, int index) {
	return orgInventory.at(devIndex).rules.at(index);
}

int MOrganization::getNetworkCellularRuleNum(int netIndex) {
	return netList.at(netIndex).cellularRules.size();
}

l3Firewall MOrganization::getNetworkCellularRule(int netIndex, int index) {
	return netList.at(netIndex).cellularRules.at(index);
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

int MOrganization::getOrgVPNFirewallRulesNum() {
	return vpnFirewallRules.size();
}

l3Firewall MOrganization::getOrgVPNFirewallRule(int index) {
	return vpnFirewallRules.at(index);
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

ssid MOrganization::getNetworkSSID(int netIndex, int index) {
	return netList.at(netIndex).netSSIDs.at(index);
}

merakiVPN MOrganization::getNetworkS2SVPN(int netIndex) {
	return netList.at(netIndex).s2sMerakiVPN;
}

int MOrganization::getNetworkTrafficFlowsNum(int netIndex) {
	return netList.at(netIndex).netFlows.size();
}

netTraffic MOrganization::getNetworkTrafficFlow(int netIndex, int index) {
	return netList.at(netIndex).netFlows.at(index);
}

int MOrganization::getNetworkAccessPoliciesNum(int netIndex) {
	return netList.at(netIndex).accessPolicies.size();
}

netAccessPolicy MOrganization::getNetworkAccessPolicy(int netIndex, int index) {
	return netList.at(netIndex).accessPolicies.at(index);
}

int MOrganization::getNetworkAirMarshalEntriesNum(int netIndex) {
	return netList.at(netIndex).airMarshalEntries.size();
}

netAirMarshal MOrganization::getNetworkAirMarshalEntry(int netIndex, int index) {
	return netList.at(netIndex).airMarshalEntries.at(index);
}

netBtoothSettings MOrganization::getNetworkBtoothSettings(int netIndex) {
	return netList.at(netIndex).bToothSettings;
}

int MOrganization::getNetworkPhoneNum(int netIndex) {
	return netList.at(netIndex).netPhones.size();
}

netPhone MOrganization::getNetworkPhone(int netIndex, int index) {
	return netList.at(netIndex).netPhones.at(index);
}

int MOrganization::getNetworkPhoneContactNum(int netIndex) {
	return netList.at(netIndex).netPhoneContacts.size();
}

netPhoneContact MOrganization::getNetworkPhoneContact(int netIndex, int index) {
	return netList.at(netIndex).netPhoneContacts.at(index);
}

int MOrganization::getNetworkPhoneCallgroupsNum(int netIndex) {
	return netList.at(netIndex).netPhoneCallgroups.size();
}

netPhoneCallgroup MOrganization::getNetworkPhoneCallgroupEntry(int netIndex, int index) {
	return netList.at(netIndex).netPhoneCallgroups.at(index);
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

	return -1;	// no device was found
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

	return -1;	// no device was found
}

int MOrganization::getIndexOfPhoneCallgroupId(int netIndex, QString id) {
	// given network index and id, return index of call group id
	// returns -1 if it is unable to find it
	for (int i = 0; i < netList.at(netIndex).netPhoneCallgroups.size(); i++) {
		if (netList.at(netIndex).netPhoneCallgroups.at(i).id == id) {
			return i;
		}
	}

	return -1;	// no device was found
}

int MOrganization::getIndexOfSamlRole(QString id) {
	// given id of SAML role, return index of SAML role
	// returns -1 if it is unable to find it
	for (int i = 0; i < samlRolesList.size(); i++) {
		if (samlRolesList.at(i).id == id) {
			return i;
		}
	}

	return -1;	// no device was found
}





void MOrganization::showVariables() {
	// debug function, just show all the variables in the class
	qDebug() << "Org ID: " << id;
	qDebug() << "Org name: " << name;
	qDebug() << "SAML URL: " << samlConsumerUrl;

	qDebug() << "SAML URLs: ";
	for (int i = 0; i < samlConsumerUrls.size(); i++) {
		qDebug() << "\t" << samlConsumerUrls.at(i);
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


