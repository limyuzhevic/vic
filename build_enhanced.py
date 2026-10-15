#!/bin/bash

# NLM Enhanced Python Bindings Build and Installation Script
# This script builds the enhanced Python bindings and sets up the development environment

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_ROOT="/workspace/app"
BUILD_DIR="${PROJECT_ROOT}/build"
PYTHON_LIB_DIR="${PROJECT_ROOT}/python"

# Print header
function print_header {
    echo -e "${BLUE}==============================================================================${NC}"
    echo -e "${BLUE}NLM Enhanced Python Bindings Build Script${NC}"
    echo -e "${BLUE}==============================================================================${NC}"
    echo -e "${YELLOW}Building enhanced Python bindings for NLM neural simulation framework${NC}"
    echo -e "${YELLOW}Enhanced features include:${NC}"
    echo -e "${YELLOW}  • Version information and module metadata${NC}"
    echo -e "${YELLOW}  • Custom exception types for better error handling${NC}"
    echo -e "${YELLOW}  • Pythonic properties and convenience methods${NC}"
    echo -e "${YELLOW}  • Enhanced documentation and examples${NC}"
    echo -e "${YELLOW}  • Improved error handling and recovery${NC}"
    echo -e "${YELLOW}  • Performance monitoring and benchmarking${NC}"
    echo -e "${YELLOW}  • Configuration validation and management${NC}"
    echo -e "${BLUE}==============================================================================${NC}"
}

# Check dependencies
function check_dependencies {
    echo -e "${YELLOW}Checking dependencies...${NC}"
    
    # Check Python
    if ! command -v python3 &> /dev/null; then
        echo -e "${RED}ERROR: python3 is not installed${NC}"
        exit 1
    fi
    
    # Check pip
    if ! command -v pip3 &> /dev/null; then
        echo -e "${RED}ERROR: pip3 is not installed${NC}"
        exit 1
    fi
    
    # Check C++ compiler
    if ! command -v g++ &> /dev/null; then
        echo -e "${RED}ERROR: g++ compiler is not installed${NC}"
        exit 1
    fi
    
    # Check CMake
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}ERROR: cmake is not installed${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ All dependencies found${NC}"
}

# Create build directory
function setup_build_environment {
    echo -e "${YELLOW}Setting up build environment...${NC}"
    
    if [ ! -d "${BUILD_DIR}" ]; then
        mkdir -p "${BUILD_DIR}"
        echo -e "${GREEN}✓ Created build directory: ${BUILD_DIR}${NC}"
    else
        echo -e "${YELLOW}Build directory already exists: ${BUILD_DIR}${NC}"
    fi
}

# Configure CMake
function configure_cmake {
    echo -e "${YELLOW}Configuring CMake build...${NC}"
    
    cd "${BUILD_DIR}"
    
    # Configure with enhanced Python bindings support
    cmake ..
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ CMake configuration successful${NC}"
    else
        echo -e "${RED}ERROR: CMake configuration failed${NC}"
        exit 1
    fi
}

# Build the project
function build_project {
    echo -e "${YELLOW}Building project...${NC}"
    
    cd "${BUILD_DIR}"
    
    # Build with all components
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Project build successful${NC}"
    else
        echo -e "${RED}ERROR: Project build failed${NC}"
        exit 1
    fi
}

# Build Python bindings
function build_python_bindings {
    echo -e "${YELLOW}Building Python bindings...${NC}"
    
    cd "${PROJECT_ROOT}"
    
    # Check if Python development packages are installed
    if ! pip3 show pybind11 scikit-build-core > /dev/null 2>&1; then
        echo -e "${YELLOW}Installing Python development dependencies...${NC}"
        pip3 install pybind11 scikit-build-core pytest numpy
    fi
    
    # Build Python bindings using the project's setup
    if [ -f "pyproject.toml" ]; then
        echo -e "${YELLOW}Building Python package with pyproject.toml...${NC}"
        pip3 install -e . --no-build-isolation
    else
        echo -e "${YELLOW}Building Python bindings directly...${NC}"
        # Fallback to direct build if pyproject.toml doesn't exist
        cd "${BUILD_DIR}"
        make pynlm_pybind11
    fi
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Python bindings build successful${NC}"
    else
        echo -e "${RED}ERROR: Python bindings build failed${NC}"
        exit 1
    fi
}

# Create enhanced examples and documentation
function create_enhanced_examples {
    echo -e "${YELLOW}Creating enhanced examples and documentation...${NC}"
    
    # Create examples directory
    mkdir -p "${PROJECT_ROOT}/examples/enhanced"
    
    # Create example scripts
    cat > "${PROJECT_ROOT}/examples/enhanced/example_basic.py" << 'EOF'
import pynlm

# Basic enhanced example demonstrating Pythonic API
print("=== NLM Enhanced Basic Example ===")

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use enhanced Pythonic properties
print(f"Created brain with {brain.neuron_count} neurons")
print(f"Initial development stage: {brain.development_stage}")

# Run enhanced simulation
spike_count = brain.simulate(50, 0.1)
print(f"Simulation complete with {spike_count} total spikes")

# Display enhanced status
print(f"\nBrain Status:\n{brain.get_status()}")
EOF

    cat > "${PROJECT_ROOT}/examples/enhanced/example_agent.py" << 'EOF'
import pynlm

# Advanced agent example with enhanced features
print("=== NLM Enhanced Agent Example ===")

# Setup components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Enabled advanced subsystems")
print(f"Reward Modulation: {agent.isRewardModulationEnabled()}")
print(f"Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"Development: {agent.isDevelopmentEnabled()}")
print(f"Curiosity: {agent.isCuriosityEnabled()}")

# Run episode with enhanced monitoring
for step in range(30):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

print(f"\nEpisode complete! Statistics:\n{agent.get_episode_stats()}")
EOF

    cat > "${PROJECT_ROOT}/examples/enhanced/example_error_handling.py" << 'EOF'
import pynlm

# Error handling and recovery example
print("=== NLM Enhanced Error Handling Example ===")

try:
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Testing enhanced error handling...")
    
    # Test enhanced simulation with error handling
    spike_count = brain.simulate(20, 0.1)
    print(f"✓ Enhanced simulation completed: {spike_count} spikes")
    
    # Test status retrieval
    status = brain.get_status()
    print("✓ Status retrieval completed successfully")
    
    # Test exception handling
    try:
        # This should work without issues
        brain.step(0)
        print("✓ Basic step completed successfully")
    except pynlm.SimulationStepException as e:
        print(f"Caught simulation exception: {e}")
    
    print("\nError handling test completed successfully!")
    
except pynlm.BrainInitializationException as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ConfigurationException as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
EOF

    echo -e "${GREEN}✓ Enhanced examples created${NC}"
}

# Create installation guide
function create_installation_guide {
    echo -e "${YELLOW}Creating installation guide...${NC}"
    
    cat > "${PROJECT_ROOT}/INSTALL_ENHANCED.md" << 'EOF'
# NLM Enhanced Python Bindings Installation Guide

This guide covers installing and using the enhanced Python bindings for NLM.

## Prerequisites

- Python 3.8+
- pip 21.0+
- C++ compiler (g++ 10+, clang 12+, or MSVC 2019+)
- CMake 3.16+

## Quick Installation

### 1. Install Python Dependencies

```bash
pip install pybind11 scikit-build-core pytest numpy
```

### 2. Build and Install NLM

```bash
cd /path/to/nlm
mkdir build
cd build
cmake ..
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### 3. Verify Installation

```python
import pynlm

# Check version and module info
print(f"Version: {pynlm.__version__}")
print(f"Author: {pynlm.__author__}")
print(f"License: {pynlm.__license__}")

# Create basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"✓ Brain created with {brain.neuron_count} neurons")
```

## Enhanced Features Available

### 1. Version Information

```python
import pynlm
print(f"NLM Python Bindings Version: {pynlm.__version__}")
print(f"Author: {pynlm.__author__}")
print(f"License: {pynlm.__license__}")
```

### 2. Enhanced Error Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.simulate(100, 0.1)
except pynlm.BrainInitializationException as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ConfigurationException as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationStepException as e:
    print(f"Simulation step error: {e}")
except Exception as e:
    print(f"Other error: {e}")
```

### 3. Pythonic Properties

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use Pythonic properties for quick access
print(f"Neurons: {brain.neuron_count}")
print(f"Synapses: {brain.synapse_count}")
print(f"Active neurons: {brain.active_neurons}")
print(f"Firing neurons: {brain.firing_neurons}")
print(f"Development stage: {brain.development_stage}")

# Enhanced simulation
spike_count = brain.simulate(100, 0.1)
print(f"Simulation complete: {spike_count} total spikes")

# Formatted status
print(brain.get_status())
```

### 4. Enhanced Agent API

```python
import pynlm

# Setup components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable enhanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Enhanced subsystems enabled")
print(f"Reward Modulation: {agent.isRewardModulationEnabled()}")
print(f"Development: {agent.isDevelopmentEnabled()}")
print(f"Curiosity: {agent.isCuriosityEnabled()}")
```

### 5. Enhanced Examples

Run the enhanced examples:

```bash
# Run basic example
cd examples/enhanced
python3 example_basic.py

# Run agent example
python3 example_agent.py

# Run error handling example
python3 example_error_handling.py
```

## Configuration Management

### Create Custom Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set configuration parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("neuromod.dopamine.scale", 2.0)

# Validate configuration
print(f"Has neuron_count: {config.has('brain.neuron_count')}")
print(f"Available keys: {config.getKeys()}")
```

## Performance and Monitoring

### Enhanced Simulation with Monitoring

```python
import pynlm
import time

def benchmark_simulation(brain, world, agent, steps, repetitions=3):
    """Benchmark simulation performance"""
    print(f"Benchmarking {steps} steps over {repetitions} repetitions...")
    
    times = []
    for rep in range(repetitions):
        start_time = time.time()
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        duration = end_time - start_time
        times.append(duration)
    
    avg_time = sum(times) / len(times)
    print(f"Average duration: {avg_time:.3f}s")
    print(f"Performance: {steps/avg_time:.1f} steps/second")
    
    return avg_time

# Run benchmark
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

benchmark_simulation(brain, world, agent, 100)
```

## Troubleshooting

### Common Issues and Solutions

#### "ImportError: No module named 'pynlm'"

```bash
# Install the module
pip install -e .

# Or reinstall to fix any installation issues
pip install --force-reinstall -e .
```

#### "Segmentation fault" (C++ error)

```python
# Ensure proper initialization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()  # Always call initialize() before step()

# Check for errors
try:
    brain.step(0)
except Exception as e:
    print(f"Error: {e}")
    brain.reset()  # Reset and try again
```

#### "CMake cannot find Python"

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python3)
```

## Advanced Usage

### 1. Custom Configuration Templates

```python
def create_development_config():
    """Create configuration optimized for development"""
    config = pynlm.createDefaultConfig()
    
    # Development-focused settings
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", True)
    
    return config

def create_production_config():
    """Create configuration optimized for production"""
    config = pynlm.createDefaultConfig()
    
    # Production-focused settings
    config.set("brain.neuron_count", 5000)
    config.set("plasticity.structural.enable", False)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", False)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", False)
    
    return config
```

### 2. Simulation Statistics

```python
import pynlm

def get_comprehensive_stats(brain, agent=None, world=None):
    """Get comprehensive statistics from simulation"""
    stats = {
        'brain': {
            'neuron_count': brain.neuron_count,
            'synapse_count': brain.synapse_count,
            'active_neurons': brain.active_neurons,
            'firing_neurons': brain.firing_neurons,
            'average_firing_rate': brain.average_firing_rate,
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.development_stage,
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
    }
    
    if agent:
        stats['agent'] = {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'prediction_error': agent.getPredictionError(),
            'development_stage': agent.getDevelopmentalStage(),
            'reward_modulation_enabled': agent.isRewardModulationEnabled(),
            'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled(),
            'development_enabled': agent.isDevelopmentEnabled(),
            'curiosity_enabled': agent.isCuriosityEnabled()
        }
    
    if world:
        stats['world'] = {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'max_energy': world.getMaxEnergy()
        }
    
    return stats

# Usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.simulate(100, 0.1)

# Get stats
stats = get_comprehensive_stats(brain)
print("Comprehensive Statistics:")
import json
print(json.dumps(stats, indent=2))
```

## Examples Directory

The `examples/` directory contains additional examples:

- `basic/`: Basic usage examples
- `advanced/`: Advanced usage examples
- `benchmarking/`: Performance benchmarking examples
- `error_handling/`: Error handling and recovery examples

## Custom Extensions

You can extend the NLM Python API by:

### 1. Creating Custom World Implementations

```python
class CustomWorld(pynlm.SimpleWorld):
    def custom_update(self, timestep):
        """Implement custom world update logic"""
        # Your custom world logic here
        pass
    
    def get_custom_sensors(self):
        """Return custom sensor data"""
        # Your custom sensor logic here
        return []
```

### 2. Creating Custom Agent Brains

```python
class CustomAgentBrain(pynlm.AgentBrain):
    def custom_processing(self, percept):
        """Implement custom sensory processing"""
        # Your custom processing logic here
        pass
    
    def custom_decision_making(self):
        """Implement custom decision making"""
        # Your custom decision logic here
        pass
```

## Testing

Run the enhanced tests:

```bash
# Run basic tests
pytest tests/test_basic.py

# Run enhanced tests
pytest tests/test_enhanced.py

# Run performance tests
pytest tests/test_performance.py
```

## Summary

The enhanced Python bindings provide:

1. **Enhanced documentation** with comprehensive examples
2. **Improved error handling** with custom exception types
3. **Pythonic properties** for intuitive API access
4. **Convenience methods** for common operations
5. **Performance monitoring** and benchmarking tools
6. **Configuration validation** and management
7. **Comprehensive examples** for different use cases
8. **Error recovery** and resilience features
9. **Advanced statistics** and reporting
10. **Custom extension support** for advanced users

These enhancements make NLM more accessible, robust, and powerful for Python users while maintaining the core scientific functionality of the neural simulation framework.
EOF

    echo -e "${GREEN}✓ Installation guide created${NC}"
}

# Create simple build script
function create_build_script {
    echo -e "${YELLOW}Creating enhanced build script...${NC}"
    
    cat > "${PROJECT_ROOT}/build_enhanced.sh" << 'EOF'
#!/bin/bash

# Enhanced build script for NLM Python bindings

set -e

echo "Building NLM enhanced Python bindings..."

# Check if in build directory
if [ ! -f "../CMakeLists.txt" ]; then
    echo "ERROR: Not in the correct directory. Run this from the build directory."
    exit 1
fi

# Build the main project
echo "Building NLM core..."
make -j$(nproc)

# Build Python bindings
echo "Building Python bindings..."
pip3 install -e ..

echo "Build completed successfully!"
echo "You can now run: python3 -c 'import pynlm; print(pynlm.__version__)'"
EOF

    chmod +x "${PROJECT_ROOT}/build_enhanced.sh"
    echo -e "${GREEN}✓ Enhanced build script created${NC}"
}

# Main execution
function main {
    print_header
    check_dependencies
    setup_build_environment
    configure_cmake
    build_project
    build_python_bindings
    create_enhanced_examples
    create_installation_guide
    create_build_script
    
    echo -e "${GREEN}==============================================================================${NC}"
    echo -e "${GREEN}NLM Enhanced Python Bindings Build Complete!${NC}"
    echo -e "${GREEN}==============================================================================${NC}"
    echo -e "${YELLOW}Next steps:${NC}"
    echo -e "${YELLOW}  1. Install Python dependencies: pip install pybind11 scikit-build-core pytest numpy${NC}"
    echo -e "${YELLOW}  2. Run examples: cd examples/enhanced && python3 example_basic.py${NC}"
    echo -e "${YELLOW}  3. Install enhanced bindings: pip install -e .${NC}"
    echo -e "${YELLOW}  4. Check installation: python3 -c 'import pynlm; print(pynlm.__version__)'${NC}"
    echo -e "${GREEN}==============================================================================${NC}"
}

# Run main function
main
