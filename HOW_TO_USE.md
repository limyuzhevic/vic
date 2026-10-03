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
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

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

### Complete Agent Example with Advanced Features

```python
import pynlm

def run_advanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 4. Create agent brain interface and initialize with world
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 5. Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation loop with detailed logging
    for step in range(num_steps):
        # Update world simulation
        world.update(timestep=0.1)
        
        # Get current sensory percept from world
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent to brain
        agent.processSensoryInput(percept)
        
        # Run brain step (LIF dynamics, plasticity, memory updates)
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world environment
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward from world internal state
        reward = 0.0
        if percept.getInternal() and not percept.getInternal().empty():
            reward = percept.getInternal()[0]
        
        # Apply reward modulation (dopamine, learning)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update developmental stage based on simulation time
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Action: {motor_cmd}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            if brain.getWorkingMemory():
                print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
            if brain.getEpisodicMemory():
                print(f"  Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    return step

# Run the simulation
run_advanced_agent_simulation(1000)
```

### Agent Configuration Management

```python
import pynlm
import json

# Create configuration
config = pynlm.createDefaultConfig()

# Set advanced parameters
config.set("brain.neuron_count", 3000)                    # Larger brain
config.set("brain.synapse_density", 0.2)                   # More connectivity
config.set("plasticity.stdp.learning_rate", 0.002)        # Faster STDP
config.set("plasticity.hebbian.learning_rate", 0.001)     # Hebbian learning
config.set("neuromod.dopamine.scale", 2.0)                # Stronger reward
config.set("neuromod.curiosity.enable", True)              # Enable curiosity
config.set("neuromod.novelty.enable", True)               # Enable novelty detection
config.set("development.enable", True)                     # Enable development

# Save configuration to file
config.saveToFile("advanced_config.json")
print("Configuration saved to advanced_config.json")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("advanced_config.json")
print(f"Loaded configuration: {config2.get('brain.neuron_count')} neurons")

# List all configuration keys
print("All configuration keys:")
for key in config2.getKeys():
    print(f"  {key}: {config2.get(key)}")
```

### Brain State Management

```python
import pynlm
import tempfile
import os

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing rate: {brain.getAverageFiringRate():.3f}")

# Run some steps
for step in range(50):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Save checkpoint (using temporary file)
with tempfile.NamedTemporaryFile(suffix='_checkpoint.bin', delete=False) as tmp:
    checkpoint_path = tmp.name

if brain.save(checkpoint_path):
    print(f"✓ Checkpoint saved: {checkpoint_path}")
    print(f"  File size: {os.path.getsize(checkpoint_path)} bytes")
    
    # Create new brain and load checkpoint
    new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    new_brain.initialize()
    
    if new_brain.load(checkpoint_path):
        print("✓ Checkpoint loaded successfully")
        print(f"  Loaded brain has {new_brain.getTotalSpikeCount()} spikes")
        
        # Continue simulation
        for step in range(50, 100):
            new_brain.step(step)
            if step % 10 == 0:
                print(f"Step {step} after load: {new_brain.getTotalSpikeCount()} total spikes")
    else:
        print("✗ Failed to load checkpoint")
    
    # Clean up temporary file
    os.unlink(checkpoint_path)
else:
    print("✗ Failed to save checkpoint")
```

### World Management and Environment Setup

```python
import pynlm

# Create world with custom parameters
world = pynlm.createSimpleWorld()

# Configure world dimensions and agent sensing
world.configure(
    width=50,                    # World size (50x50 cells)
    height=50,
    visionWidth=10,              # Vision range (10x10 cells)
    visionHeight=10,
    maxEnergy=1000.0,             # Maximum agent energy
    timestep=0.1                # Simulation timestep (100ms)
)

# Set random seed for reproducible experiments
world.setRandomSeed(42)

# Create agent and brain
agent = pynlm.createAgentBrain()
brain = agent.getBrain()

# Initialize brain
brain.initialize()

# Initialize agent with world
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)   # Learn from rewards
agent.enableCuriosity(True)          # Explore novel areas
agent.enableDevelopment(True)        # Brain develops over time

# Set initial agent position
world.setAgentStart(25.0, 25.0)     # Center of world

print(f"World created: {world.getWidth()}x{world.getHeight()} cells")
print(f"Agent starts at: ({world.getAgentStartX()}, {world.getAgentStartY()})")
print(f"Vision range: {world.getVisionWidth()}x{world.getVisionHeight()} cells")

# Run exploration simulation
for step in range(200):
    # Update world dynamics
    world.update(0.1)
    
    # Get what agent sees
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Decide action
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward-based learning
    reward = percept.getInternal()[0] if percept.getInternal() and not percept.getInternal().empty() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Agent development
    if agent.isDevelopmentEnabled():
        agent.updateDevelopment(0.1)
    
    # Print exploration progress
    if step % 50 == 0:
        print(f"\nExploration Step {step}:")
        print(f"  Agent position: ({world.getAgentBody().getX():.1f}, {world.getAgentBody().getY():.1f})")
        print(f"  Action: {action}")
        print(f"  Reward: {reward:.2f}")
        print(f"  Cur: {agent.getCuriosityLevel():.3f} | Nov: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev stage: {brain.getDevelopmentalStage()}")

print("\nExploration complete!")
```

### Performance Monitoring and Metrics

```python
import pynlm
import time

# Create performance monitoring wrapper
class BrainPerformanceMonitor:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.step_times = []
        self.start_time = time.time()
        self.max_steps = 1000
        
    def run_monitored_simulation(self):
        print("Starting performance-monitored simulation...")
        
        for step in range(self.max_steps):
            step_start = time.time()
            
            # Simulation step
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learning and development
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            if self.agent.isDevelopmentEnabled():
                self.agent.updateDevelopment(0.1)
            
            step_time = time.time() - step_start
            self.step_times.append(step_time)
            
            # Print metrics every 200 steps
            if step % 200 == 0:
                self.print_metrics(step)
        
        self.print_final_metrics()
    
    def print_metrics(self, step):
        avg_step_time = sum(self.step_times) / len(self.step_times)
        max_step_time = max(self.step_times)
        
        print(f"\n--- Performance Metrics (Step {step}) ---")
        print(f"Step time: {step:.3f}s (avg: {avg_step_time:.4f}s, max: {max_step_time:.4f}s)")
        print(f"Brain stats:")
        print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"  E/I ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Developmental stage: {self.brain.getDevelopmentalStage()}")
        
        if self.brain.getWorkingMemory():
            print(f"  Working memory: {self.brain.getWorkingMemory().getActiveTraces()} traces")
        if self.brain.getEpisodicMemory():
            print(f"  Episodic memory: {self.brain.getEpisodicMemory().getEpisodeCount()} episodes")
        
        print(f"Agent stats:")
        print(f"  Curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
    
    def print_final_metrics(self):
        total_time = time.time() - self.start_time
        avg_step_time = sum(self.step_times) / len(self.step_times)
        max_step_time = max(self.step_times)
        
        print(f"\n=== FINAL PERFORMANCE METRICS ===")
        print(f"Total simulation time: {total_time:.2f}s")
        print(f"Average step time: {avg_step_time:.4f}s")
        print(f"Max step time: {max_step_time:.4f}s")
        print(f"Steps per second: {self.max_steps / total_time:.1f}")
        print(f"Final brain state:")
        print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"  Developmental stage: {self.brain.getDevelopmentalStage()}")

# Create and run monitored simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run with performance monitoring
monitor = BrainPerformanceMonitor(brain, agent, world)
monitor.run_monitored_simulation()
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

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
