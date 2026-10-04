#!/bin/bash
# NLM Linting and Code Quality Automation Script
# This script provides comprehensive linting and code formatting for the NLM project
# 
# Usage:
#   ./scripts/run_linting.sh [check|format|full|quick]
#
# Options:
#   check  - Run linting checks without modifying files (default)
#   format - Run clang-format to auto-format code
#   full   - Run all linting checks (format, lint, static analysis)
#   quick  - Run minimal checks for fast feedback
#
# Exit codes:
#   0 - Success
#   1 - Linting/formatting failed
#   2 - Script usage error

set -euo pipefail

# Configuration
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
SRC_DIR="${PROJECT_ROOT}/src"
TESTS_DIR="${PROJECT_ROOT}/tests"
LOG_FILE="${PROJECT_ROOT}/linting_$(date +%Y%m%d_%H%M%S).log"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    local level="$1"
    shift
    local message="$*"
    local timestamp="$(date '+%Y-%m-%d %H:%M:%S')"
    
    case "$level" in
        INFO)
            echo -e "${BLUE}[INFO] ${timestamp} ${message}${NC}" | tee -a "${LOG_FILE}"
            ;;
        WARNING)
            echo -e "${YELLOW}[WARNING] ${timestamp} ${message}${NC}" | tee -a "${LOG_FILE}"
            ;;
        ERROR)
            echo -e "${RED}[ERROR] ${timestamp} ${message}${NC}" | tee -a "${LOG_FILE}"
            ;;
        SUCCESS)
            echo -e "${GREEN}[SUCCESS] ${timestamp} ${message}${NC}" | tee -a "${LOG_FILE}"
            ;;
        *)
            echo -e "[${level}] ${timestamp} ${message}" | tee -a "${LOG_FILE}"
            ;;
    esac
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install missing dependencies
install_dependencies() {
    log INFO "Checking and installing dependencies..."
    
    local missing_commands=()
    
    for cmd in "cmake" "g++" "make"; do
        if ! command_exists "$cmd"; then
            missing_commands+=("$cmd")
        fi
    done
    
    if command_exists "clang-format"; then
        log INFO "clang-format is available"
    else
        missing_commands+=("clang-format")
    fi
    
    if command_exists "clang-tidy"; then
        log INFO "clang-tidy is available"
    else
        missing_commands+=("clang-tidy")
    fi
    
    if command_exists "cppcheck"; then
        log INFO "cppcheck is available"
    else
        missing_commands+=("cppcheck")
    fi
    
    if [ ${#missing_commands[@]} -gt 0 ]; then
        log WARNING "The following commands are missing and will need to be installed: ${missing_commands[*]}"
        log INFO "To install these dependencies, run:"
        log INFO "  # For Ubuntu/Debian: sudo apt-get update && sudo apt-get install -y ${missing_commands[*]}"
        log INFO "  # For macOS: brew install ${missing_commands[*]}"
        log INFO "  # For other systems, install them manually"
        return 1
    fi
    
    return 0
}

# Function to check CMake configuration
check_cmake_config() {
    log INFO "Checking CMake configuration..."
    
    if [ ! -d "${BUILD_DIR}" ]; then
        log WARNING "Build directory not found. Creating build directory..."
        mkdir -p "${BUILD_DIR}"
    fi
    
    cd "${BUILD_DIR}"
    
    if [ ! -f "CMakeCache.txt" ]; then
        log INFO "Running CMake configuration..."
        if ! cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | tee -a "${LOG_FILE}"; then
            log ERROR "CMake configuration failed"
            return 1
        fi
    fi
    
    if [ ! -f "Makefile" ]; then
        log ERROR "Makefile not found. CMake configuration may have failed."
        return 1
    fi
    
    log SUCCESS "CMake configuration is valid"
    return 0
}

# Function to check code formatting with clang-format
check_formatting() {
    log INFO "Checking code formatting with clang-format..."
    
    local failed_files=()
    local total_files=0
    
    # Find all C++ source files
    local cpp_files
    readarray -t cpp_files < <(find "${SRC_DIR}" "${TESTS_DIR}" -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | grep -v "/build/" | grep -v "/_deps/" | grep -v "~" | sort)
    
    total_files=${#cpp_files[@]}
    
    if [ "$total_files" -eq 0 ]; then
        log WARNING "No C++ source files found to check"
        return 0
    fi
    
    log INFO "Checking ${total_files} files for formatting issues..."
    
    for file in "${cpp_files[@]}"; do
        log INFO "Checking ${file}..."
        
        if ! clang-format --dry-run -Werror "${file}" 2>&1 | tee -a "${LOG_FILE}"; then
            log ERROR "Code formatting issues found in ${file}"
            failed_files+=("${file}")
        else
            log SUCCESS "${file}: OK"
        fi
    done
    
    if [ ${#failed_files[@]} -gt 0 ]; then
        log ERROR "Code formatting check failed for ${failed_files[*]}"
        log INFO "To fix formatting issues, run: clang-format -i <file>"
        return 1
    fi
    
    log SUCCESS "All ${total_files} files are properly formatted"
    return 0
}

# Function to run clang-tidy analysis
run_clang_tidy() {
    log INFO "Running clang-tidy analysis..."
    
    local failed_files=()
    local total_files=0
    
    # Find all C++ source files
    local cpp_files
    readarray -t cpp_files < <(find "${SRC_DIR}" "${TESTS_DIR}" -name "*.cpp" | grep -v "/build/" | grep -v "/_deps/" | grep -v "~" | sort)
    
    total_files=${#cpp_files[@]}
    
    if [ "$total_files" -eq 0 ]; then
        log WARNING "No C++ source files found for clang-tidy analysis"
        return 0
    fi
    
    log INFO "Analyzing ${total_files} files with clang-tidy..."
    
    for file in "${cpp_files[@]}"; do
        log INFO "Analyzing ${file} with clang-tidy..."
        
        # Extract directory for include paths
        local file_dir="$(dirname "${file}")"
        
        if ! clang-tidy "${file}" -- -I "${SRC_DIR}" -I "${file_dir}" 2>&1 | tee -a "${LOG_FILE}"; then
            log ERROR "Clang-tidy found issues in ${file}"
            failed_files+=("${file}")
        else
            log SUCCESS "${file}: clang-tidy OK"
        fi
    done
    
    if [ ${#failed_files[@]} -gt 0 ]; then
        log ERROR "Clang-tidy analysis failed for ${failed_files[*]}"
        return 1
    fi
    
    log SUCCESS "Clang-tidy analysis completed successfully for all ${total_files} files"
    return 0
}

# Function to run cppcheck analysis
run_cppcheck() {
    log INFO "Running cppcheck static analysis..."
    
    local failed_files=()
    local total_files=0
    
    # Find all C++ source files
    local cpp_files
    readarray -t cpp_files < <(find "${SRC_DIR}" "${TESTS_DIR}" -name "*.cpp" -o -name "*.hpp" | grep -v "/build/" | grep -v "/_deps/" | grep -v "~" | sort)
    
    total_files=${#cpp_files[@]}
    
    if [ "$total_files" -eq 0 ]; then
        log WARNING "No C++ source files found for cppcheck analysis"
        return 0
    fi
    
    log INFO "Running cppcheck on ${total_files} files..."
    
    for file in "${cpp_files[@]}"; do
        log INFO "Analyzing ${file} with cppcheck..."
        
        if ! cppcheck --enable=all --inconclusive --std=c++20 --project=. "${file}" 2>&1 | tee -a "${LOG_FILE}"; then
            log ERROR "Cppcheck found issues in ${file}"
            failed_files+=("${file}")
        else
            log SUCCESS "${file}: cppcheck OK"
        fi
    done
    
    if [ ${#failed_files[@]} -gt 0 ]; then
        log ERROR "Cppcheck analysis failed for ${failed_files[*]}"
        return 1
    fi
    
    log SUCCESS "Cppcheck analysis completed successfully for all ${total_files} files"
    return 0
}

# Function to format code with clang-format
format_code() {
    log INFO "Auto-formatting code with clang-format..."
    
    local total_files=0
    local formatted_files=0
    
    # Find all C++ source files
    local cpp_files
    readarray -t cpp_files < <(find "${SRC_DIR}" "${TESTS_DIR}" -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | grep -v "/build/" | grep -v "/_deps/" | grep -v "~" | sort)
    
    total_files=${#cpp_files[@]}
    
    if [ "$total_files" -eq 0 ]; then
        log WARNING "No C++ source files found to format"
        return 0
    fi
    
    log INFO "Formatting ${total_files} files..."
    
    for file in "${cpp_files[@]}"; do
        log INFO "Formatting ${file}..."
        
        if clang-format -i "${file}" 2>&1 | tee -a "${LOG_FILE}"; then
            log SUCCESS "${file}: Formatted successfully"
            formatted_files=$((formatted_files + 1))
        else
            log ERROR "Failed to format ${file}"
        fi
    done
    
    log INFO "Auto-formatting completed. ${formatted_files} out of ${total_files} files were formatted."
    log INFO "Formatted files are available in the log file: ${LOG_FILE}"
    log INFO "To see changes, run: git diff"
    return 0
}

# Function to run build
run_build() {
    log INFO "Building project..."
    
    cd "${BUILD_DIR}"
    
    if ! make -j$(nproc) 2>&1 | tee -a "${LOG_FILE}"; then
        log ERROR "Project build failed"
        return 1
    fi
    
    log SUCCESS "Project build completed successfully"
    return 0
}

# Function to run existing test targets
run_tests() {
    log INFO "Running existing test targets..."
    
    cd "${BUILD_DIR}"
    
    if [ ! -f "Makefile" ]; then
        log ERROR "Makefile not found. Cannot run tests."
        return 1
    fi
    
    if ! make ci_checks 2>&1 | tee -a "${LOG_FILE}"; then
        log ERROR "CI checks failed"
        return 1
    fi
    
    log SUCCESS "CI checks completed successfully"
    return 0
}

# Function to display help
show_help() {
    cat << EOF
NLM Linting and Code Quality Automation Script

Usage: ${0} [OPTION]

Options:
  check    Run linting checks without modifying files (default)
  format   Run clang-format to auto-format code
  full     Run all linting checks (format, lint, static analysis, build)
  quick    Run minimal checks for fast feedback
  help     Display this help message
  version  Display version information

Exit codes:
  0 - Success
  1 - Linting/formatting failed
  2 - Script usage error

For more information, see CONTRIBUTING.md or scripts/README.md
EOF
}

# Function to display version information
show_version() {
    cat << EOF
NLM Linting and Code Quality Automation Script
Version: 1.0.0
Created: $(date '+%Y-%m-%d')
Purpose: Comprehensive linting and code formatting for the NLM project

Features:
  - Code formatting with clang-format
  - Static analysis with clang-tidy
  - Static analysis with cppcheck
  - CMake configuration validation
  - Build verification
  - CI/CD integration support

For documentation and support, see CONTRIBUTING.md
EOF
}

# Main function
main() {
    local mode="check"
    
    # Parse command line arguments
    if [ $# -gt 0 ]; then
        case "$1" in
            check|format|full|quick|help|version)
                mode="$1"
                shift
                ;;
            *)
                echo "Unknown option: $1"
                show_help
                exit 2
                ;;
        esac
    fi
    
    # Initialize log file
    log INFO "Starting NLM linting and code quality checks"
    log INFO "Mode: ${mode}"
    log INFO "Log file: ${LOG_FILE}"
    
    # Install dependencies
    if ! install_dependencies; then
        log WARNING "Some dependencies are missing. Linting may fail."
    fi
    
    # Check CMake configuration
    if ! check_cmake_config; then
        log ERROR "CMake configuration check failed"
        exit 1
    fi
    
    # Run checks based on mode
    case "$mode" in
        check)
            log INFO "Running quick linting check..."
            
            # Check formatting
            if ! check_formatting; then
                log ERROR "Code formatting check failed"
                exit 1
            fi
            
            # Run clang-tidy (optional for quick mode)
            log INFO "Skipping clang-tidy for quick mode (can be added with 'full' mode)"
            
            # Run cppcheck (optional for quick mode)
            log INFO "Skipping cppcheck for quick mode (can be added with 'full' mode)"
            
            log SUCCESS "Quick linting check completed successfully"
            ;;
            
        format)
            log INFO "Running code formatting..."
            if ! format_code; then
                log ERROR "Code formatting failed"
                exit 1
            fi
            log SUCCESS "Code formatting completed"
            ;;
            
        full)
            log INFO "Running full linting suite..."
            
            # Check formatting
            if ! check_formatting; then
                log ERROR "Code formatting check failed"
                exit 1
            fi
            
            # Run clang-tidy
            if ! run_clang_tidy; then
                log ERROR "Clang-tidy analysis failed"
                exit 1
            fi
            
            # Run cppcheck
            if ! run_cppcheck; then
                log ERROR "Cppcheck analysis failed"
                exit 1
            fi
            
            # Build project
            if ! run_build; then
                log ERROR "Build failed"
                exit 1
            fi
            
            # Run existing tests
            if ! run_tests; then
                log ERROR "Test targets failed"
                exit 1
            fi
            
            log SUCCESS "Full linting suite completed successfully"
            ;;
            
        quick)
            log INFO "Running quick feedback mode..."
            
            # Only check formatting
            if ! check_formatting; then
                log ERROR "Code formatting check failed"
                exit 1
            fi
            
            log SUCCESS "Quick feedback mode completed successfully"
            ;;
    esac
    
    log SUCCESS "NLM linting and code quality checks completed successfully!"
    log INFO "For detailed results, see: ${LOG_FILE}"
}

# Run main function
main "$@"
