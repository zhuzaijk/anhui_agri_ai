#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H

#include <drogon/HttpController.h>

using namespace drogon;

namespace controllers
{
    class ChatController : public drogon::HttpController<ChatController>
    {
    public:
        METHOD_LIST_BEGIN

        ADD_METHOD_TO(ChatController::ask, "/api/chat/ask", Post, "filters::AuthFilter");
        ADD_METHOD_TO(ChatController::getHistory, "/api/chat/history", Get, "filters::AuthFilter");

        METHOD_LIST_END
        void ask(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
        void getHistory(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    };
}


#endif