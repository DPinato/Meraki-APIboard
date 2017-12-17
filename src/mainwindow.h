#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QList>
#include <QByteArray>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


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



public slots:
	void replyFinished(QNetworkReply *reply);


private:
	Ui::MainWindow *ui;

	QString apiKey;		// holds the API key being used

	QNetworkAccessManager *manager;
	QJsonDocument jDoc;


	// hold variables from "Organizations" API responses
	QVector<double> orgIDs;	// org IDs can be quite long, 15+ digits
	QVector<QString> orgName;
	QVector<QString> samlURL;
	QVector<QList<QString>> samlURLs;


};

#endif // MAINWINDOW_H
