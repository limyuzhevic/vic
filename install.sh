#!/bin/bash

# Installation script for NLM (熙然) neural learning machine
# Phase 6: Final Integration Installer

echo "=== NLM (熙然) Installation Script ==="
echo "Phase 6: Installing Neural Learning Machine"
echo ""

# Configuration
INSTALL_DIR="${1:-/usr/local}"
BUILD_DIR="build"
PYTHON_INSTALL="true"
SYSTEM_INSTALL="true"
CREATE_CONFIG_DIR="true"
CREATE_SAMPLE_DATA="true"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --prefix)
            INSTALL_DIR="$2"
            shift 2
            ;;
        --no-python)
            PYTHON_INSTALL="false"
            shift
            ;;
        --no-system)
            SYSTEM_INSTALL="false"
            shift
            ;;
        --no-config)
            CREATE_CONFIG_DIR="false"
            shift
            ;;
        --no-sample-data)
            CREATE_SAMPLE_DATA="false"
            shift
            ;;
        --help)
            echo "Usage: $0 [options] [install_dir]"
            echo "Options:"
            echo "  --prefix DIR      Install to DIR (default: /usr/local)"
            echo "  --no-python       Don't install Python bindings"
            echo "  --no-system       Don't install system files"
            echo "  --no-config       Don't create configuration directory"
            echo "  --no-sample-data  Don't create sample data"
            echo "  --help            Show this help"
            exit 0
            ;;
        *)
            INSTALL_DIR="$1"
            shift
            ;;
    esac
done

# Check if running as root for system installation
if [ "$SYSTEM_INSTALL" = true ] && [ "$(id -u)" -ne 0 ]; then
    echo "WARNING: Running as non-root user. System installation may require sudo."
    echo "Some files may be installed to user directories instead."
    echo ""
fi

# Create install directory
mkdir -p "$INSTALL_DIR"

echo "Installing NLM to: $INSTALL_DIR"
echo ""

# Install build script
echo "Installing build script..."
mkdir -p "$INSTALL_DIR/bin"
cp "build.sh" "$INSTALL_DIR/bin/nlm-build"
chmod +x "$INSTALL_DIR/bin/nlm-build"

echo "✓ Build script installed to: $INSTALL_DIR/bin/nlm-build"

# Install main executables if available
echo ""
echo "Installing main executables..."

if [ -d "$BUILD_DIR" ]; then
    # Install main executable
    if [ -f "$BUILD_DIR/nlm" ]; then
        mkdir -p "$INSTALL_DIR/bin"
        cp "$BUILD_DIR/nlm" "$INSTALL_DIR/bin/nlm"
        chmod +x "$INSTALL_DIR/bin/nlm"
        echo "✓ Main executable installed to: $INSTALL_DIR/bin/nlm"
    fi
    
    # Install demo executables
    for demo in nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo nlm_test; do
        if [ -f "$BUILD_DIR/$demo" ]; then
            mkdir -p "$INSTALL_DIR/bin"
            cp "$BUILD_DIR/$demo" "$INSTALL_DIR/bin/$demo"
            chmod +x "$INSTALL_DIR/bin/$demo"
            echo "✓ $demo installed to: $INSTALL_DIR/bin/$demo"
        fi
    done
    
    # Install Python bindings if available
    if [ "$PYTHON_INSTALL" = true ]; then
        echo ""
        echo "Installing Python bindings..."
        
        if [ -f "./%PYPROJECT_FILE%" ]; then
            echo "Python bindings package found"
            echo "To install Python bindings manually, run:"
            echo "  pip install -e ."
        fi
        
        # Create Python package structure
        mkdir -p "$INSTALL_DIR/lib/python/nlm"
        if [ -d "python" ]; then
            cp -r python/* "$INSTALL_DIR/lib/python/nlm/" 2>/dev/null || echo "Warning: Could not copy Python bindings"
        fi
        
        echo "✓ Python bindings installed to: $INSTALL_DIR/lib/python/nlm"
    fi
else
    echo "WARNING: Build directory not found. Please run './build.sh' first."
    echo "Some executables may not be available for installation."
fi

# Create configuration directory
echo ""
echo "Creating configuration directory..."

if [ "$CREATE_CONFIG_DIR" = true ]; then
    mkdir -p "$INSTALL_DIR/etc/nlm"
    
    # Create default configuration file
    cat > "$INSTALL_DIR/etc/nlm/default.cfg" << 'EOF'
# NLM (熙然) Configuration File
# Default configuration for neural learning machine
# Phase 6: Final Integration

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
EOF
    
    echo "✓ Default configuration installed to: $INSTALL_DIR/etc/nlm/default.cfg"
fi

# Create sample data
echo ""
echo "Creating sample data..."

if [ "$CREATE_SAMPLE_DATA" = true ]; then
    mkdir -p "$INSTALL_DIR/share/nlm/samples"
    
    # Create sample Python script
    cat > "$INSTALL_DIR/share/nlm/samples/brain_explorer.py" << 'EOF'
#!/usr/bin/env python3
"""
Sample script for exploring NLM (熙然) Python API
"""

import pynlm
import time

def main():
    print("=== NLM Brain Explorer Sample ===")
    print("This sample demonstrates basic brain operations with NLM")
    print()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print()
    
    # Run simulation
    print("Running simulation...")
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            firing = brain.getFiringNeuronCount()
            avg_rate = brain.getAverageFiringRate()
            print(f"Step {step:3d}: {firing:4d} neurons firing, {avg_rate:.2f} Hz average")
    
    print()
    print("Simulation complete!")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Save brain state
    brain.save("brain_sample.bin")
    print("Brain state saved to brain_sample.bin")

if __name__ == "__main__":
    main()
EOF
    
    chmod +x "$INSTALL_DIR/share/nlm/samples/brain_explorer.py"
    
    # Create simple C++ example
    cat > "$INSTALL_DIR/share/nlm/samples/hello_nlm.cpp" << 'EOF'
// Simple NLM example in C++
// Demonstrates basic neural computation

#include <iostream>
#include <memory>

// Include NLM headers (adjust path as needed)
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"

int main() {
    std::cout << "=== NLM (熙然) Hello World ===" << std::endl;
    std::cout << "Phase 6: Final Integration of Neural Systems" << std::endl;
    std::cout << "" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.15f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "" << std::endl;
    
    // Run a few steps
    std::cout << "Running simulation..." << std::endl;
    for (int step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 10 == 0) {
            std::cout << "Step " << step << 
                         ": " << brain->getFiringNeuronCount() << 
                         " neurons firing" << std::endl;
        }
    }
    
    std::cout << "" << std::endl;
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
EOF
    
    echo "✓ Sample data created in: $INSTALL_DIR/share/nlm/samples"
fi

# Create documentation directory
echo ""
echo "Creating documentation..."

mkdir -p "$INSTALL_DIR/share/doc/nlm"

# Create basic README
 cat > "$INSTALL_DIR/share/doc/nlm/README.md" << 'EOF'
# NLM (熙然) - Neural Learning Machine

Phase 6: Final Integration of All Neural Systems

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Features

- **Real LIF neuron dynamics**: Leaky Integrate-and-Fire neurons with event-driven spike propagation
- **Multiple plasticity mechanisms**: STDP, Hebbian learning, and structural plasticity
- **Integrated memory systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, and novelty detection
- **Prediction system**: Sensory prediction and error computation
- **Cognition systems**: Neural planning, concept formation, and attention
- **Development**: Behavioral development affecting plasticity and learning
- **Checkpointing**: State save/load for resuming simulations
- **Replay**: Memory consolidation through replay mechanisms

## Installation

### From Source

```bash
# Build the project
cd /path/to/nlm
./build.sh

# Install system-wide (requires sudo)
sudo ./build.sh --install

# Or install to custom location
./build.sh --prefix=/opt/nlm --install
```

### Python Bindings

```bash
# Install Python bindings
pip install -e .
```

## Running

### Main Simulation

```bash
cd build
./nlm [options]
```

### Phase 6 Integration Demo

```bash
cd build
./nlm_phase6_demo
```

### Unit Tests

```bash
cd build
./nlm_test
```

### Python Examples

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

## Configuration

Configuration files are located in:
- `/usr/local/etc/nlm/default.cfg` (system default)
- `./configs/default.cfg` (project default)

Configuration can be overridden using command-line arguments or environment variables.

## Documentation

- `HOW_TO_USE.md`: Complete Python API documentation
- `easy_usage.md`: Beginner-friendly Python usage guide
- `docs/ARCHITECTURE.md`: System architecture overview
- `docs/SCIENCE.md`: Scientific background
- `docs/ROADMAP.md`: Development roadmap

## License

MIT License

## Authors

Research project — See docs for scientific background.
EOF
    
    echo "✓ Documentation installed to: $INSTALL_DIR/share/doc/nlm/README.md"

# Print installation summary
echo ""
echo "=== Installation Complete ==="
echo ""
echo "Installation directory: $INSTALL_DIR"
echo ""

if [ "$SYSTEM_INSTALL" = true ] && [ "$(id -u)" -eq 0 ]; then
    echo "System files installed with root privileges."
else
    echo "Note: Installation performed as user. Some files may be in user directories."
fi

echo ""
echo "Installed files:"
echo "  - Build script: $INSTALL_DIR/bin/nlm-build"
echo "  - Main executable: $INSTALL_DIR/bin/nlm"
echo "  - Demo executables: $INSTALL_DIR/bin/nlm_phase*_demo"
echo "  - Test executable: $INSTALL_DIR/bin/nlm_test"
echo "  - Python bindings: $INSTALL_DIR/lib/python/nlm/"
echo "  - Configuration: $INSTALL_DIR/etc/nlm/default.cfg"
echo "  - Sample data: $INSTALL_DIR/share/nlm/samples/"
echo "  - Documentation: $INSTALL_DIR/share/doc/nlm/README.md"
echo ""
echo "Next steps:"
echo "  1. Build the project: $INSTALL_DIR/bin/nlm-build"
echo "  2. Run tests: $INSTALL_DIR/bin/nlm-build --no-tests"
echo "  3. Install Python bindings: pip install -e ."
echo "  4. Run main simulation: $INSTALL_DIR/bin/nlm [options]"
echo "  5. Run Phase 6 demo: $INSTALL_DIR/bin/nlm_phase6_demo"
echo ""
echo "For more information, see: $INSTALL_DIR/share/doc/nlm/README.md"
