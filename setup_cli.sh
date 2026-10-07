#!/usr/bin/env bash

# Quick setup script for NLM CLI improvements
# This script helps set up the enhanced NLM command-line interface

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_color() {
    echo -e "${1}${2}${NC}"
}

# Check if we're in the right directory
if [ ! -f "pyproject.toml" ]; then
    print_color "$RED" "ERROR: pyproject.toml not found. Make sure you're in the NLM project root."
    exit 1
fi

print_color "$GREEN" "NLM CLI Setup Script"
print_color "$YELLOW" "========================"

# Check for required tools
print_color "$GREEN" "Checking for required tools..."

if ! command -v python3 &> /dev/null; then
    print_color "$RED" "ERROR: python3 not found"
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    print_color "$RED" "ERROR: cmake not found"
    exit 1
fi

if ! command -v make &> /dev/null; then
    print_color "$RED" "ERROR: make not found"
    exit 1
fi

print_color "$GREEN" "All required tools found."

# Check if Python bindings can be built
print_color "$GREEN" "Checking Python bindings configuration..."

if [ ! -f "python/bindings.cpp" ]; then
    print_color "$RED" "ERROR: python/bindings.cpp not found"
    exit 1
fi

print_color "$GREEN" "Python bindings source found."

# Create directories if they don't exist
print_color "$GREEN" "Creating necessary directories..."
mkdir -p build
mkdir -p configs
mkdir -p docs/cli

# Check for existing build
if [ -f "build/nlm" ]; then
    print_color "$YELLOW" "WARNING: NLM executable already exists. It will be rebuilt."
else
    print_color "$GREEN" "NLM executable will be built."
fi

# Check Python environment
print_color "$GREEN" "Checking Python environment..."

if python3 -c "import pybind11; import numpy; print('Python bindings and numpy available')" 2>/dev/null; then
    print_color "$GREEN" "Python environment looks good."
else
    print_color "$YELLOW" "WARNING: Some Python dependencies might be missing."
    print_color "$YELLOW" "To install dependencies, run: pip install pybind11 numpy"
fi

# Build NLM executable
print_color "$GREEN" "Building NLM executable..."

cd build

cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -eq 0 ]; then
    make -j4
    
    if [ $? -eq 0 ]; then
        print_color "$GREEN" "NLM built successfully!"
    else
        print_color "$RED" "ERROR: Failed to build NLM."
        exit 1
    fi
else
    print_color "$RED" "ERROR: Failed to configure NLM build."
    exit 1
fi

cd ..

# Create CLI scripts directory
print_color "$GREEN" "Setting up CLI utilities..."

if [ ! -f "nlm_cli.py" ]; then
    print_color "$YELLOW" "WARNING: nlm_cli.py not found. Please check the project structure."
else
    chmod +x nlm_cli.py
    print_color "$GREEN" "CLI script made executable."
fi

# Generate example configuration if it doesn't exist
if [ ! -f "configs/default.cfg" ]; then
    print_color "$YELLOW" "Generating default configuration..."
    python3 -c "
import json
config = {
    'random_seed': 42,
    'simulation_timestep': 0.001,
    'neuron_count': 1000,
    'region_count': 2,
    'connection_probability': 0.1,
    'plasticity_learning_rate': 0.01,
    'stdp_ltp_weight': 0.01,
    'stdp_ltd_weight': 0.012,
    'stdp_time_constant': 20.0,
    'development_synaptogenesis_rate': 0.001,
    'development_pruning_rate': 0.0001,
    'dopamine_baseline': 0.1,
    'reward_discount_factor': 0.99,
    'environment_name': 'GridWorld',
    'environment_width': 10,
    'environment_height': 10,
    'log_level': 'INFO',
    'log_to_file': False,
    'log_filename': 'nlm.log',
    'max_simulation_steps': 10000,
    'simulation_time_limit': 0.0,
    'visualization_enabled': False,
    'visualization_update_rate': 30.0
}

# Write configuration as INI-style
with open('configs/default.cfg', 'w') as f:
    f.write('# NLM Configuration File\\n')
    f.write('# This is the default configuration for Phase 1 skeleton\\n\\n')
    
    for key, value in config.items():
        f.write(f'{key} = {value}\\n')
    
print('Default configuration generated.')
" 2>/dev/null

    if [ $? -eq 0 ]; then
        print_color "$GREEN" "Default configuration generated successfully."
    else
        print_color "$RED" "ERROR: Failed to generate configuration."
        exit 1
    fi
else
    print_color "$GREEN" "Default configuration already exists."
fi

# Create example usage documentation
print_color "$GREEN" "Creating documentation..."

cat > docs/cli/README.md << 'EOF'
# NLM CLI - Command Line Interface

This directory contains the NLM (Neural Learning Machine) command-line interface tools.

## Overview

The NLM CLI provides enhanced command-line utilities for:
- Configuration management
- Simulation control
- Brain state management
- Analysis and monitoring
- Experiment management

## Installation

1. Ensure NLM is built:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   cd ..
   ```

2. Install Python dependencies:
   ```bash
   pip install pybind11 numpy
   ```

3. Run setup script:
   ```bash
   bash setup_cli.sh
   ```

## Usage

### Basic Commands

#### Run Simulation
```bash
nlm-cli --steps 1000 --timestep 0.001 --neurons 500 --regions 1
```

#### Configuration Management
```bash
# Generate default config
nlm-cli --generate-config

# Validate config
nlm-cli --validate-config

# Set a parameter
nlm-cli --set random_seed=42

# Set multiple parameters
nlm-cli --set neuron_count=1000 --set connection_probability=0.1
```

#### Brain State Management
```bash
# Save brain state
nlm-cli --save brain_state.json

# Load brain state
nlm-cli --load brain_state.json

# Create backup
nlm-cli --backup
```

#### Analysis and Monitoring
```bash
# Analyze brain state
nlm-cli --analyze

# Monitor simulation
nlm-cli --monitor

# Show statistics
nlm-cli --stats

# Generate plots
nlm-cli --plot
```

### Experiment Types

`nlm-cli` supports different experiment types:
- `connectivity`: Basic neural connectivity test
- `plasticity`: Plasticity and learning experiments
- `stdp`: Spike-timing dependent plasticity verification
- `all`: Run all experiment types (default)

Example:
```bash
nlm-cli --steps 500 --experiment connectivity
```

### Configuration File Format

NLM uses INI-style configuration files:

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
```

## Advanced Features

### Python Integration

The NLM Python bindings provide a high-level interface:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Error Handling

The enhanced NLM bindings provide detailed error messages:

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    config.set("invalid.parameter", "value")
except pynlm.NLMError as e:
    print(f"Configuration error: {e}")
```

## Files

- `nlm_cli.py` - Main CLI script
- `python/bindings.cpp` - Enhanced Python bindings with error handling
- `configs/default.cfg` - Default configuration file
- `docs/cli/README.md` - This documentation

## Troubleshooting

### Common Issues

1. **Python bindings not available**
   ```bash
   pip install pybind11 scikit-build-core
   pip install -e .
   ```

2. **NLM executable not found**
   Build NLM in the build directory:
   ```bash
   cd build
   make -j4
   ```

3. **Configuration file errors**
   Check syntax and required parameters in the configuration file.

4. **Permission denied**
   Ensure scripts are executable:
   ```bash
   chmod +x nlm_cli.py
   ```

## Development

### Adding New Commands

To add new CLI commands:

1. Add argument to `create_parser()` function in `nlm_cli.py`
2. Implement command logic in appropriate function
3. Update documentation

### Enhancing Error Handling

The NLM bindings provide comprehensive error handling:

- Custom `NLMError` exception class
- Detailed error messages with context
- Type safety for parameters
- Graceful failure on invalid operations

### Performance Monitoring

Monitor simulation performance:

```bash
nlm-cli --monitor --steps 1000 --verbose
```

## License

MIT License. See LICENSE file for details.

## Author

NLM Research Team
EOF

print_color "$GREEN" "Documentation created."

# Create example scripts
print_color "$GREEN" "Creating example scripts..."

cat > examples/run_basic.py << 'EOF'
"""
Example: Run basic NLM simulation
"""
import pynlm
import sys

def main():
    print("NLM Basic Simulation Example")
    print("=" * 40)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("simulation_timestep", 0.001)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("Simulation completed successfully!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
EOF

cat > examples/run_agent.py << 'EOF'
"""
Example: Run NLM agent in a world
"""
import pynlm
import sys

def main():
    print("NLM Agent Simulation Example")
    print("=" * 40)
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Agent initialized and learning enabled")
    print(f"Sensory input size: {agent.getSensoryInputSize()}")
    print(f"Motor output size: {agent.getMotorOutputSize()}")
    
    # Run agent simulation
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Agent decides action
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 10 steps
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    print("Agent simulation completed!")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
EOF

print_color "$GREEN" "Example scripts created."

print_color "$GREEN" "All setup completed successfully!"
print_color "$YELLOW" "\nNext steps:")
print_color "$YELLOW" "1. Build NLM: cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
print_color "$YELLOW" "2. Install Python dependencies: pip install pybind11 numpy"
print_color "$YELLOW" "3. Run NLM: ./build/nlm --help"
print_color "$YELLOW" "4. Run CLI: ./nlm_cli.py --help"
print_color "$YELLOW" "5. Try examples: python3 examples/run_basic.py"

# Check if we can import pynlm
try:
    import pynlm
    print_color "$GREEN" "Python bindings available."
except ImportError:
    print_color "$YELLOW" "Note: Python bindings not available. Install with: pip install -e ."
EOF
chmod +x setup_cli.sh
