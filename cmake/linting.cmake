# CMake script for running linting tools in NLM project
# This script provides targets for code formatting and static analysis

cmake_minimum_required(VERSION 3.16)
project(NLM_Linting LANGUAGES)

# Find linting tools
find_program(CLANG_FORMAT "clang-format")
find_program(CLANG_TIDY "clang-tidy")
find_program(CPACHECK "cppcheck")

if(NOT CLANG_FORMAT)
    message(WARNING "clang-format not found. Code formatting checks will be skipped.")
endif()

if(NOT CLANG_TIDY)
    message(WARNING "clang-tidy not found. Static analysis checks will be skipped.")
endif()

if(NOT CPACHECK)
    message(WARNING "cppcheck not found. Static analysis checks will be skipped.")
endif()

# Set source file patterns for linting
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Collect all source files for linting
file(GLOB_RECURSE ALL_SRC_FILES "${SRC_DIR}/*.cpp" "${TESTS_DIR}/*.cpp")
file(GLOB_RECURSE ALL_HEADER_FILES "${SRC_DIR}/*.hpp" "${TESTS_DIR}/*.hpp")

# Separate source files for different linting tools
foreach(file ${ALL_SRC_FILES})
    list(APPEND LINTING_SRC_FILES ${file})
endforeach()

foreach(file ${ALL_HEADER_FILES})
    list(APPEND LINTING_HEADER_FILES ${file})
endforeach()

# Create a list of source files for easy iteration
set(SOURCE_FILES_FOR_LINTING ${LINTING_SRC_FILES})

# Function to check code formatting
function(check_formatting)
    if(NOT CLANG_FORMAT)
        message(WARNING "clang-format not available, skipping formatting check")
        return()
    endif()

    message(STATUS "Running clang-format check...")
    
    set(formatting_errors 0)
    
    foreach(file ${LINTING_HEADER_FILES} ${LINTING_SRC_FILES})
        if(EXISTS ${file})
            message(STATUS "  Checking ${file}...")
            execute_process(
                COMMAND ${CLANG_FORMAT} --dry-run -Werror ${file}
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output
                ERROR_VARIABLE error
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            
            if(NOT result EQUAL 0)
                math(EXPR formatting_errors "${formatting_errors} + 1")
                message(WARNING "  Formatting issues in ${file}")
                if(${formatting_errors} EQUAL 1)
                    message("Formatting errors found:")
                endif()
                message("    ${file}")
            endif()
        endif()
    endforeach()
    
    if(formatting_errors GREATER 0)
        message(FATAL_ERROR "${formatting_errors} files have formatting issues. Run clang-format -i to fix.")
    else()
        message(STATUS "All files are properly formatted.")
    endif()
endfunction()

# Function to run clang-tidy analysis
function(run_clang_tidy)
    if(NOT CLANG_TIDY)
        message(WARNING "clang-tidy not available, skipping static analysis")
        return()
    endif()

    message(STATUS "Running clang-tidy analysis...")
    
    set(tidy_errors 0)
    
    foreach(file ${LINTING_SRC_FILES})
        if(EXISTS ${file})
            message(STATUS "  Analyzing ${file}...")
            execute_process(
                COMMAND ${CLANG_TIDY} ${file} -- -I${SRC_DIR}
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output
                ERROR_VARIABLE error
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            
            if(NOT result EQUAL 0)
                math(EXPR tidy_errors "${tidy_errors} + 1")
                message(WARNING "  Static analysis issues in ${file}")
                if(${tidy_errors} EQUAL 1)
                    message("Clang-tidy errors found:")
                endif()
                message("    ${file}")
            endif()
        endif()
    endforeach()
    
    if(tidy_errors GREATER 0)
        message(FATAL_ERROR "${tidy_errors} files have static analysis issues. Review the output above.")
    else()
        message(STATUS "Clang-tidy analysis completed successfully.")
    endif()
endfunction()

# Function to run cppcheck analysis
function(run_cppcheck)
    if(NOT CPACHECK)
        message(WARNING "cppcheck not available, skipping cppcheck analysis")
        return()
    endif()

    message(STATUS "Running cppcheck analysis...")
    
    set(cppcheck_errors 0)
    
    foreach(file ${LINTING_SRC_FILES})
        if(EXISTS ${file})
            message(STATUS "  Analyzing ${file} with cppcheck...")
            execute_process(
                COMMAND ${CPACHECK} --enable=all --inconclusive --std=c++20 ${file}
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output
                ERROR_VARIABLE error
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            
            if(NOT result EQUAL 0)
                math(EXPR cppcheck_errors "${cppcheck_errors} + 1")
                message(WARNING "  Cppcheck issues in ${file}")
                if(${cppcheck_errors} EQUAL 1)
                    message("Cppcheck errors found:")
                endif()
                message("    ${file}")
            endif()
        endif()
    endforeach()
    
    if(cppcheck_errors GREATER 0)
        message(FATAL_ERROR "${cppcheck_errors} files have cppcheck issues. Review the output above.")
    else()
        message(STATUS "Cppcheck analysis completed successfully.")
    endif()
endfunction()

# Main linting target
add_custom_target(linting_all
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_FILE}
    COMMENT "Running all linting tools"
    VERBATIM
)

add_custom_target(linting_format
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_FILE}
    COMMAND ${CLANG_FORMAT} --version
    COMMENT "Checking code formatting with clang-format"
    VERBATIM
)

add_custom_target(linting_tidy
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_FILE}
    COMMAND ${CLANG_TIDY} --version
    COMMENT "Running clang-tidy analysis"
    VERBATIM
)

add_custom_target(linting_static
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_FILE}
    COMMAND ${CPACHECK} --version
    COMMENT "Running cppcheck static analysis"
    VERBATIM
)

# Add help target to show linting options
add_custom_target(linting_help
    COMMAND echo "Available linting targets:"
    COMMAND echo "  make linting_all    - Run all linting tools (format + static analysis)"
    COMMAND echo "  make linting_format - Check code formatting only"
    COMMAND echo "  make linting_tidy   - Run clang-tidy analysis"
    COMMAND echo "  make linting_static - Run cppcheck analysis"
    COMMAND echo ""
    COMMAND echo "To run linting using the automation script:"
    COMMAND echo "  ./scripts/run_linting.sh full   - Run complete linting suite"
    COMMAND echo "  ./scripts/run_linting.sh check   - Quick linting check"
    COMMAND echo "  ./scripts/run_linting.sh format - Auto-format code"
    COMMENT "Show linting help and usage information"
    VERBATIM
)

# Message about linting availability
if(CLANG_FORMAT AND CLANG_TIDY AND CPACHECK)
    message(STATUS "Linting tools are available. Targets: linting_all, linting_format, linting_tidy, linting_static")
else()
    message(STATUS "Some linting tools are missing. Only basic build targets are available.")
    if(NOT CLANG_FORMAT)
        message(STATUS "- Missing: clang-format")
    endif()
    if(NOT CLANG_TIDY)
        message(STATUS "- Missing: clang-tidy")
    endif()
    if(NOT CPACHECK)
        message(STATUS "- Missing: cppcheck")
    endif()
endif()
