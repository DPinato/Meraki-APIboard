/* This class is meant to do all the heavy lifting in terms of making API requests
 * GET, PUT, POST
 * Also, this will put stuff directly in the appropriate objects used to store
 * variables for organization and networks
 *
 *
 */

#include "mainwindow.h"

#ifndef APIHELPER_H
#define APIHELPER_H


class APIHelper : public QWidget {
	Q_OBJECT
public:
	APIHelper(QString key);
	~APIHelper();


	void getOrgAdminData();


	// set
	void setApiKey(QString key);
	void setOrgQueryURL(QUrl u);
	void setNetworkQueryURL(QUrl u);


	// get
	QString getApiKey();
	QUrl getOrgQueryURL();
	QUrl getNetworkQueryURL();


public slots:
	void replyFinished(QNetworkReply *reply);

private:

	QString apiKey;		// holds the API key being used

	QNetworkAccessManager *manager;

	// TODO: think about a better arrangement for these, read from file?
	QUrl orgQueryURL;
	QUrl networkQueryURL;


};

#endif // APIHELPER_H
