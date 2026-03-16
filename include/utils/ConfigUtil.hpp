#pragma once
#include <string>
#include <drogon/drogon.h>
#include <iostream>
#include <json/json.h>

namespace utils
{
    class ConfigUtil//单例模式
    {
    public:
        static ConfigUtil& getInstance()
        {
            static ConfigUtil instance;//静态实例只会创建一次
            return instance;
        }
        void init()//手动初始化，使得main先读取配置文件再初始化
        {
            readJwtConfig();
        }
        std::string getJwtSecretKey() const
        {
            return jwt_secret_key_;
        }

        int getJwtExpireHours() const
        {
            return jwt_expire_hours_;
        }
    private:
        ConfigUtil()//私有化构造函数
        {
        }
        //禁止拷贝构造以及赋值
        ConfigUtil(const ConfigUtil&) = delete;
        ConfigUtil& operator=(const ConfigUtil&) = delete;

        void readJwtConfig()
        {
            Json::Value custom_config = drogon::app().getCustomConfig();
            Json::Value jwt_config = custom_config["jwt"];
            Json::StreamWriterBuilder builder;
            std::cout << Json::writeString(builder, jwt_config) << std::endl;

            if (!jwt_config.isMember("secret_key") || !jwt_config.isMember("expire_hours")) 
            {
                LOG_FATAL << "JWT配置缺失：缺少 secret_key 或 expire_hours";
                return; // LOG_FATAL 会终止，这里 return 只是为了逻辑完整性
            }
            
            jwt_secret_key_ = jwt_config["secret_key"].asString();
            jwt_expire_hours_ = jwt_config["expire_hours"].asInt();
            if (jwt_secret_key_.empty() || jwt_secret_key_.size() < 32)
            {
                LOG_FATAL << "JWT密钥配置错误：不能为空且至少得是32位";
            }
            if (jwt_expire_hours_ <= 0)
            {
                LOG_WARN << "JWT过期时间非法，默认为2小时";
                jwt_expire_hours_ = 2;
            }
        }

        std::string jwt_secret_key_;
        int jwt_expire_hours_;
        
    };
}