#pragma once
#include <string>
#include <optional>
#include <drogon/drogon.h>
#include "daos/UserDao.h"
// 用户仓库：负责数据库操作
namespace repositories
{
    class UserRepository
    {
    public://创建用户
        static std::optional<int> create(const std::string& username
                                , const std::string& hashed_pasword);
        //根据用户名查找密码和用户ID
        static std::optional<std::pair<int, std::string>> findPasswordByUsername(const std::string& username);
    };
}