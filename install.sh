#!/bin/bash

# NLM Project - Installation and Setup Script
# 
# This script sets up the NLM (Neural Learning Machine) project for development and usage.
# It handles both development and installation setups.

set -e

PROJECT_NAME="NLM"
PROJECT_VERSION="0.1.0 (Phase 6 Final Integration)"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print functions
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

# Check if we're in the right directory
check_environment() {
    print_info "Checking NLM project environment..."
    
    # Check if we're in the project root
    if [ ! -f "$PROJECT_ROOT/README.md" ]; then
        print_error "README.md not found. Are you in the correct directory?"
        exit 1
    fi
    
    # Check for essential files
    local essential_files=(
        "src/main.cpp"
        "pyproject.toml"
        "CMakeLists.txt"
        "easy_usage.md"
        "HOW_TO_USE.md"
    )
    
    for file in "${essential_files[@]}"; do
        if [ ! -f "$PROJECT_ROOT/$file" ]; then
            print_error "Essential file not found: $file"
            exit 1
        fi
    done
    
    print_success "Environment check passed"
}

# Install Python dependencies
install_python_deps() {
    print_info "Installing Python dependencies..."
    
    # Check if pip is available
    if ! command -v pip &> /dev/null && ! command -v pip3 &> /dev/null; then
        print_error "pip or pip3 is not available"
        exit 1
    fi
    
    # Install required packages
    pip3 install --upgrade pip setuptools wheel scikit-build-core pybind11 numpy pytest
    
    if [ $? -eq 0 ]; then
        print_success "Python dependencies installed successfully"
    else
        print_error "Failed to install Python dependencies"
        exit 1
    fi
}

# Build the C++ library
build_cpp() {
    print_info "Building NLM C++ library..."
    
    # Create build directory
    mkdir -p "$PROJECT_ROOT/build"
    cd "$PROJECT_ROOT/build"
    
    # Configure with CMake
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_CXX_STANDARD=20 \
             -DCMAKE_CXX_STANDARD_REQUIRED=ON \
             -DPYTHON_EXECUTABLE=$(which python3)
    
    if [ $? -ne 0 ]; then
        print_error "CMake configuration failed"
        exit 1
    fi
    
    # Build the library
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        print_success "C++ library built successfully"
    else
        print_error "Build failed"
        exit 1
    fi
    
    cd "$SCRIPT_DIR"
}

# Install Python package
install_python_package() {
    print_info "Installing NLM Python package..."
    
    # Install in development mode
    cd "$PROJECT_ROOT"
    pip3 install -e .
    
    if [ $? -eq 0 ]; then
        print_success "Python package installed successfully"
    else
        print_error "Python package installation failed"
        exit 1
    fi
}

# Install CLI tool
install_cli() {
    print_info "Installing NLM CLI tool..."
    
    # Copy CLI script to bin directory
    mkdir -p "$HOME/.local/bin"
    cp "$PROJECT_ROOT/src/nlm_cli.py" "$HOME/.local/bin/nlm"
    chmod +x "$HOME/.local/bin/nlm"
    
    # Add to PATH if not already present
    if ! echo "$PATH" | grep -q "\.local/bin"; then
        print_warning "Adding ~/.local/bin to PATH"
        echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
        echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.zshrc" 2>/dev/null || true
    fi
    
    print_success "CLI tool installed to ~/.local/bin/nlm"
}

# Create development shortcuts
create_dev_shortcuts() {
    print_info "Creating development shortcuts..."
    
    # Create compile script
    cat > "$PROJECT_ROOT/compile.sh" << 'EOF'
#!/bin/bash
# Compile NLM project

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

mkdir -p "$PROJECT_ROOT/build"
cd "$PROJECT_ROOT/build"

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

cd "$PROJECT_ROOT"

echo "Build complete. Run with:"
echo "  ./build/nlm           # Run demo"
echo "  ./build/nlm_test     # Run tests"
EOF

    # Create run script
    cat > "$PROJECT_ROOT/run.sh" << 'EOF'
#!/bin/bash
# Run NLM simulation

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Run the Phase 6 demo
./build/nlm_phase6_demo
EOF

    # Create test script
    cat > "$PROJECT_ROOT/test.sh" << 'EOF'
#!/bin/bash
# Run NLM tests

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ -f "$PROJECT_ROOT/build/nlm_test" ]; then
    ./build/nlm_test
else
    echo "Test executable not found. Run compile.sh first."
fi
EOF

    # Make scripts executable
    chmod +x "$PROJECT_ROOT/compile.sh" "$PROJECT_ROOT/run.sh" "$PROJECT_ROOT/test.sh"
    
    print_success "Development shortcuts created"
}

# Create documentation
create_documentation() {
    print_info "Creating additional documentation..."
    
    # Create API documentation file
    cat > "$PROJECT_ROOT/API_REFERENCE.md" << 'EOF'
# NLM Python API Reference

## Overview

NLM (Neural Learning Machine) provides a Python interface to the computational brain simulation framework.

## Core Classes

### Brain

#### Methods
- `__init__(config)` - Create brain with configuration
- `initialize()` - Initialize the brain
- `step(step_number)` - Run one simulation step
- `step(step_number, time)` - Run step with timestamp
- `reset()` - Reset brain state
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getFiringNeuronCount()` - Get firing neuron count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I ratio
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

### Config

#### Methods
- `createDefaultConfig()` - Create default configuration
- `set(key, value, source=ConfigSource::Default)` - Set configuration value
- `get(key)` - Get configuration value
- `has(key)` - Check if configuration has key
- `getKeys()` - Get all configuration keys
- `loadFromFile(filepath)` - Load from file
- `saveToFile(filepath)` - Save to file

### AgentBrain

#### Methods
- `__init__(brain)` - Create agent with brain
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward
- `updateDevelopment(timestep)` - Update development
- `getDevelopmentalStage()` - Get developmental stage
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getNeuromodulationLevel()` - Get neuromodulation level

### SimpleWorld

#### Methods
- `createSimpleWorld()` - Create simple world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `update(timestep)` - Update world
- `setAgentStart(x, y)` - Set agent start position
- `applyMotorCommand(action, time)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body
- `getVisionWidth()` - Get vision width
- `getVisionHeight()` - Get vision height

## Neuron Types

- `NeuronType.Excitatory` - Excitatory neurons
- `NeuronType.Inhibitory` - Inhibitory neurons
- `NeuronType.Sensory` - Sensory neurons
- `NeuronType.Motor` - Motor neurons
- `NeuronType.Modulatory` - Modulatory neurons
- `NeuronType.Internal` - Internal neurons

## Developmental Stages

- `DevelopmentalStage.Initial` - Initial stage
- `DevelopmentalStage.CriticalPeriod` - Critical period
- `DevelopmentalStage.Maturation` - Maturation stage
- `DevelopmentalStage.Adult` - Adult stage
- `DevelopmentalStage.Aging` - Aging stage

## Action Types

- `ActionType.MoveForward` - Move forward
- `ActionType.MoveBackward` - Move backward
- `ActionType.TurnLeft` - Turn left
- `ActionType.TurnRight` - Turn right
- `ActionType.Interact` - Interact
- `ActionType.Eat` - Eat
- `ActionType.LookLeft` - Look left
- `ActionType.LookRight` - Look right
- `ActionType.Wait` - Wait

## World Object Types

- `WorldObjectType.Empty` - Empty space
- `WorldObjectType.Resource` - Resource
- `WorldObjectType.Hazard` - Hazard
- `WorldObjectType.Wall` - Wall
- `WorldObjectType.Marker` - Marker
EOF

    # Create CLI documentation
    cat > "$PROJECT_ROOT/CLI_REFERENCE.md" << 'EOF'
# NLM Command Line Interface Reference

## Overview

NLM provides a comprehensive command-line interface for running simulations, demos, and analysis.

## Commands

### nlm run
Run a simulation with custom configuration.

#### Options
- `--brain-config PATH` - Path to configuration file (default: configs/default.cfg)
- `--world-width INT` - World width (default: 30)
- `--world-height INT` - World height (default: 30)
- `--world-vision-width INT` - Vision width (default: 10)
- `--world-vision-height INT` - Vision height (default: 10)
- `--steps INT` - Number of steps (default: 1000)
- `--timestep FLOAT` - Timestep in seconds (default: 0.1)
- `--output FILE` - Output file for results
- `--checkpoint FILE` - Load checkpoint
- `--save-checkpoint` - Enable checkpoint saving
- `--load-checkpoint FILE` - Load checkpoint

### nlm demo
Run a demonstration with preset settings.

#### Options
- `--mode {phase2,phase3,phase4,phase5,phase6}` - Demo mode (default: phase6)
- `--fast` - Run in fast mode
- `--detailed` - Run in detailed mode
- `--output FILE` - Output file for results

### nlm benchmark
Run performance benchmarks.

#### Options
- `--brain-types {small,medium,large,xlarge}` - Brain types to benchmark
- `--repeats INT` - Number of repeats (default: 3)
- `--parallel INT` - Parallel threads (default: 1)
- `--metrics {speed,memory,spikes,learning}` - Metrics to collect
- `--output FILE` - Output file for results

### nlm config
Configuration management.

#### Commands
- `nlm config create --output FILE --brain-count INT --region-count INT --connection-probability FLOAT` - Create configuration
- `nlm config list` - List configuration options

### nlm explore
Explore different simulation scenarios.

#### Options
- `--scene {maze,open,complex,social}` - Simulation scene (default: open)
- `--size INT` - World size (default: 30)
- `--enable-learning` - Enable learning subsystems
- `--steps INT` - Number of steps (default: 500)

### nlm analyze
Analyze simulation results.

#### Options
- `--input FILE` - Input results file
- `--type {performance,behavior,learning}` - Analysis type (default: performance)
- `--output FILE` - Output file for analysis

## Environment Variables

- `NLM_LOG_LEVEL` - Log level (debug, info, warning, error)
- `NLM_PARALLEL_THREADS` - Number of parallel threads
- `NLM_CHECKPOINT_DIR` - Checkpoint directory

## Configuration File Format

NLM uses simple key=value format for configuration files:

```
# NLM Configuration
brain.neuron_count = 1000
brain.synapse_density = 0.1
region_count = 1
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
development.enable = true
simulation_timestep = 0.001
random_seed = 42
```

## Output Formats

### JSON Results
All commands that produce output support JSON format:

```json
{
  "steps": 1000,
  "duration": 45.23,
  "speed": 22.1,
  "final_firing_rate": 12.5,
  "total_spikes": 5000,
  "curiosity_level": 0.75,
  "novelty_level": 0.45,
  "development_stage": "Adult",
  "working_memory_traces": 50,
  "episodes_stored": 25,
  "avg_reward": 0.82
}
```

## Examples

### Run a Simulation
```bash
nlm run --brain-config my_config.json --world-size 50 --steps 2000
```

### Run Phase 6 Demo
```bash
nlm demo --mode phase6 --detailed
```

### Benchmark Different Brain Sizes
```bash
nlm benchmark --brain-types small medium large --repeats 5
```

### Explore Maze Scene
```bash
nlm explore --scene maze --size 40 --enable-learning
```
EOF

    print_success "Documentation created"
}

# Update project configuration
update_project_config() {
    print_info "Updating project configuration..."
    
    # Update pyproject.toml to include CLI script
    if [ -f "$PROJECT_ROOT/pyproject.toml" ]; then
        # Add CLI script to project structure
        cat >> "$PROJECT_ROOT/pyproject.toml" << 'EOF'

# Scripts
[project.scripts]
nlm = "nlm_cli:main"
EOF
        
        print_success "Updated pyproject.toml"
    fi
}

# Run tests
test_installation() {
    print_info "Testing installation..."
    
    # Check if CLI is available
    if command -v nlm &> /dev/null; then
        print_success "CLI tool available"
        
        # Check help
        if nlm --help &> /dev/null; then
            print_success "CLI help works"
        else
            print_warning "CLI help may not be working"
        fi
    else
        print_error "CLI tool not found in PATH"
        return 1
    fi
    
    # Check Python package
    if python3 -c "import pynlm; print(f'NLM version: {pynlm.__version__}')" &> /dev/null; then
        print_success "Python package import successful"
    else
        print_error "Python package import failed"
        return 1
    fi
    
    print_success "Installation test passed"
}

# Main installation function
main() {
    echo "=============================================="
    echo "NLM Project Installation and Setup Script"
    echo "Phase 6: Final Integration"
    echo "=============================================="
    echo
    
    # Check environment
    check_environment
    echo
    
    # Install Python dependencies
    install_python_deps
    echo
    
    # Build C++ library
    build_cpp
    echo
    
    # Install Python package
    install_python_package
    echo
    
    # Install CLI tool
    install_cli
    echo
    
    # Create development shortcuts
    create_dev_shortcuts
    echo
    
    # Create documentation
    create_documentation
    echo
    
    # Update project configuration
    update_project_config
    echo
    
    # Test installation
    test_installation
    echo
    
    print_success "NLM installation completed successfully!"
    echo
    echo "=============================================="
    echo "Next steps:"
    echo "=============================================="
    echo "1. Run: $ nlm run --brain-config configs/default.cfg"
    echo "2. Run: $ nlm demo --mode phase6"
    echo "3. Run: $ nlm benchmark --brain-types small medium large"
    echo "4. Run: $ nlm explore --scene maze --enable-learning"
    echo "5. View documentation: cat easy_usage.md"
    echo "6. View API reference: cat API_REFERENCE.md"
    echo "7. View CLI reference: cat CLI_REFERENCE.md"
    echo
    echo "For development, use:"
    echo "  $ $PROJECT_ROOT/compile.sh    # Build library"
    echo "  $ $PROJECT_ROOT/run.sh        # Run demo"
    echo "  $ $PROJECT_ROOT/test.sh       # Run tests"
    echo "=============================================="
}

# Run main function
main "$@"