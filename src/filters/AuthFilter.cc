#include "filters/AuthFilter.h"
#include "utils/ConfigUtil.hpp"
#include "utils/RedisUtil.hpp"
#include <chrono>
#include <drogon/HttpResponse.h>
#include <string>
#include <stdexcept>
#include <string> // 用于std::stoll
//用于检测用户请求时是否合规，也就是检查Token

using namespace filters;

void AuthFilter::doFilter(const drogon::HttpRequestPtr &req,
                          drogon::FilterCallback &&fcb,
                          drogon::FilterChainCallback &&fccb) {
    // 检查Authorization请求头
    auto authHeader = req->getHeader("Authorization");
    if (authHeader.empty()) {
        Json::Value respJson;
        respJson["code"] = 400;
        respJson["msg"] = "缺少Authorization请求头！";
        respJson["data"] = "";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
        resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
        fcb(resp);
        return;
    }

    // 解析Authorization格式
    const std::string BEARER_PREFIX = "Bearer ";
    if (authHeader.substr(0, BEARER_PREFIX.size()) != BEARER_PREFIX) {
        Json::Value respJson;
        respJson["code"] = 401;
        respJson["msg"] = "Authorization格式错误！正确格式：Bearer {Token}";
        respJson["data"] = "";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
        resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
        fcb(resp);
        return;
    }
    std::string token = authHeader.substr(BEARER_PREFIX.size());
    if (token.empty()) {
        Json::Value respJson;
        respJson["code"] = 401;
        respJson["msg"] = "Token不能为空！";
        respJson["data"] = "";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
        resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
        fcb(resp);
        return;
    }

// 验证Token
try {
    // 显式指定自己的utils命名空间
    auto& config = ::utils::ConfigUtil::getInstance();
    std::string jwtSecret = config.getJwtSecretKey();

    // 解码Token
    auto decoded = jwt::decode(token);
    
    // 旧版jwt-cpp仅支持allow_algorithm
    auto verifier = jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256(jwtSecret))
        .with_issuer("ahau_agri_ai");

    // 验证签名和签发者
    verifier.verify(decoded);

    // ========== 【关键修改】手动验证过期时间（用jwt-cpp自带方法，最稳妥） ==========
    auto expireTime = decoded.get_expires_at();
    auto now = std::chrono::system_clock::now();

    // 打印调试
    int64_t expSec = std::chrono::duration_cast<std::chrono::seconds>(expireTime.time_since_epoch()).count();
    int64_t nowSec = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::cout << "【Token验证】exp: " << expSec << ", now: " << nowSec << std::endl;

    if (expireTime < now) {
        throw std::runtime_error("Token已过期");
    }
    //检查redis白名单
    std::string userId = decoded.get_payload_claim("user_id").as_string();
    std::string redisKey = "jwt:user_id:" + userId;
    std::cout << "dasdasdasdassdas: " << userId << std::endl;

    auto& redis = ::utils::RedisUtil::getInstance();
    auto redisToken = redis.getToken(redisKey);

    if (!redisToken || *redisToken != token) {
        throw std::runtime_error("Token已被撤销或无效");
    }
    req->attributes()->insert("userId",  userId);  // 将用户ID放入请求属性，供后续处理使用
    fccb();

} catch (const std::exception& e) {
    Json::Value respJson;
    respJson["code"] = 401;
    respJson["msg"] = "Token无效或已过期！原因：" + std::string(e.what());
    respJson["data"] = "";
    auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
    resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
    fcb(resp);
}
}