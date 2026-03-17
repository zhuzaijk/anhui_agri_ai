#pragma once
#include <string>
#include <vector>
// 这个 DTO 主要用于 ChatController 的请求和响应，定义在这里可以让 Controller 和 Service 层都能方便地使用
namespace dto
{   //对话请求DTO，包含用户问题、会话ID等信息，供ChatController使用
    struct ChatAskRequest
    {
        std::string question;
        std::string chat_id;
        std::string session_id;
    };

    struct HistoryMessageItem
    {
        std::string question;
        std::string answer;
        std::string createdAt;
    };

    struct ChatHistoryResponse
    {
        int code = 0;
        std::string msg;
        std::vector<HistoryMessageItem> data;
    };
}
