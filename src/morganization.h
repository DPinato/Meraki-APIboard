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
	QString notes;	// from PUT /networks/[network_id]/sm/device/fields

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

struct smClarityVendorConfig {
	QString key;
	QString type;
	QString value;	// this can be an int ???
};

struct smClarityProfile {
	int profile_id;
	QString name;
	bool filterBrowsers;
	bool filterSockets;
	QVector<smClarityVendorConfig> vendorConfig;
	QString pluginBundleID;
};

struct smWifiPayload {
	// :(
	QString id;
	QString configuration;
	bool isManualConfig;		// is_manual_config
	bool hiddenNetwork;			// HIDDEN_NETWORK
	QString encryptionType;		// EncryptionType
	QString password;			// Password
	QString ssidStr;			// SSID_STR
	bool autoJoin;				// AutoJoin
	QString payloadUUID;		// PayloadUUID
	QString proxyType;			// ProxyType
	QString pccMobileConfigId;	// pcc_mobile_config_id
	QString qosEnable;			// QoSEnable
	bool useUsernameAsCn;		// use_username_as_cn

};

struct smProfile {
	QString id;
	QString payloadDisplayName;		// payload_display_name
	QString payloadIdentifier;		// payload_identifier
	QString payloadDescription;		// payload_description
	QString scope;
	QVector<QString> tags;
	QVector<smWifiPayload> wifis;	// if profile has a wifi payload
	QVector<QString> payloadTypes;	// payload_types;
};

struct radiusServer {
	QString host;
	QString port;
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
	// group policies in a network
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
	// this is for the Meraki devices in the particular network
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

	bool walledGardenEnabled;
	QString walledGardenRanges;

	QVector<l3Firewall> fwRules;		// L3 firewall rules of an SSID
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

struct detectedAirMarshal {
	QString device;		// device serial number
	int rssi;
};

struct bssidAirMarshal {
	QString bssid;
	bool contained;
	QVector<detectedAirMarshal> detectedBy;		// who detected this BSSID
};

struct netAirMarshal {
	QString ssid;		// name of SSID seen
	QVector<bssidAirMarshal> bssids;
	QVector<int> channels;		// these are returned as an array with no key, just [1,6,48,...]
	double firstSeen;
	double lastSeen;
	QVector<QString> wiredMacs;	// these are returned as an array of strings, like channels
	QVector<int> wiredVlans;	// returned like wiredMacs and channels
	double wiredLastSeen;
};

struct netBtoothSettings {
	QString id;		// network ID presumably, from my testing this is not returned
	bool scanningEnabled;
	bool advertisingEnabled;
	QString uuid;
	QString majorMinorAssignmentMode;
	int major;
	int minor;	// not returned if majorMinorAssignmentMode is "Unique"
};

struct netPhone {
	QString serial;
	QString contactId;
	QString contactType;
	QVector<QString> publicNumber;
	QString ext;
};

struct netPhoneContact{
	int id;
	QString name;
	QString type;
};

struct netPhoneCallgroup {
	QString id;
	QString name;
	QString publicNumber;
	QString ext;
};

struct netStaticRoute {
	QString id;
	QString networkId;
	QString name;
	QString gatewayIp;
	QString subnet;
};

struct netVlan {
	QString id;
	QString networkId;
	QString name;
	QString applianceIp;
	QString subnet;
};

struct networkVars {
	QString netID;			// network ID
	QString orgID;			// ID of parent organization
	QString netType;		// type of network, i.e. wireless, switch, appliance ...
	QString netName;		// name of network
	QString netTimezone;	// timezone given to network
	QString netTags;		// tags assigned to network, these are returned as a single string

	QVector<smDevice> smDevices;			// if SM network, put SM devices here
	QVector<networkGroupPolicy> gPolicies;	// group policies in the network
	QVector<deviceInNetwork> netDevices;	// devices in network
	QVector<ssid> netSSIDs;					// SSIDs in the network
	QVector<l3Firewall> cellularRules;	// if there is an MX in the network, list cellular rules. no need to
										//  have another struct, it returns the same variables that are in l3Firewall
	merakiVPN s2sMerakiVPN;				// site-to-site auto-VPN
	QVector<netTraffic> netFlows;		// network flows seen by traffic analytics
										// TODO: what is being returned if traffic analytics is disabled?
	QVector<netAccessPolicy> accessPolicies;		// access policies in the MS network
	QVector<netAirMarshal> airMarshalEntries;		// list of entries detected by air marshal
	netBtoothSettings bToothSettings;				// settings for bluetooth scanning
	QVector<netPhone> netPhones;					// phones and contact assignments
	QVector<netPhoneContact> netPhoneContacts;		// phone contacts
	QVector<netPhoneCallgroup> netPhoneCallgroups;	// call groups

	QVector<QString> publicNumbers;					// list of the phone numbers in the network
	QVector<QString> availablePublicNumbers;		// list of the available phone numbers in the network

	QVector<netStaticRoute> netStaticRoutes;		// static routes in the network
	QVector<netVlan> netVlans;						// VLANs in the network
	QVector<smProfile> smProfiles;					// Systems Manager profiles in the network

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
	// group policy assigned to a client in a network
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

struct orgSamlRoles {
	QString id;
	QString role;
	QString orgAccess;
	QVector<adminNetPermission> networks;	// networks in which SAML role has permissions, (id, access)
	QVector<adminTag> tags;					// (tag, access)
};

struct configTemplate{
	QString id;		// network ID
	QString name;	// name of configuration template
};


class MOrganization {
	public:
		MOrganization();
		MOrganization(QString orgName, QString orgId);

		~MOrganization();


		// set
		void setOrgID(QString orgId);
		void setOrgName(QString orgName);
		void setOrgSamlUrl(QString url);
		void setOrgSamlUrlsNum(int num);
		void setOrgSamlUrlEntry(QString s, int index);
		void setNetworksNum(int n);
		void setNetwork(networkVars v, int index);
		void setLicenseStatus(QString s);
		void setLicenseExpDate(QString d);
		void setLicenseDeviceNum(int n);
		void setLicensePerDevice(licensesPerDevice a, int index);
		void setAdminsNum(int n);
		void setAdmin(adminStruct a, int index);
		void setOrgSamlRolesNum(int n);
		void setOrgSamlRole(orgSamlRoles s, int index);
		void setOrgInventorySize(int n);
		void setOrgInventoryDevice(deviceInInventory a, int index);
		void setOrgConfigTemplatesNum(int n);
		void setOrgConfigTemplate(configTemplate n, int index);
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
		void setNetworkSMDevicesNum(int netIndex, int n);
		void setNetworkSMDevice(int netIndex, smDevice s, int index);
		void setNetworkSMProfilesNum(int netIndex, int n);
		void setNetworkSMProfile(int netIndex, smProfile s, int index);


		void setNetworkGroupPolicyNum(int netIndex, int n);
		void setNetworkGroupPolicy(int netIndex, networkGroupPolicy s, int index);
		void setClientsConnectedNum(int devIndex, int n);
		void setClientConnected(int devIndex, clientConnected s, int index);
		void setNetworkDevicesNum(int netIndex, int n);
		void setNetworkDevice(int netIndex, deviceInNetwork s, int index);
		void setNetworkSSID(int netIndex, ssid s, int ssidIndex);
		void setNetworkSSIDFwRulesNum(int netIndex, int ssidIndex, int n);
		void setNetworkSSIDFwRule(int netIndex, int ssidIndex, l3Firewall f, int index);
		void setNetworkS2SVPN(int netIndex, merakiVPN s);
		void setNetworkTrafficFlowsNum(int netIndex, int num);
		void setNetworkTrafficFlow(int netIndex, netTraffic s, int index);
		void setNetworkAccessPoliciesNum(int netIndex, int num);
		void setNetworkAccessPolicy(int netIndex, netAccessPolicy s, int index);
		void setNetworkAirMarshalEntriesNum(int netIndex, int num);
		void setNetworkAirMarshalEntry(int netIndex, netAirMarshal s, int index);
		void setNetworkBtoothSettings(int netIndex, netBtoothSettings s);
		void setNetworkPhoneNum(int netIndex, int num);
		void setNetworkPhone(int netIndex, netPhone s, int index);
		void setNetworkPhoneContactNum(int netIndex, int num);
		void setNetworkPhoneContact(int netIndex, netPhoneContact s, int index);
		void setNetworkPhoneCallgroupsNum(int netIndex, int num);
		void setNetworkPhoneCallgroupEntry(int netIndex, netPhoneCallgroup s, int index);
		void setNetworkPublicNumbersNum(int netIndex, int num);
		void setNetworkPublicNumberEntry(int netIndex, QString n, int index);
		void setNetworkAvailablePublicNumbersNum(int netIndex, int num);
		void setNetworkAvailablePublicNumberEntry(int netIndex, QString n, int index);
		void setNetworkStaticRoutesNum(int netIndex, int num);
		void setNetworkStaticRoute(int netIndex, netStaticRoute s, int index);
		void setNetworkVlansNum(int netIndex, int num);
		void setNetworkVlan(int netIndex, netVlan s, int index);



		// get
		QString getOrgID();
		QString getOrgName();
		QString getOrgSamlUrl();
		int getOrgSamlUrlsNum();
		QString getOrgSamlUrlEntry(int index);
		int getNetworksNum();
		networkVars getNetwork(int index);
		QString getLicenseStatus();
		QString getLicenseExpireDate();
		int getLicenseListSize();
		licensesPerDevice getLicensePerDevice(int index);
		int getAdminListSize();
		adminStruct getAdmin(int index);
		int getOrgSamlRolesNum();
		orgSamlRoles getOrgSamlRole(int index);
		int getOrgInventorySize();
		deviceInInventory getOrgInventoryDevice(int index);
		deviceInInventory getOrgDeviceFromSerial(QString serial);
		int getConfigTemplatesNum();
		configTemplate getConfigTemplate(int index);
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
		int getNetworkSMDevicesNum(int netIndex);
		smDevice getNetworkSMDevice(int netIndex, int index);
		int getNetworkSMProfilesNum(int netIndex);
		smProfile getNetworkSMProfile(int netIndex, int index);

		int getNetworkGroupPolicyNum(int netIndex);
		networkGroupPolicy getNetworkGroupPolicy(int netIndex, int index);
		int getClientsConnectedNum(int devIndex);
		clientConnected getClientConnected(int devIndex, int index);
		int getNetworkDevicesNum(int netIndex);
		deviceInNetwork getNetworkDevice(int netIndex, int index);
		ssid getNetworkSSID(int netIndex, int ssidIndex);
		int getNetworkSSIDFwRulesNum(int netIndex, int ssidIndex);
		l3Firewall getNetworkSSIDFwRule(int netIndex, int ssidIndex, int index);
		merakiVPN getNetworkS2SVPN(int netIndex);
		int getNetworkTrafficFlowsNum(int netIndex);
		netTraffic getNetworkTrafficFlow(int netIndex, int index);
		int getNetworkAccessPoliciesNum(int netIndex);
		netAccessPolicy getNetworkAccessPolicy(int netIndex, int index);
		int getNetworkAirMarshalEntriesNum(int netIndex);
		netAirMarshal getNetworkAirMarshalEntry(int netIndex, int index);
		netBtoothSettings getNetworkBtoothSettings(int netIndex);
		int getNetworkPhoneNum(int netIndex);
		netPhone getNetworkPhone(int netIndex, int index);
		int getNetworkPhoneContactNum(int netIndex);
		netPhoneContact getNetworkPhoneContact(int netIndex, int index);
		int getNetworkPhoneCallgroupsNum(int netIndex);
		netPhoneCallgroup getNetworkPhoneCallgroupEntry(int netIndex, int index);

		int getNetworkPublicNumbersNum(int netIndex);
		QString getNetworkPublicNumberEntry(int netIndex, int index);
		int getNetworkAvailablePublicNumbersNum(int netIndex);
		QString getNetworkAvailablePublicNumberEntry(int netIndex, int index);


		int getNetworkStaticRoutesNum(int netIndex);
		netStaticRoute getNetworkStaticRoute(int netIndex, int index);
		int getNetworkVlansNum(int netIndex);
		netVlan getNetworkVlan(int netIndex, int index);



		// remove functions
		bool removeOrgAdmin(int index);
		bool removeNetwork(int index);
		bool removeOrgConfigTemplate(int index);
		bool removeOrgSamlAdminRole(int index);
		bool removeNetworkPhoneAssignment(int netIndex, int index);
		bool removeNetworkPhoneCallGroup(int netIndex, int index);
		bool removeNetworkPhoneContact(int netIndex, int index);
		bool removeNetworkStaticRoute(int netIndex, int index);
		bool removeNetworkVlan(int netIndex, int index);



		// functions to help navigating lists and vectors
		int getIndexOfOrgAdmin(QString id);
		int getIndexOfConfigTemplate(QString id);
		int getIndexOfInventoryDevice(QString serial);
		int getIndexOfClientConnected(QString netID, QString mac);
		int getIndexOfNetwork(QString netID);
		int getIndexOfNetworkDevice(QString netID, QString serial);
		int getIndexOfPhoneCallgroupId(int netIndex, QString id);
		int getIndexOfPhoneContact(int netIndex, QString id);
		int getIndexOfSamlRole(QString id);
		int getIndexOfNetworkStaticRoute(int netIndex, QString id);
		int getIndexOfNetworkVlan(int netIndex, QString id);
		int getIndexOfNetworkPhone(int netIndex, QString serial);
		int getIndexOfNetworkSMDevice(int netIndex, QString id);


		// debug
		void showVariables();


	private:
		QString id;		// organization ID, this number can be really long, better as a string
		QString name;						// organization name
		QString samlConsumerUrl;			// SAML URL for organization
		QVector<QString> samlConsumerUrls;	// SAML URLs for organization
		QVector<networkVars> netList;	// list of networks associated with this organization

		// licensing state
		QString licStatus;			// state of the licensing for the org
		QString licExpireDate;		// licensing expiration date
		QVector<licensesPerDevice> licenseList;

		QVector<deviceInInventory> orgInventory;	// organization inventory
		orgSNMP snmpSettings;						// organization SNMP
		QVector<configTemplate> configTemplates;	// configuration templates in organization

		// non-Meraki site-to-site VPN
		QVector<nonMerakiVPNPeer> nonMerakiVPNs;	// holds list of non-Meraki site-to-site VPN peers
		QVector<l3Firewall> vpnFirewallRules;

		// administrators
		QVector<adminStruct> adminList;
		QVector<orgSamlRoles> samlRolesList;


};

#endif // MORGANIZATION_H
