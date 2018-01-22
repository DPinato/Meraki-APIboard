#ifndef MORGANIZATION_H
#define MORGANIZATION_H

#include "mainwindow.h"


struct smDevice {
	// default fields
	QString id;
	QString name;
	QVector<QString> tags;
	QString ssid;
	QString wifiMac;
	QString osName;
	QString systemModel;
	QString uuid;
	QString serialNumber;

	// optional fields
	QString ip;
	QString systemType;
	double availableDeviceCapacity;
	QString kioskAppName;
	QString biosVersion;
	double lastConnected;
	double missingAppsCount;
	QString userSuppliedAddress;
	QString location;
	QString lastUser;
	QString publicIp;
	QString phoneNumber;
	QString diskInfoJson;
	double deviceCapacity;
	bool isManaged;
	bool hadMdm;
	bool isSupervised;
	QString meid;
	QString imei;
	QString iccid;
	QString simCarrierNetwork;
	double cellularDataUsed;
	bool isHotspotEnabled;
	double createdAt;
	QString batteryEstCharge;
	bool quarantined;
	QString avName;
	QString avRunning;
	QString asName;
	QString fwName;
	bool isRooted;
	bool loginRequired;		// ?
	bool screenLockEnabled;	// ?
	QString screenLockDelay;
	bool autoLoginDisabled;
	bool hasMdm;
	bool hasDesktopAgent;
	bool diskEncryptionEnabled;
	QString hardwareEncryptionCaps;
	bool passCodeLock;
};

struct groupPolicy {
	QString name;		// name of group policy
	int groupPolicyId;	// id of group policy
};

struct networkVars {
	QString netID;			// network ID
	QString orgID;			// ID of parent organization
	QString netType;		// type of network, i.e. wireless, switch, appliance ...
	QString netName;		// name of network
	QString netTimezone;	// timezone given to network
	QString netTags;		// tags assigned to network, these are returned as a single string

	QVector<smDevice> smDevices;	// if SM network, put SM devices here
	QVector<groupPolicy> gPolicies;	// group policies in the network
};

struct licensesPerDevice {
	QString deviceType;		// device type, i.e. MS225, MX100, ...
	int count;				// licenses available for the device type
};

struct switchPort {
	int number;
	QString name;
	QString tags;			// tags assigned to port, returned as a single string, not as an array
	bool enabled;
	bool poeEnabled;
	QString type;
	int nativeVlan;
	int voiceVlan;
	QString allowedVLANs;	// allowed VLANs, returned as a single string, not as an array
	bool isolationEnabled;
	bool rstpEnabled;
	QString stpGuard;
	QString accessPolicyNumber;		// what is this? maybe when a port is set to Access
};

struct mxL3Firewall {
	QString comment;		// comment for rule
	QString policy;			// allow or deny
	QString protocol;		// TCP, UDP, ICMP or Any
	QString destPort;		// L4 destination port
	QString destCidr;		// destination subnet
	QString srcPort;		// L4 source port
	QString srcCidr;		// source subnet (needs to be either a local or Meraki VPN subnet)
	bool syslogEnabled;		// Syslog logging enabled
};

struct clientConnected{
	double sent;			// data sent by client, in kiloBytes
	double recv;			// data received by client, in kilobytes
	QString id;				// client id
	QString description;	// client description
	QString mdnsName;		// MDNS name of client
	QString dhcpHostname;	// DHCP hostname
	QString mac;			// MAC address of client
	QString ip;				// IP address of client
	int vlan;				// VLAN of client, looks like that if there is no VLAN, an empty string is returned
	QString switchport;		// switchport to which client is connected, only returned if querying an MS, null otherwise
};

struct deviceInInventory {
	QString mac;			// MAC address of device
	QString serial;			// serial number of device
	QString netID;			// network ID in which the device is
	QString model;			// device model
	QString claimedAt;		// when the device was claimed
	QString publicIP;		// public IP address of device

	QVector<switchPort> ports;			// info about switch ports (in case device is an MS)
	QVector<mxL3Firewall> rules;		// info about L3 firewall rules (in case device is an MX)
	QVector<clientConnected> clients;	// clients connected to device

};

struct nonMerakiVPNPeer {
	QString peerName;			// name of non-Meraki site-to-site VPN peer
	QString peerPublicIP;		// public IP of peer
	QVector<QString> privateSubnets;	// private subnets reachable through peer
	QString secret;				// secret for site-to-site VPN
	QVector<QString> tags;		// tags assigned to this VPN
};

struct adminNetPermission {
	QString netID;			// ID of network in which admin has access to
	QString accessLevel;	// level of access in network
	QString networkType;	// an admin that has access only to all cameras in the org will get this fiels
};

struct adminTag {
	QString adminAccessLevel;	// access level of administrator to networks with tag "tag"
	QString tag;				// tag of network
};

struct adminStruct {
	QString name;		// name of administrator
	QString email;		// email address
	QString id;			// ID of administrator
	QString orgAccess;	// level of organization permissions, "full" or "none"

	QVector<adminNetPermission> nets;		// networks in which network admin has permissions on
	QVector<adminTag> tags;					// tags associated with administrator
	QVector<adminNetPermission> cNets;		// level of permissions for camera-only administrators
};

struct orgSNMP {
	bool snmp2cOrgEnabled;			// true of SNMP v2c is enabled for the organization
	bool snmp3OrgEnabled;			// true of SNMP v3 is enabled for the organization
	QString snmpAuthMode;			// 0: MD5, 1: SHA1
	QString snmpPrivMode;			// 0: DES, 1: AES128
	QString snmpAuthPass;			// authentication password, only when updating settings
	QString snmpPrivPass;			// privacy password, only when updating settings
	QVector<QString> snmpPeerIPs;	// allowed peer IPs

	QString hostname;		// hostname for SNMP
	QString port;			// port number for SNMP

	QString v2CommString;	// community string when using SNMPv2
	QString v3User;			// user when using SNMPv3
};


class MOrganization {
	public:
		MOrganization();
		MOrganization(QString orgName, QString orgId);

		~MOrganization();


		// set
		void setOrgID(QString orgId);
		void setOrgName(QString orgName);
		void setSamlURL(QString url);
		void setNetworksNum(int n);
		void setNetwork(networkVars v, int index);
		void setLicenseStatus(QString s);
		void setLicenseExpDate(QString d);
		void setLicenseDeviceNum(int n);
		void setLicensePerDevice(licensesPerDevice a, int index);
		void setAdminsNum(int n);
		void setAdmin(adminStruct a, int index);
		void setOrgInventorySize(int n);
		void setOrgInventoryDevice(deviceInInventory a, int index);
		void setSwitchPortNum(int devIndex, int n);
		void setSwitchPort(int devIndex, switchPort s, int index);
		void setMXL3RulesNum(int devIndex, int n);
		void setMXL3Rule(int devIndex, mxL3Firewall s, int index);
		void setOrgSNMPSettings(orgSNMP s);
		void setOrgVPNPeerNum(int n);
		void setOrgVPNPeer(nonMerakiVPNPeer p, int index);
		void setSMDevicesNum(int netIndex, int n);
		void setSMDevice(int netIndex, smDevice s, int index);
		void setGroupPolicyNum(int netIndex, int n);
		void setGroupPolicy(int netIndex, groupPolicy s, int index);
		void setClientsConnectedNum(int devIndex, int n);
		void setClientConnected(int devIndex, clientConnected s, int index);


		// get
		QString getOrgID();
		QString getOrgName();
		QString getSamlURL();
		int getNetworksNum();
		networkVars getNetwork(int index);
		QString getLicenseStatus();
		QString getLicenseExpireDate();
		int getLicenseListSize();
		licensesPerDevice getLicensePerDevice(int index);
		int getAdminListSize();
		adminStruct getAdmin(int index);
		int getOrgInventorySize();
		deviceInInventory getOrgInventoryDevice(int index);
		deviceInInventory getOrgDeviceFromSerial(QString serial);
		int getSwitchPortNum(int devIndex);
		switchPort getSwitchport(int devIndex, int index);
		int getMXL3RulesNum(int devIndex);
		mxL3Firewall getMXL3Rule(int devIndex, int index);
		orgSNMP getOrgSNMPSettings();
		int getOrgVPNPeerNum();
		nonMerakiVPNPeer getOrgVPNPeer(int index);
		int getSMDevicesNum(int netIndex);
		smDevice getSMDevice(int netIndex, int index);
		int getGroupPolicyNum(int netIndex);
		groupPolicy getGroupPolicy(int netIndex, int index);
		int getClientsConnectedNum(int devIndex);
		clientConnected getClientConnected(int devIndex, int index);


		// functions to help navigating lists and vectors
		int getIndexOfInventoryDevice(QString serial);
		int getIndexOfNetwork(QString netID);


		// debug
		void showVariables();


	private:
		QString id;					// organization ID, this number can be really long, store it as string for now
		QString name;				// organization name
		QString samlURL;			// SAML URL for organization
		QVector<QString> samlURLs;	// SAML URLs for organization, not implemented yet
		QVector<networkVars> netList;	// list of networks associated with this organization

		// licensing state
		QString licStatus;			// state of the licensing for the org
		QString licExpireDate;		// licensing expiration date
		QVector<licensesPerDevice> licenseList;

		// organization inventory
		QVector<deviceInInventory> orgInventory;	// organization inventory

		// organization SNMP
		orgSNMP snmpSettings;

		// non-Meraki site-to-site VPN
		QVector<nonMerakiVPNPeer> nonMerakiVPNs;	// holds list of non-Meraki site-to-site VPN peers

		// administrators
		QVector<adminStruct> adminList;

};

#endif // MORGANIZATION_H
