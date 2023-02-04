cmake_minimum_required(VERSION 3.22.1)
project(web-server)

#include(CTest)
#enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
