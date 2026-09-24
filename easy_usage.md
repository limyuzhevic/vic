# NLM - Easy Usage Guide

## Quick Start

Welcome to NLM (Neural Learning Machine)! This guide will help you get started quickly with the neural simulation framework.

### What is NLM?
NLM is a brain-inspired spiking neural network simulator that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

## Installation

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- Python packages: `pybind11`, `scikit-build-core`, `pytest`, `numpy`

### Build from Source

```bash
# Clone the repository
# Build the C++ core
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install .
```

### Quick Installation with One Command

```bash
# Clone and build in one go (may take a few minutes)
cd /path/to
mkdir nlm
cd nlm
git clone https://github.com/your-repo/nlm .
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
pip install .
```

## Running Examples

### Phase 2: Basic Neural Computation

```bash
# Run the basic demo
./nlm

# Or with custom configuration
./nlm --config my_config.cfg
```

### Phase 3: World Interaction

```bash
# Run Phase 3 demo
./nlm_phase3_demo

# With number of trials
./nlm_phase3_demo 100
```

### Phase 4: Emerging Cognition

```bash
# Run Phase 4 demo
./nlm_phase4_demo

# With number of trials
./nlm_phase4_demo 50
```

### Phase 6: Integration Test

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo
```

## Python API Usage

NLM provides a Python interface for easy scripting and experimentation.

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## Configuration Files

### Default Configuration
The project includes a `configs/default.cfg` file with sensible defaults:

```
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
```

### Creating Custom Configurations

```bash
# Copy and modify the default configuration
cp configs/default.cfg my_config.cfg

# Edit my_config.cfg with your parameters
# Then use it with any demo:
./nlm --config my_config.cfg
```

## Command Line Options

### Common Options

```bash
./nlm [options]

Options:
  --config <file>      Load configuration from file
  --help              Show help message
  --version           Show version information
  --neurons <N>       Set number of neurons
  --regions <N>       Set number of regions
  --timestep <t>      Set simulation timestep
  --seed <N>          Set random seed
  --verbose           Enable verbose output
  --quiet             Suppress output
```

### Example Usage

```bash
# Run with custom configuration
./nlm --config my_config.cfg

# Override specific parameters
./nlm --neurons 2000 --regions 3 --timestep 0.0005

# Run with specific random seed for reproducibility
./nlm --seed 12345 --verbose
```

## Advanced Usage

### For Power Users

```python
# Advanced Python configuration
import pynlm

# Create configuration with custom parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 2.0)

# Create brain with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced plasticity
brain.getSTDP().enable(true, true, 0.01f)
brain.getHebbian().enable(true, true)
brain.getStructuralPlasticity().enable(true, 0.0005f)
```

### Configuration Validation

```python
# Validate configuration before use
config = pynlm.createDefaultConfig()

# Check if required keys exist
required_keys = ["brain.neuron_count", "brain.connection_probability"]
for key in required_keys:
    if not config.has(key):
        print(f"Warning: Missing required configuration key: {key}")

# Get all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Save configuration
config.saveToFile("my_config.json")
```

## Troubleshooting

### Common Issues

**Issue: Build fails**
```bash
# Solution: Install build dependencies first
pip install pybind11 scikit-build-core pytest numpy

# Or use conda for better dependency management
conda install pybind11 scikit-build-core pytest numpy
```

**Issue: Python import fails**
```bash
# Solution: Install the package in development mode
pip install -e .

# Or force reinstall
pip install --force-reinstall .
```

**Issue: Segmentation fault**
```
# Solution: Ensure proper initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Call initialize before step()
```

### Getting Help

- **Documentation**: Check the `docs/` directory for detailed documentation
- **Examples**: Look in `src/experiments/` for various Phase demo programs
- **Community**: For questions and discussions, visit the project repository
- **Bug Reports**: Report issues at the project issue tracker

## Quick Cheat Sheet

### Essential Commands

```bash
# Build and install
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
pip install .

# Run basic demo
./nlm

# Run with custom config
./nlm --config my_config.cfg

# Python quick start
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

### Python Essentials

```python
# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()}")
```

## Conclusion

NLM provides a powerful, flexible framework for neural network simulation. Start with the quick start guide, explore the examples, and gradually move to advanced features as you become comfortable with the system.

Enjoy experimenting with neural computation! 🚀