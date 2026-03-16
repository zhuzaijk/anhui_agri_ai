#ifndef RAGFLOWSERVICE_H
#define RAGFLOWSERVICE_H

#pragma once
#include <drogon/HttpClient.h>
#include <drogon/HttpResponse.h>
#include <json/json.h>
#include <string>
#include <functional>

//回调函数类型：是否成功，ai回复，错误信息
using RAGFlowCallback = std::function<void(bool, const std::string&, const std::string&)>;

namespace services
{
class RAGFlowService
{
public:
    static RAGFlowService& instance();
    // 调用 RAGFlow 对话接口
    void askQuestion(const std::string& userQuestion, 
                    const std::string& chat_id, 
                    const std::string& session_id, 
                    const RAGFlowCallback& callback);


private:
    RAGFlowService();
    ~RAGFlowService() = default;
    RAGFlowService(const RAGFlowService&) = delete;
    RAGFlowService& operator=(const RAGFlowService&) = delete;
    // 构建 RAGFlow 请求体
    Json::Value buildRequest(const std::string& userQuestion, const std::string& session_id);

    std::shared_ptr<drogon::HttpClient> httpClient_;
    std::string apiKey = "ragflow-Q2ZTliMmZjMjBmZTExZjE4N2ZhNDZlOW";
    const std::string ragflowApiUrl_ = "http://127.0.0.1:80";
};
}


#endif