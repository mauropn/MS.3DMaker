cmake_minimum_required(VERSION 3.21)
project(MS3DMaker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(MS3D_BUILD_TESTS "Build MS3DMaker automated tests" ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)
if(MS3D_BUILD_TESTS)
  find_package(Qt6 REQUIRED COMPONENTS Test)
endif()

# OpenCASCADE (OCCT)
if(NOT DEFINED ENV{CASROOT})
  if(EXISTS "C:/Dev/OCCT/install")
    set(ENV{CASROOT} "C:/Dev/OCCT/install")
  elseif(EXISTS "F:/Dev/OCCT/install")
    set(ENV{CASROOT} "F:/Dev/OCCT/install")
  elseif(EXISTS "$ENV{HOME}/Dev/OCCT/install")
    set(ENV{CASROOT} "$ENV{HOME}/Dev/OCCT/install")
  endif()
endif()
if(DEFINED ENV{CASROOT} AND NOT OpenCASCADE_DIR)
  foreach(_d
      "$ENV{CASROOT}/cmake"
      "$ENV{CASROOT}/lib/cmake/opencascade"
      "$ENV{CASROOT}/lib/cmake/OpenCASCADE")
    if(EXISTS "${_d}/OpenCASCADEConfig.cmake")
      set(OpenCASCADE_DIR "${_d}" CACHE PATH "OCCT CMake package dir" FORCE)
      break()
    endif()
  endforeach()
endif()
find_package(OpenCASCADE REQUIRED)

set(MS3D_SOURCES
  src/app/main.cpp
  src/features/FeatureHistory.cpp
  src/features/FeatureHistory.h
  src/features/FeatureTypes.h
  src/geometry/SolidFactory.cpp
  src/geometry/SolidFactory.h
  src/i18n/LanguageService.cpp
  src/i18n/LanguageService.h
  src/sketch/Sketch.cpp
  src/sketch/Sketch.h
  src/sketch/SketchTypes.h
  src/ui/FeatureEditDialog.cpp
  src/ui/FeatureEditDialog.h
  src/ui/MainWindow.cpp
  src/ui/MainWindow.h
  src/ui/PreferencesDialog.cpp
  src/ui/PreferencesDialog.h
  src/viewport/OcctView.cpp
  src/viewport/OcctView.h
)

add_executable(MS3DMaker ${MS3D_SOURCES})
target_include_directories(MS3DMaker PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  ${OpenCASCADE_INCLUDE_DIR}
)
target_link_libraries(MS3DMaker PRIVATE
  Qt6::Widgets
  TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo TKPrim
  TKBO TKBool TKShHealing
  TKService TKV3d TKOpenGl
)
if(WIN32)
  target_link_libraries(MS3DMaker PRIVATE user32 gdi32 opengl32)
  set_target_properties(MS3DMaker PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

set(MS3D_TS_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/translations/MS3DMaker_en.ts"
  "${CMAKE_CURRENT_SOURCE_DIR}/translations/MS3DMaker_pt_BR.ts"
)

qt_add_lupdate(MS3DMaker
  TS_FILES ${MS3D_TS_FILES}
  SOURCE_TARGETS MS3DMaker
)

set(MS3D_QM_DIR "${CMAKE_CURRENT_BINARY_DIR}/translations")
file(MAKE_DIRECTORY "${MS3D_QM_DIR}")

qt_add_lrelease(MS3DMaker
  TS_FILES ${MS3D_TS_FILES}
  QM_FILES_OUTPUT_VARIABLE MS3D_QM_FILES
)

add_custom_command(TARGET MS3DMaker POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:MS3DMaker>/translations"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${MS3D_QM_FILES} "$<TARGET_FILE_DIR:MS3DMaker>/translations"
  COMMENT "Copying translation catalogs next to MS3DMaker"
  VERBATIM
)

# Copy OCCT runtime DLLs on Windows so the app runs without a pre-set PATH
if(WIN32 AND DEFINED OpenCASCADE_BINARY_DIR)
  set(_ms3d_occt_runtime_dlls
    TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo TKPrim
    TKService TKV3d TKOpenGl TKMesh TKShHealing TKBO TKBool TKHLR
  )
  foreach(_lib IN LISTS _ms3d_occt_runtime_dlls)
    if(EXISTS "${OpenCASCADE_BINARY_DIR}/${_lib}.dll")
      add_custom_command(TARGET MS3DMaker POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${OpenCASCADE_BINARY_DIR}/${_lib}.dll"
          "$<TARGET_FILE_DIR:MS3DMaker>"
        VERBATIM
      )
    endif()
  endforeach()
endif()

add_custom_target(ms3d_lupdate DEPENDS MS3DMaker_lupdate)
add_custom_target(ms3d_lrelease DEPENDS MS3DMaker_lrelease)

if(MS3D_BUILD_TESTS)
  enable_testing()

  add_executable(test_language_service
    tests/test_language_service.cpp
    src/i18n/LanguageService.cpp
    src/i18n/LanguageService.h
  )
  target_include_directories(test_language_service PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
  target_compile_definitions(test_language_service PRIVATE MS3D_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
  target_link_libraries(test_language_service PRIVATE Qt6::Test Qt6::Core)
  add_custom_command(TARGET test_language_service POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:test_language_service>/translations"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${MS3D_QM_FILES} "$<TARGET_FILE_DIR:test_language_service>/translations"
    VERBATIM
  )
  add_dependencies(test_language_service MS3DMaker_lrelease)
  add_test(NAME language_service COMMAND test_language_service)

  add_executable(test_geometry
    tests/test_geometry.cpp
    src/geometry/SolidFactory.cpp
    src/geometry/SolidFactory.h
  )
  target_include_directories(test_geometry PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${OpenCASCADE_INCLUDE_DIR}
  )
  target_link_libraries(test_geometry PRIVATE
    Qt6::Test
    TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo TKPrim
    TKBO TKBool TKShHealing
  )
  if(WIN32 AND DEFINED OpenCASCADE_BINARY_DIR)
    foreach(_lib TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo TKPrim TKBO TKBool TKShHealing)
      if(EXISTS "${OpenCASCADE_BINARY_DIR}/${_lib}.dll")
        add_custom_command(TARGET test_geometry POST_BUILD
          COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${OpenCASCADE_BINARY_DIR}/${_lib}.dll"
            "$<TARGET_FILE_DIR:test_geometry>"
          VERBATIM
        )
      endif()
    endforeach()
  endif()
  add_test(NAME geometry COMMAND test_geometry)

  add_executable(test_sketch
    tests/test_sketch.cpp
    src/sketch/Sketch.cpp
    src/sketch/Sketch.h
    src/sketch/SketchTypes.h
  )
  target_include_directories(test_sketch PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${OpenCASCADE_INCLUDE_DIR}
  )
  target_link_libraries(test_sketch PRIVATE
    Qt6::Test
    TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo
  )
  if(WIN32 AND DEFINED OpenCASCADE_BINARY_DIR)
    foreach(_lib TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo)
      if(EXISTS "${OpenCASCADE_BINARY_DIR}/${_lib}.dll")
        add_custom_command(TARGET test_sketch POST_BUILD
          COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${OpenCASCADE_BINARY_DIR}/${_lib}.dll"
            "$<TARGET_FILE_DIR:test_sketch>"
          VERBATIM
        )
      endif()
    endforeach()
  endif()
  add_test(NAME sketch COMMAND test_sketch)

  add_executable(test_feature_history
    tests/test_feature_history.cpp
    src/features/FeatureHistory.cpp
    src/features/FeatureHistory.h
    src/features/FeatureTypes.h
    src/geometry/SolidFactory.cpp
    src/geometry/SolidFactory.h
  )
  target_include_directories(test_feature_history PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${OpenCASCADE_INCLUDE_DIR}
  )
  target_link_libraries(test_feature_history PRIVATE
    Qt6::Test
    TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo TKPrim
    TKBO TKBool TKShHealing
  )
  if(WIN32 AND DEFINED OpenCASCADE_BINARY_DIR)
    foreach(_lib TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo TKPrim TKBO TKBool TKShHealing)
      if(EXISTS "${OpenCASCADE_BINARY_DIR}/${_lib}.dll")
        add_custom_command(TARGET test_feature_history POST_BUILD
          COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${OpenCASCADE_BINARY_DIR}/${_lib}.dll"
            "$<TARGET_FILE_DIR:test_feature_history>"
          VERBATIM
        )
      endif()
    endforeach()
  endif()
  add_test(NAME feature_history COMMAND test_feature_history)
endif()

install(TARGETS MS3DMaker RUNTIME DESTINATION bin)
install(FILES ${MS3D_QM_FILES} DESTINATION bin/translations)
