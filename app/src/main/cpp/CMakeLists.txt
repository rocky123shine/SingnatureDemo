cmake_minimum_required(VERSION 3.18.1)


project("signature")

# 引入所有cpp 和 c
file(GLOB CPP_FILE_LIST ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB C_FILE_LIST ${PROJECT_SOURCE_DIR}/*.c)

add_library(
        signature
        SHARED
        ${CPP_FILE_LIST}
        ${C_FILE_LIST}
)

find_library(
        log-lib
        log)

target_link_libraries(
        signature
        ${log-lib})