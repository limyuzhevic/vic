# Installation and packaging configuration for NLM project

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Configuration header generation
function(generate_config_header)
    # Generate compile_commands.json for tooling
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
    # Generate configuration header
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config/nlm_config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/nlm_config.h"
        @ONLY
    )
    
    # Add generated header to targets
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
    )
endfunction()

# Package configuration
function(setup_package_configuration)
    # Generate and install package configuration
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT OFF
    )
    
    # Generate and install package config
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    )
    
    # Install package files
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    )
    
    # Export targets for downstream usage
    install(EXPORT NLMTargets
        FILE NLMTargets.cmake
        NAMESPACE NLM::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    )
endfunction()

# Target installation
function(install_targets)
    # Libraries
    install(TARGETS nlm_core
        EXPORT NLMTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    install(TARGETS nlm_agent
        EXPORT NLMTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    install(TARGETS nlm_world
        EXPORT NLMTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    # Phase 3 and 4 libraries
    install(TARGETS nlm_phase3 nlm_phase4
        EXPORT NLMTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    # Executables
    install(TARGETS nlm
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    install(TARGETS nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endfunction()

# Header installation
function(install_headers)
    # Main headers
    install(DIRECTORY src/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
        FILES_MATCHING PATTERN "*.hpp"
        FILES_MATCHING PATTERN "*.h"
    )
    
    # Generated headers
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/nlm_config.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    )
    
    # Python bindings headers
    install(DIRECTORY python/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm/python
        FILES_MATCHING PATTERN "*.h"
        FILES_MATCHING PATTERN "*.hpp"
    )
endfunction()

# Documentation installation
function(install_documentation)
    # Install documentation
    install(DIRECTORY docs/
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        FILES_MATCHING PATTERN "*.md"
        FILES_MATCHING PATTERN "*.rst"
        FILES_MATCHING PATTERN "*.txt"
    )
    
    # Install Doxygen documentation
    if (DOXYGEN_FOUND AND TARGET docs)
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html/
            DESTINATION ${CMAKE_INSTALL_DOCDIR}/html
        )
    endif()
endfunction()

# Runtime configuration files
function(install_runtime_configs)
    # Configuration files
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/nlm
    )
    
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/configs/advanced_schema.json
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/nlm
    )
    
    # Initialize configuration template
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg.in"
        "${CMAKE_CURRENT_BINARY_DIR}/configs/default.cfg"
        @ONLY
    )
endfunction()

# Package generation
function(generate_debian_package)
    # Generate Debian package control file
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/DEBIAN/control.in"
        "${CMAKE_CURRENT_BINARY_DIR}/debian/nlm/DEBIAN/control"
        @ONLY
    )
    
    # Install files to staging area
    set(DEB_DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/debian/nlm")
    install(TARGETS nlm DESTINATION ${DEB_DEST_DIR}/usr/bin)
    install(DIRECTORY src/ DESTINATION ${DEB_DEST_DIR}/usr/include/nlm FILES_MATCHING PATTERN "*.hpp")
    
    # Add custom target for package generation
    add_custom_target(debian-package
        COMMAND dpkg-deb --build ${DEB_DEST_DIR} ${CMAKE_CURRENT_BINARY_DIR}/nlm_${PROJECT_VERSION}_all.deb
        COMMENT "Generate Debian package"
        VERBATIM
    )
endfunction()

function(generate_rpm_package)
    # Generate RPM package
    # This requires RPM build tools and additional configuration
    message(WARNING "RPM package generation requires additional setup")
endfunction()

function(generate_cmake_package)
    # Generate CMake package config
    include(CMakePackageConfigHelpers)
    
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )
    
    # Create package config
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    )
    
    # Install package files
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    )
    
    # Export targets
    install(EXPORT NLMTargets
        FILE NLMTargets.cmake
        NAMESPACE NLM::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    )
endfunction()