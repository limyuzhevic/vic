# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM (熙然, meaning "serene flow") is a **research-grade spiking neural network simulator** written in C++20. It implements **Leaky Integrate-and-Fire (LIF)** neurons with **event-driven spike propagation**, **synaptic delays**, and **multiple plasticity mechanisms** including:

- **STDP** (Spike-Timing-Dependent Plasticity)
- **Hebbian learning** 
- **Structural plasticity**
- **Reward-modulated learning**

Based on the project structure (Phase 6 complete), NLM simulates an **artificial developmental brain** that learns from experience through interaction with environments.

## Building from Source (C++)

### Prerequisites

```bash
# System dependencies
build-essential g++ cmake python3-dev

# Python bindings dependencies
pip install pybind11 scikit-build-core pytest numpy
```

### Standard Build Process

```bash
# Navigate to project directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets with parallel jobs
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Purpose |
|--------|------|---------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |
| `nlm_test` | Executable | Unit test suite |

### Advanced Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Specific compiler choice
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
make

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with performance optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_MODE=advanced
make -j$(nproc)

# Build with verbose logging
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_LOGGING=ON
make
```

## Python Library Installation

### From Source (Recommended for Development)

```bash
# Install build dependencies first
pip install scikit-build-core pybind11 pytest numpy

# Build and install NLM Python bindings
pip install .

# Or install in development mode (editable)
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### Using Modern Python Packaging

```bash
# Install scikit-build-core (required for pybind11)
pip install scikit-build-core pybind11

# Build distribution package
pip install build
python -m build

# Install from wheel (recommended for production)
pip install dist/*.whl
```

### Installation Verification

```python
import pynlm

# Check version
print(f"NLM Python bindings version: {pynlm.__version__}")

# Create basic test brain
config = pynlm.createDefaultConfig()
print(f"Default neuron count: {config.get('brain.neuron_count')}")

brain = pynlm.createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

## Python API Usage

### Basic Python Interface

```python
import pynlm

# 1. Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Run neural simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Advanced Python Examples

#### Complete Agent with Learning

```python
import pynlm

def run_advanced_agent_simulation(config_file=None, steps=2000):
    """Run a complete agent simulation with all features enabled."""
    
    # 1. Load or create configuration
    if config_file:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
    else:
        config = create_optimized_config()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world environment
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8,
        maxEnergy=100.0
    )
    world.reset()
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    agent.enableRewardModulation(True)      # Reward-prediction learning
    agent.enableStructuralPlasticity(True)  # Synapse growth/pruning
    agent.enableDevelopment(True)           # Brain maturation
    agent.enableCuriosity(True)             # Novelty-driven exploration
    
    # 7. Advanced simulation loop with metrics collection
    metrics = []
    for step in range(steps):
        # Update world and get sensory input
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Run brain neural computation
        brain.step(step)
        
        # Decode motor commands
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply actions to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply learning signals
        reward = extract_reward_from_world(world)
        prediction = agent.getPredictedReward()
        agent.applyRewardModulation(reward, prediction)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        if step % 50 == 0:
            metrics.append({
                'step': step,
                'firing': brain.getFiringNeuronCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dev_stage': brain.getDevelopmentalStage(),
                'prediction_error': agent.getPredictionError()
            })
    
    return metrics

def extract_reward_from_world(world):
    """Extract reward signal from world state."""
    percept = world.getSensoryPercept()
    if percept and percept.getInternal():
        return percept.getInternal()[0]
    return 0.0
```

#### Configuration Management

```python
import pynlm
import json

def save_config_to_file(config, filename):
    """Save configuration to JSON file."""
    config_dict = {}
    keys = config.getKeys()
    for key in keys:
        config_dict[key] = config.get(key)
    
    with open(filename, 'w') as f:
        json.dump(config_dict, f, indent=2)
    print(f"Configuration saved to {filename}")

def load_config_from_file(filename):
    """Load configuration from JSON file."""
    config = pynlm.createDefaultConfig()
    
    with open(filename, 'r') as f:
        config_dict = json.load(f)
    
    for key, value in config_dict.items():
        config.set(key, value)
    
    print(f"Configuration loaded from {filename}")
    return config
```

#### Brain State Management

```python
import pynlm

def create_brain_checkpoint(brain, filename):
    """Save brain state to checkpoint file."""
    brain.save(filename)
    print(f"Brain state saved to {filename}")

def load_brain_from_checkpoint(filename):
    """Load brain state from checkpoint file."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.load(filename)
    print(f"Brain state loaded from {filename}")
    return brain
```

## Key Classes and Methods

### Brain Class

```python
# Creation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Core simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(data)   # Inject sensory data
brain.produceAction()             # Get motor action

# State persistence
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")      # Load state

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing neurons
brain.getAverageFiringRate()      # Average firing rate (Hz)
brain.getExcitationInhibitionRatio()  # E/I balance

# Development and plasticity
brain.getDevelopmentalStage()    # Current development stage
brain.getPlasticityRate()         # Current learning rate
brain.getSynapticStrength()      # Synapse weight statistics
```

### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory and motor processing
agent.processSensoryInput(percept)  # Convert world percepts to neural input
agent.decodeMotorCommand()         # Convert neural activity to actions

# Neuromodulation and learning
agent.applyRewardModulation(reward, predicted)  # Reward-prediction learning
agent.updateDevelopment(time)       # Progress brain maturation

# Subsystem control
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()           # Novelty-seeking motivation
agent.getNoveltyLevel()             # Surprise detection
agent.getPredictionError()          # Prediction accuracy
agent.getNeuromodulationLevel()     # Overall neuromodulation strength
```

### SimpleWorld Class

```python
# Creation and configuration
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control and simulation
world.reset()                       # Reset to initial state
world.update(timestep)              # Advance simulation time
world.applyMotorCommand(action, time)  # Execute actions

# State access
percept = world.getSensoryPercept()  # Get agent's observations
body = world.getAgentBody()         # Get agent physical state

# World object management
world.addObject(object)             # Add items to world
world.removeObject(x, y)           # Remove items
world.isValidPosition(x, y)         # Check position validity

# Properties and settings
world.getWidth(), world.getHeight()  # World dimensions
world.getSimulationTime()           # Current simulation time
world.getMaxEnergy()                # Maximum energy capacity
world.setMaxEnergy(value)           # Set energy capacity
```

### Configuration Class

```python
# Create and modify configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.01)

# Query configuration
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Get all keys
keys = config.getKeys()

# File operations
config.saveToFile("config.json")   # Save to JSON
config.loadFromFile("config.json")  # Load from JSON
```

## Enumeration Types

### Neuron Types

```python
import pynlm

# Neuron type classifications
pynlm.NeuronType.Excitatory    # Standard excitatory neuron
pynlm.NeuronType.Inhibitory    # Standard inhibitory neuron
pynlm.NeuronType.Sensory       # Sensory input neurons
pynlm.NeuronType.Motor         # Motor output neurons
pynlm.NeuronType.Modulatory    # Neuromodulatory neurons
pynlm.NeuronType.Internal      # Internal processing neurons
```

### Developmental Stages

```python
import pynlm

# Brain development stages
pynlm.DevelopmentalStage.Initial      # Primitive neural system
pynlm.DevelopmentalStage.CriticalPeriod  # Rapid plasticity
pynlm.DevelopmentalStage.Maturation    # Structure refinement
pynlm.DevelopmentalStage.Adult        # Mature performance
pynlm.DevelopmentalStage.Aging        # Age-related decline
```

### Action Types

```python
import pynlm

# Motor actions
pynlm.ActionType.MoveForward    # Move forward
pynlm.ActionType.TurnLeft       # Turn left
pynlm.ActionType.TurnRight      # Turn right
pynlm.ActionType.Interact       # Interact with objects
pynlm.ActionType.Eat           # Consume resources
pynlm.ActionType.Sleep         # Rest and recover
pynlm.ActionType.Search        # Explore environment
```

### World Object Types

```python
import pynlm

# Environment objects
pynlm.WorldObjectType.Empty      # Empty space
pynlm.WorldObjectType.Resource    # Consumable resource
pynlm.WorldObjectType.Hazard     # Harmful object
pynlm.WorldObjectType.Wall       # Impassable barrier
pynlm.WorldObjectType.Marker     # Reference point
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Connection probability |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable developmental progression |
| `development.initial_stage` | string | "Initial" | Starting developmental stage |
| `development.maturation_rate` | float | 0.1 | Rate of maturation process |
| `development.pruning_threshold` | float | 0.1 | Threshold for synapse pruning |

## Example Scripts

### Minimal Working Example

```python
import pynlm

# Complete minimal example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction Example

```python
import pynlm

# Setup agent-environment system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Run interaction loop
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

print("Agent-environment interaction complete!")
```

### Advanced Configuration Example

```python
import pynlm

def create_research_brain(neuron_count=5000, enable_all_features=True):
    """Create a brain configured for research experiments."""
    
    config = pynlm.createDefaultConfig()
    
    # Core configuration
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.05)
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.growth_rate", 0.001)
    config.set("plasticity.structural.pruning_rate", 0.0005)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.5)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.3)
    
    # Development
    config.set("development.enable", True)
    config.set("development.initial_stage", "Initial")
    config.set("development.maturation_rate", 0.05)
    config.set("development.pruning_threshold", 0.1)
    config.set("development.synaptogenesis_rate", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if enable_all_features:
        # Note: Features enabled at agent level, not brain level
        pass
    
    return brain
```

## C++ Executable Usage

### Running Phase 6 Demo

```bash
# Navigate to build directory
cd build

# Run Phase 6 integration demo
./nlm_phase6_demo

# Phase 6 demo runs comprehensive integration tests:
# - Memory systems integration
# - Neuromodulation coordination
# - Prediction system integration
# - Development stage progression
# - Complete brain loop validation
```

### Running Tests

```bash
# Run unit tests
./nlm_test

# Or use CTest
ctest --output-on-failure

# Test categories:
# - neuron_tests.cpp: LIF neuron dynamics
# - synapse_tests.cpp: Synaptic connections
# - stdp_tests.cpp: Spike-timing dependent plasticity
# - config_tests.cpp: Configuration management
# - main_tests.cpp: Integration tests
```

### Running Main Simulation

```bash
# Navigate to build directory
cd build

# Run main simulation
./nlm

# Command line options:
# --neurons N      Number of neurons (default: 1000)
# --steps N        Simulation steps (default: 1000)
# --world-size X   World width/height (default: 10)
# --help           Show help message
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration values are valid

**Performance issues:**
- Use Release build for production (`-DCMAKE_BUILD_TYPE=Release`)
- Monitor memory usage for large brain configurations
- Consider using SIMD optimizations if available

### Python-Specific Issues

**Module not found in virtual environment:**
```bash
pip install -e .
# or
python setup.py install
```

**Python bindings compilation errors:**
```bash
# Clean and rebuild
rm -rf build dist
pip install --force-reinstall -e .
```

**Incompatible numpy version:**
```bash
pip install numpy==1.24.0  # Adjust version as needed
```

## Performance Tips

### Configuration Optimization

```python
import pynlm

# High-performance configuration
def create_high_performance_config():
    config = pynlm.createDefaultConfig()
    
    # Optimize neuron count for available memory
    config.set("brain.neuron_count", 2000)  # Adjust based on RAM
    
    # Enable performance optimizations
    config.set("brain.use_neural_pools", True)
    config.set("brain.use_event_queues", True)
    config.set("brain.enable_simd", True)
    
    # Adjust plasticity rates for stability
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    return config
```

### Simulation Optimization

```python
# Optimized simulation loop
def optimized_simulation(b brain, world=None, agent=None, steps=5000):
    """Optimized simulation with performance monitoring."""
    
    start_time = time.time()
    spike_count = 0
    
    for step in range(steps):
        if world and agent:
            # Batch world updates where possible
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        
        # Core neural computation
        brain.step(step)
        spike_count += brain.getTotalSpikeCount()
        
        if agent:
            action = agent.decodeMotorCommand()
            if world:
                world.applyMotorCommand(action, world.getSimulationTime())
    
    elapsed = time.time() - start_time
    print(f"Simulation complete: {steps} steps in {elapsed:.2f}s")
    print(f"Performance: {steps/elapsed:.0f} steps/sec")
    print(f"Average spikes per step: {spike_count/steps:.1f}")
    
    return elapsed
```

### Memory Management

```python
# Brain checkpoint management for long simulations
def managed_simulation(brain, agent, world, total_steps=10000):
    """Simulation with periodic checkpointing."""
    
    checkpoint_interval = 1000
    last_checkpoint = 0
    
    for step in range(total_steps):
        # Run simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Periodic checkpointing
        if step - last_checkpoint >= checkpoint_interval:
            checkpoint_file = f"checkpoint_step_{step}.bin"
            brain.save(checkpoint_file)
            print(f"Checkpoint saved: {checkpoint_file}")
            last_checkpoint = step
    
    # Final checkpoint
    brain.save("final_checkpoint.bin")
    print("Final checkpoint saved")
```

## Best Practices

### Python Development Practices

```python
# 1. Use context managers for resources
with open("config.json", "r") as f:
    config_data = json.load(f)

# 2. Validate configuration before use
config = validate_config(create_default_config())

# 3. Use logging for debugging
import logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# 4. Implement error handling
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    logger.error(f"Failed to initialize brain: {e}")
    raise

# 5. Monitor performance
import time
start = time.time()
for step in range(1000):
    brain.step(step)
elapsed = time.time() - start
logger.info(f"Simulation completed in {elapsed:.2f}s")
```

### C++ Development Practices

```cpp
// 1. Use const where possible for parameters
void simulateBrain(const BrainConfig& config, size_t steps) {
    // Implementation
}

// 2. Use RAII for resource management
class NLMSimulation {
public:
    NLMSimulation(const std::string& config_file) {
        // Load configuration
    }
    
    ~NLMSimulation() {
        // Cleanup
    }
    
    void run() {
        // Run simulation
    }
};

// 3. Implement proper error handling
try {
    auto brain = createBrain(config);
    brain->initialize();
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}

// 4. Use proper logging
#include "logger.h"
Logger::getInstance().setLevel(LogLevel::INFO);
```

## Further Reading

### Documentation References

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture and component interfaces
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and biological inspiration
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and protocols
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit and validation
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization guidelines
- [BIOLOGICAL_FIDELITY.md](docs/BIOLOGICAL_FIDELITY.md) - Biological accuracy assessment
- [RESEARCH_METHODS.md](docs/RESEARCH_METHODS.md) - Experimental methodology

### Key Resources

- **Phase 6 Demo**: Run `./nlm_phase6_demo` for complete integration demonstration
- **Unit Tests**: Run `./nlm_test` for comprehensive test coverage
- **GitHub Repository**: Source code and development documentation
- **Python Package**: `pip install pynlm` for immediate usage

### Research Publications

Check the `docs/` directory for:
- Experimental results and analysis
- Performance benchmarks
- Scientific validation studies
- Limitation assessments

## Summary

This documentation provides comprehensive guidance for:

1. **Building and installing** NLM from source (C++ and Python)
2. **Using the Python API** for simulation and experimentation
3. **Configuring the brain** for different research scenarios
4. **Troubleshooting common issues** during development
5. **Optimizing performance** for large-scale simulations
6. **Following best practices** for maintainable code

NLM (熙然) is a **complete, research-grade artificial developmental brain** that:

- **Implements biologically-inspired** spiking neural networks
- **Supports multiple plasticity mechanisms** (STDP, Hebbian, structural)
- **Features integrated memory systems** (working, episodic, associative)
- **Includes neuromodulation** for motivation and learning control
- **Supports developmental progression** from primitive to adult stages
- **Provides comprehensive prediction and planning capabilities**
- **Operates as a closed-loop system** connecting perception to action

The system is designed for **scientific research** into artificial cognitive systems and the emergence of intelligent behavior from neural dynamics and development.
