# CPack Configuration for NLM
# This file configures package generation using CPack for creating distributions
# Supports DEB, RPM, and TGZ packages

cmake_minimum_required(VERSION 3.16)

# Package metadata
set(CPACK_PACKAGE_NAME "NLM")
set(CPACK_PACKAGE_VENDOR "NLM Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neural Learning Machine - Advanced brain simulation framework")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_CONTACT "support@nlm.org")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

# Package identifier
set(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
set(CPACK_PACKAGE_URL "https://github.com/nlm/nlm")

# Distribution-specific configuration

# DEB package configuration
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Team")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.28), libstdc++6 (>= 9), libboost-system1.67.0, libboost-filesystem1.67.0")

# RPM package configuration
set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_RELEASE_DIST ".el8")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_VENDOR "${CPACK_PACKAGE_VENDOR}")
set(CPACK_RPM_PACKAGE_ARCH "x86_64")
set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.28, libstdc++ >= 9, boost-system >= 1.67.0, boost-filesystem >= 1.67.0")

# TGZ package configuration
set(CPACK_GENERATOR "TGZ")

# Installation prefix
set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Install components for selective packaging
include(CPackComponent)

# Core component
cpack_component(core
    DISPLAY_NAME "Core Libraries"
    DESCRIPTION "Core NLM libraries (nlm_core, nlm_agent, nlm_world, nlm_phase3, nlm_phase4)"
    GROUP "Runtime"
)

# Executable component
cpack_component(executables
    DISPLAY_NAME "Executables"
    DESCRIPTION "NLM executables (nlm, nlm_phase3_demo, nlm_phase4_demo, nlm_advanced_cli)"
    GROUP "Runtime"
)

# Python bindings component
cpack_component(python_bindings
    DISPLAY_NAME "Python Bindings"
    DESCRIPTION "Python bindings for NLM (pynlm)"
    GROUP "Runtime"
)

# Tests component
cpack_component(tests
    DISPLAY_NAME "Tests"
    DESCRIPTION "NLM test executables and data"
    GROUP "Development"
)

# Documentation component
cpack_component(docs
    DISPLAY_NAME "Documentation"
    DESCRIPTION "NLM documentation and examples"
    GROUP "Development"
)

# Configure component installation
cpack_set_install_variant(components "core executables")
cpack_set_install_variant(docs "core")
cpack_set_install_variant(tests "components")

# Set default installation component
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")

# Output directories
set(CPACK_OUTPUT_DIR "${CMAKE_BINARY_DIR}/package")

# Package file names
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")

# File naming
function(set_package_names)
    # Set package names based on generator
    if(CPACK_GENERATOR STREQUAL "DEB")
        set(CPACK_DEBIAN_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_SYSTEM_NAME}.deb")
    elseif(CPACK_GENERATOR STREQUAL "RPM")
        set(CPACK_RPM_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.el${CPACK_RPM_PACKAGE_RELEASE_DIST}.x86_64.rpm")
    endif()
endfunction()

# Installation validation function
function(validate_installation)
    message(STATUS "Validating installation...")
    
    # Check for critical files
    set(required_files
        "${CMAKE_CURRENT_BINARY_DIR}/lib/libnlm_core.a"
        "${CMAKE_CURRENT_BINARY_DIR}/include/nlm/"
    )
    
    foreach(file IN LISTS required_files)
        if(NOT EXISTS "${file}")
            message(WARNING "Expected file not found: ${file}")
        endif()
    endforeach()
    
    # Check for required targets
    if(NOT TARGET nlm_core)
        message(WARNING "Target nlm_core not found")
    endif()
    
    message(STATUS "Installation validation completed")
endfunction()

# Performance optimization
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CPACK_STRIP_FILES TRUE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type for packaging" FORCE)
endif()

# Custom packaging script for additional files
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/package_scripts")
    file(GLOB package_scripts "${CMAKE_CURRENT_SOURCE_DIR}/package_scripts/*.sh")
    foreach(script IN LISTS package_scripts)
        add_custom_target(postinstall-${script}
            COMMAND bash ${script}
            COMMENT "Running post-install script: ${script}"
            VERBATIM
        )
    endforeach()
endif()

# Add components to CPACK
if(NLM_BUILD_TESTS)
    cpack_add_component(tests)
endif()

if(NLM_BUILD_PYTHON_BINDINGS)
    cpack_add_component(python_bindings)
endif()

# Finalize configuration
include(CPack)

# Installation verification
if(NLM_ENABLE_CPACK)
    install(CODE "validate_installation()")
endif()

message(STATUS "CPack configuration completed")
