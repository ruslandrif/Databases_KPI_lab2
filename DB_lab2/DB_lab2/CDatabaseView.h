#pragma once
#include <memory>

namespace model {
	class CDatabaseModel;
}

namespace view {
	class CDatabaseView
	{
	public:
		CDatabaseView(std::shared_ptr<model::CDatabaseModel> model_);

		void print();

	private:
		std::shared_ptr<model::CDatabaseModel> m_model;
	};
}

