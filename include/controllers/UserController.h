#pragma once

#include <drogon/HttpController.h>
#include "dto/AuthDto.h"
#include "services/UserService.h"

using namespace drogon;

namespace controllers
{
class UserController : public drogon::HttpController<UserController>
{
  public:
    void registerUser(const HttpRequestPtr& req,
                      std::function<void (const HttpResponsePtr &)> &&callback);
    void loginUser(const HttpRequestPtr& req,
                      std::function<void (const HttpResponsePtr &)> &&callback);
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(UserController::get, "/{2}/{1}", Get); // path is /controllers/UserController/{arg2}/{arg1}
    // METHOD_ADD(UserController::your_method_name, "/{1}/{2}/list", Get); // path is /controllers/UserController/{arg1}/{arg2}/list
    // ADD_METHOD_TO(UserController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(UserController::registerUser, "/api/user/register", Post);
    ADD_METHOD_TO(UserController::loginUser, "/api/user/login", Post);
    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
}
