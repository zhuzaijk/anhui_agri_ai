#include "repositories/UserRepository.h"

namespace repositories
{
    //创建用户
    std::optional<int> UserRepository::create(const std::string& username, const std::string& hashed_pasword)
    {
        try{
            daos::UserDao newUser;
            newUser.username = username;
            newUser.password = hashed_pasword;

            if (newUser.save())
            {
                return std::nullopt;
            }
            return newUser.id;
        }
        catch(const drogon::orm::DrogonDbException& e)
        {
            return std::nullopt;
        }
    }
    //根据用户名查找密码和用户ID
    std::optional<std::pair<int, std::string>> UserRepository::findPasswordByUsername(const std::string& username)
    {
  
        auto user = daos::UserDao::findByUsername(username);
        if (!user)
        {
            return std::nullopt;
        }

        return std::make_pair(user->id, user->password);
    }
}