#pragma once
#include <string>
#include <optional>
#include "dto/ChatDto.h"
//依旧单例模式，提供对话相关的核心业务逻辑接口，供ChatController调用
namespace services
{
    class ChatService
    {
    public:
        static ChatService& instance()
        {
            static ChatService  instance;
            return instance;
        }
        //请求RAGFlow服务，获取AI回复，调用RAGFlowService的askQuestion方法，传入用户问题和回调函数
        std::optional<std::string> ask(int userId, const dto::ChatAskRequest& req);

        bool saveConversation(int userId, const std::string& sessionId,
                              const std::string& question, const std::string& answer);
        dto::ChatHistoryResponse getHistory(int userId, const std::string &sessionId);

        private:
        ChatService() = default;
        ~ChatService() = default;
        ChatService(const ChatService&) = delete;
        ChatService& operator=(const ChatService&) = delete;
    };
}