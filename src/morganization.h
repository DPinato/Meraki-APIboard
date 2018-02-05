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

struct radiusServer {
	QString host;
	QString port;
};

struct ssid {
	// TODO: I am sure that is radiusAccountingEnabled is true, there are more settings
	// the API endpoint always reports all 15 configurable SSIDs, even if some were never configured
	int number;
	QString name;
	bool enabled;
	QString splashPage;
	bool ssidAdminAccessible;
	QString authMode;
	QString psk;
	QString encryptionMode;
	QString wpaEncryptionMode;

	QVector<radiusServer> radiusServers;
	bool radiusAccountingEnabled;
	bool radiusCoaEnabled;
	QString radiusAttributeForGroupPolicies;

	QString ipAssignmentMode;
	bool useVlanTagging;
	int minBitrate;
	QString bandSelection;
	double perClientBandwidthLimitUp;
	double perClientBandwidthLimitDown;
};

struct merakiVPNHub {
	QString hubId;			// network ID of the hub the spoke is pointing to
	bool useDefaultRoute;	// true if full-tunnel site-to-site VPN
};

struct merakiVPNSubnets {
	QString localSubnet;	// local subnet on the MX
	bool useVpn;			// whether subnet is advertised through site-to-site VPN
};

struct merakiVPN {
	QString mode;						// hub or spoke
	QVector<merakiVPNHub> hubs;			// if spoke, these will be the hubs the spoke is pointing to
	QVector<merakiVPNSubnets> subnets;	// local subnets and whether they are advertised
};

struct networkGroupPolicy {
	QString name;		// name of group policy
	int groupPolicyId;	// id of group policy
};

struct deviceUplink {
	QString interface;
	QString status;
	QString ip;
	QString gateway;
	QString publicIp;
	QString dns;			// DNS servers are returned as a single string separated by commas, "x.x.x.x, y.y.y.y"
	bool usingStaticIp;
};

struct deviceCDP {
	QString deviceId;
	QString portId;
	QString address;
	QString sourcePort;
};

struct deviceLLDP {
	QString systemName;
	QString portId;
	QString managementAddress;
	QString sourcePort;
};

struct deviceInNetwork {
	// this is for the devices in the particular network
	QString lanIp;
	QString serial;
	QString mac;
	double lat;
	double lng;
	QString address;
	QString name;
	QString model;
	QString networkId;

	QString wan1Ip;		// for MX
	QString wan2Ip;		// for MX

	// these are returned in case of GET /networks/[networkId]/devices/[serial]/uplink
	int uplinkNum;
	deviceUplink uplink[2];

	// these are returned in case of GET /networks/[networkId]/devices/[serial]/lldp_cdp
	QVector<deviceCDP> devCDP;
	QVector<deviceLLDP> devLLDP;
};

struct l3Firewall {
	QString comment;		// comment for rule
	QString policy;			// allow or deny
	QString protocol;		// TCP, UDP, ICMP or Any
	QString destPort;		// L4 destination port
	QString destCidr;		// destination subnet
	QString srcPort;		// L4 source port
	QString srcCidr;		// source subnet (needs to be either a local or Meraki VPN subnet)
	bool syslogEnabled;		// Syslog logging enabled
};

struct netTraffic {
    QString application;
    QString destination;
    QString protocol;
    int port;       // careful because sometimes a string "-" is returned
    double recv;
    double sent;
    int flows;
    double activeTime;
    int numClients;
};

struct netAccessPolicy {
	// only valid for MS networks
	int number;		// access policy number, like an ID so it also indicates position
	QString name;
	QString accessType;
	int guestVlan;
	QVector<radiusServer> radiusServers;		// here the "host" is reported as "ip"
};

struct networkVars {
	QString netID;			// network ID
	QString orgID;			// ID of parent organization
	QString netType;		// type of network, i.e. wireless, switch, appliance ...
	QString netName;		// name of network
	QString netTimezone;	// timezone given to network
	QString netTags;		// tags assigned to network, these are returned as a single string

	QVector<smDevice> smDevices;	// if SM network, put SM devices here
	QVector<networkGroupPolicy> gPolicies;	// group policies in the network
	QVector<deviceInNetwork> netDevices;	// devices in network
	QVector<ssid> netSSIDs;			// SSIDs in the network
	QVector<l3Firewall> cellularRules;	// if there is an MX in the network, list cellular rules. no need to
										//  have another struct, it returns the same variables that are in l3Firewall
	merakiVPN s2sMerakiVPN;		// site-to-site auto-VPN
	QVector<netTraffic> netFlows;	// network flows seen by traffic analytics
									// TODO: what is being returned if traffic analytics is disabled?
	QVector<netAccessPolicy> accessPolicies;		// access policies in the MS network
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
	QString accessPolicyNumber;		// access policy number
};

struct clientGroupPolicy {
	QString mac;				// mac address of client
	QString type;				// I suppose this will either be "Group policy" if applied from dashboard, or 802.1X if from RADIUS
	int groupPolicyId = -1;		// group policy ID, this is only returned if a group policy is assigned to the client
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

	clientGroupPolicy policy;	// group policy assigned to the client
};

struct deviceInInventory {
	// this is for devices in the whole organization inventory
	QString mac;			// MAC address of device
	QString serial;			// serial number of device
	QString netID;			// network ID in which the device is
	QString model;			// device model
	QString claimedAt;		// when the device was claimed
	QString publicIP;		// public IP address of device

	QVector<switchPort> ports;			// info about switch ports (in case device is an MS)
	QVector<l3Firewall> rules;		// info about L3 firewall rules (in case device is an MX)
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
		void setMXL3Rule(int devIndex, l3Firewall s, int index);
		void setNetworkCellularRulesNum(int netIndex, int n);
		void setNetworkCellularRule(int netIndex, l3Firewall s, int index);
		void setOrgSNMPSettings(orgSNMP s);
		void setOrgVPNPeerNum(int n);
		void setOrgVPNPeer(nonMerakiVPNPeer p, int index);
		void setOrgVPNFirewallRulesNum(int n);
		void setOrgVPNFirewallRule(l3Firewall s, int index);
		void setSMDevicesNum(int netIndex, int n);
		void setSMDevice(int netIndex, smDevice s, int index);
		void setNetworkGroupPolicyNum(int netIndex, int n);
		void setNetworkGroupPolicy(int netIndex, networkGroupPolicy s, int index);
		void setClientsConnectedNum(int devIndex, int n);
		void setClientConnected(int devIndex, clientConnected s, int index);
		void setNetworkDevicesNum(int netIndex, int n);
		void setNetworkDevice(int netIndex, deviceInNetwork s, int index);
		void setNetworkSSID(int netIndex, ssid s, int index);
		void setNetworkS2SVPN(int netIndex, merakiVPN s);
		void setNetworkTrafficFlowsNum(int netIndex, int num);
		void setNetworkTrafficFlow(int netIndex, netTraffic s, int index);
		void setNetworkAccessPoliciesNum(int netIndex, int num);
		void setNetworkAccessPolicy(int netIndex, netAccessPolicy s, int index);



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
		l3Firewall getMXL3Rule(int devIndex, int index);
		int getNetworkCellularRuleNum(int netIndex);
		l3Firewall getNetworkCellularRule(int netIndex, int index);
		orgSNMP getOrgSNMPSettings();
		int getOrgVPNPeerNum();
		nonMerakiVPNPeer getOrgVPNPeer(int index);
		int getOrgVPNFirewallRulesNum();
		l3Firewall getOrgVPNFirewallRule(int index);
		int getSMDevicesNum(int netIndex);
		smDevice getSMDevice(int netIndex, int index);
		int getNetworkGroupPolicyNum(int netIndex);
		networkGroupPolicy getNetworkGroupPolicy(int netIndex, int index);
		int getClientsConnectedNum(int devIndex);
		clientConnected getClientConnected(int devIndex, int index);
		int getNetworkDevicesNum(int netIndex);
		deviceInNetwork getNetworkDevice(int netIndex, int index);
		ssid getNetworkSSID(int netIndex, int index);
		merakiVPN getNetworkS2SVPN(int netIndex);
		int getNetworkTrafficFlowsNum(int netIndex);
		netTraffic getNetworkTrafficFlow(int netIndex, int index);
		int getNetworkAccessPoliciesNum(int netIndex);
		netAccessPolicy getNetworkAccessPolicy(int netIndex, int index);



		// functions to help navigating lists and vectors
		int getIndexOfInventoryDevice(QString serial);
		int getIndexOfClientConnected(QString netID, QString mac);
		int getIndexOfNetwork(QString netID);
		int getIndexOfNetworkDevice(QString netID, QString serial);


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
		QVector<l3Firewall> vpnFirewallRules;

		// administrators
		QVector<adminStruct> adminList;

};

#endif // MORGANIZATION_H
