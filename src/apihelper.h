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

#define MAX_SSID_NUM 15			// maximum number of configurable SSIDs
#define MAX_SWITCH_PORTS 54		// 48 LAN ports + 4 uplinks + 2 stack ports


struct urlRequest {
	int reqType = 0;	// 1: GET, 2: PUT, 3: POST, 4: DELETE
	QString url = "";
};

struct eventRequest {
	// used to maintain a queue for HTTP requests to make
	urlRequest req;
	int urlListIndex = -1;		// which index in the urlList it corresponds to
	int orgIndex = -1;			// organization index in orgList
	int netIndex = -1;			// network index in the orgList object
	int ssidIndex = -1;			// SSID index for queries at index 90 and 91
	int portIndex = -1;			// switch port index for queries like 99 and 100
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
	bool processOrgQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkBindToTemplateQuery(QJsonDocument doc, eventRequest e);
	bool processSerialKeyOrderClaimQuery(QJsonDocument doc, eventRequest e);
	bool processLicenseQuery(QJsonDocument doc, eventRequest e);
	bool processOrgAdminsQuery(QJsonDocument doc, eventRequest e);
	bool processOrgConfigTemplatesQuery(QJsonDocument doc, eventRequest e);
	bool processSamlRolesQuery(QJsonDocument doc, eventRequest e);

	bool processOrgInventoryQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkDevicesQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkDeviceUplinkQuery(QJsonDocument doc, eventRequest e);
	bool processOrgSNMPQuery(QJsonDocument doc, eventRequest e);
	bool processOrgVPNQuery(QJsonDocument doc, eventRequest e);
	bool processOrgVPNFirewallRulesQuery(QJsonDocument doc, eventRequest e);
	bool processSwitchPortQuery(QJsonDocument doc, eventRequest e);

	bool processl3FirewallQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkCellularFirewallQuery(QJsonDocument doc, eventRequest e);

	bool processNetworkSMDevicesQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkSMProfilesQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkSMTagsUpdateQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkSMFieldsUpdateQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkSMDeviceLockQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkSMDeviceWipeQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkSMDeviceCheckinQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkSMDeviceMoveQuery(QJsonDocument doc, eventRequest e);

	bool processNetworkGroupPolicyQuery(QJsonDocument doc, eventRequest e);
	bool processClientsConnectedQuery(QJsonDocument doc, eventRequest e);
	bool processClientGroupPolicyQuery(QJsonDocument doc, eventRequest e);
	bool processDeviceLLDPCDPQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkSSIDsQuery(QJsonDocument doc, eventRequest e);
	bool processMerakiS2SVPNQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkTrafficQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkAccessPoliciesQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkAirMarshalQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkBtoothSettingsQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkPhonesQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkPhoneContactsQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkPhoneCallgroupsQuery(QJsonDocument doc, eventRequest e);

	bool processNetworkPhoneNumbersQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkAvailablePhoneNumbersQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkStaticRoutesQuery(QJsonDocument doc, eventRequest e);
	bool processNetworkVlansQuery(QJsonDocument doc, eventRequest e);

	bool processNetworkSSIDFirewallRulesQuery(QJsonDocument doc, eventRequest e);
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
