#pragma once
#include <libpq-fe.h>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
namespace model {

	using childTableRelation = std::pair<std::string, std::string>; //table name and relation field
	using parentTableRelation = std::pair<std::string, std::string>; //table name and relation field

	struct relation {
		parentTableRelation parent;
		childTableRelation child;
	};

	const std::vector<relation> relations = {
		{{"Project order","buyer id"},{"Buyer","id"}},
		{{"Project order","project id"},{"Project","id"}},
		{{"Development","project id"},{"Project","id"}},
		{{"Development","team id"},{"Team","id"}},
		{{"Employee","team id"},{"Team","id"}},
		{{"Employee","position id"},{"Position","id"}},
	};

	enum class dataTypes {
		text,
		integer,
		characterVarying,
		interval,
		unk
	};

	constexpr int randomDataCount = 100000;

	const std::unordered_map<std::string, dataTypes> dataTypesMap = {
		{"text",dataTypes::text},
		{"integer",dataTypes::integer},
		{"interval",dataTypes::interval},
		{"character varying",dataTypes::characterVarying},
	};

	class CDatabaseModel {
	public:
		CDatabaseModel();
		CDatabaseModel(const std::string& username, const std::string& db, const std::string& passw);

		PGresult* query(const std::string& query);

		std::string GetLastError() const;

		std::vector<const char*> tables() const;

		std::vector<const char*> columnsInTable(int tableIndex);

		std::vector<std::vector<const char*>> rowsInTable(int tableIndex);

		std::vector<const char*> dataTyperInTable(int tableIndex);

		std::vector<std::vector<std::string>> getTuples(PGresult* res);

		std::string getTablePrimaryKey(int tableIndex);

		void SetUserName(const std::string& username);
		void SetPassword(const std::string& passw);
		void SetDbName(const std::string& db);

		std::string userName() const;
		std::string dbName() const;
		std::string password() const;
	private:

		std::vector<const char*> m_tables;

		std::vector<const char*> retrieveTables(PGresult *res);

		void connect();

		void terminate();

		PGconn* m_connection = nullptr;

		std::string m_lastError;

		std::string m_userName;
		std::string m_dbName;
		std::string m_password;
	};
}

