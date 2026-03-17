#include "services/ChatService.h"
#include "repositories/ConversationRepository.h"
#include "services/RAGFlowService.h"
#include <chrono>  //用于获取当前时间戳
#include <iomanip> //用于格式化时间输出
#include <sstream> //用于将时间转换为字符串
#include <mutex>

namespace services
{
    std::optional<std::string> ChatService::ask(int userId, const dto::ChatAskRequest &req)
    {
        std::string resultAnswer;
        bool requestSuccess = false;
        std::string errorMsg;

        // 用于同步等待的互斥锁和条件变量
        std::mutex mtx;
        std::condition_variable cv;
        bool callbackDone = false;

        RAGFlowService::instance().askQuestion(
            req.question,
            req.chat_id,
            req.session_id,
            [&, userId, req](bool success, const std::string &ans, const std::string &err)
            {
                // 锁保护共享变量
                std::lock_guard<std::mutex> lock(mtx);

                if (success)
                {
                    resultAnswer = ans;
                    requestSuccess = true;
                    // 保存对话记录（注意用 ans，不是外层的空变量）
                    saveConversation(userId, req.session_id, req.question, ans);
                }
                else
                {
                    errorMsg = err;
                    LOG_ERROR << "获取AI回复失败：" << err;
                }

                callbackDone = true;
                cv.notify_one(); // 通知主线程继续
            });

        // 等待回调完成
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]()
                    { return callbackDone; });
        }

        if (requestSuccess)
        {
            return resultAnswer;
        }
        else
        {
            return std::nullopt;
        }
    }
    bool ChatService::saveConversation(int userId, const std::string &sessionId,
                                       const std::string &question, const std::string &answer)
    {
        models::Conversation conv;
        conv.user_id = userId;
        conv.session_id = sessionId;
        conv.question = question;
        conv.answer = answer;
        return repositories::ConversationRepository::save(conv);
    }

    dto::ChatHistoryResponse ChatService::getHistory(int userId, const std::string &sessionId)
    {
        dto::ChatHistoryResponse resp;
        auto conversations = repositories::ConversationRepository::findByUserAndSession(userId, sessionId);

        for (const auto &conv : conversations)
        {
            dto::HistoryMessageItem item;
            item.question = conv.question;
            item.answer = conv.answer;

            // 时间格式化
            std::time_t t = std::chrono::system_clock::to_time_t(conv.created_at);
            std::tm tm = *std::localtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            item.createdAt = oss.str();

            resp.data.push_back(item);
        }

        return resp;
    }
}
