#!/bin/bash

# NLM Development Workflow Script
# This script provides common development workflow commands

set -e

echo "=== NLM Development Workflow ==="

# Function to print colored output
print_info() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

print_success() {
    echo -e "\033[1;32m[SUCCESS]\033[0m $1"
}

print_warning() {
    echo -e "\033[1;33m[WARNING]\033[0m $1"
}

# Setup development environment
setup_dev_env() {
    print_info "Setting up development environment..."
    
    # Check if build directory exists
    if [ ! -d "build" ]; then
        print_info "Creating build directory..."
        mkdir -p build
        print_info "Configuring CMake..."
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON -DNLM_ENABLE_COVERAGE=ON
    fi
    
    print_info "Building development version..."
    cmake --build build --parallel
    
    print_success "Development environment ready"
}

# Run development simulations
run_dev_simulation() {
    local simulation_type="$1"
    
    print_info "Running $simulation_type development simulation..."
    
    case $simulation_type in
        phase6)
            if [ -f "build/nlm_phase6_demo" ]; then
                ./build/nlm_phase6_demo
            else
                print_error "Phase 6 demo not found. Build the project first."
                exit 1
            fi
            ;;
        phase4)
            if [ -f "build/nlm_phase4_demo" ]; then
                ./build/nlm_phase4_demo
            else
                print_error "Phase 4 demo not found. Build the project first."
                exit 1
            fi
            ;;
        phase3)
            if [ -f "build/nlm_phase3_demo" ]; then
                ./build/nlm_phase3_demo
            else
                print_error "Phase 3 demo not found. Build the project first."
                exit 1
            fi
            ;;
        debug)
            if [ -f "build/nlm" ]; then
                ./build/nlm --debug-mode
            else
                print_error "Main executable not found. Build the project first."
                exit 1
            fi
            ;;
        *)
            print_error "Unknown simulation type: $simulation_type"
            print_info "Available types: phase6, phase4, phase3, debug"
            exit 1
            ;;
    esac
}

# Debug development code
debug_code() {
    print_info "Setting up debugging environment..."
    
    # Generate debug build if needed
    if [ ! -d "build_debug" ]; then
        print_info "Creating debug build..."
        mkdir -p build_debug
        cmake -S . -B build_debug -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON -DNLM_ENABLE_COVERAGE=ON
    fi
    
    print_info "Building debug version..."
    cmake --build build_debug --parallel
    
    print_info "Starting debugger..."
    if command -v gdb &> /dev/null; then
        gdb ./build_debug/nlm
    elif command -v lldb &> /dev/null; then
        lldb ./build_debug/nlm
    else
        print_error "No debugger found. Install GDB or LLDB."
        exit 1
    fi
}

# Analyze development code
analyze_code() {
    print_info "Analyzing development code..."
    
    # Check code style
    print_info "Checking code style..."
    if command -v clang-format &> /dev/null; then
        find src include -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | while read file; do
            echo "Checking $file"
            clang-format -style=file -i "$file" 2>/dev/null || true
        done
        print_info "Code style check completed."
    fi
    
    # Run static analysis
    print_info "Running static analysis..."
    if [ -f "build/CMakeCache.txt" ]; then
        cd build && ctest --output-on-failure 2>&1 | tail -20
        cd ..
    fi
}

# Profile development code
profile_code() {
    print_info "Profiling development code..."
    
    # Generate build with profiling
    if [ ! -d "build_profile" ]; then
        print_info "Creating profiling build..."
        mkdir -p build_profile
        cmake -S . -B build_profile -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PROFILING=ON
        cmake --build build_profile --parallel
    fi
    
    print_info "Running with profiling..."
    cd build_profile && ./nlm --profile=perfetto
    cd ..
}

# Check code quality
check_quality() {
    print_info "Checking code quality..."
    
    # Generate code coverage
    if [ -f "build/CMakeCache.txt" ]; then
        cd build && ctest --output-on-failure
        if [ -f "coverage_report.html" ]; then
            print_info "Coverage report: coverage_report.html"
        fi
        cd ..
    fi
    
    # Check for memory issues
    print_info "Checking for memory issues..."
    if [ -f "build/CMakeCache.txt" ]; then
        # Run with memory sanitizer if enabled
        if grep -q "NLM_ENABLE_SANITIZERS=ON" build/CMakeCache.txt; then
            ./build/nlm --memory-monitor 2>&1 | head -50
        fi
    fi
}

# Show development status
show_dev_status() {
    print_info "Development Status:"
    echo
    echo "Build directories:"
    ls -la | grep -E "build|build_"
    echo
    
    echo "Available executables:"
    if [ -d "build" ]; then
        find build -maxdepth 1 -type f -executable -name "nlm*" 2>/dev/null || echo "None"
    fi
    echo
    
    echo "Available tests:"
    if [ -d "build" ]; then
        find build -name "*.test" -o -name "*test*" 2>/dev/null | head -10 || echo "None"
    fi
    echo
}

# Main function
main() {
    case "$1" in
        setup)
            setup_dev_env
            ;;
        simulate)
            run_dev_simulation "$2"
            ;;
        debug)
            debug_code
            ;;
        analyze)
            analyze_code
            ;;
        profile)
            profile_code
            ;;
        quality)
            check_quality
            ;;
        status)
            show_dev_status
            ;;
        *)
            echo "Usage: $0 {setup|simulate|debug|analyze|profile|quality|status}"
            echo ""
            echo "Commands:"
            echo "  setup          Setup development environment"
            echo "  simulate TYPE  Run development simulation (phase6|phase4|phase3|debug)"
            echo "  debug          Start debugging session"
            echo "  analyze        Analyze code and run checks"
            echo "  profile        Profile performance"
            echo "  quality        Check code quality and coverage"
            echo "  status         Show development status"
            exit 1
            ;;
    esac
}

# Execute main function
if [ $# -eq 0 ]; then
    echo "Usage: $0 {setup|simulate|debug|analyze|profile|quality|status}"
    exit 1
fi

main "$1"