#pragma once
#include "models/Conversation.h"
#include <optional>
#include <vector>
#include <drogon/orm/DbClient.h>

namespace daos
{   //保存和找回
    class ConversationDao
    {
    public:
        static bool save(const drogon::orm::DbClientPtr &dbClient, 
                         const models::Conversation &conversation);

        static std::vector<models::Conversation> findByUserAndSession(
                                const drogon::orm::DbClientPtr &dbClient,
                                int user_id,
                                const std::string &session_id);
    };
}