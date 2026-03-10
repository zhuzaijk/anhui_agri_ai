#include <drogon/drogon.h>
#include <iostream>

using namespace drogon;

int main() {
    // 健康检查接口
    app().registerHandler(
        "/api/health",
        [](const HttpRequestPtr&,
           std::function<void(const HttpResponsePtr&)>&& callback) {
            Json::Value json;
            json["code"] = 200;
            json["status"] = "ok";
            json["service"] = "anhui_agri_ai_backend";
            auto resp = HttpResponse::newHttpJsonResponse(json);
            callback(resp);
        },
        {Get}
    );

    // 测试对话接口
    app().registerHandler(
        "/api/v1/chat/test",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback) {
            Json::Value json;
            
            auto req_json = req->getJsonObject();
            if (!req_json) {
                json["code"] = 400;
                json["msg"] = "无效的JSON请求";
                auto resp = HttpResponse::newHttpJsonResponse(json);
                callback(resp);
                return;
            }

            std::string user_msg = (*req_json)["message"].asString();
            json["code"] = 200;
            json["msg"] = "success";
            json["data"]["reply"] = "安徽农业大学AI后端已就绪，收到消息：" + user_msg;
            
            auto resp = HttpResponse::newHttpJsonResponse(json);
            callback(resp);
        },
        {Post}
    );

    std::cout << "===== 安徽农业大学专属AI后端启动成功 =====" << std::endl;
    std::cout << "服务监听地址: http://0.0.0.0:8080" << std::endl;
    std::cout << "健康检查接口: http://localhost:8080/api/health" << std::endl;
    std::cout << "测试对话接口: http://localhost:8080/api/v1/chat/test" << std::endl;

    app().addListener("0.0.0.0", 8080).run();
    return 0;
}