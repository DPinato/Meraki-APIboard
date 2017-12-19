#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QStandardItem>
#include <QStandardItemModel>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


#include "morganization.h"

class MOrganization;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	QString getAPIkeyFromFile(QString file);
	void updateUI();


	void processOrgQuery(QJsonDocument doc);
	void processNetworkQuery(QJsonDocument doc);



public slots:
	void replyFinished(QNetworkReply *reply);
	void runOrgQuery();
	void runNetworkQuery();

signals:
	void orgQueryFinished();

private:
	Ui::MainWindow *ui;

	QString apiKey;		// holds the API key being used

	QNetworkAccessManager *manager;

	// keep them here for now, these will most likely have to be in a separate class
	QUrl orgQueryURL;
	QUrl networkQueryURL;
	QString tmpNetURL;		// used for comparison with networkQueryURL


	// hold variables from "Organizations" API responses
	QVector<MOrganization *> orgList;

//	QVector<double> orgIDs;	// org IDs can be quite long, 15+ digits
//	QVector<QString> orgName;
//	QVector<QString> samlURL;
//	QVector<QList<QString>> samlURLs;


};

#endif // MAINWINDOW_H
