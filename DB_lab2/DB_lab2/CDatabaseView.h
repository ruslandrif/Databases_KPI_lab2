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

		void printOperationResult(controller::userAction userAct,PGresult* res) const;

		void printDB() const;
		void printDB(int tableIndex) const;

	private:

		void printInsert(PGresult* res) const;
		void printEdit(PGresult* res) const;
		void printGenerate(PGresult* res) const;
		void printSearch(PGresult* res) const;
		void printRemove(PGresult* res) const;

		std::shared_ptr<model::CDatabaseModel> m_model;
	};
}

