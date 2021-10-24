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

		int chooseTableForAction(userAction ua);

		int chooseColumnInTable(int tableNum);

		int chooseRowInTable(int tableIndex);

		std::string requestData(const char* colName, model::dataTypes type);

		bool performInsert(int tableIndex);
		bool performRemove(int tableIndex, int rowIndex);
		bool performEdit(int tableIndex, int rowIndex);
		bool performGeneratingRandomData(int tableIndex);
		bool performSearch(int tableIndex);

		std::shared_ptr<model::CDatabaseModel> m_model;
		std::shared_ptr<view::CDatabaseView> m_view;
	};
}

