#include "services/RAGFlowService.h"
#include <drogon/drogon.h>
#include <iostream>

namespace services
{
    RAGFlowService& RAGFlowService::instance()
    {
        static RAGFlowService instance;
        return instance;
    }

    RAGFlowService::RAGFlowService()
    {
        httpClient_ = drogon::HttpClient::newHttpClient(ragflowApiUrl_);
        if (!httpClient_)
        {
            LOG_FATAL << "无法创建HTTP客户端，RAGFlowService初始化失败！";
        }
    }

    Json::Value RAGFlowService::buildRequest(const std::string& userQuestion, const std::string& session_id)
    {
        if (httpClient_ == nullptr)
        {
            LOG_ERROR << "HTTP客户端未初始化，无法构建RAGFlow请求！";
            return Json::Value();
        }
        //还要改，这里估计得从用户请求里得到session_id和其他参数
        Json::Value requestJson;
        requestJson["question"] = userQuestion;
        requestJson["stream"]= false;
        requestJson["session_id"] = session_id;
        requestJson["return_reference"] = false;
        requestJson["quote"] = false;
        std::cout << "构建的RAGFlow请求体：" << Json::FastWriter().write(requestJson) << std::endl;
        return requestJson;
    }

    void RAGFlowService::askQuestion(const std::string& userQuestion, const std::string& chat_id, const std::string& session_id, const RAGFlowCallback& callback)
    {
        if (httpClient_ == nullptr)
        {
            LOG_ERROR << "HTTP客户端未初始化，无法发送RAGFlow请求！";
            callback(false, "", "HTTP客户端未初始化");
            return;
        }
    
        Json::Value requestJson = buildRequest(userQuestion, session_id);
        LOG_ERROR << requestJson.toStyledString();
        std::string requestBody = Json::FastWriter().write(requestJson);
        //构建请求体
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::HttpMethod::Post);
        req->setPath("/api/v1/chats/" + chat_id + "/completions");//这个路径得改，得从用户请求里得到session_id
        req->setBody(requestBody);
        req->addHeader("Content-Type", "application/json");
        req->addHeader("Authorization", "Bearer " + apiKey);

        httpClient_->sendRequest(req, [callback](drogon::ReqResult result,
                                                const drogon::HttpResponsePtr& resp)
        {
            if (result != drogon::ReqResult::Ok || !resp)
            {
                callback(false, "", "RAGFlow请求失败");
                return;
            }

            if (resp->getStatusCode() != drogon::HttpStatusCode::k200OK)
            {
                LOG_ERROR << "RAGFlow请求失败，HTTP状态码：" << resp->getStatusCode();
                callback(false, "", "请求失败，HTTP状态码：" + std::to_string(resp->getStatusCode()));
                return;
            }

            std::cout << resp->getBody() << std::endl;

            // 3. 解析JSON
            std::shared_ptr<Json::Value> respJson = resp->getJsonObject();
            if (!respJson)
            {
                LOG_ERROR << "RAGFlow响应不是合法JSON";
                callback(false, "", "响应解析失败");
                return;
            }

            // ✅ 修正1：先判断根节点的code是否为0
            if (!respJson->isMember("code") || (*respJson)["code"].asInt() != 0)
            {
                LOG_ERROR << "RAGFlow业务失败，code：" << (*respJson)["code"].asInt();
                callback(false, "", "RAGFlow业务调用失败");
                return;
            }

            // ✅ 修正2：先取二级的data字段！！！
            if (!respJson->isMember("data") || (*respJson)["data"].isNull())
            {
                LOG_ERROR << "RAGFlow响应无data字段";
                callback(false, "", "响应格式异常：无data");
                return;
            }

            // 拿到data节点
            Json::Value dataNode = (*respJson)["data"];

            // ✅ 修正3：在data节点里找answer！！！
            if (!dataNode.isMember("answer") || dataNode["answer"].isNull())
            {
                LOG_ERROR << "RAGFlow响应无answer字段";
                callback(false, "", "响应格式异常：无answer");
                return;
            }

            // ✅ 成功拿到answer
            std::string answer = dataNode["answer"].asString();
            std::cout << "=== 成功提取AI回答 ===" << std::endl;
            std::cout << answer << std::endl;

            callback(true, answer, "");
            return;
        }, 5000);

    }
}