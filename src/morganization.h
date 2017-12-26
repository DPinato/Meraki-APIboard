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

		// this for licensing state
		QString licStatus;				// state of the licensing for the org
		QString licExpireDate;		// licensing expiration date
		QVector<licensesPerDevice> licenseList;


};

#endif // MORGANIZATION_H
