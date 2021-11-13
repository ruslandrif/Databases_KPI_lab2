#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <libpq-fe.h>

#include "CDatabaseModel.h"
#include "CDatabaseView.h"
#include "CDatabaseController.h"
#include <memory>


int main()
{
    auto model = std::make_shared<model::CDatabaseModel>("postgres", "IT-company", "passw");
    auto view = std::make_shared<view::CDatabaseView>(model);

    auto controller = std::make_shared<controller::CDatabaseController>(model, view);

    auto choose = controller->requestAction();
    controller->performAction(choose);
    return 0;
}