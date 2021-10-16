#pragma once
#include <libpq-fe.h>
#include <string>
namespace model {
	class CDatabaseModel {
	public:
		CDatabaseModel();
		CDatabaseModel(const std::string& username, const std::string& db, const std::string& passw);

		PGresult* query(const std::string& query);

		std::string GetLastError() const;

		void SetUserName(const std::string& username);
		void SetPassword(const std::string& passw);
		void SetDbName(const std::string& db);

		std::string userName() const;
		std::string dbName() const;
		std::string password() const;
	private:

		void connect();

		void terminate();

		PGconn* m_connection = nullptr;

		std::string m_lastError;

		std::string m_userName;
		std::string m_dbName;
		std::string m_password;
	};
}

