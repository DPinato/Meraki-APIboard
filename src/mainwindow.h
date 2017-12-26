#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


#include "morganization.h"
#include "apihelper.h"

class MOrganization;
class APIHelper;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	QString getAPIkeyFromFile(QString file);
	void updateUI();		// update the tree view of the organization/network
	void updateNetworkUI(QModelIndex &index);




	// TODO: I am sure there is a better arrangement than this
	QVector<MOrganization *> orgList;


public slots:
	void replyFinished(QNetworkReply *reply);

signals:

private slots:
	void on_treeView_doubleClicked(const QModelIndex &index);
	void on_snmpCheck_clicked(bool checked);
	void on_nonMVPNCheck_clicked(bool checked);


private:
	Ui::MainWindow *ui;

	QString apiKey;		// holds the API key being used
	QNetworkAccessManager *manager;


	APIHelper *apiHelpObj;

	// keep them here for now, these will most likely have to be in a separate class
	QUrl orgQueryURL;
	QUrl networkQueryURL;
	QString tmpNetURL;		// used for comparison with networkQueryURL




//	QVector<double> orgIDs;	// org IDs can be quite long, 15+ digits
//	QVector<QString> orgName;
//	QVector<QString> samlURL;
//	QVector<QList<QString>> samlURLs;


};

#endif // MAINWINDOW_H
