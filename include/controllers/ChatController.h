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

        ADD_METHOD_TO(ChatController::ask, "/api/chat/ask", Post);

        METHOD_LIST_END
        void ask(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    };
}


#endif