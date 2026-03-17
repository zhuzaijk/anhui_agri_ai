#pragma once
#include <string>
#include <chrono>
// 这个模型主要用于封装对话数据，包含用户ID、会话ID、问题、答案和创建时间等字段
namespace models
{
    struct Conversation 
    {
        int id = -1;
        int user_id = -1;
        std::string session_id;
        std::string question;
        std::string answer;
        std::chrono::system_clock::time_point created_at;
    };
}