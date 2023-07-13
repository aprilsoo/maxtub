cmake_minimum_required(VERSION 3.5)

project(MAXTUB LANGUAGES CXX)

add_executable(maxtub ./src/main.cpp)

add_library(logger STATIC ${PROJECT_SOURCE_DIR}/src/logger/Logger.cpp)
target_link_libraries(logger ${PROJECT_SOURCE_DIR}/resources/log4c/lib/liblog4cpp.a)
TARGET_LINK_LIBRARIES(logger pthread)


target_link_libraries(maxtub logger)

include_directories("${PROJECT_SOURCE_DIR}/include/logger")
include_directories("${PROJECT_SOURCE_DIR}/resources/log4c/include")
