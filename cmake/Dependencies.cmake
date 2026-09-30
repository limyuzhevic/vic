# Dependency Management Module
# Provides comprehensive dependency handling for NLM build system.

# Find required system dependencies
find_package(Threads REQUIRED)

# Python development dependencies
find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development)

# Try to find pybind11 with multiple approaches
find_package(pybind11 CONFIG REQUIRED)
if(NOT pybind11_FOUND)
    # Fallback: use find_package module
    find_package(pybind11 2.11 REQUIRED)
endif()

# Optional dependencies based on configuration
if(NLM_ENABLE_TESTS)
    # Testing framework dependencies
    enable_testing()
    
    # Check for valgrind
    find_program(VALGRIND_EXECUTABLE valgrind)
    if(VALGRIND_EXECUTABLE)
        set(NLM_HAS_VALGRIND TRUE)
        message(STATUS "Valgrind found: ${VALGRIND_EXECUTABLE}")
    endif()
    
    # Check for AddressSanitizer support
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        check_cxx_compiler_flag(-fsanitize=address HAS_ASAN)
        if(HAS_ASAN)
            set(NLM_HAS_ASAN TRUE)
            message(STATUS "AddressSanitizer support available")
        endif()
    endif()
endif()

# Performance and optimization dependencies
if(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS)
    # Check for OpenMP (parallel processing)
    find_package(OpenMP)
    if(OpenMP_Found)
        set(NLM_HAS_OPENMP TRUE)
        message(STATUS "OpenMP found: ${OpenMP_CXX_FLAGS}")
    endif()
endif()

# Memory profiling tools (optional)
if(NLM_ENABLE_VALGRIND_TESTS AND VALGRIND_EXECUTABLE)
    add_custom_target(valgrind_all
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full --track-origins=yes
            $<TARGET_FILE:nlm_test_core>
            --gtest_list_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running memory leak check on unit tests"
    )
endif()

# Configuration validation
function(validate_nlm_dependencies)
    message(STATUS "Validating NLM dependencies...")
    
    # Check Python
    if(NOT Python_FOUND)
        message(FATAL_ERROR "Python development components not found")
    endif()
    
    # Check pybind11
    if(NOT pybind11_FOUND)
        message(FATAL_ERROR "pybind11 not found - required for Python bindings")
    endif()
    
    # Check compiler features
    if(CMAKE_CXX_STANDARD LESS 20)
        message(FATAL_ERROR "C++ standard must be at least C++20")
    endif()
    
    # Check for required compiler features
    check_cxx_compiler_flag(-std=c++20 COMPILER_HAS_CPP20)
    if(NOT COMPILER_HAS_CPP20)
        message(WARNING "C++20 support may not be available")
    endif()
    
    message(STATUS "Dependency validation complete")
endfunction()

# Dependency status reporting
function(report_dependency_status)
    message(STATUS "=== NLM Dependency Status ===")
    message(STATUS "Python: ${Python_VERSION}")
    message(STATUS "Python executable: ${Python_EXECUTABLE}")
    message(STATUS "pybind11: ${pybind11_VERSION}")
    message(STATUS "Threads: ${CMAKE_THREAD_LIBS_INIT}")
    
    if(NLM_HAS_VALGRIND)
        message(STATUS "Valgrind: Available")
    endif()
    
    if(NLM_HAS_ASAN)
        message(STATUS "AddressSanitizer: Available")
    endif()
    
    if(NLM_HAS_OPENMP)
        message(STATUS "OpenMP: Available")
    endif()
    
    message(STATUS "===========================")
endfunction()

# Cross-compilation support
set(CMAKE_CROSSCOMPILING FALSE CACHE BOOL "Enable cross-compilation support")

if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compilation mode enabled")
    
    # Check for cross-compilation toolchain
    if(CMAKE_TOOLCHAIN_FILE)
        message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
    endif()
    
    # Configure Python for cross-compilation
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    if(NOT Python_FOUND)
        message(WARNING "Python development packages may not be available for target platform")
    endif()
    
    # pybind11 cross-compilation configuration
    if(pybind11_FOUND)
        # Ensure pybind11 configuration works for cross-compilation
        set(pybind11_INCLUDE_DIR "${pybind11_INCLUDE_DIR}")
    endif()
endif()

# Dependency cache management
set(DEPENDENCY_CACHE_FILE ${CMAKE_BINARY_DIR}/dependency_cache.cmake)

if(EXISTS ${DEPENDENCY_CACHE_FILE})
    message(STATUS "Loading dependency cache")
    include(${DEPENDENCY_CACHE_FILE})
endif()

# Update dependency cache
function(update_dependency_cache)
    file(WRITE ${DEPENDENCY_CACHE_FILE}
        "# NLM Dependency Cache\n"
        "set(NLM_PYTHON_VERSION \"${Python_VERSION}\")\n"
        "set(NLM_PYBIND11_VERSION \"${pybind11_VERSION}\")\n"
        "set(NLM_HAS_VALGRIND ${NLM_HAS_VALGRIND})\n"
        "set(NLM_HAS_ASAN ${NLM_HAS_ASAN})\n"
        "set(NLM_HAS_OPENMP ${NLM_HAS_OPENMP})\n"
    )
endfunction()

# Initialize dependencies
validate_nlm_dependencies()
update_dependency_cache()
report_dependency_status()
