# NLM Export Configuration
# Configuration export for downstream usage of NLM libraries.

# Generate version file for package configuration
include(CMakePackageConfigHelpers)

# Configure export set
export(NAMESPACE nlm::
    FILE ${CMAKE_BINARY_DIR}/nlm/NLMTargets.cmake
    GLOBAL_PROPS SOME_PROPS
)

# Create a config file for the package
set(NLM_PACKAGE_CONFIG ${CMAKE_BINARY_DIR}/nlm/NLMConfig.cmake)

# Configure the config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${NLM_PACKAGE_CONFIG}
    @ONLY
)

# Write version file
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/nlm/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install the export files
install(EXPORT nlmTargets
    FILE NLMTargets.cmake
    NAMESPACE nlm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

install(FILES ${NLM_PACKAGE_CONFIG} ${CMAKE_BINARY_DIR}/nlm/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

# Create a CMake function for downstream projects
set(NLM_FUNCTIONS_FILE ${CMAKE_BINARY_DIR}/nlm/NLMFunctions.cmake)

file(WRITE ${NLM_FUNCTIONS_FILE}
"# NLM Functions for Downstream Projects
# These functions help downstream projects integrate with NLM.

# Function to add NLM to a project
function(add_nlm_project project_name)
    cmake_minimum_required(VERSION 3.16)
    
    # Find NLM package
    find_package(nlm REQUIRED)
    
    # Add the executable
    add_executable(\${project_name} \${ARGN})
    
    # Link against NLM
    target_link_libraries(\${project_name} PRIVATE nlm::nlm_core nlm::nlm_agent nlm::nlm_world)
    
    # Set include directories
    target_include_directories(\${project_name} PRIVATE \${NLM_SOURCE_DIR})
    
    # Set compiler flags
    if(MSVC)
        target_compile_options(\${project_name} PRIVATE /W4)
    else()
        target_compile_options(\${project_name} PRIVATE -Wall -Wextra)
    endif()
    
    # Set C++ standard
    set_target_properties(\${project_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
endfunction()

# Function to configure NLM for testing
function(setup_nlm_testing target_name)
    # Enable testing
    enable_testing()
    
    # Add test target
    add_test(NAME test_\${target_name}
        COMMAND \${target_name}
    )
    
    # Set test properties
    set_tests_properties(test_\${target_name} PROPERTIES
        TIMEOUT 300
        LABELS "test;\${target_name}"
    )
endfunction()
")

# Install the functions file
install(FILES ${NLM_FUNCTIONS_FILE}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

message(STATUS "Export configuration complete")
