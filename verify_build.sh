#!/bin/bash

# NLM Build Verification Script
# Validates the build system, runs smoke tests, checks requirements, and verifies Python bindings
# Part of NLM (熙然) - Neural Learning Machine Project

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
LOG_FILE="$PROJECT_ROOT/verify_build.log"
BUILD_CONFIG="${BUILD_CONFIG:-Release}"
PYTHON="${PYTHON:-python3}"
CMAKE="${CMAKE:-cmake}"
MAKE="${MAKE:-make}"

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"
}

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Display header
print_header() {
    echo "==============================================="
    echo "NLM Build Verification Script"
    echo "==============================================="
    echo "Project: NLM (熙然) - Neural Learning Machine"
    echo "Build Type: $BUILD_CONFIG"
    echo "Date: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "==============================================="
    echo
}

# Display footer
print_footer() {
    echo "==============================================="
    if [[ $verification_status -eq 0 ]]; then
        echo "Build verification PASSED ✓"
    else
        echo "Build verification FAILED ✗"
        echo "Check $LOG_FILE for details"
    fi
    echo "==============================================="
}

# Initialize verification
verification_status=0

# Main verification steps
main() {
    print_header
    log_info "Starting build verification"
    
    # Step 1: Check required tools
    log_info "Checking required tools..."
    
    local tools_missing=0
    
    if ! command_exists "$PYTHON"; then
        log_error "Python ($PYTHON) not found"
        tools_missing=1
    else
        log_info "Python found: $($PYTHON --version)"
    fi
    
    if ! command_exists "$CMAKE"; then
        log_error "CMake not found"
        tools_missing=1
    else
        log_info "CMake found: $($CMAKE --version | head -n1)"
    fi
    
    if ! command_exists "$MAKE"; then
        log_error "Make not found"
        tools_missing=1
    else
        log_info "Make found: $($MAKE --version | head -n1)"
    fi
    
    if [[ $tools_missing -ne 0 ]]; then
        verification_status=1
        log_error "Missing required tools - build verification failed"
    fi
    
    # Step 2: Check project structure
    log_info "Validating project structure..."
    
    local required_dirs=(
        "$PROJECT_ROOT/src"
        "$PROJECT_ROOT/tests"
        "$PROJECT_ROOT/configs"
        "$PROJECT_ROOT/docs"
    )
    
    for dir in "${required_dirs[@]}"; do
        if [[ ! -d "$dir" ]]; then
            log_error "Required directory not found: $dir"
            verification_status=1
        else
            log_info "Directory exists: $dir"
        fi
    done
    
    # Step 3: Check configuration files
    log_info "Checking configuration files..."
    
    local required_files=(
        "$PROJECT_ROOT/CMakeLists.txt"
        "$PROJECT_ROOT/pyproject.toml"
        "$PROJECT_ROOT/src/CMakeLists.txt"
    )
    
    for file in "${required_files[@]}"; do
        if [[ ! -f "$file" ]]; then
            log_error "Required configuration file not found: $file"
            verification_status=1
        else
            log_info "Configuration file exists: $file"
        fi
    done
    
    # Step 4: Validate build system
    log_info "Validating build system..."
    
    # Check if build directory exists
    if [[ -d "$BUILD_DIR" ]]; then
        log_info "Build directory exists: $BUILD_DIR"
        
        # Check if Makefile exists
        if [[ -f "$BUILD_DIR/Makefile" ]]; then
            log_info "Makefile found - build system configured"
        else
            log_warn "Makefile not found in build directory"
            # Try to configure build
            log_info "Attempting to configure build system..."
            if ! configure_build; then
                verification_status=1
            fi
        fi
        
        # Check if compiled executables exist
        local executables=("nlm" "nlm_phase3_demo" "nlm_phase4_demo" "nlm_phase6_demo")
        local executables_found=0
        
        for exe in "${executables[@]}"; do
            if [[ -f "$BUILD_DIR/$exe" ]]; then
                log_info "Executable found: $exe"
                ((executables_found++))
            else
                log_warn "Executable not found: $exe"
            fi
        done
        
        log_info "Executables found: $executables_found/${#executables[@]}"
        
    else
        log_warn "Build directory not found: $BUILD_DIR"
        log_info "Attempting to configure build system..."
        if ! configure_build; then
            verification_status=1
        fi
    fi
    
    # Step 5: Build project if needed
    log_info "Building project if required..."
    
    if [[ ! -f "$BUILD_DIR/nlm" ]] || [[ ! -f "$BUILD_DIR/nlm_phase3_demo" ]]; then
        if ! build_project; then
            verification_status=1
        fi
    else
        log_info "Project already built - skipping rebuild"
    fi
    
    # Step 6: Run smoke tests
    log_info "Running basic smoke tests..."
    
    if ! run_smoke_tests; then
        verification_status=1
    fi
    
    # Step 7: Verify Python bindings
    log_info "Verifying Python bindings..."
    
    if ! verify_python_bindings; then
        verification_status=1
    fi
    
    # Step 8: Check for build artifacts
    log_info "Checking build artifacts..."
    
    if ! check_build_artifacts; then
        verification_status=1
    fi
    
    # Final status
    print_footer
    
    return $verification_status
}

# Configure build system
configure_build() {
    log_info "Configuring build directory..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    if ! $CMAKE .. -DCMAKE_BUILD_TYPE=$BUILD_CONFIG; then
        log_error "CMake configuration failed"
        return 1
    fi
    
    log_info "Build system configured successfully"
    return 0
}

# Build project
build_project() {
    log_info "Building project..."
    
    cd "$BUILD_DIR"
    
    if ! $MAKE -j$(nproc); then
        log_error "Build failed"
        return 1
    fi
    
    log_info "Project built successfully"
    return 0
}

# Run smoke tests
run_smoke_tests() {
    log_info "Running smoke tests..."
    
    cd "$BUILD_DIR"
    
    # Test 1: Check if main executable runs
    log_info "Testing main executable..."
    if ! [[ -x "nlm" ]]; then
        log_error "Main executable not found or not executable"
        return 1
    fi
    
    # Try to run with help/--help if available
    if ./nlm --help 2>/dev/null; then
        log_info "Main executable help test PASSED"
    elif ./nlm -h 2>/dev/null; then
        log_info "Main executable help test PASSED"
    else
        log_warn "Could not test main executable (no help option)"
    fi
    
    # Test 2: Check Phase 3 demo
    log_info "Testing Phase 3 demo..."
    if ! [[ -x "nlm_phase3_demo" ]]; then
        log_error "Phase 3 demo not found or not executable"
        return 1
    fi
    
    # Run quick test (1 episode, 10 steps) - use timeout
    log_info "Running quick Phase 3 test..."
    timeout 30 ./nlm_phase3_demo 1 10 2>&1 | tail -20
    
    if [[ $? -eq 0 ]]; then
        log_info "Phase 3 demo test PASSED"
    else
        log_error "Phase 3 demo test FAILED"
        return 1
    fi
    
    # Test 3: Check Phase 6 demo
    log_info "Testing Phase 6 demo..."
    if ! [[ -x "nlm_phase6_demo" ]]; then
        log_error "Phase 6 demo not found or not executable"
        return 1
    fi
    
    # Run quick integration test - use timeout
    log_info "Running quick Phase 6 integration test..."
    timeout 60 ./nlm_phase6_demo 2>&1 | tail -30
    
    if [[ $? -eq 0 ]]; then
        log_info "Phase 6 demo test PASSED"
    else
        log_error "Phase 6 demo test FAILED"
        return 1
    fi
    
    log_info "All smoke tests PASSED"
    return 0
}

# Verify Python bindings
verify_python_bindings() {
    log_info "Verifying Python bindings..."
    
    # Check if pyproject.toml exists
    if [[ ! -f "$PROJECT_ROOT/pyproject.toml" ]]; then
        log_error "pyproject.toml not found"
        return 1
    fi
    
    # Try to build Python bindings if not already built
    if [[ ! -d "$PROJECT_ROOT/python" ]] || [[ ! -f "$PROJECT_ROOT/python/pynlm*.so" ]]; then
        log_info "Building Python bindings..."
        
        cd "$PROJECT_ROOT"
        
        if ! $PYTHON -m pip install -e . --no-build-isolation 2>&1 | tail -20; then
            log_error "Python bindings build failed"
            return 1
        fi
    fi
    
    # Try to import the module
    log_info "Testing Python module import..."
    
    python_code="
import sys
try:
    import pynlm
    print('SUCCESS: pynlm imported successfully')
    print(f'Version: {getattr(pynlm, \"__version__\", \"unknown\")}')
except ImportError as e:
    print(f'FAILED: {e}')
    sys.exit(1)
"
    
    if ! echo "$python_code" | $PYTHON; then
        log_error "Python bindings verification failed"
        return 1
    fi
    
    log_info "Python bindings verification PASSED"
    return 0
}

# Check build artifacts
check_build_artifacts() {
    log_info "Checking build artifacts..."
    
    cd "$BUILD_DIR"
    
    # Check for expected artifacts
    local artifacts=(
        "Makefile"
        "CMakeCache.txt"
    )
    
    for artifact in "${artifacts[@]}"; do
        if [[ -f "$artifact" ]]; then
            log_info "Build artifact found: $artifact"
        else
            log_warn "Build artifact not found: $artifact"
        fi
    done
    
    # Check library files
    if ls *.a 2>/dev/null | grep -q .; then
        log_info "Static libraries found"
    else
        log_warn "No static libraries found"
    fi
    
    log_info "Build artifact check completed"
    return 0
}

# Execute main function
main "$@"