cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(jsonho)

jsonho_get_version(VERSION)

project(jsonho VERSION ${VERSION} LANGUAGES CXX)
set("PROJECT_DESCRIPTION" "A header-only json tool")
set("PROJECT_HOMEPAGE_URL" "https://github.com/hyforgh/jsonho")

include(GNUInstallDirs)

set(JSONHO_STANDALONE_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(JSONHO_STANDALONE_PROJECT ON)
endif()

option(JSONHO_BUILD_EXAMPLES "Set to ON to build examples" ${JSONHO_STANDALONE_PROJECT})
option(JSONHO_BUILD_TESTS "Set to ON to build tests" ${JSONHO_STANDALONE_PROJECT})
option(JSONHO_ENABLE_INSTALL "Set to ON to generate install target" ${JSONHO_STANDALONE_PROJECT})

add_library(jsonho INTERFACE)
add_library(jsonho::jsonho ALIAS jsonho)
add_subdirectory(include)

if (JSONHO_ENABLE_INSTALL)
    jsonho_install()
endif()

function(cliarg_add_inner_bin_target target_name)
    add_executable(${target_name} ${target_name}.cpp)
    message(STATUS "Building target: ${target_name}")
    target_compile_features(${target_name} PRIVATE cxx_std_11)
    target_compile_options(${target_name} PRIVATE -Wall)
    target_include_directories(${target_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction(cliarg_add_inner_bin_target target_name)

if (JSONHO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (JSONHO_BUILD_TESTS)
    add_subdirectory(tests)
endif()
