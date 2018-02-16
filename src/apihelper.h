/* This class is meant to do all the heavy lifting in terms of making API requests
 * GET, PUT, POST
 * Also, this will put stuff directly in the appropriate objects used to store
 * variables for organization and networks
 *
 *
 */

#include "mainwindow.h"

class MainWindow;
class MOrganization;

#ifndef APIHELPER_H
#define APIHELPER_H


struct urlRequest {
	int reqType = 0;	// 1: GET, 2: PUT, 3: POST, 4: DELETE
	QString url = "";
};

struct eventRequest {
	// used to maintain a queue for HTTP requests to make
	urlRequest req;
	int urlListIndex;			// which index in the urlList it corresponds to
	int orgIndex;				// organization index in orgList
	int netIndex = -1;			// network index in the orgList object
	int ssidIndex = -1;			// SSID index for queries at index 90 and 91
	QString deviceSerial = "";	// in case the query is specific to a particular device, i.e. a switch
	QString clientMac = "";		// client MAC address, i.e. when getting group policies associated to the client
	QByteArray data = 0;		// data to use in case of a PUT or POST
	QString id = "";			// generic id, used for admins, contacts, call groups, VLANs, static route, etc.

	bool responseReceived = false;	// indicates whether a response for this request was received
	bool responseProcessed = false;	// indicates whether this response was processed
};


class APIHelper {
public:
	APIHelper(QString key, MainWindow *p = 0);
	~APIHelper();

	bool readURLListFromFile(QString urlFile);

	void runQuery(eventRequest e);
	void processQuery(QNetworkReply *r);
	void putEventInQueue(eventRequest e, bool force = false);

	// functions to process data returned from queries
	bool processOrgQuery(QJsonDocument doc, int orgIndex = -1);
	bool processNetworkQuery(QJsonDocument doc, int orgIndex, int netIndex = -1);
	bool processLicenseQuery(QJsonDocument doc, int orgIndex);

	bool processOrgAdminsQuery(QJsonDocument doc, eventRequest e);
	bool processOrgConfigTemplatesQuery(QJsonDocument doc, eventRequest e);

	bool processSamlRolesQuery(QJsonDocument doc, int orgIndex, QString id = "");
	bool processOrgInventoryQuery(QJsonDocument doc, int orgIndex);
	bool processNetworkDevicesQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkDeviceUplinkQuery(QJsonDocument doc, eventRequest e);

	bool processOrgSNMPQuery(QJsonDocument doc, int orgIndex);
	bool processOrgVPNQuery(QJsonDocument doc, int orgIndex);
	bool processOrgVPNFirewallRulesQuery(QJsonDocument doc, int orgIndex);
	bool processSwitchPortQuery(QJsonDocument doc, int orgIndex, QString devSerial, QString id = "");
	bool processl3FirewallQuery(QJsonDocument doc, int orgIndex, QString devSerial);

	bool processNetworkCellularFirewallQuery(QJsonDocument doc, eventRequest e);

	bool processSMDevicesQuery(QJsonDocument doc, int orgIndex, int netIndex);

	bool processNetworkGroupPolicyQuery(QJsonDocument doc, int orgIndex, int netIndex);

	bool processClientsConnectedQuery(QJsonDocument doc, eventRequest e);
	bool processClientGroupPolicyQuery(QJsonDocument doc, eventRequest e);
	bool processDeviceLLDPCDPQuery(QJsonDocument doc, eventRequest e);

	bool processNetworkSSIDsQuery(QJsonDocument doc, int orgIndex, int netIndex, int ssidIndex = -1);
	bool processMerakiS2SVPNQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processNetworkTrafficQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processNetworkAccessPoliciesQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processNetworkAirMarshalQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processNetworkBtoothSettingsQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processNetworkPhonesQuery(QJsonDocument doc, int orgIndex, int netIndex, QString serial = "");
	bool processNetworkPhoneContactsQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processNetworkPhoneCallgroupsQuery(QJsonDocument doc, int orgIndex, int netIndex, QString id = "");
	bool processNetworkStaticRoutesQuery(QJsonDocument doc, int orgIndex, int netIndex, QString id = "");
	bool processNetworkVlansQuery(QJsonDocument doc, int orgIndex, int netIndex, QString id = "");
	bool processNetworkSMProfilesQuery(QJsonDocument doc, int orgIndex, int netIndex);

	bool processNetworkDeviceClaimed(QJsonDocument doc, eventRequest e);



	// set
	void setApiKey(QString key);
	void setOrgQueryURL(QUrl u);
	void setNetworkQueryURL(QUrl u);


	// get
	QString getApiKey();
	QUrl getOrgQueryURL();
	QUrl getNetworkQueryURL();
	QString getOrgIDFromURL(QUrl u);
	int getEventQueueSize();
	eventRequest getNextEvent();
	int getEventIndex();


	// utility
	int findOrg(int orgID);


public slots:
	// TODO: figure out how to use this one, instead of the one in MainWindow
	void replyFinished(QNetworkReply *reply);


signals:
//	void queryProcessingFinished();

private:
	MainWindow *parent;
	QVector<eventRequest> queueEventRequests;
	int eventIndex;

	QString apiKey;		// holds the API key being used
	QNetworkAccessManager *manager;

	// TODO: think about a better arrangement for these, read from file?
	QUrl orgQueryURL;
	QUrl networkQueryURL;
	QUrl licenseQueryURL;
	QString tmpURL;		// used for comparison with networkQueryURL and more


	// read the URLs from file
	QString baseURL;
	QVector<urlRequest> urlList;

};

#endif // APIHELPER_H
