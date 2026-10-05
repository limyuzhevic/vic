# Core subsystem CMakeLists.txt
# This file provides target-based configuration for the core subsystem

# Create core libraries with modern CMake practices
add_library(nlm_core_types STATIC
    Types/Types.cpp
    Types/Types.hpp
)

add_library(nlm_core_config STATIC
    Config/Config.cpp
    Config/Config.hpp
)

add_library(nlm_core_random STATIC
    Random/Random.cpp
    Random/Random.hpp
)

add_library(nlm_core_logger STATIC
    Logger/Logger.cpp
    Logger/Logger.hpp
)

add_library(nlm_core_clock STATIC
    SimulationClock/SimulationClock.cpp
    SimulationClock/SimulationClock.hpp
)

add_library(nlm_core_enhanced_config STATIC
    EnhancedConfig.cpp
    EnhancedConfig.hpp
)

add_library(nlm_core_parameter_validator STATIC
    ParameterValidator.cpp
    ParameterValidator.hpp
)

# Set target properties for core libraries
target_compile_features(nlm_core_types PUBLIC cxx_std_20)
target_compile_features(nlm_core_config PUBLIC cxx_std_20)
target_compile_features(nlm_core_random PUBLIC cxx_std_20)
target_compile_features(nlm_core_logger PUBLIC cxx_std_20)
target_compile_features(nlm_core_clock PUBLIC cxx_std_20)
target_compile_features(nlm_core_enhanced_config PUBLIC cxx_std_20)
target_compile_features(nlm_core_parameter_validator PUBLIC cxx_std_20)

# Apply compiler-specific settings
target_compile_options(nlm_core_types PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

target_compile_options(nlm_core_config PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

target_compile_options(nlm_core_random PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

target_compile_options(nlm_core_logger PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

target_compile_options(nlm_core_clock PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

target_compile_options(nlm_core_enhanced_config PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

target_compile_options(nlm_core_parameter_validator PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

# Set include directories for core libraries
target_include_directories(nlm_core_types PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(nlm_core_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(nlm_core_random PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(nlm_core_logger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(nlm_core_clock PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(nlm_core_enhanced_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(nlm_core_parameter_validator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Create main nlm_core target that aggregates all core libraries
add_library(nlm_core INTERFACE)

# Link all core libraries to the main nlm_core interface target
target_link_libraries(nlm_core INTERFACE
    nlm_core_types
    nlm_core_config
    nlm_core_random
    nlm_core_logger
    nlm_core_clock
    nlm_core_enhanced_config
    nlm_core_parameter_validator
)

# Create target aliases for convenience
target_link_libraries(nlm_core_config INTERFACE nlm_core_types)
target_link_libraries(nlm_core_random INTERFACE nlm_core_types)
target_link_libraries(nlm_core_logger INTERFACE nlm_core_types)
target_link_libraries(nlm_core_clock INTERFACE nlm_core_types)
target_link_libraries(nlm_core_enhanced_config INTERFACE nlm_core_config)
target_link_libraries(nlm_core_parameter_validator INTERFACE nlm_core_config)

# Apply platform-specific optimizations
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

apply_platform_specific_options(nlm_core_types)
apply_platform_specific_options(nlm_core_config)
apply_platform_specific_options(nlm_core_random)
apply_platform_specific_options(nlm_core_logger)
apply_platform_specific_options(nlm_core_clock)
apply_platform_specific_options(nlm_core_enhanced_config)
apply_platform_specific_options(nlm_core_parameter_validator)

# Apply compiler optimizations
function(apply_compiler_optimizations target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target_name} PRIVATE -march=native)
        target_link_libraries(${target_name} PRIVATE -fno-exceptions -fno-rtti)
    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE /arch:AVX2)
        target_compile_definitions(${target_name} PRIVATE -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
    endif()
endfunction()

apply_compiler_optimizations(nlm_core_types)
apply_compiler_optimizations(nlm_core_config)
apply_compiler_optimizations(nlm_core_random)
apply_compiler_optimizations(nlm_core_logger)
apply_compiler_optimizations(nlm_core_clock)
apply_compiler_optimizations(nlm_core_enhanced_config)
apply_compiler_optimizations(nlm_core_parameter_validator)

# Generate and install headers
generate_config_header()

# Export targets for downstream usage
export(TARGETS nlm_core_types nlm_core_config nlm_core_random nlm_core_logger
        nlm_core_clock nlm_core_enhanced_config nlm_core_parameter_validator
        FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMCoreTargets.cmake")

# Add dependency graph generation
function(generate_dependency_graph)
    find_package(Graphviz QUIET)
    if (Graphviz_FOUND)
        add_custom_target(dependency_graph
            COMMAND cmake --graphviz=${CMAKE_BINARY_DIR}/core_dependency_graph.dot
                ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
            COMMENT "Generate core dependency graph"
            VERBATIM
        )
    endif()
endfunction()

generate_dependency_graph()

# Add performance analysis targets
function(add_performance_targets)
    # Add performance benchmarks for core components
    add_executable(core_benchmark core_benchmark.cpp)
    target_link_libraries(core_benchmark PRIVATE nlm_core_types nlm_core_config)
    
    # Add memory profiling targets
    add_custom_target(memory_profile
        COMMAND valgrind --tool=massif --stack=yes ${CMAKE_BINARY_DIR}/core_benchmark
        COMMENT "Run memory profiler on core_benchmark"
        VERBATIM
    )
endfunction()

add_performance_targets()