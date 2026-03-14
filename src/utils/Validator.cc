#include "utils/Validator.h"
#include <regex>

namespace utils
{
    bool Validator::validateUsername(const std::string& username)
    {
        if (username.empty() && username.length() > 10)//不能为空，和小于10个字符
        {
            return false;
        }
        std::regex pattern("^[a-zA-Z0-9_]+$");//只能包含字母、数字和下划线
        if (!std::regex_match(username, pattern))
        {
            return false;
        }
        return true;
    }

    bool Validator::validatePassword(const std::string& password) 
    {
        // 规则：≥6位
        if (password.length() < 6) {
            return false;
        }
        return true;
    }
}