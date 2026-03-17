#include "controllers/ChatController.h"
#include "services/RAGFlowService.h"
#include "services/ChatService.h"
#include <drogon/drogon.h>

namespace controllers
{
    using namespace controllers;

    void ChatController::ask(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
    {
        Json::Value respJson;
        respJson["code"] = 0;
        respJson["msg"] = "success";
        auto reqJson = req->getJsonObject();
        if (!reqJson)
        {
            LOG_ERROR << "请求体不是有效的JSON格式！";
            respJson["code"] = 400;
            respJson["msg"] = "请求体格式错误";
            respJson["data"] = "";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
            callback(resp);
            return;
        }

        // 校验参数，要改成session_id和其他参数
        dto::ChatAskRequest chatReq;
        chatReq.question = (*reqJson)["question"].asString();
        chatReq.chat_id = (*reqJson)["chat_id"].asString();
        chatReq.session_id = (*reqJson)["session_id"].asString();
        int userId = 0;
        userId = std::stoi(req->attributes()->get<std::string>("userId"));//一定要先转string！！

        if (chatReq.question.empty() || chatReq.chat_id.empty() || chatReq.session_id.empty())
        {
            LOG_ERROR << "请求参数缺失！";
            respJson["code"] = 400;
            respJson["msg"] = "请求参数缺失";
            respJson["data"] = "";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
            callback(resp);
            return;
        }

        auto answerOpt = services::ChatService::instance().ask(userId, chatReq);
        if (!answerOpt)
        {
            respJson["code"] = 2;
            respJson["msg"] = "获取 AI 回复失败";
            callback(drogon::HttpResponse::newHttpJsonResponse(respJson));
            return;
        }
        respJson["data"] = *answerOpt;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
        callback(resp);
    };

    void ChatController::getHistory(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
    {
        Json::Value respJson;
        respJson["code"] = 0;
        respJson["msg"] = "sucess";

        try
        {
            int userId = req->attributes()->get<int>("userId");
            std::string sessionId = req->getParameter("session_id");
            if (sessionId.empty())
            {
                respJson["code"] = 1;
                respJson["msg"] = "缺少 session_id 参数";
                callback(drogon::HttpResponse::newHttpJsonResponse(respJson));
            }
            auto historyResp = services::ChatService::instance().getHistory(userId, sessionId);

            respJson["data"] = Json::Value(Json::arrayValue);
            for (const auto &item : historyResp.data)
            {
                Json::Value itemJson;
                itemJson["question"] = item.question;
                itemJson["answer"] = item.answer;
                itemJson["created_at"] = item.createdAt;
                respJson["data"].append(itemJson);
            }
        }
        catch (const std::exception &e)
        {
            respJson["code"] = 500;
            respJson["msg"] = "服务器内部错误：" + std::string(e.what());
            respJson["data"] = "";
            LOG_ERROR << e.what() << '\n';
        }

        callback(drogon::HttpResponse::newHttpJsonResponse(respJson));
    }
}
