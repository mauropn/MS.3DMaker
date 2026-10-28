cmake_minimum_required(VERSION 3.21)
project(occt_box LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FindMs3dOcct.cmake")

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
