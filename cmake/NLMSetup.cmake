# Modern CMake setup utility for NLM
# Provides common configuration and setup functions

# Function to set up compiler options for a target
function(setup_compile_options target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target_name} PRIVATE 
            -Wall -Wextra -Wpedantic -Werror
            -Wno-unused-parameter -Wno-unused-variable
        )
    endif()

    # Build type specific optimization flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target_name} PRIVATE -g -O0)
        target_compile_definitions(${target_name} PRIVATE NLM_DEBUG=1)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target_name} PRIVATE -O3 -DNDEBUG)
        target_link_options(${target_name} PRIVATE -O3)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_options(${target_name} PRIVATE -O2 -g)
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        target_compile_options(${target_name} PRIVATE -Os -DNDEBUG)
    endif()

    # Feature-specific options
    if(NLM_ENABLE_COVERAGE)
        target_compile_options(${target_name} PRIVATE --coverage)
        target_link_options(${target_name} PRIVATE --coverage)
    endif()

    if(NLM_ENABLE_SANITIZERS)
        target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined)
        target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

# Function to configure project for a specific platform
function(setup_platform target_name)
    if(NLM_PLATFORM_WINDOWS)
        target_compile_definitions(${target_name} PRIVATE _WINDOWS)
        target_link_libraries(${target_name} PRIVATE kernel32 user32)
    elseif(NLM_PLATFORM_MACOS)
        target_compile_definitions(${target_name} PRIVATE _APPLE)
    elseif(NLM_PLATFORM_UNIX)
        target_compile_definitions(${target_name} PRIVATE _UNIX)
        target_link_libraries(${target_name} PRIVATE pthread dl)
    endif()
endfunction()

# Function to create component-based installation
macro(install_component target_name component_name)
    install(TARGETS ${target_name}
        EXPORT NLMTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        COMPONENT ${component_name}
    )
endmacro()

# Function to configure dependency injection
function(add_nlm_dependency target_name dependency)
    target_link_libraries(${target_name} PUBLIC NLM::${dependency})
endfunction()

# Function to check for required dependencies
function(check_dependency dependency_name var_name)
    if(NOT ${var_name})
        message(FATAL_ERROR "Required dependency ${dependency_name} not found")
    endif()
endfunction()

# Print configuration summary
function(print_config_summary)
    message(STATUS "")
    message(STATUS "=== NLM Configuration Summary ===")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "Build docs: ${BUILD_DOCS}")
    message(STATUS "Build tests: ${BUILD_TESTS}")
    message(STATUS "Build examples: ${BUILD_EXAMPLES}")
    message(STATUS "Build demos: ${BUILD_DEMOS}")
    message(STATUS "Build shared libs: ${BUILD_SHARED_LIBS}")
    message(STATUS "Platform: ")
    if(NLM_PLATFORM_WINDOWS)
        message(STATUS "  Windows")
    elseif(NLM_PLATFORM_MACOS)
        message(STATUS "  macOS")
    elseif(NLM_PLATFORM_UNIX)
        message(STATUS "  Unix/Linux")
    endif()
    message(STATUS "===========================")
    message(STATUS "")
endfunction()

# Apply configuration summary
print_config_summary()
