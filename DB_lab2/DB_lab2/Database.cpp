#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <libpq-fe.h>
//
#include "CDatabaseModel.h"
#include "CDatabaseView.h"
#include "CDatabaseController.h"
#include <memory>
//// (c) Aleksander Alekseev 2016 | http://eax.me/
//
//#define UNUSED(x) (void)(x)
//
//static const char* user_phone_arr[][2] = {
//    { "user111", "phone111" },
//    { "user222", "phone222" },
//    { "user333", "phone333" },
//    { NULL, NULL }
//};
//
//static PGconn* conn = NULL;
//static PGresult* res = NULL;
//
//static void
//terminate(int code)
//{
//    if (code != 0)
//        std::wcout <<  PQerrorMessage(conn);
//
//    if (res != NULL)
//        PQclear(res);
//
//    if (conn != NULL)
//        PQfinish(conn);
//
//    exit(code);
//}
//
//static void
//clearRes()
//{
//    PQclear(res);
//    res = NULL;
//}
//
//static void
//processNotice(void* arg, const char* message)
//{
//    UNUSED(arg);
//    UNUSED(message);
//
//    // do nothing
//}

int main()
{

    auto model = std::make_shared<model::CDatabaseModel>("postgres", "IT-company", "reussite54321");
    auto view = std::make_shared<view::CDatabaseView>(model);

    auto controller = std::make_shared<controller::CDatabaseController>(model, view);

    auto choose = controller->requestAction();
    controller->performAction(choose);
    //const std::string connection_string =
    //    "user=postgres password=reussite54321 host = 127.0.0.1 dbname= postgres";
    //PGconn* conn = PQconnectdb(connection_string.c_str());
    ///* Check to see that the backend connection was successfully made */
    //if (PQstatus(conn) != CONNECTION_OK) {
    //    std::cout << "Connection to database failed: " << PQerrorMessage(conn)
    //        << std::endl;
    //    PQfinish(conn);
    //    return 1;
    //}
    //else {
    //    std::cout << "Connection to database succeed." << std::endl;
    //}

    //auto* res = PQexec(conn, "select * from phonebook;");
    //if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    //    std::cout << "Select failed: " << PQresultErrorMessage(res) << std::endl;
    //}
    //else {
    //    std::cout << "Get " << PQntuples(res) << "tuples, each tuple has "
    //        << PQnfields(res) << "fields" << std::endl;
    //    // print column name
    //    for (int i = 0; i < PQnfields(res); i++) {
    //        std::cout << PQfname(res, i) << "              ";
    //    }
    //    std::cout << std::endl;
    //    // print column values
    //    for (int i = 0; i < PQntuples(res); i++) {
    //        for (int j = 0; j < PQnfields(res); j++) {
    //            std::cout << PQgetvalue(res, i, j) << "   ";
    //        }
    //        std::cout << std::endl;
    //    }
    //}
    //PQclear(res);
    return 0;
}