# Visual Studio project generation script for NLM
# This script helps configure Visual Studio projects with proper CMake integration

function(generate_vs_project)
    if(WIN32)
        # Set up Visual Studio environment
        set(CMAKE_PROJECT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateVSProject.cmake")
        
        # Visual Studio-specific configurations
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "MSVC Runtime Library")
        set(CMAKE_VS_GLOBALS "EnableImportAndExportData=true")
        
        # Add Visual Studio-specific compile options
        target_compile_options(nlm_core PRIVATE /permissive- /std:c++20)
        target_compile_options(nlm_agent PRIVATE /permissive- /std:c++20)
        target_compile_options(nlm_world PRIVATE /permissive- /std:c++20)
        
        # Enable Windows exports
        target_compile_definitions(nlm_core PRIVATE "_WINDOWS")
        
        # Set up include directories for Visual Studio
        target_include_directories(nlm_core PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )
        
        message(STATUS "Visual Studio project generation enabled")
    endif()
endfunction()

# Visual Studio-specific project properties
function(set_vs_project_properties target)
    if(WIN32 AND MSVC)
        # Set Visual Studio project properties
        set_target_properties(${target} PROPERTIES
            DEBUG_INFORMATION_FORMAT "EditAndContinue"
            ENABLE_EXPORTS TRUE
            ENABLE_PRECOMPILED_HEADERS TRUE
        )
        
        # Configure deployment options
        if(WIN32)
            set_target_properties(${target} PROPERTIES
                WIN32_EXECUTABLE FALSE
                MACOSX_BUNDLE FALSE
            )
        endif()
    endif()
endfunction()

# Register VS project generation
generate_vs_project()

# Set VS project properties for all targets
foreach(target nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    if(TARGET ${target})
        set_vs_project_properties(${target})
    endif()
endforeach()

# Add Visual Studio documentation target
if(TARGET docs)
    add_custom_target(vstudio_docs
        COMMAND ${CMAKE_COMMAND} -E echo "Visual Studio documentation target available"
        DEPENDS docs
        COMMENT "Visual Studio documentation target"
    )
endif()
