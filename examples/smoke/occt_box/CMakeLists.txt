cmake_minimum_required(VERSION 3.21)
project(occt_box LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED ENV{CASROOT})
  if(EXISTS "C:/Dev/OCCT/install")
    set(ENV{CASROOT} "C:/Dev/OCCT/install")
  elseif(EXISTS "F:/Dev/OCCT/install")
    set(ENV{CASROOT} "F:/Dev/OCCT/install")
  endif()
endif()

set(OpenCASCADE_DIR "$ENV{CASROOT}/cmake" CACHE PATH "OCCT CMake package dir")
find_package(OpenCASCADE REQUIRED)

add_executable(occt_box main.cpp)
target_include_directories(occt_box PRIVATE ${OpenCASCADE_INCLUDE_DIR})
target_link_libraries(occt_box PRIVATE
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
