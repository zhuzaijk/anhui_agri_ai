#include "daos/UserDao.h"

namespace daos
{
    void UserDao::fillFromRow(const drogon::orm::Row& row) 
    {
        if (!row["id"].isNull()) id = row["id"].as<int>();
        if (!row["username"].isNull()) username = row["username"].as<std::string>();
        if (!row["password"].isNull()) password = row["password"].as<std::string>();
    }
    bool UserDao::save() 
    {
        try {
            auto dbClient = drogon::app().getDbClient();
            
            // 手写 INSERT SQL，简单直接
            auto result = dbClient->execSqlSync(
                "INSERT INTO user (username, password) VALUES (?, ?)",
                username, password
            );

            // 插入成功后，获取数据库生成的自增 ID
            id = static_cast<int>(result.insertId());
            return true;
        }
        catch (const drogon::orm::DrogonDbException& e) {
            return false;
        }
    } 
    std::optional<UserDao> UserDao::findByUsername(const std::string& username) 
    {
        try {
            auto dbClient = drogon::app().getDbClient();
            
            // 手写 SELECT SQL
            auto result = dbClient->execSqlSync(
                "SELECT id, username, password FROM user WHERE username = ?",
                username
            );

            if (result.empty()) {
                return std::nullopt; // 没查到
            }

            // 查到了，构造对象并返回
            UserDao user;
            user.fillFromRow(result[0]);
            return user;
        }
        catch (...) {
            return std::nullopt;
        }
    }
    bool UserDao::checkIsExist(const std::string& username) 
    {
        try {
            auto dbClient = drogon::app().getDbClient();
            
            // 手写 SELECT SQL
            auto result = dbClient->execSqlSync(
                "SELECT id FROM user WHERE username = ?",
                username
            );
            return !result.empty();
        }
        catch (...) {
            std::cout << "查找是否存在失败！" << std::endl;
            return false;
        }
    }
}