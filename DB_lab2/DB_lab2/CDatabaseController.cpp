#include "CDatabaseModel.h"
#include "CDatabaseController.h"
#include "CDatabaseView.h"
#include <boost/format.hpp>
#include <iostream>
#include <iomanip>
#include <regex>
using namespace controller;

userAction CDatabaseController::requestAction() {
	using std::cout;

	cout << "What do you want to do?\n";
	
	cout << "1.Insert data in the database\n";
	cout << "2.Remove data from database\n";
	cout << "3.Edit data in the database\n";
	cout << "4.Generate random data in the database\n";
	cout << "5.Search data in the database\n";
	cout << "6.Print data in the database\n";
	cout << "Enter your choice:";

	int choose = -1;

	std::cin >> choose;

	constexpr int max_userAction_val = 7;
	constexpr int min_userAction_val = 1;

	if (choose > max_userAction_val && choose < min_userAction_val)
		return userAction::unknown;

	return static_cast<userAction>(choose);
}

void CDatabaseController::performAction(userAction ua) {

	int tableIndex = -1;

	int tableRow = -1;

	//int tableIndex = tableNum - 1;

	switch (ua) {
	case userAction::edit:
		tableIndex = chooseTableForAction(ua);
		tableRow = chooseRowInTable(tableIndex);
		m_view->printOperationResult(ua,performEdit(tableIndex, tableRow));
		//std::cout << "chosen edit" << std::endl;
		break;
	case userAction::remove:
		tableIndex = chooseTableForAction(ua);
	    tableRow = chooseRowInTable(tableIndex);
		m_view->printOperationResult(ua, performRemove(tableIndex, tableRow));
		//std::cout << "chosen remove" << std::endl;
		break;
	case userAction::insert:
		tableIndex = chooseTableForAction(ua);
		m_view->printOperationResult(ua, performInsert(tableIndex));
		//std::cout << "chosen insert" << std::endl;
		break;
	case userAction::generateRandomData:
		tableIndex = chooseTableForAction(ua);
		m_view->printOperationResult(ua, performGeneratingRandomData(tableIndex));
		m_view->printDB(tableIndex);
		//std::cout << "chosen generateRandomData" << std::endl;
		break;
	case userAction::search:
		m_view->printOperationResult(ua, performSearch());
		//std::cout << "chosen search" << std::endl;
		break;
	case userAction::print:
		m_view->printOperationResult(ua,nullptr);
		//std::cout << "chosen search" << std::endl;
		break;
	case userAction::unknown:
	default:
		//std::cout << "chosen unk" << std::endl;

		break;
	}
}

PGresult* CDatabaseController::performInsert(int tableIndex) {

	auto cols = m_model->columnsInTable(tableIndex);
	auto types = m_model->dataTyperInTable(tableIndex);
	

	std::vector<std::string> userData;
	int indx = 0;
	for (auto col : cols) {
		auto key = model::dataTypesMap.find(types[indx++]);
		if (key != model::dataTypesMap.end()) {
			userData.push_back(requestData(col, key->second));
		}
		else
			std::cout << "key is end!\n";
	}

	for (int i = 0; i < userData.size(); ++i) {
		const bool needParentheses =
			typeFromString(types[i]) == model::dataTypes::interval ||
			typeFromString(types[i]) == model::dataTypes::characterVarying ||
			typeFromString(types[i]) == model::dataTypes::text;

		if (needParentheses) {
			userData[i] = "'" + userData[i] + "'";
		}
	}

	std::string userInputStr;

	for (auto input : userData) {
		userInputStr += std::string(input) + ",";
	}

	userInputStr.pop_back();
	userInputStr.push_back(')');
	userInputStr.insert(0, 1, '(');
	

	std::string insertQuery = (boost::format(
		"INSERT INTO \"%s\"\n"
		"VALUES %s"
	) % m_model->tables()[tableIndex] % userInputStr).str();
	auto* insertQueryRes = m_model->query(insertQuery.c_str());

	if (PQresultStatus(insertQueryRes) != PGRES_COMMAND_OK)
		std::wcout << reinterpret_cast<const wchar_t*>(m_model->GetLastError().data());

	return insertQueryRes;
}

PGresult* CDatabaseController::performRemove(int tableIndex, int rowIndex) {
	auto tables = m_model->tables();
	auto cols = m_model->columnsInTable(tableIndex);
	auto rows = m_model->rowsInTable(tableIndex);

	auto pKey = m_model->getTablePrimaryKey(tableIndex);

	auto pKeyIter = std::find_if(cols.begin(), cols.end(), [&](const char* col) {
		return std::string(col) == pKey; 
	});

	const int pKeyIndex = pKeyIter - cols.begin();

	std::string removeQuery = (boost::format(
		"DELETE FROM public.\"%s\"\n"
		"WHERE \"%s\" = %s"
	) % tables[tableIndex] % cols[pKeyIndex] % rows[rowIndex][pKeyIndex]).str();

	for(auto it = model::relations.begin();it != model::relations.end();++it)
	if (it->child.first == tables[tableIndex]) {
		

		auto parentTableIndex = std::find_if(tables.begin(), tables.end(), [&](const char* t) {
			return std::string(t) == it->parent.first; 
		}) - tables.begin();

		auto parentTableCols = m_model->columnsInTable(parentTableIndex);
		auto parentFieldIndex = std::find_if(parentTableCols.begin(), parentTableCols.end(), [&](const char* col) {
			return std::string(col) == it->parent.second;
		}) - parentTableCols.begin();

		auto childFieldIndex = std::find_if(cols.begin(), cols.end(), [&](const char* col) {
			return std::string(col) == it->child.second;
		}) - cols.begin();

		auto parentRows = m_model->rowsInTable(parentTableIndex);
		
		bool found = false;
		int indx = 0;
		for (auto& r : parentRows) {
			if (std::string(r[parentFieldIndex]) == std::string(rows[rowIndex][childFieldIndex])) {
				found = true;

				performRemove(parentTableIndex, indx);
				//break;
			}
			indx++;
		}
	}

	return m_model->query(removeQuery);
}

PGresult* CDatabaseController::performEdit(int tableIndex, int rowIndex) {

	auto cols = m_model->columnsInTable(tableIndex);
	auto types = m_model->dataTyperInTable(tableIndex);

	auto row = m_model->rowsInTable(tableIndex)[rowIndex];

	std::vector<std::string> userInput;
	std::string input;
	std::getline(std::cin, input);
	for (int i = 0; i < cols.size(); ++i) {

		auto data = requestData(cols[i], typeFromString(types[i]), true);
		
		if (data == "*")
			data = row[i];
		userInput.push_back(data);
	}

	std::string updateQueryStr = (boost::format("UPDATE public.\"%s\" SET ") % m_model->tables()[tableIndex]).str();

	for (int i = 0; i < userInput.size();++i) {
		const bool needParentheses = 
			typeFromString(types[i]) == model::dataTypes::interval ||
			typeFromString(types[i]) == model::dataTypes::characterVarying ||
			typeFromString(types[i]) == model::dataTypes::text;

		const std::string updateVarstr = "\"" + (std::string(cols[i]) + "\"" + " = " + 
			(needParentheses ? 
				"'" + userInput[i] + "'" : 
				userInput[i]) + ", ");
		updateQueryStr += updateVarstr;
	}

	updateQueryStr.pop_back();
	updateQueryStr.pop_back();
	updateQueryStr += "\nWHERE ";

	for (int i = 0; i < userInput.size(); ++i) {
		const bool needParentheses =
			typeFromString(types[i]) == model::dataTypes::interval ||
			typeFromString(types[i]) == model::dataTypes::characterVarying ||
			typeFromString(types[i]) == model::dataTypes::text;
		const std::string currVarStr = "\"" + (std::string(cols[i]) + "\"" + " = " + (needParentheses ? "'" + std::string(row[i]) + "'" : row[i]) + (i == userInput.size() - 1 ? "" : " AND "));
		updateQueryStr += currVarStr;
	}
	std::cout << updateQueryStr << std::endl;

	auto res = m_model->query(updateQueryStr);

	
	return res;
}

PGresult* CDatabaseController::performGeneratingRandomData(int tableIndex) {

	auto randomStrByType = [&](model::dataTypes dt) -> std::string {
		//std::cout << "type: " << static_cast<int>(dt) << std::endl;
		using namespace model;
		switch (dt) {
		case dataTypes::characterVarying:
			return "substr(md5(random()::text), 1, 9)";
		case dataTypes::integer:
			return "(random()*10000)::integer";
		case dataTypes::interval:
			return "INTERVAL '00:00:00' * (random()*10)::integer";
		case dataTypes::text:
			return "substr(md5(random()::text), 0, 15)";
		case dataTypes::unk:
			return "";
		}
	};

	auto types = m_model->dataTyperInTable(tableIndex);

	auto cols = m_model->columnsInTable(tableIndex);

	auto tables = m_model->tables();

	std::vector<std::string> generatingStrings;

	for (auto& t : types) {
		auto type = typeFromString(t);

		generatingStrings.push_back(randomStrByType(type));
	}

	std::string generateQuery = "INSERT INTO public.\"" + std::string(tables[tableIndex]) + "\"(";
	for (auto& col : cols)
		generateQuery += "\"" + std::string(col) + "\",";

	generateQuery.pop_back();
	generateQuery += ")";
	generateQuery += "\nSELECT";

	for (auto& randStr : generatingStrings)
		generateQuery += "\n" + randStr + ",";
	generateQuery.pop_back();
	generateQuery += (boost::format("\nFROM generate_series(1,%d)\nON CONFLICT DO NOTHING") % model::randomDataCount).str();
	return m_model->query(generateQuery);
}

PGresult* CDatabaseController::performSearch() {

	std::cout << "Choose what to search:\n";
	int count = 0;
	for (auto& pair : searchModeMap) {
		std::cout << ++count << "." << pair.second << std::endl;
	}
	std::cout << "Your choice:";
	int choose = -1;
	std::cin >> choose;

	constexpr int max_searchMode_val = 3;
	constexpr int min_searchMode_val = 1;

	if (choose < min_searchMode_val || choose > max_searchMode_val) {
		std::cout << "wrong input!";
		return nullptr;
	}

	auto mode = static_cast<searchType>(choose - 1);

	const int cnt = GetCountElemsForSearch(mode);

	std::unordered_map<int, int> searchMap;
	auto tables = m_model->tables();
	PGresult* res = nullptr;
	switch (mode) {
	case searchType::employeesBySalary:
		res = searchEmployees(cnt);
		break;
	case searchType::projectsByBuyers:
		res = searchProjects(cnt);
		break;
	case searchType::teamsByEmployeesCount:
		res = searchTeams(cnt);
		break;
	default:
		break;
	}
	return res;
}

int CDatabaseController::chooseTableForAction(userAction ua) {
	using std::cout;

	cout << "Choose table to " << userActionMap.at(ua) << std::endl;
	auto tables = m_model->tables();
	for (int i = 0; i < tables.size(); ++i)
		cout << i + 1 << "." << tables[i] << std::endl;
	cout << "Enter your choice:";
	
	int res = -1;
	std::cin >> res;

	return (res >= 1 && res <= tables.size()) ? res - 1 : -1;
}

CDatabaseController::CDatabaseController(std::shared_ptr<model::CDatabaseModel> model, std::shared_ptr<view::CDatabaseView> view) 
: m_model(model), m_view(view) 
{

}

int CDatabaseController::chooseColumnInTable(int tableNum) {
	auto tables = m_model->tables();

	auto cols = m_model->columnsInTable(tableNum);

	std::cout << "Choose column number in table " << tables[tableNum] << std::endl;
	for (int i = 0; i < cols.size(); ++i) {
		std::cout << i + 1 << "." << cols[i] << std::endl;
	}

	std::cout << "Enter number:";
	int choose = -1;
	std::cin >> choose;

	return (choose >= 1 && choose <= cols.size() ? choose - 1 : -1);
}

int CDatabaseController::chooseRowInTable(int tableIndex) {

	auto rows = m_model->rowsInTable(tableIndex);
	auto cols = m_model->columnsInTable(tableIndex);

	if (rows.empty())
		return -1;



	std::cout << "Choose row in table.\n";
	std::cout 
		<< "Choose rows to show:(1 option by default)\n"
		<< "1.All rows\n"
		<< "2.First 15 rows\n"
		<< "3.Last 15 rows\n";
	int showChoose = -1;
	std::cout << "Your choice:";
	std::cin >> showChoose;
	if (showChoose < 1 || showChoose > 3 || rows.size() <= 15)
		showChoose = 1;
	int left = -1, right = -1;
	switch (showChoose) {
	case 1:
		left = 0;
		right = rows.size();
		break;
	case 2:
		left = 0;
		right = 15;
		break;
	case 3:
		left = 15;
		right = rows.size();
		break;
	}

	for (int i = left; i < right; ++i) {
		std::cout << i + 1 << ".";

		for (int j = 0; j < rows[i].size(); ++j) {
			std::cout << "\t" << rows[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << "Your choice:";
	int choose = -1;

	std::cin >> choose;

	return choose >= 1 && choose <= rows.size() ? choose - 1 : -1;
}

std::string CDatabaseController::requestData(const char* colName, model::dataTypes type, bool canBeLeaved) {
	std::string userInput;
	bool dataValid = true;
	do {
		if (!dataValid) {
			std::cout << "Invalid data!\n";
		}
		std::cout << "Enter value for column " << colName << " ";
		if (canBeLeaved) {
			std::cout << " or Enter * to leave it in current state: ";
		}
		

		std::cin >> userInput;
		if (canBeLeaved && userInput == "*")
			return userInput;

		dataValid = false;
	} while (!checkValidInput(userInput,type));
	switch (type) {
	case model::dataTypes::characterVarying:
		return userInput;
	case model::dataTypes::integer:
		return (userInput);
	case model::dataTypes::interval:
		//std::cout << "\nInput format: hh:mm:ss";
		return userInput;
	case model::dataTypes::text:
		return userInput;
	default:
		return "";
	}
	return "";
}

model::dataTypes CDatabaseController::typeFromString(const char* str) {
	auto key = model::dataTypesMap.find(std::string(str));

	if (key != model::dataTypesMap.end())
		return key->second;

	return model::dataTypes::unk;
}


int CDatabaseController::GetCountElemsForSearch(searchType st) {
	int count = 0;
	switch (st) {
	case searchType::employeesBySalary:
		std::cout << "Enter salary:";
		break;
	case searchType::projectsByBuyers:
		std::cout << "Enter count of buyers:";
		break;
	case searchType::teamsByEmployeesCount:
		std::cout << "Enter count of employees:";
		break;
	default:
		break;
	}

	std::cin >> count;

	return count;
}



PGresult* CDatabaseController::searchEmployees(int salary) {
	PGresult* res = nullptr;

	auto tables = m_model->tables();

	auto iterEmployees = std::find_if(tables.begin(), tables.end(), [this](const char* tableName) {
		return std::string(tableName) == "Employee";
	});

	auto iterPositions = std::find_if(tables.begin(), tables.end(), [this](const char* tableName) {
		return std::string(tableName) == "Position";
	});

	auto positionCols = m_model->columnsInTable(iterPositions - tables.begin());
	auto employeesCols = m_model->columnsInTable(iterEmployees - tables.begin());

	std::string PositionsQuery = "SELECT ";
	for (auto& i : positionCols)
		PositionsQuery += "\"" + std::string(i) + "\",";
	PositionsQuery.pop_back();
	PositionsQuery += "\nFROM public.\"Position\"\n";
	PositionsQuery += (boost::format("WHERE \"salary\" < %d") % salary).str();

	res = m_model->query(PositionsQuery);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		std::cout << "Error search positions!";
		return res;
	}

	std::cout << "Search employees result:\n";
	for (auto& col : employeesCols) {
		std::cout << std::setw(20) << col;
	}
	std::cout << std::endl;

	auto tpls = m_model->getTuples(res);

	std::vector<int> neededPositionsId;
	for (auto& tuple : tpls) {
		neededPositionsId.push_back(std::stoi(tuple[0]));
	}

	std::string employeesQuery = "SELECT ";
	for (auto& i : employeesCols)
		employeesQuery += "\"" + std::string(i) + "\",";

	employeesQuery.pop_back();
	employeesQuery += "\nFROM public.\"Employee\"\n";
	employeesQuery += "WHERE \"id\" = " + std::to_string(neededPositionsId[0]);
	for (int i = 1; i < neededPositionsId.size(); ++i) {
		employeesQuery += " OR \"id\"= " + std::to_string(neededPositionsId[i]);
	}

	res = m_model->query(employeesQuery);
	
	return res;
}

PGresult* CDatabaseController::searchTeams(int employeesCount) {
	PGresult* res = nullptr;

	auto tables = m_model->tables();

	auto iterEmployees = std::find_if(tables.begin(), tables.end(), [this](const char* tableName) {
		return std::string(tableName) == "Employee";
	});

	auto iterTeams = std::find_if(tables.begin(), tables.end(), [this](const char* tableName) {
		return std::string(tableName) == "Team";
	});

	using team_id = int;
	std::unordered_map<team_id, int> teamEmployees;

	auto rows = m_model->rowsInTable(iterEmployees - tables.begin());

	constexpr int teamIdIndex = 2;

	for (auto& row : rows) {
		teamEmployees[std::stoi(row[teamIdIndex])]++;
	}

	for (auto& pair : teamEmployees) {
		std::cout << "team id: " << pair.first << " count: " << pair.second << std::endl;
	}

	std::string teamsArray = "ANY(ARRAY[";
	for (auto& pair : teamEmployees) {
		if (pair.second < employeesCount)
			teamsArray += std::to_string(pair.first) + ",";
	}
	teamsArray.pop_back();
	teamsArray += "])";

	std::string teamsQuery = "SELECT * FROM public.\"Team\" WHERE \"id\" = " + teamsArray;

	//std::cout << teamsQuery << std::endl;

	res = m_model->query(teamsQuery);

	return res;
}

PGresult* CDatabaseController::searchProjects(int buyersCount) {
	PGresult* res = nullptr;


	const auto tables = m_model->tables();

	const auto iterProjectOrders = std::find_if(tables.begin(), tables.end(), [this](const char* tableName) {
		return std::string(tableName) == "Project order";
	});

	const auto iterProjects = std::find_if(tables.begin(), tables.end(), [this](const char* tableName) {
		return std::string(tableName) == "Project";
	});

	auto projectOrderCols = m_model->columnsInTable(iterProjectOrders - tables.begin());

	constexpr int idIndexProject = 0;

	std::unordered_map<int, int> buyersForProject;
	std::vector<std::pair<int, int>> usedPairs;

	auto rowsOrders = m_model->rowsInTable(iterProjectOrders - tables.begin());
	auto rowsProjects = m_model->rowsInTable(iterProjects - tables.begin());

	for (auto& proj : rowsProjects) {
		const int id = std::stoi(proj[0]);
		buyersForProject[id] = 0;
		for (auto& order : rowsOrders) {
			const int currProjectId = std::stoi(order[3]);

			if (currProjectId == id) {
				const int currBuyerId = std::stoi(order[1]);
				if (std::find(usedPairs.begin(), usedPairs.end(), std::pair<int,int>(currProjectId,currBuyerId)) == usedPairs.end()) {
					buyersForProject[currProjectId]++;
					usedPairs.push_back(std::pair<int, int>(currProjectId, currBuyerId));
				}
			}
		}
	}

	std::string projectsArray = "ANY(ARRAY[";
	for (auto& pair : buyersForProject) {
		if (pair.second < buyersCount)
			projectsArray += std::to_string(pair.first) + ",";
	}
	projectsArray.pop_back();
	projectsArray += "])";

	std::string projectsQuery = "SELECT * FROM public.\"Project\" WHERE \"id\" = " + projectsArray;

	res = m_model->query(projectsQuery);

	return res;
}

bool CDatabaseController::checkValidInput(const std::string& input, model::dataTypes dataType) {
	using namespace model;
	const std::regex timeRegex("([01]?[0-9]|2[0-3]):[0-5][0-9]");
	switch (dataType) {
	case dataTypes::characterVarying:
		return input.size() == 9; //in that database, the only field which is character varying is passport number, it must have 9 characters
	case dataTypes::integer:
		for (auto& c : input) {
			if (!std::isdigit(c))
				return false;
		}
		return true;
	case dataTypes::interval:
		return std::regex_match(input, timeRegex);
	case dataTypes::text:
		return true;
	case dataTypes::unk:
		return false;
	}
}