if (CMAKE_VERSION VERSION_GREATER 3.10 OR CMAKE_VERSION VERSION_EQUAL 3.10)
    # Use include_guard() added in cmake 3.10
    include_guard()
endif()

include(CMakePackageConfigHelpers)

function(jsoncfg_get_version out_arg_version)
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/jsoncfg.hpp" jsoncfg_version_define
        REGEX "#define JSONCFG_VERSION_(MAJOR|MINOR|PATCH)")
    foreach(ver ${jsoncfg_version_define})
        if(ver MATCHES "#define JSONCFG_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
            set(JSONCFG_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
        endif()
    endforeach()
    set(VERSION ${JSONCFG_VERSION_MAJOR}.${JSONCFG_VERSION_MINOR}.${JSONCFG_VERSION_PATCH})

    if (CMAKE_VERSION VERSION_GREATER 3.15 OR CMAKE_VERSION VERSION_EQUAL 3.15)
        message(DEBUG "jsoncfg version ${VERSION}")
    else()
        message(STATUS "jsoncfg version ${VERSION}")
    endif()
    set(${out_arg_version} ${VERSION} PARENT_SCOPE)
endfunction()

function(jsoncfg_install)
    if(CMAKE_LIBRARY_ARCHITECTURE)
        string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
    else()
        # On some systems (e.g. NixOS), `CMAKE_LIBRARY_ARCHITECTURE` can be empty
        set(CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_DATAROOTDIR}")
    endif()
    if(${CMAKE_VERSION} VERSION_GREATER "3.14")
        set(OPTIONAL_ARCH_INDEPENDENT "ARCH_INDEPENDENT")
    endif()
    set(JSONCFG_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/jsoncfg"

    CACHE STRING "Installation directory for cmake files, relative to ${CMAKE_INSTALL_PREFIX}.")
    set(version_config "${PROJECT_BINARY_DIR}/jsoncfg-config-version.cmake")
    set(project_config "${PROJECT_BINARY_DIR}/jsoncfg-config.cmake")
    set(targets_export_name jsoncfg-targets)
    set(package_template_dir "${PROJECT_SOURCE_DIR}/cmake")

    # Generate the version, config and target files into the build directory.
    write_basic_package_version_file(
        ${version_config}
        VERSION ${VERSION}
        COMPATIBILITY AnyNewerVersion
        ${OPTIONAL_ARCH_INDEPENDENT}
    )
    configure_package_config_file(
        ${package_template_dir}/jsoncfg-config.cmake.in
        ${project_config}
        INSTALL_DESTINATION ${JSONCFG_CMAKE_DIR}
    )
    export(TARGETS jsoncfg NAMESPACE jsoncfg::
        FILE ${PROJECT_BINARY_DIR}/${targets_export_name}.cmake
    )

    # Install version, config and target files.
    install(
        FILES ${project_config} ${version_config}
        DESTINATION ${JSONCFG_CMAKE_DIR})
        install(EXPORT ${targets_export_name} DESTINATION ${JSONCFG_CMAKE_DIR}
        NAMESPACE jsoncfg::
    )

    # Install the header file and export the target
    install(TARGETS jsoncfg EXPORT ${targets_export_name} DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES ${PROJECT_SOURCE_DIR}/include/jsoncfg.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VENDOR "jsoncfg developers")
    set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
    set(CPACK_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_MAINTAINER}")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_NAME}-dev")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6-dev")
    set(CPACK_DEBIAN_PACKAGE_SUGGESTS "cmake, pkg-config, pkg-conf")

    set(CPACK_RPM_PACKAGE_NAME "lib${PROJECT_NAME}-devel")
    set(CPACK_RPM_PACKAGE_SUGGESTS "${CPACK_DEBIAN_PACKAGE_SUGGESTS}")

    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_RPM_COMPONENT_INSTALL ON)
    set(CPACK_NSIS_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")

    set(PKG_CONFIG_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")
    configure_file("${package_template_dir}/pkgconfig.pc.in" "${PKG_CONFIG_FILE_NAME}" @ONLY)
    install(FILES "${PKG_CONFIG_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/pkgconfig"
    )

    include(CPack)
endfunction(jsoncfg_install)
