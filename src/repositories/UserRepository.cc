#include "repositories/UserRepository.h"
#include <string>

namespace repositories
{
    //创建用户
    std::optional<int> UserRepository::create(const std::string& username, const std::string& hashed_pasword)
    {
        try{
            daos::UserDao newUser;
            newUser.username = username;
            newUser.password = hashed_pasword;
            std::cout << newUser.username << newUser.password << std::endl;

            if (newUser.save())
            {
                std::cout << "保存成功，用户ID：" << newUser.id << std::endl;
                return newUser.id;
            }
            return std::nullopt;
        }
        catch(const drogon::orm::DrogonDbException& e)
        {
            std::cout << username << hashed_pasword;
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