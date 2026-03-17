#include "daos/ConversationDao.h"
#include <drogon/drogon.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace daos
{
    bool ConversationDao::save(const drogon::orm::DbClientPtr &dbClient, const models::Conversation &conversation)
    {
        try
        {
            std::string sql = R"(
            INSERT INTO conversation (user_id, 
                                    session_id,
                                    question,
                                    answer)
            VALUES (?, ?, ?, ?)  -- 必须加这一行！
        )";
            auto result = dbClient->execSqlSync(sql, conversation.user_id,
                                                conversation.session_id,
                                                conversation.question,
                                                conversation.answer); // 执行写入命令
            return result.affectedRows() > 0;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << e.what() << '\n';
            return false;
        }
    }
    // optoin默认是值拷贝
    std::vector<models::Conversation> ConversationDao::findByUserAndSession(
        const drogon::orm::DbClientPtr &dbClient,
        int user_id,
        const std::string &session_id)
    {
        std::vector<models::Conversation> conversations;
        try
        { // 查找，并安装时间顺序返回
            std::string sql = R"(
                SELECT id, user_id, session_id, question, answer, created_at
                FROM conversation
                WHERE user_id = ? AND session_id = ?
                ORDER BY created_at ASC
            )";
            auto result = dbClient->execSqlSync(sql, user_id, session_id);
            for (size_t i = 0; i < result.size(); ++i)
            {
                models::Conversation conv;
                conv.id = result[i]["id"].as<int>();
                conv.user_id = result[i]["user_id"].as<int>();
                conv.session_id = result[i]["session_id"].as<std::string>();
                conv.question = result[i]["question"].as<std::string>();
                conv.answer = result[i]["answer"].as<std::string>();

                // ========== 重点修改这部分时间解析逻辑 ==========
                // 1. 先把数据库的created_at转为字符串（Drogon支持直接转string）
                std::string time_str = result[i]["created_at"].as<std::string>();

                // 2. 定义tm结构体存储解析后的时间
                std::tm tm_time = {};

                // 3. 用stringstream解析MySQL的DATETIME格式（YYYY-MM-DD HH:MM:SS）
                std::istringstream time_stream(time_str);
                time_stream >> std::get_time(&tm_time, "%Y-%m-%d %H:%M:%S");

                // 4. 转换为std::chrono::system_clock::time_point
                if (!time_stream.fail())
                {
                    // 解析成功：tm转time_t，再转time_point
                    std::time_t t = std::mktime(&tm_time);
                    conv.created_at = std::chrono::system_clock::from_time_t(t);
                }
                else
                {
                    // 解析失败：设置为当前时间，同时打日志提醒
                    conv.created_at = std::chrono::system_clock::now();
                    LOG_WARN << "解析created_at失败，时间字符串：" << time_str << "，已设为当前时间";
                }
                // ==============================================

                conversations.push_back(conv);
            }
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << "查询历史对话失败：" << e.what() << '\n';
        }
        return conversations;
    }
}