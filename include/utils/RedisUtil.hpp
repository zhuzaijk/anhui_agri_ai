// utils/RedisUtil.hpp
#pragma once
#include <string>
#include <optional>
#include <hiredis/hiredis.h>
#include <drogon/drogon.h>
#include <mutex>

namespace utils {

    // 1. 将 RedisConfig 移到类外部，放在最前面
    struct RedisConfig {
        RedisConfig()
            : host("127.0.0.1"),
              port(6379),
              password(""),
              db(0) {}

        std::string host;
        int port;
        std::string password;
        int db;
    };

    class RedisUtil {
    public:
        // 获取单例
        static RedisUtil& getInstance() {
            static RedisUtil instance;
            return instance;
        }

        // 2. 现在 RedisConfig 已经在前面声明了，这里可以正常使用
        void init(const RedisConfig& config) {
            std::lock_guard<std::mutex> lock(mutex_);
            redis_config = config;

            if (!connectInternal()) {
                LOG_ERROR << "Redis初始化失败！";
            }
        }

        // ========== 1. 存Token ==========
        bool setToken(const std::string& key, const std::string& value, int expireSeconds) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!checkConnection()) return false;
            
            auto reply = (redisReply*)redisCommand(ctx_, "SET %s %s EX %d", 
                                                    key.c_str(), value.c_str(), expireSeconds);
            if (!reply) {
                LOG_ERROR << "Redis SET失败：" << ctx_->errstr;
                return false;
            }
            
            bool success = (reply->type == REDIS_REPLY_STATUS && std::string(reply->str) == "OK");
            freeReplyObject(reply);
            return success;
        }

        // ========== 2. 取Token ==========
        std::optional<std::string> getToken(const std::string& key) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!checkConnection()) return std::nullopt;
            
            auto reply = (redisReply*)redisCommand(ctx_, "GET %s", key.c_str());
            if (!reply) {
                LOG_ERROR << "Redis GET失败：" << ctx_->errstr;
                return std::nullopt;
            }
            
            std::optional<std::string> result;
            if (reply->type == REDIS_REPLY_STRING) {
                result = std::string(reply->str, reply->len);
            }
            
            freeReplyObject(reply);
            return result;
        }

        // ========== 3. 删Token ==========
        bool deleteToken(const std::string& key) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!checkConnection()) return false;
            
            auto reply = (redisReply*)redisCommand(ctx_, "DEL %s", key.c_str());
            if (!reply) {
                LOG_ERROR << "Redis DEL失败：" << ctx_->errstr;
                return false;
            }
            
            bool success = (reply->type == REDIS_REPLY_INTEGER && reply->integer > 0);
            freeReplyObject(reply);
            return success;
        }

        // 析构
        ~RedisUtil() {
            if (ctx_) {
                redisFree(ctx_);
            }
        }

        // 禁止拷贝
        RedisUtil(const RedisUtil&) = delete;
        RedisUtil& operator=(const RedisUtil&) = delete;

    private:
        bool connectInternal() {
            if (ctx_) {
                redisFree(ctx_);
                ctx_ = nullptr;
            }

            // 连接
            ctx_ = redisConnect(redis_config.host.c_str(), redis_config.port);
            if (ctx_ == nullptr || ctx_->err) {
                if (ctx_) {
                    LOG_FATAL << "Redis连接失败：" << ctx_->errstr;
                    redisFree(ctx_);
                    ctx_ = nullptr;
                } else {
                    LOG_FATAL << "Redis连接失败：无法分配上下文";
                }
                return false;
            }

            // 认证
            if (!redis_config.password.empty()) {
                auto reply = (redisReply*)redisCommand(ctx_, "AUTH %s", redis_config.password.c_str());
                if (reply->type == REDIS_REPLY_ERROR) {
                    LOG_FATAL << "Redis认证失败：" << reply->str;
                    freeReplyObject(reply);
                    redisFree(ctx_);
                    ctx_ = nullptr;
                    return false;
                }
                freeReplyObject(reply);
            }

            // 选库
            if (redis_config.db != 0) {
                auto reply = (redisReply*)redisCommand(ctx_, "SELECT %d", redis_config.db);
                if (reply->type == REDIS_REPLY_ERROR) {
                    LOG_FATAL << "Redis选择数据库失败：" << reply->str;
                    freeReplyObject(reply);
                    redisFree(ctx_);
                    ctx_ = nullptr;
                    return false;
                }
                freeReplyObject(reply);
            }

            LOG_INFO << "Redis连接成功！已连接到 " << redis_config.host << ":" << redis_config.port;
            return true;
        }

        bool checkConnection() {
            if (!ctx_ || ctx_->err) {
                LOG_WARN << "Redis连接不可用，正在尝试重连...";
                return connectInternal();
            }
            return true;
        }

        // 私有构造
        RedisUtil() : ctx_(nullptr) {}

        RedisConfig redis_config;
        redisContext* ctx_;
        std::mutex mutex_;
    };

} // namespace utils