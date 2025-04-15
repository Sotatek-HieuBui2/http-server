cmake_minimum_required(VERSION 3.10)
project(HttpRestServer)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)
link_directories(/usr/local/lib/x86_64-linux-gnu)

add_executable(server main.cpp)

target_link_libraries(server pistache pthread)
