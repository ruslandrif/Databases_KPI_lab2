#include "CDatabaseView.h"
#include "CDatabaseModel.h"
#include "CDatabaseController.h"
#include <iostream>
using namespace view;

CDatabaseView::CDatabaseView(std::shared_ptr<model::CDatabaseModel> model)
	: m_model(model) {}

void CDatabaseView::print(controller::userAction userAct, PGresult* res) {
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
	case controller::userAction::unknown:
		break;
	}
}


void CDatabaseView::printInsert(PGresult* res) {
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
		std::cout << "Insert ends with error: " << m_model->GetLastError() << std::endl;
}

void CDatabaseView::printEdit(PGresult* res) {
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
		std::cout << "Update ends with error: " << m_model->GetLastError() << std::endl;

	std::cout << "Succesfully updated " << PQcmdTuples(res) << " tuples";
}

void CDatabaseView::printSearch(PGresult* res) {
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
		std::cout << "Search ends with error: " << m_model->GetLastError() << std::endl;
}

void CDatabaseView::printRemove(PGresult* res) {
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
		std::cout << "Remove ends with error: " << m_model->GetLastError() << std::endl;
}

void CDatabaseView::printGenerate(PGresult* res) {
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
		std::cout << "Generating data ends with error: " << m_model->GetLastError() << std::endl;
}