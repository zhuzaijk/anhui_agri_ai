#include "repositories/ConversationRepository.h"
#include "daos/ConversationDao.h"
#include <drogon/drogon.h>

namespace repositories {

bool ConversationRepository::save(const models::Conversation &conv) {
    auto dbClient = drogon::app().getDbClient();
    return daos::ConversationDao::save(dbClient, conv);
}

std::vector<models::Conversation> ConversationRepository::findByUserAndSession(
    int userId, 
    const std::string &sessionId
) {
    auto dbClient = drogon::app().getDbClient();
    return daos::ConversationDao::findByUserAndSession(dbClient, userId, sessionId);
}

} // namespace repositories