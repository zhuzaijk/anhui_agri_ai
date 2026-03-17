// include/repositories/ConversationRepository.h
#pragma once
#include "models/Conversation.h"
#include <optional>
#include <vector>
// 这个 Repository 主要负责封装对 ConversationDao 的调用，提供更高层次的接口给 Service 层使用
namespace repositories {

class ConversationRepository {
public:
    static bool save(const models::Conversation &conv);
    static std::vector<models::Conversation> findByUserAndSession(int userId, const std::string &sessionId);
};

} // namespace repositories