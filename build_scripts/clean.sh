#!/bin/bash

# clean.sh - NLM Clean Script
# Clean build artifacts and temporary files

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Clean build directories
clean_build_dirs() {
    log_info "Cleaning build directories..."
    
    # Remove standard build directory
    if [[ -d "build" ]]; then
        log_info "Removing build directory..."
        rm -rf build
    fi
    
    # Remove alternative build directories
    for dir in "build-debug" "build-release" "build-relwithdebinfo" "build-minsize" "build-clang" "build-gcc"; do
        if [[ -d "$dir" ]]; then
            log_info "Removing ${dir} directory..."
            rm -rf "$dir"
        fi
    done
    
    # Remove CMake cache files
    if [[ -f "CMakeCache.txt" ]]; then
        rm -f CMakeCache.txt
    fi
    
    # Remove compile commands
    if [[ -f "compile_commands.json" ]]; then
        rm -f compile_commands.json
    fi
    
    # Remove Python build artifacts
    if [[ -d "dist" ]]; then
        log_info "Removing dist directory..."
        rm -rf dist
    fi
    
    if [[ -d "*.egg-info" ]]; then
        log_info "Removing *.egg-info directories..."
        rm -rf *.egg-info
    fi
    
    # Remove virtual environment
    if [[ -d "venv" ]]; then
        log_info "Removing virtual environment..."
        rm -rf venv
    fi
    
    # Remove temporary files
    log_info "Removing temporary files..."
    rm -f *.o
    rm -f *.so
    rm -f *.dylib
    rm -f *.dll
    rm -f nlm
    rm -f nlm_*.demo
    rm -f benchmark
    rm -f performance_tests
    
    # Remove Python cache files
    find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
    find . -name "*.pyc" -delete
    find . -name "*.pyo" -delete
    find . -name ".coverage" -delete
    find . -name "coverage.xml" -delete
    
    # Remove documentation build artifacts
    if [[ -d "docs/_build" ]]; then
        log_info "Removing documentation build directory..."
        rm -rf docs/_build
    fi
    
    if [[ -d "htmlcov" ]]; then
        log_info "Removing HTML coverage report..."
        rm -rf htmlcov
    fi
    
    # Remove benchmark results
    if [[ -f "benchmark_results.json" ]]; then
        log_info "Removing benchmark results..."
        rm -f benchmark_results.json
    fi
    
    # Remove performance metrics
    if [[ -f "performance_metrics.json" ]]; then
        log_info "Removing performance metrics..."
        rm -f performance_metrics.json
    fi
    
    # Remove test results
    if [[ -d "test-results" ]]; then
        log_info "Removing test results..."
        rm -rf test-results
    fi
    
    # Remove quality reports
    if [[ -f "quality_report.html" ]]; then
        log_info "Removing quality reports..."
        rm -f quality_report.html
    fi
    
    if [[ -f "quality_report.json" ]]; then
        log_info "Removing quality reports..."
        rm -f quality_report.json
    fi
    
    # Remove license check files
    if [[ -f "license_check.txt" ]]; then
        log_info "Removing license check files..."
        rm -f license_check.txt
    fi
    
    # Remove dependency tree
    if [[ -f "dependency_tree.txt" ]]; then
        log_info "Removing dependency tree..."
        rm -f dependency_tree.txt
    fi
    
    # Remove security reports
    if [[ -f "bandit-report.json" ]]; then
        log_info "Removing security reports..."
        rm -f bandit-report.json
    fi
}

# Clean system dependencies (optional)
clean_system_deps() {
    log_info "Cleaning system dependency files..."
    
    # Remove log files
    find . -name "*.log" -delete
    
    # Remove temporary build files
    find . -name "*.tmp" -delete
    find . -name "*.temp" -delete
    
    # Remove backup files
    find . -name "*~" -delete
    find . -name "*.bak" -delete
    find . -name "*.backup" -delete
}

# Clean git cache (optional)
clean_git_cache() {
    if [[ -d ".git" ]]; then
        log_info "Cleaning git cache..."
        git clean -fd
        git reset --hard HEAD
    fi
}

# Print disk usage
print_disk_usage() {
    log_info "Disk usage summary:"
    echo "Build directory:"
    du -sh build 2>/dev/null || echo "  (not found)"
    echo "Dist directory:"
    du -sh dist 2>/dev/null || echo "  (not found)"
    echo "Virtual environment:"
    du -sh venv 2>/dev/null || echo "  (not found)"
}

# Main clean function
main() {
    log_info "Starting NLM clean process..."
    
    # Clean build directories
    clean_build_dirs
    
    # Clean system dependencies
    clean_system_deps
    
    # Optionally clean git cache
    if [[ "$1" == "--git" ]]; then
        log_warning "This will also clean git cache. Continue? (y/N)"
        read -r response
        if [[ "$response" =~ ^[Yy]$ ]]; then
            clean_git_cache
        fi
    fi
    
    # Print disk usage summary
    print_disk_usage
    
    log_success "Clean process completed successfully"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --git)
            SHIFT
            ;;
        *)
            log_error "Unknown option: $1"
            echo "Usage: $0 [--git]"
            exit 1
            ;;
    esac
    shift
done

# Run main function
main "$@"