#include "services/UserService.h"
#include "utils/PasswordUtil.h"
#include "repositories/UserRepository.h"
#include "utils/Validator.h"

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
        auto validnp = UserService::checkValidNameAndPwd(req.username, req.password);
        // std::cout << "validnp: " << validnp << std::endl;
        // std::cout << "username: " << req.username << std::endl;
        // std::cout << "password: " << req.password << std::endl;
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

        resp.code = 200;
        resp.msg = "登录成功";
        resp.data = std::to_string(userId);//之后换成JWT Token
        return resp;
    }
}




