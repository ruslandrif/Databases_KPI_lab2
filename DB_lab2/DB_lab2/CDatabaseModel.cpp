#include "CDatabaseModel.h"
#include <exception>
#include <iostream>
#include <locale>
#include <utility>
using namespace model;


CDatabaseModel::CDatabaseModel() {}

CDatabaseModel::CDatabaseModel(const std::string& username, const std::string& db, const std::string& passw)
	: m_dbName(db),m_password(passw),m_userName(username) {

}

std::string CDatabaseModel::GetLastError() const {
	return PQerrorMessage(m_connection);
}

void CDatabaseModel::SetUserName(const std::string& username) {
	m_userName = username;
}
void CDatabaseModel::SetPassword(const std::string& passw) {
	m_password = passw;
}
void CDatabaseModel::SetDbName(const std::string& db) {
	m_dbName = db;
}

std::string CDatabaseModel::userName() const {
	return m_userName;
}

std::string CDatabaseModel::dbName() const {
	return m_dbName;
}

std::string CDatabaseModel::password() const {
	return m_password;
}

void CDatabaseModel::connect() {
	//"user=postgres password= host=127.0.0.1 dbname=postgres"
	const std::string connection_string = 
		"user=" + m_userName + 
		" password=" + m_password + 
		" host = 127.0.0.1 dbname=" + m_dbName;

	m_connection = PQconnectdb(connection_string.c_str());

	//if(PQstatus(m_connection) != CONNECTION_OK) 
}

void CDatabaseModel::terminate() {
	throw std::runtime_error(GetLastError());
}

PGresult* CDatabaseModel::query(const std::string& query) {
	return PQexec(m_connection, query.c_str());
}