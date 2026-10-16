#!/usr/bin/env bash
# NLM Utility Script Collection
# A collection of useful command-line tools for working with NLM simulations

# Configuration
NLM_EXECUTABLE="nlm"
PHASE6_DEMO="nlm_phase6_demo"
CONFIG_FILE="configs/default.cfg"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_debug() {
    echo -e "${BLUE}[DEBUG]${NC} $1"
}

# Function to run a simple brain simulation
run_simple_simulation() {
    local steps=${1:-100}
    local neurons=${2:-1000}
    
    log_info "Running simple simulation with ${neurons} neurons for ${steps} steps"
    
    # Create build directory if it doesn't exist
    if [ ! -d "build" ]; then
        log_info "Creating build directory..."
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
        cd ..
    fi
    
    # Run the simulation
    if [ -f "build/nlm" ]; then
        log_info "Running NLM simulation..."
        cd build
        ./nlm --neuron_count=${neurons} --max_steps=${steps}
        cd ..
    else
        log_error "NLM executable not found. Please build the project first."
        log_info "Run: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
        exit 1
    fi
}

# Function to run Phase 6 integration demo
run_phase6_demo() {
    log_info "Running Phase 6 integration demo..."
    
    if [ ! -f "build/nlm_phase6_demo" ]; then
        log_warn "Phase 6 demo not built. Building it now..."
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make nlm_phase6_demo
        cd ..
    fi
    
    if [ -f "build/nlm_phase6_demo" ]; then
        cd build
        ./nlm_phase6_demo
        cd ..
    else
        log_error "Failed to build Phase 6 demo."
        exit 1
    fi
}

# Function to run memory system tests
run_memory_tests() {
    log_info "Running memory system tests..."
    
    if [ ! -d "tests" ]; then
        log_error "Tests directory not found."
        exit 1
    fi
    
    # Create test directory if it doesn't exist
    if [ ! -d "build/test" ]; then
        mkdir -p build/test
    fi
    
    cd build/test
    cmake ../../tests ../.. -DCMAKE_BUILD_TYPE=Release
    make -j4
    
    if [ -f "test_brain" ]; then
        ./test_brain
    else
        log_error "Memory tests not found."
        cd ../../
        exit 1
    fi
    cd ../../
}

# Function to show system status
show_system_status() {
    log_info "=== NLM System Status ==="
    
    # Check build status
    if [ -f "build/nlm" ]; then
        log_info "✅ NLM executable: Available"
    else
        log_warn "❌ NLM executable: Not built"
    fi
    
    if [ -f "build/nlm_phase6_demo" ]; then
        log_info "✅ Phase 6 demo: Available"
    else
        log_warn "❌ Phase 6 demo: Not built"
    fi
    
    # Check for Python bindings
    if [ -f "python/bindings.cpp" ]; then
        log_info "✅ Python bindings source: Available"
    else
        log_warn "❌ Python bindings source: Not found"
    fi
    
    # Check configuration files
    if [ -f "$CONFIG_FILE" ]; then
        log_info "✅ Configuration file: $CONFIG_FILE"
        echo "   $(head -5 "$CONFIG_FILE")"
    else
        log_warn "❌ Configuration file: Not found"
    fi
    
    # Check documentation
    if [ -f "README.md" ]; then
        log_info "✅ Documentation: README.md"
    else
        log_warn "❌ Documentation: Not found"
    fi
}

# Function to run all tests
run_all_tests() {
    log_info "Running all NLM tests..."
    
    # Build the project if not already built
    if [ ! -f "build/nlm" ]; then
        log_info "Building project..."
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
        cd ..
    fi
    
    # Run Phase 6 demo
    run_phase6_demo
    
    # Run memory tests
    run_memory_tests
}

# Function to show help
show_help() {
    echo "NLM Utility Script"
    echo "=================="
    echo ""
    echo "Usage: $0 <command> [options]"
    echo ""
    echo "Commands:"
    echo "  help                    Show this help message"
    echo "  status                  Show system status"
    echo "  simple <steps> <neurons>  Run a simple simulation"
    echo "  phase6                  Run Phase 6 integration demo"
    echo "  memory                  Run memory system tests"
    echo "  all                     Run all tests"
    echo ""
    echo "Examples:"
    echo "  $0 simple 100 1000     Run simulation with 1000 neurons for 100 steps"
    echo "  $0 phase6              Run Phase 6 integration demo"
    echo "  $0 all                 Run all tests"
}

# Main script logic
case "$1" in
    simple)
        run_simple_simulation "$2" "$3"
        ;;
    phase6)
        run_phase6_demo
        ;;
    memory)
        run_memory_tests
        ;;
    all)
        run_all_tests
        ;;
    status)
        show_system_status
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        log_error "Unknown command: $1"
        echo ""
        show_help
        exit 1
        ;;
esac
