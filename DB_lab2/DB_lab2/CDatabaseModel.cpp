#include "CDatabaseModel.h"
#include <exception>
#include <iostream>
#include <locale>
#include <utility>
#include <boost/format.hpp>
using namespace model;

CDatabaseModel::CDatabaseModel() {}

CDatabaseModel::CDatabaseModel(const std::string& username, const std::string& db, const std::string& passw)
	: m_dbName(db),m_password(passw),m_userName(username) {

	connect();

	const char* querySearchTables = "SELECT table_name\n"
									"FROM information_schema.tables\n"
									"WHERE table_type = 'BASE TABLE'\n"
									"AND table_schema = 'public';\n";

	auto tablesQueryResult = query(querySearchTables);

	if (PQresultStatus(tablesQueryResult) != PGRES_TUPLES_OK) {
		auto errMsg = std::string(PQresultErrorMessage(tablesQueryResult));
		std::cout << "Select failed: " << errMsg << std::endl;
	}
	else
		m_tables = retrieveTables(tablesQueryResult);
}

std::string CDatabaseModel::GetLastError() const {

	std::ofstream out("1.txt", std::fstream::out);

	auto msg = PQerrorMessage(m_connection);

	out << msg;

	return msg;
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
		" host = 127.0.0.1 dbname=" + m_dbName +
		" client_encoding = LATIN1";

	m_connection = PQconnectdb(connection_string.c_str());
	if (m_connection == nullptr) {
		terminate();
	}
}

std::vector<const char*> CDatabaseModel::retrieveTables(PGresult* res) {
	std::vector<const char*> resVec;

	for (int i = 0; i < PQntuples(res); i++) {
		for (int j = 0; j < PQnfields(res); j++) {
			auto name = PQgetvalue(res, i, j);
			resVec.push_back(name);
		}
	}

	return resVec;
}

void CDatabaseModel::terminate() {
	throw std::runtime_error(GetLastError());
}

PGresult* CDatabaseModel::query(const std::string& query) {
	return PQexec(m_connection, query.c_str());
}

std::vector<const char*> CDatabaseModel::tables() const {
	return m_tables;
}

std::vector<const char*>  CDatabaseModel::columnsInTable(int tableIndex) {
	std::vector<const char*> res;
	std::string columnsQuery = (boost::format(
		"SELECT column_name\n"
		"FROM information_schema.columns\n"
		"WHERE table_schema = '%s'\n"
		"AND table_name = '%s';") % "public" % m_tables[tableIndex]).str();
		
	auto columnsQueryResult = query(columnsQuery);

	for (int i = 0; i < PQntuples(columnsQueryResult); i++) {
		for (int j = 0; j < PQnfields(columnsQueryResult); j++) {
			auto name = PQgetvalue(columnsQueryResult, i, j);
			res.push_back(name);
		}
	}

	return res;
}

std::vector<const char*> CDatabaseModel::dataTyperInTable(int tableIndex) {
	std::vector<const char*> res;

	std::string dataTyperQuery = (boost::format(
		"SELECT data_type\n"
		"FROM   information_schema.columns\n"
		"WHERE table_schema = 'public'\n"
		"AND  table_name = '%s'") % m_tables[tableIndex]).str();

	auto dataTypesQueryResult = query(dataTyperQuery.c_str());

	for (int i = 0; i < PQntuples(dataTypesQueryResult); i++) {
		for (int j = 0; j < PQnfields(dataTypesQueryResult); j++) {
			auto name = PQgetvalue(dataTypesQueryResult, i, j);
			res.push_back(name);
		}
	}

	//std::string charlength = (boost::format(
	//	"select character_maximum_length\n"
	//	"from INFORMATION_SCHEMA.COLUMNS\n"
	//	"WHERE table_schema = 'public'\n"
	//	"AND  table_name = '%s'") % m_tables[tableIndex]).str();

	//dataTypesQueryResult = query(dataTyperQuery.c_str());

	for (int i = 0; i < PQntuples(dataTypesQueryResult); i++) {
		for (int j = 0; j < PQnfields(dataTypesQueryResult); j++) {
			auto name = PQgetvalue(dataTypesQueryResult, i, j);
			//std::cout << name << std::endl;
		}
	}

	return res;
}

std::vector<std::vector<const char*>> CDatabaseModel::rowsInTable(int tableIndex) {
	std::vector<std::vector<const char*>> res;

	std::string dataQuery = (boost::format(
		"select *\n"
		"from \"%s\";"
	) % m_tables[tableIndex]).str();


	auto* queryRes = query(dataQuery.c_str());

	std::cout << dataQuery << std::endl;

	if (PQresultStatus(queryRes) != PGRES_TUPLES_OK)
		std::cout << GetLastError() << "-";

	for (int i = 0; i < PQntuples(queryRes); i++) {
		res.push_back(std::vector<const char*>(PQnfields(queryRes)));
		for (int j = 0; j < PQnfields(queryRes); j++) {
			auto name = PQgetvalue(queryRes, i, j);
			//std::cout << name << " ";
			res[i][j] = name;
		}
		std::cout << std::endl;
	}

	return res;
}

std::vector<std::vector<std::string>> CDatabaseModel::getTuples(PGresult* res) {
	std::vector<std::vector<std::string>> resVec;

	for (int i = 0; i < PQntuples(res); i++) {
		resVec.push_back(std::vector<std::string>(PQnfields(res)));
		for (int j = 0; j < PQnfields(res); j++) {
			auto name = PQgetvalue(res, i, j);
			//std::cout << name << " ";
			resVec[i][j] = name;
		}

	}

	return resVec;
}