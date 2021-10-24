#pragma once
#include <memory>
#include <libpq-fe.h>
namespace model {
	class CDatabaseModel;
}

namespace controller {
	enum class userAction;
	class CDatabaseController;
}

namespace view {
	class CDatabaseView
	{
	public:
		CDatabaseView(std::shared_ptr<model::CDatabaseModel> model_);

		void print(controller::userAction userAct,PGresult* res);

	private:
		std::shared_ptr<model::CDatabaseModel> m_model;
	};
}

