#pragma once
#include <string>
#include <libpq-fe.h>
#include <memory>
#include <unordered_map>
namespace model {
	class CDatabaseModel;
}

namespace view {
	class CDatabaseView;
}

namespace controller {

	enum class searchType {
		employeesBySalary,
		projectsByBuyers,
		teamsByEmployeesCount,
		unknown
	};

	const std::unordered_map<searchType, std::string> searchModeMap = {
		{searchType::employeesBySalary,"Search employees,which salary is less than N"},
		{searchType::projectsByBuyers,"Search projects, which has less than N buyers"},
		{searchType::teamsByEmployeesCount,"Search teams,which has less than N employees"},
	};

	enum class userAction : int {
		insert = 1,
		remove,
		edit,
		generateRandomData,
		search,
		unknown
	};

	const std::unordered_map<userAction, std::string> userActionMap = {
		{userAction::edit,"edit data in database"},
		{userAction::remove,"remove data from database"},
		{userAction::insert,"insert data in database"},
		{userAction::generateRandomData,"insert big amount of random data in database"},
		{userAction::search,"search data in database"},
		{userAction::unknown,"UNKNOWN_ACTION"}
	};

	class CDatabaseController {
	public:
		CDatabaseController(std::shared_ptr<model::CDatabaseModel> model, std::shared_ptr<view::CDatabaseView> view);
	
		userAction requestAction();

		void performAction(userAction ua);
	private:

		int GetCountElemsForSearch(searchType st);

		int chooseTableForAction(userAction ua);

		int chooseColumnInTable(int tableNum);

		int chooseRowInTable(int tableIndex);

		PGresult* searchEmployees(int salary);
		PGresult* searchTeams(int employeesCount);
		PGresult* searchProjects(int buyersCount);

		std::string requestData(const char* colName, model::dataTypes type,bool canBeLeaved = false);

		model::dataTypes typeFromString(const char* str);

		PGresult* performInsert(int tableIndex);
		PGresult* performRemove(int tableIndex, int rowIndex);
		PGresult* performEdit(int tableIndex, int rowIndex);
		PGresult* performGeneratingRandomData(int tableIndex);
		PGresult* performSearch();

		std::shared_ptr<model::CDatabaseModel> m_model;
		std::shared_ptr<view::CDatabaseView> m_view;
	};
}

