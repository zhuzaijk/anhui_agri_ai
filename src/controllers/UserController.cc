#include "controllers/UserController.h"
#include "services/UserService.h"
#include "utils/RedisUtil.hpp"
#include "json/json.h"
#include <jwt-cpp/jwt.h>

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

void UserController::getUserInfo(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto jsonOpt = req->getJsonObject();
    Json::Value resqJson;
    resqJson["code"] = "200";
    resqJson["msg"] = "您的token有效！";
    resqJson["data"] = "";
    auto resq = HttpResponse::newHttpJsonResponse(resqJson);
    callback(resq);
}

void UserController::logoutUser(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    (void)req;
    Json::Value respJson;
    try
    {   //从请求头获取Token
        auto authHeader = req->getHeader("Authorization");
        const std::string BEARER_PREFIX = "Bearer ";
        std::string token = authHeader.substr(BEARER_PREFIX.size());

        //获取key，并删除
        auto decoded = jwt::decode(token);
        std::string userId = decoded.get_payload_claim("user_id").as_string();
        std::string redisKey = "jwt:user_id:" + userId;
        auto& redis = ::utils::RedisUtil::getInstance();
        redis.deleteToken(redisKey);

        respJson["code"] = 200;
        respJson["msg"] = "注销成功！";
        respJson["data"] = "";
    }
    catch (const std::exception& e)
    {
        respJson["code"] = 400;
        respJson["msg"] = "注销失败：" + std::string(e.what());
        respJson["data"] = "";
    }

    auto resp = HttpResponse::newHttpJsonResponse(respJson);
    callback(resp);
}
} // namespace controllers