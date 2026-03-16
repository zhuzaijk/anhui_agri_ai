#pragma once
#include <jwt-cpp/jwt.h>
#include "utils/ConfigUtil.hpp"
#include <drogon/HttpFilter.h>

using namespace drogon;

namespace filters
{
    class AuthFilter : public HttpFilter<AuthFilter>
    {
    public:
        AuthFilter() {}
        void doFilter(const HttpRequestPtr &req,
                    FilterCallback &&fcb,
                    FilterChainCallback &&fccb) override;
    };
}

