project(web-server)

set(SOURCE_FILES server.cpp
                 socket_helper.cpp)

add_executable(web-server ${SOURCE_FILES})

