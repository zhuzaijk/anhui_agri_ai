#include "controllers/ChatController.h"
#include "services/RAGFlowService.h"

namespace controllers
{
    using namespace controllers;

    void ChatController::ask(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
    {
        Json::Value respJson;  
        auto reqJson = req->getJsonObject();
        if (!reqJson)
        {
            LOG_ERROR << "请求体不是有效的JSON格式！";
            respJson["code"] = 400;
            respJson["msg"] = "请求体格式错误";
            respJson["data"] = "";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
            callback(resp);
        }

        //校验参数，要改成session_id和其他参数
        std::string question = (*reqJson)["question"].asString();
        std::string chat_id = (*reqJson)["chat_id"].asString();
        std::string session_id = (*reqJson)["session_id"].asString();

        if (question.empty() || chat_id.empty() || session_id.empty())
        {
            LOG_ERROR << "请求参数缺失！";
            respJson["code"] = 400;
            respJson["msg"] = "请求参数缺失";
            respJson["data"] = "";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
            callback(resp);
            return; 
        }

        services::RAGFlowService::instance().askQuestion(question, chat_id, session_id,
            [callback](bool success, const std::string& answer, const std::string& errMsg)
        {
            Json::Value respJson;
            if (success)
            {
                respJson["code"] = 200;
                respJson["msg"] = "成功获取AI回复！";
                respJson["data"] = answer;
            }
            else
            {
                respJson["code"] = 500;
                respJson["msg"] = "获取AI回复失败：" + errMsg;
                respJson["data"] = "";
            }
            auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
            callback(resp);
        });
    }
}





