cmake_minimum_required(VERSION 3.13)
project(git-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

set(SOURCE_FILES src/Server.cpp)
list(APPEND SOURCE_FILES src/btree.cpp)
list(APPEND SOURCE_FILES src/utility.cpp)
list(APPEND SOURCE_FILES src/command.cpp)
list(APPEND SOURCE_FILES src/page.cpp)
#add_compile_options(-g -fsanitize=undefined -fsanitize=address)
add_executable(server ${SOURCE_FILES})