cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(jsoncfg)

jsoncfg_get_version(VERSION)

project(jsoncfg VERSION ${VERSION} LANGUAGES CXX)
set("PROJECT_DESCRIPTION" "A header-only json tool")
set("PROJECT_HOMEPAGE_URL" "https://github.com/hyforgh/jsoncfg")

include(GNUInstallDirs)

set(JSONCFG_STANDALONE_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(JSONCFG_STANDALONE_PROJECT ON)
endif()

option(JSONCFG_BUILD_EXAMPLES "Set to ON to build examples" ${JSONCFG_STANDALONE_PROJECT})
option(JSONCFG_BUILD_TESTS "Set to ON to build tests" ${JSONCFG_STANDALONE_PROJECT})
option(JSONCFG_ENABLE_INSTALL "Set to ON to generate install target" ${JSONCFG_STANDALONE_PROJECT})

add_library(jsoncfg INTERFACE)
add_library(jsoncfg::jsoncfg ALIAS jsoncfg)
add_subdirectory(include)

if (JSONCFG_ENABLE_INSTALL)
    jsoncfg_install()
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

if (JSONCFG_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (JSONCFG_BUILD_TESTS)
    add_subdirectory(tests)
endif()
