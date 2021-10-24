#include "CDatabaseView.h"
#include "CDatabaseModel.h"
#include "CDatabaseController.h"

using namespace view;

CDatabaseView::CDatabaseView(std::shared_ptr<model::CDatabaseModel> model)
	: m_model(model) {}

void CDatabaseView::print(controller::userAction userAct, PGresult* res) {
	switch (userAct) {
	case controller::userAction::edit:
	case controller::userAction::generateRandomData:
	case controller::userAction::insert:
	case controller::userAction::remove:
	case controller::userAction::search:
	case controller::userAction::unknown:
		break;
	}
}