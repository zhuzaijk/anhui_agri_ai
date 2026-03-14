cmake_minimum_required(VERSION 3.22)
project(anhui_agri_ai_backend CXX)

# 强制C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Release模式默认开启O3优化
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 编译选项：开启所有警告，警告视为错误（工业级代码规范）
add_compile_options(-Wall -Wextra -Werror -O3)

# 查找依赖库
find_package(Drogon REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenSSL REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# 头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party
    ${JSONCPP_INCLUDE_DIRS}
)

# 源文件
file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cc
    ${PROJECT_SOURCE_DIR}/src/**/*.cc
)

# 生成可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接依赖库
target_link_libraries(${PROJECT_NAME}
    Drogon::Drogon
    ${JSONCPP_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)