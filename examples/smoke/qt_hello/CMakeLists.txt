cmake_minimum_required(VERSION 3.21)
project(qt_hello LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(qt_hello main.cpp)
target_link_libraries(qt_hello PRIVATE Qt6::Widgets)

if(WIN32)
  set_target_properties(qt_hello PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
