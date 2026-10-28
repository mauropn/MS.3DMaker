cmake_minimum_required(VERSION 3.21)
project(qt_occt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

if(NOT DEFINED ENV{CASROOT})
  if(EXISTS "C:/Dev/OCCT/install")
    set(ENV{CASROOT} "C:/Dev/OCCT/install")
  elseif(EXISTS "F:/Dev/OCCT/install")
    set(ENV{CASROOT} "F:/Dev/OCCT/install")
  endif()
endif()
set(OpenCASCADE_DIR "$ENV{CASROOT}/cmake" CACHE PATH "OCCT CMake package dir")
find_package(OpenCASCADE REQUIRED)

add_executable(qt_occt main.cpp)
target_include_directories(qt_occt PRIVATE ${OpenCASCADE_INCLUDE_DIR})
target_link_libraries(qt_occt PRIVATE
  Qt6::Widgets
  TKernel
  TKMath
  TKG2d
  TKG3d
  TKGeomBase
  TKBRep
  TKGeomAlgo
  TKTopAlgo
  TKPrim
)

if(WIN32)
  set_target_properties(qt_occt PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
