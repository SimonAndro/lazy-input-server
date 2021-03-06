# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.5)

# 项目信息
project("LazyInputServer")

set(CMAKE_RC_COMPILER_INIT windres)
ENABLE_LANGUAGE(RC)
SET(CMAKE_RC_COMPILE_OBJECT
"<CMAKE_RC_COMPILER> -i <SOURCE> -o <OBJECT>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../bin)

add_subdirectory(network)
add_subdirectory(server)
add_subdirectory(simulation)

# 设置可执行文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)

# 生成debug版本
#SET(CMAKE_BUILD_TYPE "debug")
# 生成release版本
SET(CMAKE_BUILD_TYPE "release")
if (CMAKE_BUILD_TYPE STREQUAL debug)
    add_definitions(-D_DEBUG)
endif ()

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c11")

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include_directories(include)
file(GLOB SOURCES "src/*")


# 指定生成目标
add_executable(LazyInput WIN32 ${SOURCES})
# add_executable(LazyInput ${SOURCES})

target_link_libraries(LazyInput
    server
    network 
    simulation
)

