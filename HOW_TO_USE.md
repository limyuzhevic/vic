# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**Phase 6: Final Integration - All Systems Connected**

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API
4. Running comprehensive learning experiments
5. Configuration and customization

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

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

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable (Phase 2: Basic Neural Computation) |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration (World interaction) |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration (Cognition and Memory) |
| `nlm_phase5_demo` | Executable | Phase 5 demonstration (Performance and Optimization) |
| `nlm_phase6_demo` | Executable | Phase 6 integration test (All systems connected) |
| `nlm_phase6_text_demo` | Executable | Phase 6 text/thought demonstration |
| `nlm_benchmark` | Executable | Performance benchmark |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build with Python bindings
 cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

> **Phase 6 Update**: `pip install .` now genuinely builds `pynlm` (verified:
> fresh venv, `pip install .`, then `import pynlm` and the easy_usage
> Example 1 flow run). Previously the root CMakeLists never built the
> module, so `pip install .` succeeded yet `import pynlm` failed. A
> plain C++ `cmake` build still does NOT include pynlm unless you pass
> `-DNLM_BUILD_PYTHON_BINDINGS=ON` (and have pybind11 installed); the
> standalone `cmake -S python -B build` route also still works.

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")

# Run a quick test
brain.initialize()
print("Brain initialized successfully!")
```

---

## Part 3: Using the NLM Python API

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

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

### Complete Agent Example (Phase 6)

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration with Phase 6 settings
    config = pynlm.createDefaultConfig()
    
    # Enable all subsystems for Phase 6 integration
    config.set("neuromodulation.enabled", True)
    config.set("memory.episodic.enabled", True)
    config.set("prediction.enabled", True)
    config.set("cognition.enabled", True)
    config.set("development.enabled", True)
    
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
    
    # 6. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enablePrediction(True)
    
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
            print(f"  Preference: {brain.getLearnedPreference():.3f}")

# Run the simulation
run_agent_simulation(1000)
```

### Learning Experiment Utilities

For advanced users interested in neural computation experiments:

```python
import pynlm

# Use the learning experiment utilities
from nlm.experiments import LearningExperimentUtils

# Create brain and run comprehensive learning experiments
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run basic connectivity test
LearningExperimentUtils.runBasicConnectivityTest(brain)

# Reset and run plasticity experiment
brain.reset()
brain.initialize()
LearningExperimentUtils.runPlasticityExperiment(brain)

# Reset and run STDP verification
brain.reset()
brain.initialize()
LearningExperimentUtils.runStdpVerification(brain)

print("All learning experiments completed successfully!")
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

### Phase 6 Integration Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `replay_interval` | int | 100 | Episode replay interval (steps) |
| `consolidation_interval` | int | 1000 | Memory consolidation interval (steps) |
| `memory_replay_enabled` | bool | true | Enable episodic memory replay |
| `neuromodulation_enabled` | bool | true | Enable neuromodulation systems |
| `prediction_enabled` | bool | true | Enable prediction systems |
| `cognition_enabled` | bool | true | Enable cognitive systems |
| `development_enabled` | bool | true | Enable developmental system |

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
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level |
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

## Part 6: Running NLM Demonstrations

### Command Line Examples

```bash
# Phase 2: Basic neural computation (learning experiments)
./nlm

# Phase 3: World interaction
./nlm_phase3_demo

# Phase 4: Cognition and memory
./nlm_phase4_demo

# Phase 5: Performance optimization
./nlm_phase5_demo

# Phase 6: Complete integration test
./nlm_phase6_demo

# Phase 6: Text and thought processing
./nlm_phase6_text_demo

# Performance benchmark
./nlm_benchmark
```

### Python Examples

```python
import subprocess
import sys

# Run Phase 6 integration demo
print("Running Phase 6 integration demo...")
result = subprocess.run(["nlm_phase6_demo"], capture_output=True, text=True)
if result.returncode == 0:
    print("Phase 6 demo completed successfully!")
    print(result.stdout)
else:
    print("Error running Phase 6 demo:")
    print(result.stderr)
```

---

## Part 7: Troubleshooting

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
- Verify configuration parameters are valid

---

## Part 8: Advanced Usage

### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Customize parameters for specific experiments
config.set("brain.neuron_count", 500)  # Smaller brain for faster testing
config.set("brain.synapse_density", 0.05)
config.set("simulation_timestep", 0.001)
config.set("max_simulation_steps", 5000)

# Enable/disable specific subsystems
config.set("neuromodulation.enabled", True)
config.set("memory.episodic.enabled", True)
config.set("prediction.enabled", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Memory Management

```python
import pynlm

# Configure memory systems for different experimental needs
config = pynlm.createDefaultConfig()

# For memory-intensive experiments
config.set("memory.working_memory.capacity", 1000)
config.set("memory.episodic.max_episodes", 10000)

# For long-term experiments (checkpointing)
config.set("performance.checkpoint.enabled", True)
config.set("performance.checkpoint.interval", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Save checkpoint at regular intervals
if brain.save("checkpoint_001.bin"):
    print("Checkpoint saved successfully!")
```

### Performance Optimization

```python
import pynlm

# Configure for maximum performance
config = pynlm.createDefaultConfig()

# Use larger brain for better performance characteristics
config.set("brain.neuron_count", 5000)
config.set("region_count", 2)

# Optimize for speed
config.set("plasticity.stdp.enable", False)  # Disable for speed
config.set("neuromodulation.enabled", False)  # Disable for speed

brain = pynlm.createBrain(config)
brain.initialize()

print("High-performance brain configuration loaded!")
```

---

## Further Reading

- [README.md](README.md) - Project overview and status
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit documentation
- [docs/PHASE6_FINAL_RESULTS.md](docs/PHASE6_FINAL_RESULTS.md) - Phase 6 final results

---

**Documentation Generated:** 2026-11-02
**Version:** Phase 6 - Complete Integration
**Status:** Ready for research and educational use