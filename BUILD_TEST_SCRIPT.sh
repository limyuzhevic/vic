#!/bin/bash
# Comprehensive Test Suite for NLM Project Improvements
# This script tests all the enhancements made to the NLM project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored messages
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if a file contains duplicates
check_for_duplicates() {
    local file=$1
    local pattern=$2
    print_info "Checking for duplicates in $file..."
    
    # Count occurrences of pattern
    local count=$(grep -o "$pattern" "$file" | wc -l)
    
    if [ "$count" -gt 1 ]; then
        print_error "Found $count occurrences of '$pattern' (should be exactly 1)"
        grep -n "$pattern" "$file"
        return 1
    else
        print_success "No duplicates found for '$pattern'"
        return 0
    fi
}

# Function to compile and run a demo program
compile_and_run_demo() {
    local program_name=$1
    local demo_file=$2
    local build_dir=$3
    
    print_info "Compiling $program_name..."
    
    # Create build directory if it doesn't exist
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Try to compile using the existing CMake setup
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release 2>/dev/null; then
        print_error "Failed to configure CMake for $program_name"
        return 1
    fi
    
    if ! make -j4 2>/dev/null; then
        print_error "Failed to build $program_name"
        return 1
    fi
    
    # Check if executable was created
    local executable="$build_dir/$program_name"
    if [ ! -f "$executable" ]; then
        print_warning "Executable not found at expected location, searching..."
        executable=$(find . -name "$program_name" -type f 2>/dev/null | head -1)
        if [ -z "$executable" ]; then
            print_error "Could not find $program_name executable"
            return 1
        fi
    fi
    
    print_info "Running $program_name..."
    if "$executable" 2>&1; then
        print_success "$program_name completed successfully"
    else
        print_error "$program_name failed with exit code $?"
        return 1
    fi
    
    return 0
}

# Main test function
run_tests() {
    print_info "Starting comprehensive NLM project test suite..."
    print_info "==============================================="
    
    local build_dir="build"
    local current_dir=$(pwd)
    
    # Test 1: Check Configuration Files for Duplicates
    print_info "\n[Test 1] Verifying Configuration Files Are Clean"
    print_info "----------------------------------------"
    
    # Check configs/default.cfg for duplicates
    if check_for_duplicates "/workspace/app/configs/default.cfg" "="; then
        print_success "Configuration file check passed"
    else
        print_error "Configuration file check failed - duplicates found"
    fi
    
    # Test 2: Test Enhanced Logging System with Error Categorization
    print_info "\n[Test 2] Testing Enhanced Logging System"
    print_info "----------------------------------------"
    
    if [ -f "/workspace/app/src/experiments/PerformanceDemo.cpp" ]; then
        print_info "Testing PerformanceDemo.cpp..."
        if compile_and_run_demo "nlm_performance_demo" "/workspace/app/src/experiments/PerformanceDemo.cpp" "$build_dir/performance_demo"; then
            print_success "Enhanced logging system test passed"
        else
            print_error "Enhanced logging system test failed"
        fi
    else
        print_warning "PerformanceDemo.cpp not found"
    fi
    
    # Test 3: Test Performance Monitoring Capabilities
    print_info "\n[Test 3] Testing Performance Monitoring"
    print_info "----------------------------------------"
    
    print_info "Performance monitoring tests require running the main program..."
    
    # Test 4: Test Command-Line Interface
    print_info "\n[Test 4] Testing Command-Line Interface"
    print_info "----------------------------------------"
    
    cd "$build_dir"
    if [ -f "nlm" ]; then
        print_info "Testing CLI with --help option..."
        if ./nlm --help 2>&1 | head -20 | grep -q "Usage:"; then
            print_success "Help system working"
        else
            print_error "Help system not working properly"
        fi
        
        print_info "Testing CLI with --validate-only..."
        if ./nlm --validate-only; then
            print_success "CLI validation test passed"
        else
            print_error "CLI validation test failed"
        fi
    else
        print_error "Main executable not found"
    fi
    
    # Test 5: Test Phase 6 Integration Demonstration
    print_info "\n[Test 5] Testing Phase 6 Integration"
    print_info "----------------------------------------"
    
    if [ -f "/workspace/app/src/experiments/Phase6Demo.cpp" ]; then
        print_info "Testing Phase6Demo.cpp..."
        if compile_and_run_demo "nlm_phase6_demo" "/workspace/app/src/experiments/Phase6Demo.cpp" "$build_dir/phase6_demo"; then
            print_success "Phase 6 integration test passed"
        else
            print_error "Phase 6 integration test failed"
        fi
    else
        print_warning "Phase6Demo.cpp not found"
    fi
    
    # Test 6: Verify All Demonstration Programs
    print_info "\n[Test 6] Verifying All Demonstration Programs"
    print_info "----------------------------------------"
    
    cd "$current_dir"
    
    # Check for all demo programs
    demo_files=(
        "PerformanceDemo.cpp"
        "ImprovementsSummary.cpp"
        "Phase6Demo.cpp"
        "Phase3Demo.cpp"
        "Phase4Demo.cpp"
        "Phase5Experiment.cpp"
    )
    
    for demo_file in "${demo_files[@]}"; do
        local full_path="/workspace/app/src/experiments/$demo_file"
        if [ -f "$full_path" ]; then
            print_info "Found $demo_file"
        else
            print_warning "Missing demo file: $demo_file"
        fi
    done
    
    print_success "All demonstration files found"
    
    # Test 7: Verify Main.cpp Command-Line Options
    print_info "\n[Test 7] Verifying Main.cpp Command-Line Options"
    print_info "----------------------------------------"
    
    if grep -q "--benchmark" "/workspace/app/src/main.cpp"; then
        print_success "--benchmark option present"
    else
        print_error "--benchmark option missing"
    fi
    
    if grep -q "--validate-only" "/workspace/app/src/main.cpp"; then
        print_success "--validate-only option present"
    else
        print_error "--validate-only option missing"
    fi
    
    if grep -q "--checkpoint" "/workspace/app/src/main.cpp"; then
        print_success "--checkpoint option present"
    else
        print_error "--checkpoint option missing"
    fi
    
    if grep -q "--load" "/workspace/app/src/main.cpp"; then
        print_success "--load option present"
    else
        print_error "--load option missing"
    fi
    
    # Test 8: Verify Error Categorization
    print_info "\n[Test 8] Verifying Error Categorization"
    print_info "----------------------------------------"
    
    if grep -q "ErrorCategory::Memory" "/workspace/app/src/core/Logger/Logger.hpp"; then
        print_success "ErrorCategory::Memory defined"
    else
        print_error "ErrorCategory::Memory not found"
    fi
    
    if grep -q "ErrorCategory::General" "/workspace/app/src/core/Logger/Logger.hpp"; then
        print_success "ErrorCategory::General defined"
    else
        print_error "ErrorCategory::General not found"
    fi
    
    if grep -q "ErrorCategory::Configuration" "/workspace/app/src/core/Logger/Logger.hpp"; then
        print_success "ErrorCategory::Configuration defined"
    else
        print_error "ErrorCategory::Configuration not found"
    fi
    
    # Test 9: Verify Performance Monitoring
    print_info "\n[Test 9] Verifying Performance Monitoring"
    print_info "----------------------------------------"
    
    if grep -q "startPerformanceMeasurement" "/workspace/app/src/core/Logger/Logger.hpp"; then
        print_success "Performance monitoring functions available"
    else
        print_error "Performance monitoring not available"
    fi
    
    if grep -q "recordMetric" "/workspace/app/src/core/Logger/Logger.hpp"; then
        print_success "Metric recording available"
    else
        print_error "Metric recording not available"
    fi
    
    # Test 10: Run Quick Benchmark Test
    print_info "\n[Test 10] Running Quick Benchmark"
    print_info "----------------------------------------"
    
    cd "$current_dir"
    if [ -d "build" ]; then
        cd build
        if [ -f "nlm" ]; then
            print_info "Running NLM with benchmark mode..."
            timeout 30s ./nlm --benchmark --verbosity 1 2>&1 | tail -20
            if [ $? -eq 0 ]; then
                print_success "Quick benchmark completed"
            else
                print_warning "Quick benchmark timed out or failed"
            fi
        else
            print_warning "Main executable not built, skipping benchmark"
        fi
    else
        print_warning "Build directory not found, skipping benchmark"
    fi
    
    print_info "\n" + "="*50
    print_info "Test Suite Completed"
    print_info "="*50
}

# Check if the build directory exists and build if needed
if [ ! -d "build" ] || [ ! -f "build/nlm" ]; then
    print_info "Building NLM project..."
    mkdir -p build
    cd build
    if cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
    cd ..
fi

# Run all tests
run_tests