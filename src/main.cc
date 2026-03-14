#include <iostream>
#include "drogon/drogon.h"


int main()
{
    drogon::app().loadConfigFile("../config.json");
    drogon::app().addListener("0.0.0.0", 11444);
    drogon::app().run();
    return 0;
}