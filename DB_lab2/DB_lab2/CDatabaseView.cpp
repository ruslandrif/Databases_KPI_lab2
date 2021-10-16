#include "CDatabaseView.h"
#include "CDatabaseModel.h"

using namespace view;

CDatabaseView::CDatabaseView(std::shared_ptr<model::CDatabaseModel> model)
	: m_model(model) {}

void CDatabaseView::print() {

}