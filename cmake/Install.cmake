# Installation Configuration Module
# Provides comprehensive installation support for NLM build system.

# Installation configuration
include(GNUInstallDirs)

# Installation options
set(NLM_INSTALL_INCDIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for headers")
set(NLM_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
set(NLM_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
set(NLM_INSTALL_DATADIR ${CMAKE_INSTALL_DATAROOTDIR} CACHE PATH "Installation directory for data files")

# Python installation configuration
set(NLM_PYTHON_PACKAGE_DIR ${NLM_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages CACHE PATH "Python package installation directory")

# Documentation installation configuration
set(NLM_DOCDIR ${CMAKE_INSTALL_DOCDIR} CACHE PATH "Documentation installation directory")

# Build configuration documentation
set(NLM_BUILD_DOCS_DIR ${CMAKE_BINARY_DIR}/docs CACHE PATH "Build directory for documentation")

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${NLM_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${NLM_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
)

# Phase experiment libraries installation (optional)
if(NLM_ENABLE_PHASE3_EXPERIMENTS)
    install(TARGETS nlm_phase3
        EXPORT NLMTargets
        ARCHIVE DESTINATION ${NLM_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${NLM_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
    )
endif()

if(NLM_ENABLE_PHASE4_EXPERIMENTS)
    install(TARGETS nlm_phase4
        EXPORT NLMTargets
        ARCHIVE DESTINATION ${NLM_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${NLM_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
    )
endif()

# Executable installation
install(TARGETS nlm
    RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
)

if(NLM_ENABLE_PHASE3_EXPERIMENTS)
    install(TARGETS nlm_phase3_demo
        RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
    )
endif()

if(NLM_ENABLE_PHASE4_EXPERIMENTS)
    install(TARGETS nlm_phase4_demo
        RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
    )
endif()

# Header files installation
install(DIRECTORY ${NLM_SOURCE_DIR}/core/
    DESTINATION ${NLM_INSTALL_INCDIR}/nlm/core
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/brain/
    DESTINATION ${NLM_INSTALL_INCDIR}/nlm/brain
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/sensory/
    DESTINATION ${NLM_INSTALL_INCDIR}/nlm/sensory
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/motor/
    DESTINATION ${NLM_INSTALL_INCDIR}/nlm/motor
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/environment/
    DESTINATION ${NLM_INSTALL_INCDIR}/nlm/environment
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/memory/
    DESTINATION ${NLM_INSTALL_INCDIR}/nlm/memory
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Configuration files installation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg
    DESTINATION ${NLM_INSTALL_DATADIR}/nlm
)

# Documentation installation
if(NLM_INSTALL_DOCS)
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/HOW_TO_USE.md
        ${CMAKE_CURRENT_SOURCE_DIR}/easy_usage.md
        DESTINATION ${NLM_DOCDIR}/nlm
    )
    
    install(DIRECTORY docs/
        DESTINATION ${NLM_DOCDIR}/nlm
    )
endif()

# Python bindings installation
if(NLM_INSTALL_PYTHON_MODULE AND TARGET pynlm)
    # Determine Python site-packages directory
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import site; print(site.getsitepackages()[0])"
        OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(PYTHON_SITE_PACKAGES)
        install(TARGETS pynlm
            LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}/nlm
            ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES}/nlm
            RUNTIME DESTINATION ${PYTHON_SITE_PACKAGES}/nlm
        )
        
        # Install Python package structure
        install(DIRECTORY ${CMAKE_BINARY_DIR}/pynlm/
            DESTINATION ${PYTHON_SITE_PACKAGES}
            PATTERN "*.py"
        )
    endif()
endif()

# CMake configuration files export
include(CMakePackageConfigHelpers)

# Generate and install NLMConfig.cmake for downstream usage
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_BINARY_DIR}/nlm/NLMConfig.cmake
    INSTALL_DESTINATION ${NLM_INSTALL_LIBDIR}/cmake/nlm
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/nlm/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install CMake package configuration
install(FILES
    ${CMAKE_BINARY_DIR}/nlm/NLMConfig.cmake
    ${CMAKE_BINARY_DIR}/nlm/NLMConfigVersion.cmake
    DESTINATION ${NLM_INSTALL_LIBDIR}/cmake/nlm
)

# Export targets for downstream usage
install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE nlm::
    DESTINATION ${NLM_INSTALL_LIBDIR}/cmake/nlm
)

# Create package scripts for development convenience
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-setup.cmake.in
    ${CMAKE_BINARY_DIR}/nlm-setup.cmake
    @ONLY
)

# Development installation (for developers who want to build from source)
install(EXPORT NLMTargets
    FILE NLMDevTargets.cmake
    NAMESPACE nlm_dev::
    DESTINATION ${NLM_INSTALL_LIBDIR}/cmake/nlm-dev
)

# Create development convenience scripts
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DevelopmentSetup.cmake.in
    ${CMAKE_BINARY_DIR}/DevelopmentSetup.cmake
    @ONLY
)

message(STATUS "Installation configuration complete:")
message(STATUS "  Include directory: ${NLM_INSTALL_INCDIR}")
message(STATUS "  Library directory: ${NLM_INSTALL_LIBDIR}")
message(STATUS "  Binary directory: ${NLM_INSTALL_BINDIR}")
message(STATUS "  Data directory: ${NLM_INSTALL_DATADIR}")
message(STATUS "  Python package: ${NLM_PYTHON_PACKAGE_DIR}")
message(STATUS "  Documentation directory: ${NLM_DOCDIR}")
