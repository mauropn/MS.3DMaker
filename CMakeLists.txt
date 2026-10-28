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

set(MS3D_SOURCES
  src/app/main.cpp
  src/i18n/LanguageService.cpp
  src/i18n/LanguageService.h
  src/ui/MainWindow.cpp
  src/ui/MainWindow.h
  src/ui/PreferencesDialog.cpp
  src/ui/PreferencesDialog.h
)

add_executable(MS3DMaker ${MS3D_SOURCES})
target_include_directories(MS3DMaker PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(MS3DMaker PRIVATE Qt6::Widgets)

if(WIN32)
  set_target_properties(MS3DMaker PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

set(MS3D_TS_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/translations/MS3DMaker_en.ts"
  "${CMAKE_CURRENT_SOURCE_DIR}/translations/MS3DMaker_pt_BR.ts"
)

# Extract / update catalogs from sources
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

# Place .qm next to the executable for LanguageService discovery
add_custom_command(TARGET MS3DMaker POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:MS3DMaker>/translations"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${MS3D_QM_FILES} "$<TARGET_FILE_DIR:MS3DMaker>/translations"
  COMMENT "Copying translation catalogs next to MS3DMaker"
  VERBATIM
)

# Convenience targets documented in translations/CONTRIBUTING_TRANSLATIONS.md
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
endif()

install(TARGETS MS3DMaker RUNTIME DESTINATION bin)
install(FILES ${MS3D_QM_FILES} DESTINATION bin/translations)
