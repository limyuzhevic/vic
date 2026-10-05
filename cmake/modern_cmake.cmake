# Modern CMake patterns for NLM project

# Function to add a library with modern CMake practices
function(nlm_add_library target_name)
    add_library(${target_name} STATIC)
    target_compile_features(${target_name} PUBLIC cxx_std_20)
    
    # Compiler-specific warnings
    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    )
    
    # Position independent code and visibility settings
    set_target_properties(${target_name} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
    
    # Export for downstream usage
    set_target_properties(${target_name} PROPERTIES
        EXPORT_NAME ${target_name}
    )
endfunction()

# Function to add an executable with modern CMake practices
function(nlm_add_executable target_name)
    add_executable(${target_name} ${ARGN})
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    target_link_libraries(${target_name} PRIVATE nlm_core)
endfunction()

# Target-based include directories
get_target_property(src_dir_src nlm_core SOURCES)
if(src_dir_src)
    get_filename_component(src_dir ${src_dir_src} DIRECTORY)
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
endif()

# Configuration-specific compile options
function(apply_build_type_options target_name)
    get_target_property(target_type ${target_name} TYPE)
    
    if(target_type STREQUAL "STATIC_LIBRARY")
        target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
            $<$<CONFIG:Release>:-O3 -DNDEBUG>
            $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
            $<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>
        )
    endif()
endfunction()

# Platform-specific configurations
function(apply_platform_specific_options target_name)
    # Linux optimizations
    target_link_libraries(${target_name} PRIVATE
        $<$<PLATFORM_ID:Linux>:-lm -ldl -pthread>
    )
    
    # Windows-specific configurations
    target_compile_definitions(${target_name} PRIVATE
        $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
    )
    
    # macOS-specific optimizations
    target_link_libraries(${target_name} PRIVATE
        $<$<PLATFORM_ID:Darwin>:-framework Accelerate>
    )
endfunction()

# Compiler-specific optimizations
function(apply_compiler_optimizations target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target_name} PRIVATE -march=native)
        target_link_libraries(${target_name} PRIVATE -fno-exceptions -fno-rtti)
    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE /arch:AVX2)
        target_compile_definitions(${target_name} PRIVATE -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
    endif()
endfunction()

# Dependency management helper
function(nlm_find_package package_name)
    find_package(${package_name} REQUIRED)
    if(${package_name}_FOUND)
        message(STATUS "Found ${package_name}: ${${package_name}_VERSION}")
    endif()
endfunction()

# Installation helper with proper namespace
function(nlm_install_targets)
    include(GNUInstallDirs)
    
    # Export targets for downstream usage
    install(EXPORT NLMTargets
        FILE NLMConfig.cmake
        NAMESPACE NLM::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    )
    
    # Package version file
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )
    
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
    )
endfunction()

# Target-based dependency management
function(nlm_target_link_libraries target_name)
    foreach(lib ${ARGN})
        target_link_libraries(${target_name} PUBLIC ${lib})
        # Propagate include directories
        target_include_directories(${target_name} PUBLIC
            $<TARGET_PROPERTY:${lib},INTERFACE_INCLUDE_DIRECTORIES>
        )
    endforeach()
endfunction()
