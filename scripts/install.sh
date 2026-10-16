# Easy Installation and Setup Script
# This script provides a comprehensive setup for NLM installation and configuration

#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored messages
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Default configuration
DEFAULT_BUILD_DIR="build"
DEFAULT_INSTALL_DIR="/usr/local"
DEFAULT_CONFIG_FILE="configs/default.cfg"
DEFAULT_PYTHON_PACKAGE="nlm_py"

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to create build directory
create_build_directory() {
    local build_dir="${1:-$DEFAULT_BUILD_DIR}"
    if [ ! -d "$build_dir" ]; then
        print_info "Creating build directory: $build_dir"
        mkdir -p "$build_dir"
    else
        print_info "Build directory already exists: $build_dir"
    fi
}

# Function to configure CMake
configure_cmake() {
    local build_dir="${1:-$DEFAULT_BUILD_DIR}"
    local install_dir="${2:-$DEFAULT_INSTALL_DIR}"
    
    cd "$build_dir" || { print_error "Cannot cd to $build_dir"; exit 1; }
    
    print_info "Configuring CMake with install directory: $install_dir"
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$install_dir" \
        -DNLM_ENABLE_PYTHON_BINDINGS=ON \
        -DNLM_ENABLE_TESTS=ON \
        -DNLM_ENABLE_PERFORMANCE_PROFILING=ON \
        -DNLM_ENABLE_CHECKPOINTING=ON \
        -DNLM_ENABLE_DEVELOPMENT=ON \
        -DNLM_ENABLE_MEMORY_POOLS=ON \
        -DNLM_ENABLE_VECTORIZATION=ON \
        -DNLM_ENABLE_MPI=OFF \
        -DNLM_ENABLE_OPENMP=OFF
    
    if [ $? -eq 0 ]; then
        print_info "CMake configuration successful"
    else
        print_error "CMake configuration failed"
        exit 1
    fi
}

# Function to build NLM
build_nlm() {
    local build_dir="${1:-$DEFAULT_BUILD_DIR}"
    local jobs="${2:-$(nproc)}"
    
    cd "$build_dir" || { print_error "Cannot cd to $build_dir"; exit 1; }
    
    print_info "Building NLM with $jobs jobs"
    
    make -j$jobs
    
    if [ $? -eq 0 ]; then
        print_info "Build successful"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Function to install NLM
install_nlm() {
    local build_dir="${1:-$DEFAULT_BUILD_DIR}"
    local install_dir="${2:-$DEFAULT_INSTALL_DIR}"
    
    cd "$build_dir" || { print_error "Cannot cd to $build_dir"; exit 1; }
    
    print_info "Installing NLM to: $install_dir"
    
    make install
    
    if [ $? -eq 0 ]; then
        print_info "Installation successful"
    else
        print_error "Installation failed"
        exit 1
    fi
}

# Function to setup Python bindings
install_python_bindings() {
    local python_dir="${1:-./python}"
    local install_dir="${2:-$DEFAULT_INSTALL_DIR}"
    
    if [ ! -d "$python_dir" ]; then
        print_warning "Python directory not found: $python_dir"
        return
    fi
    
    print_info "Setting up Python bindings"
    
    cd "$python_dir" || { print_error "Cannot cd to $python_dir"; exit 1; }
    
    # Check for Python development headers
    if ! command_exists python3-config; then
        print_error "Python development headers not found"
        return
    fi
    
    # Install Python package
    pip3 install -e .
    
    if [ $? -eq 0 ]; then
        print_info "Python bindings installation successful"
    else
        print_error "Python bindings installation failed"
        exit 1
    fi
}

# Function to create configuration files
create_config_files() {
    local config_dir="${1:-./configs}"
    
    if [ ! -d "$config_dir" ]; then
        print_info "Creating configs directory: $config_dir"
        mkdir -p "$config_dir"
    fi
    
    # Create default configuration file
    if [ ! -f "$config_dir/default.cfg" ]; then
        print_info "Creating default configuration file"
        cat > "$config_dir/default.cfg" << EOF
# NLM Default Configuration File
# ========================

# Core simulation parameters
neuron_count = 500
region_count = 1
simulation_timestep = 0.001
random_seed = 42
connection_probability = 0.15

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Development parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory parameters
working_memory_size = 100

# Sensory parameters
vision_width = 16
vision_height = 16

# Debug and performance parameters
enable_debug_logging = false
enable_performance_profiling = false
max_simulation_steps = 1000
checkpoint_interval = 100

# Command line arguments (can be overridden)
cmd_args = --help
EOF
        print_info "Created: $config_dir/default.cfg"
    fi
    
    # Create example configuration file
    if [ ! -f "$config_dir/example.cfg" ]; then
        print_info "Creating example configuration file"
        cat > "$config_dir/example.cfg" << EOF
# NLM Example Configuration File
# ==============================

# Custom neuron configuration
neuron_count = 1000
region_count = 2
simulation_timestep = 0.0005
random_seed = 12345
connection_probability = 0.2

# Modified plasticity
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 25.0

# Enhanced development
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002

# Larger memory
working_memory_size = 200

# Higher resolution vision
vision_width = 32
vision_height = 32

# Enable debugging
enable_debug_logging = true
enable_performance_profiling = true
max_simulation_steps = 5000
checkpoint_interval = 500

# Custom command line
# cmd_args = --mode debug --verbose --neurons 1000
EOF
        print_info "Created: $config_dir/example.cfg"
    fi
    
    # Create README for configuration
    if [ ! -f "$config_dir/README.md" ]; then
        print_info "Creating configuration README"
        cat > "$config_dir/README.md" << EOF
# NLM Configuration Files

This directory contains configuration files for the NLM (Neural Learning Machine) project.

## Configuration File Format

NLM uses a simple key=value format for configuration files:

```
key = value
```

## Parameters

### Core Simulation
- `neuron_count`: Number of neurons in the network (default: 500)
- `region_count`: Number of neural regions (default: 1)
- `simulation_timestep`: Simulation timestep in seconds (default: 0.001)
- `random_seed`: Random seed for reproducible simulations (default: 42)
- `connection_probability`: Probability of connections between neurons (default: 0.15)

### Plasticity
- `stdp_ltp_weight`: STDP long-term potentiation weight (default: 0.02)
- `stdp_ltd_weight`: STDP long-term depression weight (default: 0.015)
- `stdp_tau`: STDP time constant (default: 20.0)

### Development
- `synaptogenesis_rate`: Rate of new synapse formation (default: 0.0001)
- `pruning_rate`: Rate of synapse elimination (default: 0.00001)

### Memory
- `working_memory_size`: Size of working memory buffer (default: 100)

### Sensory
- `vision_width`: Vision sensor width (default: 16)
- `vision_height`: Vision sensor height (default: 16)

### Debug and Performance
- `enable_debug_logging`: Enable debug logging output (default: false)
- `enable_performance_profiling`: Enable performance profiling (default: false)
- `max_simulation_steps`: Maximum number of simulation steps (default: 1000)
- `checkpoint_interval`: Interval for checkpoint saving (default: 100)

## Usage

To use a configuration file with NLM:

```bash
./nlm --config configs/example.cfg
```

To override parameters from command line:

```bash
./nlm --config configs/default.cfg --neurons 1000 --plasticity
```
EOF
        print_info "Created: $config_dir/README.md"
    fi
}

# Function to create documentation
create_documentation() {
    local doc_dir="${1:-./docs}"
    
    if [ ! -d "$doc_dir" ]; then
        print_info "Creating documentation directory: $doc_dir"
        mkdir -p "$doc_dir"
    fi
    
    # Create installation guide
    cat > "$doc_dir/INSTALLATION.md" << EOF
# NLM Installation Guide

This document provides instructions for installing and setting up the NLM (Neural Learning Machine) project.

## Quick Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. Create build directory and configure:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

3. Build:
   ```bash
   make -j$(nproc)
   ```

4. Install (optional):
   ```bash
   sudo make install
   ```

## Running NLM

### Basic Usage

```bash
./nlm
```

### With Configuration File

```bash
./nlm --config configs/example.cfg
```

### Command Line Options

```bash
./nlm --help
```

### Different Modes

```bash
./nlm --mode debug
./nlm --mode profile --duration 60
./nlm --mode benchmark
```

## Configuration

Configuration files are located in the `configs/` directory.

- `configs/default.cfg`: Default configuration
- `configs/example.cfg`: Example configuration

To create custom configuration:

```bash
cp configs/example.cfg configs/custom.cfg
# Edit custom.cfg with your desired parameters
```

## Python Bindings

If Python bindings are enabled (default), you can use NLM from Python:

```python
import nlm

# Create a configuration
config = nlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 2)

# Create brain
brain = nlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)
```

## Performance Optimization

NLM supports several performance optimizations:

- **Memory Pools**: Pre-allocated memory for reduced fragmentation
- **Event Queues**: Efficient task scheduling
- **SIMD Vectorization**: Parallel processing for neural computations
- **Checkpointing**: Save and restore brain states

## Troubleshooting

### Common Issues

1. **Build failures**: Ensure all required dependencies are installed
2. **Python bindings**: Install development headers for Python
3. **Performance**: Check system resources and adjust configuration

### Getting Help

For additional help, refer to:

- Documentation in the `docs/` directory
- Command line help: `./nlm --help`
- NLM community forums

## Support

For support and issues, please visit the NLM repository.
EOF

    print_info "Created: $doc_dir/INSTALLATION.md"
}

# Function to create run script
create_run_script() {
    local script_dir="${1:-./scripts}"
    
    if [ ! -d "$script_dir" ]; then
        print_info "Creating scripts directory: $script_dir"
        mkdir -p "$script_dir"
    fi
    
    # Create run script
    cat > "$script_dir/run.sh" << 'EOF'
#!/bin/bash

# NLM Run Script
# ========================

# This script provides easy access to NLM execution modes

BUILD_DIR="${1:-build}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Function to display help
function show_help() {
    echo "NLM Run Script"
    echo "==============="
    echo ""
    echo "Usage: $0 [OPTIONS] [MODE] [ARGS...]"
    echo ""
    echo "Options:"
    echo "  --build-dir DIR    Build directory (default: build)"
    echo "  --help             Show this help message"
    echo ""
    echo "Modes:"
    echo "  demo              Run basic demo (default)"
    echo "  debug             Run with debug output"
    echo "  profile           Profile performance"
    echo "  benchmark         Run performance benchmarks"
    echo "  experiment        Run learning experiments"
    echo ""
    echo "Arguments passed to NLM binary"
    echo ""
    echo "Examples:"
    echo "  $0 demo --config configs/example.cfg"
    echo "  $0 debug --verbose --neurons 1000"
    echo "  $0 profile --duration 60"
    echo "  $0 benchmark --iterations 3"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            break
            ;;
    esac
done

# Set mode and arguments
MODE="${1:-demo}"
ARGS="${@:2}"

# Check if build directory exists
if [ ! -f "$BUILD_DIR/nlm" ]; then
    echo "Error: NLM executable not found in $BUILD_DIR"
    echo "Please build NLM first:"
    echo "  mkdir $BUILD_DIR"
    echo "  cd $BUILD_DIR"
    echo "  cmake .."
    echo "  make -j$(nproc)"
    exit 1
fi

# Run NLM
"$BUILD_DIR/nlm" "$MODE" $ARGS
EOF

    chmod +x "$script_dir/run.sh"
    print_info "Created: $script_dir/run.sh"
}

# Function to create CI/CD script
create_ci_script() {
    local script_dir="${1:-./scripts}"
    
    cat > "$script_dir/ci.sh" << 'EOF'
#!/bin/bash

# NLM CI/CD Script
# =================

# This script provides continuous integration and deployment functionality

set -e

BUILD_DIR="${1:-build}"
INSTALL_DIR="${2:-/usr/local}"
PYTHON_DIR="${3:-./python}"

print_info() {
    echo -e "\033[0;32m[CI]${NC} $1"
}

print_error() {
    echo -e "\033[0;31m[CI]${NC} $1"
}

# Create build directory
print_info "Creating build directory"
mkdir -p "$BUILD_DIR"

# Configure CMake
print_info "Configuring CMake"
cd "$BUILD_DIR"
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_PERFORMANCE_PROFILING=ON \
    -DNLM_ENABLE_CHECKPOINTING=ON \
    -DNLM_ENABLE_DEVELOPMENT=ON \
    -DNLM_ENABLE_MEMORY_POOLS=ON \
    -DNLM_ENABLE_VECTORIZATION=ON

# Build
print_info "Building NLM"
make -j$(nproc)

# Run tests
print_info "Running tests"
if [ -f "./nlm_tests" ]; then
    ./nlm_tests
else
    print_warning "Test executable not found, skipping tests"
fi

# Install
print_info "Installing NLM"
make install

# Install Python bindings
if [ -d "$PYTHON_DIR" ]; then
    print_info "Installing Python bindings"
    cd "$PYTHON_DIR"
    pip3 install -e .
fi

print_info "CI completed successfully"
EOF

    chmod +x "$script_dir/ci.sh"
    print_info "Created: $script_dir/ci.sh"
}

# Function to create development script
create_dev_script() {
    local script_dir="${1:-./scripts}"
    
    cat > "$script_dir/dev.sh" << 'EOF'
#!/bin/bash

# NLM Development Script
# ========================

# This script provides development and debugging functionality

set -e

BUILD_DIR="${1:-build}"
CONFIG_FILE="${2:-configs/default.cfg}"

print_info() {
    echo -e "\033[0;32m[DEV]${NC} $1"
}

print_error() {
    echo -e "\033[0;31m[DEV]${NC} $1"
}

# Check if build directory exists
if [ ! -f "$BUILD_DIR/nlm" ]; then
    print_error "NLM executable not found in $BUILD_DIR"
    print_info "Building NLM..."
    cd "$BUILD_DIR"
    cmake .. && make -j$(nproc)
fi

# Run NLM with development configuration
print_info "Running NLM with configuration: $CONFIG_FILE"
"$BUILD_DIR/nlm" --config "$CONFIG_FILE" --mode debug --verbose

# Capture performance data
print_info "Capturing performance data"
mkdir -p "performance"
"$BUILD_DIR/nlm" --mode profile --duration 30 --output "performance/profile_$(date +%Y%m%d_%H%M%S).json"
EOF

    chmod +x "$script_dir/dev.sh"
    print_info "Created: $script_dir/dev.sh"
}

# Main execution
main() {
    echo "NLM Easy Installation and Setup"
    echo "==============================="
    echo ""
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --build-dir)
                BUILD_DIR="$2"
                shift 2
                ;;
            --install-dir)
                INSTALL_DIR="$2"
                shift 2
                ;;
            --python-dir)
                PYTHON_DIR="$2"
                shift 2
                ;;
            --config-dir)
                CONFIG_DIR="$2"
                shift 2
                ;;
            --doc-dir)
                DOC_DIR="$2"
                shift 2
                ;;
            --script-dir)
                SCRIPT_DIR="$2"
                shift 2
                ;;
            --help)
                echo "Usage: $0 [OPTIONS]"
                echo ""
                echo "Options:"
                echo "  --build-dir DIR        Build directory (default: build)"
                echo "  --install-dir DIR      Installation directory (default: /usr/local)"
                echo "  --python-dir DIR       Python directory (default: ./python)"
                echo "  --config-dir DIR       Configuration directory (default: ./configs)"
                echo "  --doc-dir DIR          Documentation directory (default: ./docs)"
                echo "  --script-dir DIR       Scripts directory (default: ./scripts)"
                echo "  --help                 Show this help message"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
    
    # Execute setup steps
    print_info "Starting NLM setup..."
    
    create_build_directory "$BUILD_DIR"
    create_config_files "$CONFIG_DIR"
    create_documentation "$DOC_DIR"
    create_run_script "$SCRIPT_DIR"
    create_ci_script "$SCRIPT_DIR"
    create_dev_script "$SCRIPT_DIR"
    
    # Configure CMake
    print_info "Configuring CMake"
    configure_cmake "$BUILD_DIR" "$INSTALL_DIR"
    
    # Build
    print_info "Building NLM"
    build_nlm "$BUILD_DIR"
    
    # Install Python bindings (optional)
    if [ -d "$PYTHON_DIR" ]; then
        print_info "Installing Python bindings"
        install_python_bindings "$PYTHON_DIR" "$INSTALL_DIR"
    fi
    
    # Install NLM
    print_info "Installing NLM"
    install_nlm "$BUILD_DIR" "$INSTALL_DIR"
    
    print_info "NLM setup completed successfully!"
    print_info ""
    print_info "You can now run NLM using:"
    print_info "  $SCRIPT_DIR/run.sh demo"
    print_info "  $SCRIPT_DIR/dev.sh"
    print_info "  ./$BUILD_DIR/nlm --help"
    print_info ""
    print_info "For more information, see:"
    print_info "  $DOC_DIR/INSTALLATION.md"
}

# Execute main function with all arguments
main "$@"
EOF

    chmod +x "$script_dir/install.sh"
    print_info "Created: $script_dir/install.sh"
}

# Execute main function
main "$@"
