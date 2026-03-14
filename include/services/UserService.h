#pragma once
#include <string>
#include <dto/AuthDto.h>

namespace services
{
    class UserService
    {
    public:
        //用户注册
        static dto::AuthResponse registerUser(const dto::RegisterRequest& req);
        //用户登录
        static dto::AuthResponse loginUser(const dto::LoginRequest& req);
    private:
        //检查用户名和密码是否合法
        static bool checkValidNameAndPwd(const std::string& username, const std::string& password);
    };  
}