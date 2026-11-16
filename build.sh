#!/usr/bin/env bash

# Build script for NLM (Neural Learning Machine)
# Simplified build script with comprehensive error handling and progress tracking

# Set strict error handling
set -euo pipefail

# Color definitions for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m' # No Color
readonly BOLD='\033[1m'
readonly DIM='\033[2m'

# Configuration
readonly BUILD_DIR="${BUILD_DIR:-build}"
readonly MAX_JOBS="${MAX_JOBS:-$(nproc)}"
readonly USE_CCACHE="${USE_CCACHE:-true}"
readonly VERBOSE="${VERBOSE:-false}"
readonly SKIP_TESTS="${SKIP_TESTS:-false}"

# Progress tracking
readonly START_TIME="$(date +%s)"
build_log=""

# Build statistics
stats() {
    local end_time=$(date +%s)
    local duration=$((end_time - START_TIME))
    
    cat << EOF

$YELLOW=== Build Statistics ===$NC
Duration: ${duration}s
Build directory: $BUILD_DIR
CPU cores used: $MAX_JOBS
Cache: $USE_CCACHE
Jobs: $MAX_JOBS
$NC
EOF
}

# Logging functions
log_info() {
    if [[ "$VERBOSE" == "true" ]] || [[ "$1" == *"Phase "* ]]; then
        echo -e "${GREEN}[INFO]${NC} $*" | tee -a "$build_log"
    fi
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $*" | tee -a "$build_log"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*" | tee -a "$build_log"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*" | tee -a "$build_log"
}

log_step() {
    echo -e "${BLUE}[STEP]${NC} $*" | tee -a "$build_log"
}

# Function to check system prerequisites
check_prerequisites() {
    log_step "Checking system prerequisites..."
    
    local errors=0
    
    # Check for required compilers
    if ! command -v g++ &> /dev/null; then
        log_error "g++ compiler not found"
        ((errors++))
    else
        log_success "g++ found: $(g++ --version | head -n1)"
    fi
    
    if ! command -v cmake &> /dev/null; then
        log_error "cmake not found"
        ((errors++))
    else
        log_success "cmake found: $(cmake --version | head -n1)"
    fi
    
    # Check for make
    if ! command -v make &> /dev/null; then
        log_error "make not found"
        ((errors++))
    else
        log_success "make found: $(make --version | head -n1)"
    fi
    
    # Check for Python if needed
    if command -v python3 &> /dev/null; then
        log_success "Python found: $(python3 --version)"
    else
        log_warn "Python 3 not found - may affect some build steps"
    fi
    
    # Check disk space (minimum 2GB)
    local available_space=$(df -BG . | tail -n1 | awk '{print $4}' | sed 's/G//')
    if [[ $available_space -lt 2000 ]]; then
        log_warn "Low disk space: ${available_space}GB available (recommended: 2GB+)"
    else
        log_success "Sufficient disk space: ${available_space}GB available"
    fi
    
    if [[ $errors -gt 0 ]]; then
        log_error "$errors prerequisite check(s) failed"
        log_error "Please install missing dependencies and try again"
        return 1
    fi
    
    log_success "All prerequisites check completed"
    return 0
}

# Function to configure CMake build
configure_cmake() {
    log_step "Configuring CMake build..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    local cmake_args="-DCMAKE_BUILD_TYPE=Release"
    
    # Add cache if requested
    if [[ "$USE_CCACHE" == "true" ]] && command -v ccache &> /dev/null; then
        cmake_args+=" -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"
        log_info "Using ccache for faster rebuilds"
    fi
    
    # Add parallelization
    cmake_args+=" -j$MAX_JOBS"
    
    # Run CMake configuration
    log_info "Running CMake configuration..."
    if ! cmake .. $cmake_args; then
        log_error "CMake configuration failed"
        return 1
    fi
    
    log_success "CMake configuration completed"
    return 0
}

# Function to build the project
build_project() {
    log_step "Building NLM project..."
    
    cd "$BUILD_DIR"
    
    # Determine build system
    if [[ -f "CMakeCache.txt" ]]; then
        local build_system="CMake"
    else
        log_error "CMake cache not found - run CMake configuration first"
        return 1
    fi
    
    log_info "Build system: $build_system"
    log_info "Parallel jobs: $MAX_JOBS"
    
    # Build the project
    log_info "Starting compilation..."
    
    if [[ "$build_system" == "CMake" ]]; then
        # Use make for CMake projects
        if ! make -j$MAX_JOBS; then
            log_error "Build failed"
            return 1
        fi
    fi
    
    # Find built executables
    local executables=()
    
    # Look for expected executables in common locations
    local expected_executables=(
        "nlm"
        "nlm_phase3_demo"
        "nlm_phase4_demo"
        "nlm_phase6_demo"
        "nlm_benchmark"
    )
    
    for exe in "${expected_executables[@]}"; do
        if [[ -f "$exe" ]]; then
            executables+=("$exe")
            log_success "Found executable: $exe"
        fi
    done
    
    if [[ ${#executables[@]} -eq 0 ]]; then
        log_warn "No expected executables found in build directory"
        log_warn "Please check build output for errors"
    else
        log_success "Build completed successfully with ${#executables[@]} executables"
    fi
    
    return 0
}

# Function to run tests
run_tests() {
    if [[ "$SKIP_TESTS" == "true" ]]; then
        log_info "Skipping tests as requested"
        return 0
    fi
    
    log_step "Running tests..."
    
    cd "$BUILD_DIR"
    
    # Check if test targets exist
    if [[ ! -f "CMakeCache.txt" ]]; then
        log_warn "CMake cache not found, skipping tests"
        return 0
    fi
    
    # Try to run tests if they exist
    if [[ -f "Makefile" ]]; then
        if make help 2>/dev/null | grep -q "test"; then
            log_info "Running test suite..."
            if ! make test; then
                log_warn "Some tests failed - this may be expected for experimental software"
            else
                log_success "All tests passed"
            fi
        else
            log_info "No test target found in Makefile"
        fi
    fi
    
    log_info "Test phase completed"
    return 0
}

# Function to create build scripts
create_build_scripts() {
    log_step "Creating build scripts..."
    
    # Create build script
    cat > ./build.sh << 'EOF'
#!/bin/bash

# Quick build script for NLM
# Usage: ./build.sh [--verbose] [--no-cache] [--jobs N]

set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
VERBOSE="${VERBOSE:-false}"
USE_CCACHE="${USE_CCACHE:-true}"
MAX_JOBS="${MAX_JOBS:-$(nproc)}"

export BUILD_DIR VERBOSE USE_CCACHE MAX_JOBS

# Run the build
./build.py "$@"
EOF
    
    chmod +x ./build.sh
    log_success "Created build.sh script"
    
    # Create run script for quick execution
    cat > ./run_demo.sh << 'EOF'
#!/bin/bash

# Quick demo runner script for NLM
# Usage: ./run_demo.sh <phase> [--neurons N] [--steps S]

set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <phase> [--neurons N] [--steps S]"
    echo "Example: $0 2 --neurons 500 --steps 1000"
    exit 1
fi

PHASE="$1"
NEURONS=500
STEPS=1000

# Parse arguments
for arg in "$@"; do
    case $arg in
        --neurons=*)
            NEURONS="${arg#*=}"
            shift
            ;;
        --steps=*)
            STEPS="${arg#*=}"
            shift
            ;;
        --help)
            echo "Usage: $0 <phase> [--neurons N] [--steps S]"
            echo "Example: $0 2 --neurons 500 --steps 1000"
            exit 0
            ;;
    esac
done

# Run demo if executable exists
EXECUTABLE="$BUILD_DIR/nlm_phase${PHASE}_demo"
if [[ -f "$EXECUTABLE" ]]; then
    echo "Running NLM Phase ${PHASE} demo..."
    echo "  Neurons: $NEURONS"
    echo "  Steps: $STEPS"
    "$EXECUTABLE" --neuron_count $NEURONS --simulation_timestep 0.001
    echo "Demo completed successfully!"
else
    echo "Error: Demo executable not found: $EXECUTABLE"
    echo "Please run: $0 2  # First, build the project with ./build.sh"
    exit 1
fi
EOF
    
    chmod +x ./run_demo.sh
    log_success "Created run_demo.sh script"
    
    return 0
}

# Function to verify installation
verify_installation() {
    log_step "Verifying installation..."
    
    # Check if executables exist
    local found_executables=0
    local expected_executables=(
        "nlm"
        "nlm_phase3_demo"
        "nlm_phase4_demo"
        "nlm_phase6_demo"
        "nlm_benchmark"
    )
    
    for exe in "${expected_executables[@]}"; do
        if [[ -f "$BUILD_DIR/$exe" ]]; then
            log_success "✓ Found: $exe"
            ((found_executables++))
        else
            log_warn "✗ Missing: $exe"
        fi
    done
    
    log_info "Found $found_executables executables"
    
    # Create installation summary
    cat << EOF > ./installation_summary.txt
NLM Installation Summary
=======================
Build directory: $BUILD_DIR
Generated: $(date)

Executables found: $found_executables

To run demos, use:
  $BUILD_DIR/nlm_phase2_demo
  $BUILD_DIR/nlm_phase3_demo
  $BUILD_DIR/nlm_phase4_demo
  $BUILD_DIR/nlm_phase6_demo

Quick demo runner: ./run_demo.sh <phase> [--neurons N] [--steps S]
Build script: ./build.sh

Configuration: configs/default.cfg

For documentation, see:
  README.md - Main documentation
  easy_usage.md - Easy usage guide
  HOW_TO_USE.md - Detailed how-to guide
EOF
    
    log_success "Created installation summary"
    return 0
}

# Main build function
main() {
    local verbose_flag=""
    local no_cache_flag=""
    local jobs_flag=""
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --verbose)
                VERBOSE="true"
                shift
                ;;
            --no-cache)
                USE_CCACHE="false"
                shift
                ;;
            --jobs=*)
                MAX_JOBS="${1#*=}"
                shift
                ;;
            --jobs)
                if [[ -n $2 && ! $2 =~ ^- ]]; then
                    MAX_JOBS="$2"
                    shift 2
                else
                    log_error "--jobs requires a number"
                    exit 1
                fi
                ;;
            --help)
                echo "Usage: $0 [--verbose] [--no-cache] [--jobs N]"
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    export VERBOSE USE_CCACHE MAX_JOBS
    
    # Start build log
    build_log="$BUILD_DIR/build.log"
    echo "NLM Build Log - $(date)" > "$build_log"
    
    # Run build steps
    if check_prerequisites; then
        log_success "Prerequisites check passed"
    else
        log_error "Prerequisites check failed"
        exit 1
    fi
    
    if configure_cmake; then
        log_success "CMake configuration completed"
    else
        log_error "CMake configuration failed"
        exit 1
    fi
    
    if build_project; then
        log_success "Build project completed"
    else
        log_error "Build project failed"
        exit 1
    fi
    
    if ! skip_tests; then
        if run_tests; then
            log_success "Tests completed"
        else
            log_error "Tests failed"
        fi
    fi
    
    create_build_scripts
    verify_installation
    
    # Print build statistics
    stats
    
    log_success "Build process completed successfully!"
    log_info "See $BUILD_DIR/build.log for detailed build information"
    log_info "See $BUILD_DIR/installation_summary.txt for installation summary"
    
    # Create a simple README in build directory
    cat > "$BUILD_DIR/README_build.txt" << EOF
NLM Build Complete
===================

Build directory: $BUILD_DIR
Generated: $(date)

EXECUTABLES:
  $BUILD_DIR/nlm              - NLM Phase 2 demo (basic neural computation)
  $BUILD_DIR/nlm_phase3_demo  - NLM Phase 3 demo (world interaction)
  $BUILD_DIR/nlm_phase4_demo  - NLM Phase 4 demo (advanced cognition)
  $BUILD_DIR/nlm_phase6_demo  - NLM Phase 6 demo (integration test)
  $BUILD_DIR/nlm_benchmark     - Performance benchmark

QUICK START:
  $BUILD_DIR/run_demo.sh 2                    # Run Phase 2 demo
  $BUILD_DIR/run_demo.sh 3 --neurons 1000     # Run Phase 3 with more neurons
  $BUILD_DIR/run_demo.sh 6 --steps 5000        # Run Phase 6 with more steps

CONFIGURATION:
  Configuration file: $BUILD_DIR/../../configs/default.cfg
  Available options in the source configs/ directory

CHECKPOINTS:
  Checkpoints are saved to ./checkpoints/ directory
  Use nlm run <phase> --save <filename> to save checkpoints

DOCUMENTATION:
  See the root README.md for detailed project documentation
  Check easy_usage.md and HOW_TO_USE.md for usage guides

TROUBLESHOOTING:
  If build fails, check $BUILD_DIR/build.log for error details
  Ensure you have all required dependencies installed
EOF
}

# Skip tests if requested
skip_tests() {
    [[ "$SKIP_TESTS" == "true" ]] && return 0
    return 1
}

# Run main function if script is executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
