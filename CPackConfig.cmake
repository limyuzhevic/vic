# CPack Configuration File
# Comprehensive packaging configuration for NLM

set(CPACK_PACKAGE_NAME "NLM")
set(CPACK_PACKAGE_VENDOR "NLM Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neural Learning Machine - Neural Simulation Framework")
set(CPACK_PACKAGE_DESCRIPTION "NLM (Neural Learning Machine) is a comprehensive neural simulation framework with advanced features including event-driven dynamics, integrated memory systems, prediction and planning, neuromodulation, development, and curiosity-driven learning.")
set(CPACK_PACKAGE_CONTACT "nlm-authors@example.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_LICENSE "MIT")
set(CPACK_PACKAGE_COPYRIGHT "Copyright 2024 NLM Authors")

# Package metadata
set(CPACK_PACKAGE_IS_GENERATOR "ON")
set(CPACK_GENERATOR "TGZ;ZIP;NSIS;DMG;DEB;RPM")

# Package filenames
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")

# Source package configuration
if(CPACK_SOURCE_GENERATOR)
    set(CPACK_SOURCE_IGNORE_FILES "build/;\.git/;\.gitignore;.DS_Store;.git/;")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")
endif()

# Platform-specific configurations
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP;MSI")
    set(CPACK_NSIS_DISPLAY_NAME "NLM Neural Learning Machine")
    set(CPACK_NSIS_CONTACT "nlm-authors@example.com")
    set(CPACK_NSIS_PACKAGE_NAME "NLM")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/nlm-project/nlm")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/docs/favicon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/docs/favicon.ico")
    
    # Windows-specific settings
    set(CPACK_WIX_UPWARDS_COMPATIBILITY_RANGE "[8.0, 10.0]")
    set(CPACK_WIX_PRODUCT_GUID "{C1C2E3A4-5B6C-7D8E-9F0A-1B2C3D4E5F6A}")
    set(CPACK_WIX_PACKAGE_NAME "NLM${PROJECT_VERSION}")
    set(CPACK_WIX_PACKAGE_DESCRIPTION "Neural Learning Machine neural simulation framework")
    set(CPACK_WIX_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")
    set(CPACK_WIX_ROOT "C:\\Program Files\\NLM")
    
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;ZIP;TGZ")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_DMG_VOLUME_NAME "NLM-${PROJECT_VERSION}")
    set(CPACK_DMG_DS_STORE ""
    set(CPACK_DMG_BACKGROUND "docs/dmg_background.png")
    set(CPACK_DMG_BACKGROUND_ALIGNMENT "c")
    
    # macOS-specific settings
    set(CPACK_BUNDLE_NAME "NLM")
    set(CPACK_BUNDLE_ID "com.nlm-project.nlm")
    set(CPACK_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(CPACK_MACOSX_PKGID "com.nlm-project.nlm")
    set(CPACK_PKG_NAME "NLM")
    set(CPACK_PKG_DESCRIPTION "Neural Learning Machine neural simulation framework")
    set(CPACK_PACKAGE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/docs/Info.plist")
    
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    
    # DEB package configuration
    set(CPACK_DEBIAN_PACKAGE_NAME "nlm")
    set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Authors <nlm-authors@example.com>")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Neural Learning Machine neural simulation framework")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION_SUMMARY "Comprehensive neural simulation framework")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_PROVIDES "nlm-core")
    set(CPACK_DEBIAN_PACKAGE_REPLACES "nlm, nlm-core")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "python3, python3-numpy, python3-pybind11")
    
    # RPM package configuration
    set(CPACK_RPM_PACKAGE_NAME "nlm")
    set(CPACK_RPM_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_RPM_PACKAGE_RELEASE "1")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "noarch")
    set(CPACK_RPM_PACKAGE_VENDOR "NLM Project")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "Neural Learning Machine neural simulation framework")
    set(CPACK_RPM_PACKAGE_REQUIRES "python3, python3-numpy, python3-pybind11")
    set(CPACK_RPM_PACKAGE_GROUP "Science")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/nlm-project/nlm")
endif()

# Installation paths
include(GNUInstallDirs)

set(CPACK_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/NLM")
set(CPACK_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(CPACK_INSTALL_DATAROOTDIR "${CMAKE_INSTALL_DATAROOTDIR}")

# Package contents
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/filelist.txt
    "nlm;${CMAKE_INSTALL_BINDIR}/nlm|nlm_phase3_demo;${CMAKE_INSTALL_BINDIR}/nlm_phase3_demo|nlm_phase4_demo;${CMAKE_INSTALL_BINDIR}/nlm_phase4_demo"
)

if(NLM_INSTALL_PYTHON_BINDINGS)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/filelist.txt
        "nlm*;${Python_SITEARCH}/|py.typed;${Python_SITEARCH}/nlm/"
    )
endif()

# Install configuration files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    DESTINATION ${CPACK_INSTALL_DATAROOTDIR}/licenses/nlm
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION ${CPACK_INSTALL_DATAROOTDIR}/doc/nlm
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/InstallationGuide.md
    DESTINATION ${CPACK_INSTALL_DATAROOTDIR}/doc/nlm
)

# Install examples and scripts
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/
    DESTINATION ${CPACK_INSTALL_DATAROOTDIR}/nlm/examples
    FILES_MATCHING PATTERN "*.cpp" PATTERN "*.md" PATTERN "*.txt"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/nlm
    FILES_MATCHING PATTERN "*.sh" PATTERN "*.py"
    USE_SOURCE_PERMISSIONS
)

# Install configuration files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg
    DESTINATION ${CPACK_INSTALL_DATAROOTDIR}/nlm/configs
)

# Create package configuration files
include(CMakePackageConfigHelpers)

# Generate and install the NLMConfig.cmake file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION ${CPACK_INSTALL_CMAKEDIR}
    PATH_VARS CPACK_INSTALL_INCLUDEDIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CPACK_INSTALL_CMAKEDIR}
)

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${CPACK_INSTALL_CMAKEDIR}
)

# Package information files
set(CPACK_PACKAGING_INSTALL_PREFIX "${CPACK_INSTALL_PREFIX}")
set(CPACK_PACKAGING_INSTALL_PREFIX "C:/Program Files/NLM")

# Package scripts
if(CPACK_GENERATOR STREQUAL "NSIS")
    # NSIS installation script
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/nsis/install.nsh
        ${CMAKE_CURRENT_BINARY_DIR}/install.nsh @ONLY)
    
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/nsis/uninstall.nsh
        ${CMAKE_CURRENT_BINARY_DIR}/uninstall.nsh @ONLY)
endif()

# Package metadata
if(CPACK_GENERATOR STREQUAL "TGZ")
    set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
endif()

if(CPACK_GENERATOR STREQUAL "ZIP")
    set(CPACK_ZIP_COMPONENT_INSTALL ON)
endif()

# Package file associations
if(WIN32)
    file(GLOB WIX_FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/wix/*.wxs")
    foreach(WIX_FILE ${WIX_FILES})
        file(RELATIVE_PATH REL_WIX_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${WIX_FILE})
        install(FILES ${WIX_FILE}
            DESTINATION ${CPACK_INSTALL_DATAROOTDIR}/nlm/wix
        )
    endforeach()
endif()

# Package dependencies
if(DEFINED CPACK_PACKAGE_DEPENDS)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_PACKAGE_DEPENDS}")
    set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_PACKAGE_DEPENDS}")
endif()

# Packaging target
set(CPACK_PACKAGE_INSTALL_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/install.cmake")
file(WRITE ${CPACK_PACKAGE_INSTALL_SCRIPT}
    "# Post-install script for NLM
    if(WIN32)
        execute_process(COMMAND regsvr32 \"${CMAKE_INSTALL_PREFIX}//bin/nlm.dll\" )
    endif()
    ")

# Package verification
add_custom_target(package-verify
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/verify.cmake
    DEPENDS package
    COMMENT "Verify package installation"
)

# Package documentation target
add_custom_target(package-docs
    COMMAND ${CMAKE_COMMAND} --build . --target doc
    COMMAND ${CMAKE_COMMAND} --build . --target documentation
    COMMENT "Generate and package documentation"
    DEPENDS package
)

# Package validation script
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/verify.cmake
    "# Package verification script
    message(STATUS \"Verifying package installation...\")
    
    # Check for required files
    foreach(REQUIRED_FILE \"${CMAKE_CURRENT_BINARY_DIR}/filelist.txt\")
        if(NOT EXISTS \"${REQUIRED_FILE}\")
            message(FATAL_ERROR \"Missing required file: ${REQUIRED_FILE}\")
        endif()
    endforeach()
    
    # Check CMake configuration
    if(NOT EXISTS \"${CMAKE_INSTALL_PREFIX}//include/nlm/nlm.h\")
        message(WARNING \"NLM header files not found - installation may be incomplete\")
    endif()
    
    message(STATUS \"Package verification complete\")
    ")

# Package content summary
add_custom_target(package-summary
    COMMAND echo \"Package Contents:\"
    COMMAND echo \"  - Libraries: nlm_core, nlm_agent, nlm_world, nlm_phase3, nlm_phase4\"
    COMMAND echo \"  - Executables: nlm, nlm_phase3_demo, nlm_phase4_demo\"
    COMMAND echo \"  - Python bindings: nlm (Python module)\"
    COMMAND echo \"  - Documentation: HTML, PDF, API reference\"
    COMMAND echo \"  - Examples: Neural simulation examples\"
    COMMAND echo \"  - Configuration: Sample configuration files\"
    COMMENT "Display package contents summary"
)

# Package metadata
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/package_info.txt
    "Package: NLM (Neural Learning Machine)
    Version: ${PROJECT_VERSION}
    Author: NLM Authors
    Homepage: https://github.com/nlm-project/nlm
    License: MIT
    Description: Comprehensive neural simulation framework with advanced features including event-driven dynamics, integrated memory systems, prediction and planning, neuromodulation, development, and curiosity-driven learning.
    ")

message(STATUS "CPack configuration complete")
message(STATUS "Package name: ${CPACK_PACKAGE_NAME}")
message(STATUS "Package version: ${CPACK_PACKAGE_VERSION}")
message(STATUS "Package file: ${CPACK_PACKAGE_FILE_NAME}")
message(STATUS "Generators: ${CPACK_GENERATOR}")