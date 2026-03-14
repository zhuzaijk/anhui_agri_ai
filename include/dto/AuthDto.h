#pragma once
#include <string>

namespace dto
{   //注册请求
    struct RegisterRequest
    {
        std::string username;
        std::string password;
    };
    //登录请求
    struct LoginRequest
    {
        std::string username;
        std::string password;
    };
    //通用响应
    struct AuthResponse
    {
        int code;
        std::string msg;
        std::string data;
    };
}