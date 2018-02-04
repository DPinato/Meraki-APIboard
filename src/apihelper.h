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


struct eventRequest {
	// used to maintain a queue for HTTP requests to make
	int urlListIndex;			// which index in the urlList it corresponds to
	int orgIndex;				// organization index in orgList
	int netIndex = -1;			// network index in the orgList object
	int ssidIndex = -1;			// SSID index for queries at index 90 and 91
	QString deviceSerial = "";	// in case the query is specific to a particular device, i.e. a switch
	QString clientMac = "";		// client MAC address, i.e. when getting group policies associated to the client
	QByteArray data = 0;		// data to use in case of a PUT or POST

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
	bool processOrgQuery(QJsonDocument doc);
	bool processNetworkQuery(QJsonDocument doc, int orgIndex, int netIndex = -1);
	bool processLicenseQuery(QJsonDocument doc, int orgIndex);
	bool processOrgAdminsQuery(QJsonDocument doc, int orgIndex);
	bool processOrgInventoryQuery(QJsonDocument doc, int orgIndex);
	bool processNetworkDevicesQuery(QJsonDocument doc, int orgIndex, int netIndex, QString serial = "");
	bool processNetworkDeviceUplinkQuery(QJsonDocument doc, int orgIndex, int netIndex, QString devSerial);
	bool processOrgSNMPQuery(QJsonDocument doc, int orgIndex);
	bool processOrgVPNQuery(QJsonDocument doc, int orgIndex);
	bool processOrgVPNFirewallRulesQuery(QJsonDocument doc, int orgIndex);
	bool processSwitchPortQuery(QJsonDocument doc, int orgIndex, QString devSerial);
	bool processl3FirewallQuery(QJsonDocument doc, int orgIndex, QString devSerial);
	bool processNetworkCellularFirewallQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processSMDevicesQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processNetworkGroupPolicyQuery(QJsonDocument doc, int orgIndex, int netIndex);
	bool processClientsConnectedQuery(QJsonDocument doc, int orgIndex, QString devSerial);
	bool processClientGroupPolicyQuery(QJsonDocument doc, int orgIndex, int netIndex, QString clientMac);
	bool processDeviceLLDPCDPQuery(QJsonDocument doc, int orgIndex, int netIndex, QString devSerial);
	bool processNetworkSSIDsQuery(QJsonDocument doc, int orgIndex, int netIndex, int ssidIndex = -1);
	bool processMerakiS2SVPNQuery(QJsonDocument doc, int orgIndex, int netIndex);



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
	struct urlRequest {
		int reqType;	// 1: GET, 2: PUT, 3: POST, 4: DELETE
		QString url;
	};
	QVector<urlRequest> urlList;

};

#endif // APIHELPER_H
