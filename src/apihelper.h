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


class APIHelper {
public:
	APIHelper(QString key, MainWindow *p = 0);
	~APIHelper();

	void processQuery(QNetworkReply *r);

	void runOrgQuery();
	void runOrgQuery(int index);	// get all networks for the org at the index
	void runNetworkQuery(QModelIndex &index);
	void runLicenseQuery(int index);	// get license info for org


	bool processOrgQuery(QJsonDocument doc);
	void processNetworkQuery(QJsonDocument doc);
	bool processLicenseQuery(QJsonDocument doc, int orgIndex);


	// set
	void setApiKey(QString key);
	void setOrgQueryURL(QUrl u);
	void setNetworkQueryURL(QUrl u);


	// get
	QString getApiKey();
	QUrl getOrgQueryURL();
	QUrl getNetworkQueryURL();
	QString getOrgIDFromURL(QUrl u);


	// utility
	int findOrg(int orgID);


public slots:
	// TODO: figure out how to use this one, instead of the one in MainWindow
	void replyFinished(QNetworkReply *reply);


signals:
//	void queryProcessingFinished();

private:
	MainWindow *parent;

	QString apiKey;		// holds the API key being used

	QNetworkAccessManager *manager;

	// TODO: think about a better arrangement for these, read from file?
	QUrl orgQueryURL;
	QUrl networkQueryURL;
	QUrl licenseQueryURL;
	QString tmpURL;		// used for comparison with networkQueryURL and more


};

#endif // APIHELPER_H
