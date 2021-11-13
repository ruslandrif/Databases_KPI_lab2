#include "CDatabaseView.h"
#include "CDatabaseModel.h"
#include "CDatabaseController.h"
#include <iostream>
#include<iomanip>
using namespace view;

CDatabaseView::CDatabaseView(std::shared_ptr<model::CDatabaseModel> model)
	: m_model(model) {}

void CDatabaseView::printOperationResult(controller::userAction userAct, PGresult* res) const {
	switch (userAct) {
	case controller::userAction::edit:
		printEdit(res);
		break;
	case controller::userAction::generateRandomData:
		printGenerate(res);
		break;
	case controller::userAction::insert:
		printInsert(res);
		break;
	case controller::userAction::remove:
		printRemove(res);
		break;
	case controller::userAction::search:
		printSearch(res);
		break;
	case controller::userAction::print:
		printDB();
		break;
	case controller::userAction::unknown:
		break;
	}
}


void CDatabaseView::printInsert(PGresult* res) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cout << "Insert ends with error: " << m_model->GetLastError() << std::endl;
		return;
	}

	std::cout << "Row was succesfully inserted in table!";
}

void CDatabaseView::printEdit(PGresult* res) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cout << "Update ends with error: " << m_model->GetLastError() << std::endl;
		return;
	}

	std::cout << "Succesfully updated " << PQcmdTuples(res) << " tuples";
}

void CDatabaseView::printSearch(PGresult* res) const {
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		std::cout << "Search ends with error: " << m_model->GetLastError() << std::endl;
		return;
	}

	//std::cout << "Search result:\n";
	auto tpls = m_model->getTuples(res);

	for (int i = 0; i < tpls.size(); ++i) {
		std::cout << i + 1 << ": ";
		for (int j = 0; j < tpls[i].size(); ++j) {
			std::cout << std::setw(20) << tpls[i][j];
		}
		std::cout << std::endl;
	}
}

void CDatabaseView::printRemove(PGresult* res) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cout << "Remove ends with error: " << m_model->GetLastError() << std::endl;
		return;
	}

	std::cout << "Remove succesfull!";
}

void CDatabaseView::printGenerate(PGresult* res) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		std::cout << "Generating data ends with error: " << m_model->GetLastError() << std::endl;
		return;
	}

	std::cout << "Data was generated succesfully!";
}

void CDatabaseView::printDB() const {
	std::cout << "IT-company database:\n";

	const auto tables = m_model->tables();

	//std::cout << std::setw(30);
	std::cout << std::endl;
	for (int tbl = 0; tbl < tables.size(); ++tbl) {
		printDB(tbl);
	}
}

void CDatabaseView::printDB(int tableIndex) const {
	const auto tables = m_model->tables();
	std::cout << std::setw(40) << "Table: " << tables[tableIndex] << std::endl;
	const auto cols = m_model->columnsInTable(tableIndex);
	for (int col = 0; col < cols.size(); ++col) {
		std::cout << std::setw(20) << cols[col];
	}
	std::cout << std::endl;

	const auto rows = m_model->rowsInTable(tableIndex);
	for (int row = 0; row < rows.size(); ++row) {
		for (int col = 0; col < rows[row].size(); ++col) {
			std::cout << std::setw(20) << rows[row][col];
		}
		std::cout << std::endl;
	}

	std::cout << "\n\n";
}