#pragma once
#include <string>
#include <optional>

namespace utils
{
    class Validator
    {
        public:
        static bool validateUsername(const std::string& username);
        static bool validatePassword(const std::string& password);
    };
}