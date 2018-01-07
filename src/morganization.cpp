#include "morganization.h"
#include "morganization.h"


MOrganization::MOrganization() {
	samlURL = QString("");
}

MOrganization::MOrganization(QString orgName, QString orgId) {
	id = orgId;
	name = orgName;
	samlURL = QString("");
}

MOrganization::~MOrganization() {
}

void MOrganization::setOrgID(QString orgId) {
	id = orgId;
}

void MOrganization::setOrgName(QString orgName) {
	name = orgName;
}

void MOrganization::setSamlURL(QString url) {
	samlURL = url;
}

void MOrganization::setNetworksNum(int n) {
	netList.resize(n);
}

void MOrganization::setNetwork(networkVars v, int index) {
	netList[index] = v;
}

void MOrganization::setLicenseStatus(QString s) {
	licStatus = s;
}

void MOrganization::setLicenseExpDate(QString d) {
	licExpireDate = d;
}

void MOrganization::setLicenseDeviceNum(int n) {
	licenseList.resize(n);
}

void MOrganization::setLicensePerDevice(licensesPerDevice a, int index) {
	licenseList[index] = a;
}

void MOrganization::setAdminsNum(int n) {
	adminList.resize(n);
}

void MOrganization::setAdmin(adminStruct a, int index) {
	adminList[index] = a;
}

QString MOrganization::getOrgID() {
	return id;
}

QString MOrganization::getOrgName() {
	return name;
}

QString MOrganization::getSamlURL() {
	return samlURL;
}

int MOrganization::getNetworksNum() {
	return netList.size();
}

networkVars MOrganization::getNetwork(int index) {
	return netList.at(index);
}

QString MOrganization::getLicenseStatus() {
	return licStatus;
}

QString MOrganization::getLicenseExpireDate() {
	return licExpireDate;
}

int MOrganization::getLicenseListSize() {
	return licenseList.size();
}

licensesPerDevice MOrganization::getLicensePerDevice(int index) {
	return licenseList.at(index);
}

int MOrganization::getAdminListSize() {
	return adminList.size();
}

adminStruct MOrganization::getAdmin(int index) {
	return adminList.at(index);
}





void MOrganization::showVariables() {
	// debug function, just show all the variables in the class
	qDebug() << "Org ID: " << id;
	qDebug() << "Org name: " << name;
	qDebug() << "SAML URL: " << samlURL;

	qDebug() << "SAML URLs: ";
	for (int i = 0; i < samlURLs.size(); i++) {
		qDebug() << "\t" << samlURLs.at(i);
	}


	// show networks
	qDebug() << "Networks in this organization: " << netList.size();
	for (int i = 0; i < netList.size(); i++) {
		qDebug() << "\tName: " << netList.at(i).netName << "\ttype: " << netList.at(i).netType;
		qDebug() << "\tnetID: " << netList.at(i).netID << "\torgID: " << netList.at(i).orgID;
		qDebug() << "\ttimezone: " << netList.at(i).netTimezone;
		qDebug() << "\ttags: " << netList.at(i).netTags;
	}

}


