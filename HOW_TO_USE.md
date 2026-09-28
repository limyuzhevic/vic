# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

#### Option 1: Using CMake (Traditional Build)

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

#### Option 2: Using scikit-build-core (Modern Python Installation) - RECOMMENDED

```bash
# Install build dependencies
pip install scikit-build-core pybind11 numpy

# Build and install
pip install .

# Or install in development mode for faster editing
pip install -e .
```

#### Option 3: Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Python-specific Build Options

```bash
# Configure with Python bindings
cmake .. -DPYTHON_EXECUTABLE=$(which python3)

# Enable Python bindings (automatic with scikit-build-core)
-DENABLE_PYTHON_BINDINGS=ON

# Set Python module name
-DPYTHON_MODULE_NAME=pynlm

# Set Python package version
-DPYTHON_PACKAGE_VERSION=0.1.0
```

### Building Python Bindings Separately

```bash
# Build Python bindings only
cd build
cmake -DBUILD_PYTHON_BINDINGS=ON ..
make -j$(nproc)

# Install Python bindings
pip install python/bindings.cpp
```

### Troubleshooting Build Issues

#### CMake cannot find Python:
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python3)
```

#### Compilation errors:
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present
- Check for missing dependencies

#### Python binding issues:
```bash
# Clean build and rebuild
rm -rf build
mkdir build
cd build
cmake .. -DBUILD_PYTHON_BINDINGS=ON ..
make -j$(nproc)

# Test installation
pip install -e .
python -c "import pynlm; print(pynlm.__version__)"
```

### Build Verification

#### Verify C++ Library
```bash
# Build and run basic test
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run example simulation
./nlm --help  # Should show help information
```

#### Verify Python Bindings
```python
import pynlm

# Check version
print(f"NLM Python Bindings Version: {pynlm.__version__}")

# Test basic functionality
config = pynlm.createDefaultConfig()
print(f"Default configuration keys: {len(config.getKeys())}")

# Create brain
brain = pynlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Test initialization
brain.initialize()
print("Brain initialized successfully")

# Test simulation
brain.step(0)
print(f"Simulation step completed, total spikes: {brain.getTotalSpikeCount()}")
```

### Development Build (for Development)

```bash
# Development build with debugging
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0 -Wall"
make -j4

# Create compile_commands.json for tooling
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Install development headers
-DINSTALL_DEVELOPMENT_HEADERS=ON
```

### Production Build (for Deployment)

```bash
# Optimized production build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native"
make -j$(nproc)

# Strip debug symbols for smaller binary
scmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-s"
make -j$(nproc)
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Navigate to project directory
cd /path/to/nlm

# Install build dependencies
pip install scikit-build-core pybind11 numpy

# Build and install Python bindings
pip install -e .

# Or install in development mode
pip install -e .
```

### Using Pre-built Packages

```bash
# Install from PyPI (if available)
pip install pynlm

# Check installation
python -c "import pynlm; print(pynlm.__version__)"
```

### Installation Verification

```python
import pynlm

# Check version
print(f"NLM Python Bindings Version: {pynlm.__version__}")

# Test basic functionality
config = pynlm.createDefaultConfig()
print(f"Default configuration keys: {len(config.getKeys())}")

# Create brain
brain = pynlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Test initialization
brain.initialize()
print("Brain initialized successfully")

# Test simulation
brain.step(0)
print(f"Simulation step completed, total spikes: {brain.getTotalSpikeCount()}")
```

### Running Tests

```bash
# Run Python tests
pytest tests/

# Or run specific test modules
pytest tests/test_brain.py
pytest tests/test_config.py
pytest tests/test_agent.py

# Run C++ tests (if built)
./nlm_test
```

### Environment Setup

#### Set up development environment:
```bash
# Create virtual environment
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install dependencies
pip install scikit-build-core pybind11 numpy
pip install -e .
```

#### Set up production environment:
```bash
# Install system dependencies
apt-get update
apt-get install -y g++ cmake git

# Install Python dependencies
pip install pynlm numpy
```

#### Set up containerized environment:
```bash
# Dockerfile example
FROM python:3.9-slim

WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application code
COPY . .

# Build C++ extensions
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j4

# Install Python package
RUN pip install -e .

# Create non-root user
RUN useradd -m -u 1000 nlmuser
USER nlmuser

# Set up volume for data
VOLUME /data

WORKDIR /data

CMD ["python", "main.py"]
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
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

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Part 6: Troubleshooting

### Build Issues

#### CMake cannot find Python:
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python3)
```

#### Compilation errors:
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present
- Check for missing dependencies

### Runtime Issues

#### ImportError: No module named 'pynlm':
```bash
pip install --force-reinstall .
```

#### Segmentation faults:
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check memory usage for large simulations

### Common Problems and Solutions

#### Problem: "My brain isn't doing anything"
**Solution**: Did you call `brain.initialize()`? Try increasing the number of steps.

#### Problem: "The agent isn't moving"
**Solution**: Did you call `world.applyMotorCommand()`? Check that `world.update()` is being called.

#### Problem: "Everything is 0"
**Solution**: Brains need time to "warm up" - try more steps. Some neurons need input to fire - make sure sensory input is connected.

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Additional Resources

### Online Resources

- **NLM Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Issues**: https://github.com/nlm-project/nlm/issues

### Community Support

- **GitHub Discussions**: Ask questions and share experiences
- **Stack Overflow**: Tag with `[nlm]` for questions
- **Discord Server**: Real-time community support (if available)

### Research Papers

- "Neural Learning Machine: A Brain-Inspired Spiking Neural Network"
- "Phase 6: Final Integration of Artificial Developmental Brain"
- "Experimental Investigation of Neuromodulation in Computational Brain Models"

## Version Information

- **NLM Version**: 0.1.0
- **Phase**: 6 (Final Integration)
- **Python Bindings**: 2.0.0
- **C++ Standard**: C++20
- **License**: MIT

## Support

### Getting Help

1. **Check Documentation**: Read `README.md`, `TUTORIAL.md`, and `HOW_TO_USE.md`
2. **Search Issues**: Check GitHub issues for similar problems
3. **Ask Questions**: Use GitHub Discussions or Stack Overflow
4. **Report Bugs**: Create issues with detailed reproduction steps

### Reporting Issues

When reporting issues, please include:

- Environment information (OS, Python version, NLM version)
- Build configuration
- Error messages and stack traces
- Minimal reproduction code
- Expected vs. actual behavior

### Contributing

- **Code Contributions**: Fork repository and submit pull requests
- **Documentation Contributions**: Improve documentation and examples
- **Bug Reports**: Report issues and help test fixes
- **Feature Requests**: Suggest new features and enhancements

## License

MIT License. See `LICENSE` file for details.

---

*This documentation is continuously updated. For the latest information, refer to the official NLM documentation.*
