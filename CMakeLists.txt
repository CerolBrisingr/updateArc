cmake_minimum_required(VERSION 3.25) # CMP0079, allow target_link_libraries to access other directories
cmake_policy(SET CMP0079 NEW)
project(UpdateArc VERSION 8.0.1 LANGUAGES CXX)

# Building
set(CPACK_PACKAGE_NAME "UpdateArc")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}_v${PROJECT_VERSION}_win64")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(Updater_BUILD_TESTS "Build unit tests" ON)
if(Updater_BUILD_TESTS)
endif()

# Easy setup for compiler flags (avoid using in /libs)
add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Werror>
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "Install path" FORCE)
configure_file("${CMAKE_CURRENT_LIST_DIR}/resources/arc.ico" "${CMAKE_CURRENT_BINARY_DIR}/arc.ico" COPYONLY)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/version.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
    @ONLY
)

include(functions.cmake)
set(VERSION_FILES "${CMAKE_CURRENT_BINARY_DIR}/version.rc")
add_subdirectory(app)

if(Updater_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
