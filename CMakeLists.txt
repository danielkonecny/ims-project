cmake_minimum_required(VERSION 3.15)
project(IMS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -g -O2")
#? set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ?)

# ADD_LIBRARY(LibsModule model.cpp)
# target_link_libraries(LibsModule -lsimlib)
# target_link_libraries(LibsModule -lm)

add_executable(IMS model.cpp model.h House.cpp House.h)
# target_link_libraries(IMS LibsModule)
