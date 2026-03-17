#pragma once

#include "drogon/drogon.h"
#include <json/json.h>
#include <optional>


namespace utils
{
    class ConfigReader
    {
    public:
        static ConfigReader& getInstance()
        {
            static ConfigReader instance;
            return instance;
        }
    
        std::optional<Json::Value> getConfig(const std::string& key) const
        {
            if (custom_config.isNull())
            {
                LOG_ERROR << "用户配置未加载成功！";
                return std::nullopt; // 返回空的Json对象
            }
            
            if (!custom_config.isMember(key))
            {
                LOG_ERROR << "配置项 " << key << " 不存在！";
                return std::nullopt; // 返回空的Json对象
            }
            return custom_config[key];
        }
    private:
        ConfigReader(){
            try
            {
                custom_config = drogon::app().getCustomConfig();
            }
            catch(const std::exception& e)
            {
                LOG_ERROR << "获取用户配置失败！:" << e.what() << '\n';
            }
            
        }
        ~ConfigReader() = default;
        ConfigReader(const ConfigReader&) = delete;
        ConfigReader& operator=(const ConfigReader&) = delete;
        ConfigReader(ConfigReader&&) = delete;
        ConfigReader& operator=(ConfigReader&&) = delete;
        Json::Value custom_config;
    };
}
