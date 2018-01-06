#ifndef MORGANIZATION_H
#define MORGANIZATION_H

#include "mainwindow.h"

struct networkVars {
	QString netID;			// network ID
	QString orgID;			// ID of parent organization
	QString netType;		// type of network, i.e. wireless, switch, appliance ...
	QString netName;		// name of network
	QString netTimezone;	// timezone given to network
	QString netTags;		// tags assigned to network, these are returned as a single string
};

struct licensesPerDevice {
	QString deviceType;		// device type, i.e. MS225, MX100, ...
	int count;				// licenses available for the device type
};

struct deviceInInventory {
	QString mac;			// MAC address of device
	QString serial;			// serial number of device
	QString netID;			// network ID in which the device is
	QString model;			// device model
	QString claimedAt;		// when the device was claimed
	QString publicIP;		// public IP address of device
};

struct nonMerakiVPNPeer {
	QString peerName;
	QString peerPublicIP;
	QVector<QString> privateSubnets;
	QString secret;
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

		// debug
		void showVariables();


	private:


		QString id;					// organization ID, this number can be really long, store it as string for now
		QString name;				// organization name
		QString samlURL;			// SAML URL for organization
		QVector<QString> samlURLs;	// SAML URLs for organization, not implemented yet

		QVector<networkVars> netList;	// list of network associated with this organization

		// licensing state
		QString licStatus;			// state of the licensing for the org
		QString licExpireDate;		// licensing expiration date
		QVector<licensesPerDevice> licenseList;

		// organization inventory
		QVector<deviceInInventory> orgInventory;	// organization inventory

		// organization SNMP
		bool snmp2cOrgEnabled;	// true of SNMP v2c is enabled for the organization
		bool snmp3OrgEnabled;	// true of SNMP v3 is enabled for the organization
		int snmpAuthMode;			// 0: MD5, 1: SHA1
		int snmpPrivMode;			// 0: DES, 1: AES128
		QString snmpAuthPass;		// authentication password
		QString snmpPrivPass;		// privacy password
		QStringList snmpPeerIPs;	// allowed peer IPs

		// non-Meraki site-to-site VPN


};

#endif // MORGANIZATION_H
