#include "controllers/UserController.h"
#include "services/UserService.h"

namespace controllers {

using namespace services;
using namespace dto;

// ==========================
// 注册接口实现（Drogon 内置 JSON 版）
// ==========================
void UserController::registerUser(const HttpRequestPtr& req,
                                  std::function<void (const HttpResponsePtr &)> &&callback) {
    // 1. 【关键】直接用 Drogon 解析 JSON
    auto jsonOpt = req->getJsonObject();
    if (!jsonOpt) {
        // JSON 解析失败
        Json::Value jsonResp;
        jsonResp["code"] = 400;
        jsonResp["msg"] = "请求体格式错误";
        auto resp = HttpResponse::newHttpJsonResponse(jsonResp);
        callback(resp);
        return;
    }
    auto json = *jsonOpt;

    // 2. 构造请求 DTO
    RegisterRequest registerReq;
    registerReq.username = json["username"].asString();
    registerReq.password = json["password"].asString();

    // 3. 调用 Service
    auto result = UserService::registerUser(registerReq);

    // 4. 构造响应
    Json::Value jsonResp;
    jsonResp["code"] = result.code;
    jsonResp["msg"] = result.msg;
    jsonResp["data"] = result.data;

    auto resp = HttpResponse::newHttpJsonResponse(jsonResp);
    callback(resp);
}

// ==========================
// 登录接口实现（Drogon 内置 JSON 版）
// ==========================
void UserController::loginUser(const HttpRequestPtr& req,
                               std::function<void (const HttpResponsePtr &)> &&callback) {
    // 1. 【关键】直接用 Drogon 解析 JSON
    auto jsonOpt = req->getJsonObject();
    if (!jsonOpt) {
        Json::Value jsonResp;
        jsonResp["code"] = 400;
        jsonResp["msg"] = "请求体格式错误";
        auto resp = HttpResponse::newHttpJsonResponse(jsonResp);
        callback(resp);
        return;
    }
    auto json = *jsonOpt;

    // 2. 构造请求 DTO
    LoginRequest loginReq;
    loginReq.username = json["username"].asString();
    loginReq.password = json["password"].asString();

    // 3. 调用 Service
    auto result = UserService::loginUser(loginReq);

    // 4. 构造响应
    Json::Value jsonResp;
    jsonResp["code"] = result.code;
    jsonResp["msg"] = result.msg;
    jsonResp["data"] = result.data;

    auto resp = HttpResponse::newHttpJsonResponse(jsonResp);
    callback(resp);
}

} // namespace controllers