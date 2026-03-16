#include "services/UserService.h"
#include "utils/PasswordUtil.h"
#include "repositories/UserRepository.h"
#include "utils/Validator.h"
#include "utils/ConfigUtil.hpp"
#include "utils/RedisUtil.hpp"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include <string>
#include <iostream>

namespace services
{
    using namespace dto;
    using namespace repositories;
    using namespace utils;

    //检查用户名和密码是否合法
    bool UserService::checkValidNameAndPwd(const std::string& username, const std::string& password)
    {
        auto validname = Validator::validateUsername(username);
        if (!validname)
        {
            return false;
        }
        auto validpass = Validator::validatePassword(password);
        if (!validpass)
        {
            return false;
        }
        return true;
    }
    //用户注册
    AuthResponse UserService::registerUser(const RegisterRequest& req)
    {
        AuthResponse resp;
        // //检查
        bool validnp = UserService::checkValidNameAndPwd(req.username, req.password);

        if (!validnp)
        {
            resp.code = 400;
            resp.msg = "用户名和密码格式错误，用户名只能包含字母、数字和下划线不能为空且和小于10个字符\n密码长度至少为 6 位";
            return resp;
        }

       // 先检查存不存在
        bool userExist = daos::UserDao::checkIsExist(req.username);
        if (userExist)
        {
            resp.code = 400;
            resp.msg = "用户名已存在";
            return resp;
        }
        //密码加密
        auto hashResult = PasswordUtil::encrypt(req.password);//若成功，返回的是tuple

        std::string hashedPwd = PasswordUtil::serialize(hashResult);//序列化，便于存储
        //存入数据库
        auto userIdOpt = UserRepository::create(req.username, hashedPwd);

        if (!userIdOpt)
        {
            resp.code = 500;
            resp.msg = "注册失败";
            return resp;
        }
        resp.code = 200;
        resp.msg = "注册成功";
        resp.data = std::to_string(*userIdOpt);
        return resp;
    }

    AuthResponse UserService::loginUser(const dto::LoginRequest& req)
    {
        AuthResponse resp;
        // 检查用户是否存在
        bool userExist = daos::UserDao::checkIsExist(req.username);
        std::cout << "userExist: " << userExist << std::endl;
        if (!userExist)
        {
            resp.code = 400;
            resp.msg = "用户名不存在，请注册";
            return resp;
        }
        // 检查密码是否正确
        auto userOpt = UserRepository::findPasswordByUsername(req.username);
        if (!userOpt) {
            resp.code = 401;
            resp.msg = "出错，无法查找到用户！";
            return resp;
        }
        auto [userId, storedPwd] = *userOpt;
        auto hashResult = PasswordUtil::deserialize(storedPwd);//反序列化

        bool isValid = PasswordUtil::verify(req.password, std::get<0>(hashResult), 
                                            std::get<1>(hashResult), 
                                            std::get<2>(hashResult));
        
        if (!isValid) {
            resp.code = 401;
            resp.msg = "用户名或密码错误";
            return resp;
        }

        try {
            auto& jwt_config = ::utils::ConfigUtil::getInstance();
            std::string jwtSecret = jwt_config.getJwtSecretKey();
            int expireHours = jwt_config.getJwtExpireHours();
            
            auto now = std::chrono::system_clock::now();
            auto expireAt = now + std::chrono::hours(expireHours);
            
            // ✅ 推荐：用set_expires_at，jwt-cpp自动把exp存为数字类型
            auto token = jwt::create()
                .set_issuer("ahau_agri_ai")
                .set_subject("user_auth")
                .set_issued_at(now)
                .set_expires_at(expireAt) // 【关键】用这个，不要手动set_payload_claim("exp")
                .set_payload_claim("user_id", jwt::claim(std::to_string(userId)))
                .set_payload_claim("username", jwt::claim(req.username))
                .sign(jwt::algorithm::hs256(jwtSecret));

            std::string redisKey = "jwt:user_id:" + std::to_string(userId);
            int expireSeconds = expireHours * 3600;

            auto& redis = ::utils::RedisUtil::getInstance();
            bool savaSucess = redis.setToken(redisKey, token, expireSeconds);

            if (!savaSucess)
            {
                LOG_WARN << "Redis存储Token失败：\n";
            }
            // 打印调试
            int64_t expSec = std::chrono::duration_cast<std::chrono::seconds>(
                expireAt.time_since_epoch()
            ).count();
            std::cout << "【生成Token】密钥：" << jwtSecret.substr(0, 50) << "..." 
                    << "，过期时间：" << expSec << std::endl;

            resp.code = 200;
            resp.msg = "登录成功！";
            resp.data = token;
        } catch (const std::exception& e) {
            resp.code = 500;
            resp.msg = "登录成功，但token生成失败：" + std::string(e.what());
            resp.data = "";
        }
        return resp;
    }
}




