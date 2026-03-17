#pragma once
#include <string>
// 这个 DTO 主要用于 UserController 的请求和响应，定义在这里可以让 Controller 和 Service 层都能方便地使用
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