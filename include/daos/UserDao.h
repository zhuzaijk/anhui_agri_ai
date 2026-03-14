#pragma once
#include <string>
#include <optional>
#include <drogon/drogon.h>


//基础操作：保存、根据用户名查询

namespace daos
{
    class UserDao
    {
    public:
        int id = -1;
        std::string username;
        std::string password;
        UserDao() = default;
        UserDao(int id, const std::string& username, const std::string& password)
            : id(id), username(username), password(password){}
        bool save();
        static std::optional<UserDao> findByUsername(const std::string& username);
        static bool checkIsExist(const std::string& username);
        bool isValid() const { return id != -1; };
    private:
        void fillFromRow(const drogon::orm::Row& row);
    };
}