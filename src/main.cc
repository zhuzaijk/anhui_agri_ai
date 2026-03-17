#include "drogon/drogon.h"
#include "filters/AuthFilter.h" // 引入Filter头文件
#include "controllers/UserController.h"
#include "services/RAGFlowService.h"
#include "utils/ConfigUtil.hpp"
#include "utils/RedisUtil.hpp"


int main() {
    // 1. 【第一步】用绝对路径加载配置
    std::string configPath = "/home/zhu/anhui_agri_ai_backend/config.json";
    std::cout << "===== 正在加载配置：" << configPath << " =====" << std::endl;
    drogon::app().loadConfigFile(configPath);

    // 2. 【第二步】初始化其他工具
    ::utils::ConfigUtil::getInstance().init();

    // 3. 【第三步】初始化Redis（此时配置已经加载完了！）
    std::cout << "===== 正在初始化Redis... =====" << std::endl;
    auto config = ::utils::RedisConfig();
    ::utils::RedisUtil::getInstance().init(config);
    
    auto& ragflowService = services::RAGFlowService::instance();
    (void)ragflowService; // 避免未使用警告

    // 5. 【第五步】最后才run()
    std::cout << "===== 启动服务... =====" << std::endl;
    drogon::app().addListener("0.0.0.0", 11444);
    drogon::app().run();
    
    return 0;
}

// #include <iostream>
// #include <drogon/drogon.h>
// #include <json/json.h>
// #include <string>
// #include "utils/ConfigReader.hpp"

// int main() {
//     drogon::app().loadConfigFile("../config.json");//以运行路径为基准，（在build运行的）
//     Json::Value custiom_config = drogon::app().getCustomConfig();
//     auto CustomConfig = utils::ConfigReader::getInstance().getConfig("ragflow");

//     Json::Value ragflowConfig = (*CustomConfig);

//     // std::string apiKey = ragflowConfig["ragflow"]["apikey"].asString();

//     std::cout << ragflowConfig["apikey"].asString() << std::endl;
//     return 0;
// }