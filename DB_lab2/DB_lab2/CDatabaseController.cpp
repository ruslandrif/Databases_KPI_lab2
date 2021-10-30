#include "CDatabaseModel.h"
#include "CDatabaseController.h"
#include "CDatabaseView.h"
#include <boost/format.hpp>
#include <iostream>
using namespace controller;

userAction CDatabaseController::requestAction() {
	using std::cout;

	cout << "What do you want to do?\n";
	
	cout << "1.Insert data in the database\n";
	cout << "2.Remove data from database\n";
	cout << "3.Edit data in the database\n";
	cout << "4.Generate random data in the database\n";
	cout << "5.Search data in the database\n";
	cout << "Enter your choice:";

	int choose = -1;

	std::cin >> choose;

	constexpr int max_userAction_val = 6;
	constexpr int min_userAction_val = 1;

	if (choose > max_userAction_val && choose < min_userAction_val)
		return userAction::unknown;

	return static_cast<userAction>(choose);
}

void CDatabaseController::performAction(userAction ua) {

	int tableIndex = chooseTableForAction(ua);

	int tableRow = -1;

	//int tableIndex = tableNum - 1;

	switch (ua) {
	case userAction::edit:
		tableRow = chooseRowInTable(tableIndex);
		performEdit(tableIndex, tableRow);
		//std::cout << "chosen edit" << std::endl;
		break;
	case userAction::remove:
	    tableRow = chooseRowInTable(tableIndex);
		performRemove(tableIndex, tableRow);
		//std::cout << "chosen remove" << std::endl;
		break;
	case userAction::insert:
		performInsert(tableIndex);
		//std::cout << "chosen insert" << std::endl;
		break;
	case userAction::generateRandomData:
		performGeneratingRandomData(tableIndex);
		//std::cout << "chosen generateRandomData" << std::endl;
		break;
	case userAction::search:
		performSearch(tableIndex);
		//std::cout << "chosen search" << std::endl;
		break;
	case userAction::unknown:
	default:
		//std::cout << "chosen unk" << std::endl;

		break;
	}
}

bool CDatabaseController::performInsert(int tableIndex) {

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

	std::string userInputStr;

	for (auto input : userData) {
		userInputStr += std::string(input) + ",";
	}

	userInputStr.pop_back();
	userInputStr.push_back(')');
	userInputStr.insert(0, 1, '(');
	std::cout << userInputStr;

	std::string insertQuery = (boost::format(
		"INSERT INTO \"%s\"\n"
		"VALUES %s"
	) % m_model->tables()[tableIndex] % userInputStr).str();

	auto* insertQueryRes = m_model->query(insertQuery.c_str());

	if (PQresultStatus(insertQueryRes) != PGRES_COMMAND_OK)
		std::wcout << reinterpret_cast<const wchar_t*>(m_model->GetLastError().data());

	return false;
}

bool CDatabaseController::performRemove(int tableIndex, int rowIndex) {
	return false;
}

bool CDatabaseController::performEdit(int tableIndex, int rowIndex) {

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

	m_view->print(userAction::edit, res);
	return false;
}

bool CDatabaseController::performGeneratingRandomData(int tableIndex) {

	auto types = m_model->dataTyperInTable(tableIndex);

	auto cols = m_model->columnsInTable(tableIndex);

	for (int i = 0; i < cols.size(); ++i) {
		std::cout << cols[i] << ": " << types[i] << std::endl;
	}

	return false;
}

bool CDatabaseController::performSearch(int tableIndex) {
	return false;
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

	std::cout << "Choose row in table:\n";
	for (auto& col : cols)
		std::cout << "\t" << col;
	std::cout << std::endl;

	for (int i = 0; i < rows.size(); ++i) {
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
	std::cout << "Enter value for column " << colName;
	if (canBeLeaved) {
		std::cout << " or Enter * to leave it in current state: ";
	}
	std::string userInput;

	std::getline(std::cin, userInput);

	if (canBeLeaved && userInput == "*")
		return userInput;

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