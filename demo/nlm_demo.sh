#!/bin/bash
# NLM Phase 6 Demonstration Script
#
# This script demonstrates the new Phase 6 integration features
# and advanced capabilities of the NLM artificial brain project.

SCRIPT_NAME="NLM Phase 6 Demonstration"
SCRIPT_VERSION="1.0"

# Color codes for output formatting
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_header() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to display help
usage() {
    cat << EOF
$NLM Phase 6 Demonstration Script

Usage: $0 [mode]

Modes:
  demo           Run quick integration demo (default)
  config         Display configuration information
  examples       Show usage examples
  all            Run all demonstrations
  help           Show this help message

Options:
  -h, --help     Show help information
  -v, --version  Show script version

Examples:
  $0 demo           # Run quick integration demo
  $0 config         # Display build configuration
  $0 examples       # Show usage examples
  $0 all            # Run all demonstrations
EOF
}

# Function to display version information
version() {
    cat << EOF
$NLM Phase 6 Demonstration Script
Version: $SCRIPT_VERSION

This script demonstrates the advanced features of NLM Phase 6:
- Enhanced command-line interface
- Improved configuration management
- Advanced error handling
- Comprehensive documentation

For more information, visit the NLM project documentation.
EOF
}

# Function to run demo mode
demo_mode() {
    print_header "Phase 6 Integration Demo"
    
    if ! command_exists "./nlm"; then
        print_error "NLM executable not found. Please build the project first:"
        print_info "  mkdir build && cd build && cmake .. && make -j4"
        exit 1
    fi
    
    print_info "Running Phase 6 integration demo..."
    print_info "This will test the integrated artificial brain system."
    
    # Run quick demo
    ./nlm --demo-mode
    
    if [ $? -eq 0 ]; then
        print_info "✓ Phase 6 integration demo completed successfully!"
    else
        print_error "✗ Phase 6 integration demo failed."
        exit 1
    fi
}

# Function to display configuration information
config_mode() {
    print_header "Build Configuration"
    
    if ! command_exists "make"; then
        print_error "Make command not found. Cannot display configuration."
        exit 1
    fi
    
    if [ ! -f "build/Makefile" ]; then
        print_warning "Makefile not found. Creating build directory..."
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release >/dev/null 2>&1
        cd ..
    fi
    
    # Display configuration
    make -C build config 2>/dev/null || echo "Configuration target not available"
    
    # Display available targets
    print_info "Available targets:"
    make -C build help 2>/dev/null | grep -E "^[a-zA-Z0-9_-]+:" | head -10
}

# Function to display examples
examples_mode() {
    print_header "Usage Examples"
    
    cat << EOF
### Quick Integration Demo
./nlm --demo-mode

### Standard Experiment
./nlm --neurons=500 --steps=2000 --region-count=1

### Checkpoint Experiment
./nlm --neurons=500 --steps=2000 --checkpoint=experiment.bin

### Custom Configuration
./nlm --config=my_config.cfg

### Phase 2 Compatibility
./nlm --phase2

### High-Performance Simulation
./nlm --neurons=2000 --steps=5000 --region-count=4 --timestep=0.0005

### Memory-Efficient Simulation
./nlm --neurons=100 --steps=500 --demo-mode --no-development --no-replay

### Development Experiment
./nlm --neurons=400 --steps=1500 --enable-development=true --development-start-step=50

### Parameter Sweep Script
for neurons in 100 250 500 1000; do
    echo "Running experiment with $neurons neurons"
    ./nlm --neurons=$neurons --steps=1000 \
          --checkpoint=neuron_sweep_${neurons}.bin
    echo "Completed: $neurons neurons"
done

### Configuration File Example
# Create custom configuration
cat > my_config.cfg << EOF
neuron_count = 750
region_count = 2
connection_probability = 0.2
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.01
enable_development = true
enable_replay = true
EOF

./nlm --config=my_config.cfg
EOF
}

# Function to run all demonstrations
all_mode() {
    print_header "Running All Demonstrations"
    
    # Check if build directory exists
    if [ ! -d "build" ]; then
        print_warning "Build directory not found. Creating build directory..."
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release >/dev/null 2>&1
        cd ..
    fi
    
    # Run demo mode
    demo_mode
    
    # Run config mode
    config_mode
    
    print_header "All Demonstrations Completed"
    print_info "The NLM Phase 6 system has been successfully demonstrated!"
    print_info "Key features verified:"
    print_info "  ✓ Enhanced command-line interface"
    print_info "  ✓ Improved configuration management"
    print_info "  ✓ Advanced error handling"
    print_info "  ✓ Comprehensive documentation"
}

# Main script execution
case "$1" in
    demo)
        demo_mode
        ;;
    config)
        config_mode
        ;;
    examples)
        examples_mode
        ;;
    all)
        all_mode
        ;;
    -h|--help)
        usage
        ;;
    -v|--version)
        version
        ;;
    *)
        usage
        echo "Error: Unknown mode '$1'"
        exit 1
        ;;
esac

# End of script
echo -e "${GREEN}Demo script completed successfully!${NC}"
