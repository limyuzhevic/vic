# NLM (Neural Learning Machine) - Complete User Guide

**An Experimental Artificial Developmental Brain Simulator**

## Overview

NLM (熙然, meaning "serene flow") is a comprehensive brain-inspired neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This guide covers:
1. **Building and Installation** - C++ project setup and Python bindings
2. **Python API Usage** - Core components and advanced features
3. **Configuration Management** - Brain parameters and settings
4. **Development Process** - Creating custom simulations and experiments
5. **Troubleshooting and Best Practices** - Common issues and solutions

## Quick Start for Beginners

### 1. Install NLM

```bash
# Install from source (recommended)
pip install -e .

# Or check installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### 2. Create Your First Brain

```python
import pynlm

# Step 1: Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Step 2: Initialize the brain
brain.initialize()

# Step 3: Run simulation
for i in range(1000):
    brain.step(i)

# Step 4: View results
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Generated {brain.getTotalSpikeCount()} spikes")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 3. Create a Complete Agent Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent with world
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)  # Update world dynamics
    percept = world.getSensoryPercept()  # Get sensory input
    agent.processSensoryInput(percept)  # Process with brain
    brain.step(step)  # Brain thinking
    action = agent.decodeMotorCommand()  # Decide action
    world.applyMotorCommand(action, world.getSimulationTime())  # Take action
    \nprint("Agent simulation complete!")
```

## Part 1: Building from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: make, ninja, or similar

### Build Steps

#### Standard Release Build

```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

#### Debug Build (for development)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Installation

```bash
# Install to system or custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Build Outputs

After successful compilation, you'll have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable (Phase 2) |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase5_demo` | Executable | Phase 5 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |
| `nlm_benchmark` | Executable | Performance benchmarks |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |

### Python Installation

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install NLM Python package
pip install -e .

# Or install in development mode
pip install -e .[dev]
```

## Part 2: Python API Reference

### Core Classes

#### Config Class

The `Config` class manages all brain configuration parameters.

**Creation:**
```python
config = pynlm.createDefaultConfig()
```

**Setting Parameters:**
```python
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("random_seed", 123)
```

**Getting Parameters:**
```python
# Get value (returns None if not set)
value = config.get("brain.neuron_count")

# Get with default
value = config.getOr("brain.neuron_count", 1000)

# Check existence
if config.has("brain.neuron_count"):
    print("Config exists")
```

**File I/O:**
```python
# Load from file
config.loadFromFile("my_config.cfg")

# Save to file
config.saveToFile("config.json")

# Configuration from command line
config.loadFromArgs(["--brain.neuron_count=500", "--random_seed=42"])
```

#### Brain Class

The `Brain` class represents the central neural simulation system.

**Basic Usage:**
```python
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
```

**Context Manager (Recommended for clean resource management):**
```python
with pynlm.BrainContext(config) as brain:
    for step in range(1000):
        brain.step(step)
    print(f"Simulation complete with {brain.getTotalSpikeCount()} spikes")
```

**Key Methods:**

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize brain with configuration |
| `step(step, time=None)` | Perform simulation step |
| `reset()` | Reset brain state |
| `injectCurrent(neuron_id, current)` | Inject current into neuron |
| `injectCurrentToNeurons(type, current)` | Inject current to neuron type |
| `receiveSensoryInput(input)` | Process sensory input |
| `produceAction()` | Generate motor action |
| `save(filepath)` | Save brain state |
| `load(filepath)` | Load brain state |
| `addRegion(name)` | Add neural region |
| `getTotalNeuronCount()` | Get neuron count |
| `getTotalSynapseCount()` | Get synapse count |
| `getFiringNeuronCount()` | Get firing neurons |
| `getTotalSpikeCount()` | Get total spikes |
| `getAverageFiringRate()` | Get average firing rate |
| `getExcitationInhibitionRatio()` | Get E/I balance ratio |
| `getDevelopmentalStage()` | Get developmental stage |

**Advanced Usage:**
```python
# Batch operations for efficiency
brain.batchInjectCurrents([
    (pynlm.INVALID_NEURON_ID, 50.0),
    (pynlm.INVALID_NEURON_ID, 30.0),
])

# Quick simulation helper
pynlm.quickBrainSimulation(brain, 1000)

# Get comprehensive statistics
stats = brain.getBrainStats()
print(f"E/I ratio: {stats['e_i_ratio']:.2f}")
```

#### AgentBrain Class

The `AgentBrain` class connects brain activity to world interaction.

**Basic Setup:**
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

**Key Methods:**

| Method | Description |
|--------|-------------|
| `initialize(world)` | Initialize with world interface |
| `processSensoryInput(percept)` | Process sensory input |
| `decodeMotorCommand()` | Get motor action from brain |
| `applyRewardModulation(reward, predicted)` | Learn from rewards |
| `updateDevelopment(timestep)` | Update development |
| `getCuriosityLevel()` | Get exploration level |
| `getNoveltyLevel()` | Get novelty detection |
| `getPredictionError()` | Get prediction error |

#### SimpleWorld Class

The `SimpleWorld` class represents the simulated environment.

**Basic Usage:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

for step in range(100):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # Get vision/touch data
    action = agent.decodeMotorCommand()  # Get brain action
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute action
```

### SensoryInput Classes

**Vision (Image Input):**
```python
vision = pynlm.Vision(width=8, height=8, channels=3)
# Set image data (8x8 pixels, normalized 0-1)
data = [0.1, 0.2, 0.3] * 8 * 8
vision.setData(data)
```

**Audio (Sound Input):**
```python
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
# Set audio data
audio.setData([0.0] * 1024)  # Silence
audio.setSampleRate(44100)
```

**InternalSignals (Internal State):**
```python
internal = pynlm.InternalSignals()
internal.addSignal(0.8)  # hunger level
internal.addSignal(0.3)  # energy level
```

### Action Classes

**Action (Motor Output):**
```python
action = pynlm.Action(pynlm.ActionType.MoveForward)
action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
```

## Configuration Parameters

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Connection probability between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.initial_plasticity` | float | 1.0 | Initial plasticity rate |
| `development.critical_period_duration` | float | 100.0 | Critical period duration |
| `development.maturation_rate` | float | 0.01 | Maturation rate |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation timestep (seconds) |
| `simulation.max_steps` | int | 10000 | Maximum simulation steps |

## Part 3: Advanced Usage

### 1. Context Managers

```python
# Brain context manager - auto-initializes and resets
with pynlm.BrainContext(config) as brain:
    for step in range(1000):
        brain.step(step)

# World context manager  
with pynlm.WorldContext() as world:
    world.configure(10, 10, 8, 8)
    world.reset()
    # Automatic cleanup on exit
```

### 2. Batch Operations

```python
# Efficient batch current injection
injections = [
    (pynlm.INVALID_NEURON_ID, 50.0),
    (pynlm.INVALID_NEURON_ID, 30.0),
    (pynlm.INVALID_NEURON_ID, 20.0),
]
brain.batchInjectCurrents(injections)

# Batch configuration
config = pynlm.createBrainFromConfigFile("my_config.cfg")

# Quick simulation
pynlm.quickBrainSimulation(brain, 1000)
```

### 3. Utility Functions

```python
# Create brain with custom settings
def createOptimizedBrain(neuron_count=2000):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.002)
    return pynlm.createBrain(config)

# Comprehensive brain analysis
def analyzeBrainPerformance(brain):
    stats = brain.getBrainStats()
    return {
        "stability": stats["e_i_ratio"],
        "activity": stats["firing_neurons"] / max(stats["total_neurons"], 1),
        "learning_rate": stats["total_spikes"] / max(stats["total_steps"], 1)
    }
```

### 4. Error Handling

```python
def safeSimulation(brain, world, agent, steps):
    try:
        brain.initialize()
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    except Exception as e:
        print(f"Simulation error: {e}")
        return False
    return True
```

## Development Process

### 1. Creating Custom Experiments

#### Step-by-Step Guide

1. **Define Research Question**
```python
# Example: Study learning efficiency
NEURON_COUNT = 2000
CONNECTION_DENSITY = 0.15
MAX_STEPS = 500  # Steps before stabilization
```

2. **Create Configuration**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", NEURON_COUNT)
config.set("brain.synapse_density", CONNECTION_DENSITY)
config.set("plasticity.stdp.learning_rate", 0.002)
```

3. **Implement Experiment**
```python
def learning_efficiency_experiment():
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with learning
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Record initial state
    initial_weights = recordSynapticWeights(brain)
    
    # Run simulation
    for step in range(MAX_STEPS):
        # Environment interaction...
        # Learning...
        pass
        
    # Compare final state
    final_weights = recordSynapticWeights(brain)
    change = calculateWeightChange(initial_weights, final_weights)
    
    return change
```

### 2. Experiment Templates

#### Template 1: Memory Formation

```python
def memory_formation_experiment(num_episodes=100):
    """Test how brains form episodic memories"""
    config = pynlm.createDefaultConfig()
    config.set("memory.max_episodes", num_episodes)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Store episodes during simulation
    episodes = []
    for step in range(num_episodes):
        # Simulate...
        # Agent learns...
        
        if step % 10 == 0:  # Save episode snapshot
            episodes.append(brain.getEpisodicMemory().getCurrentEpisode())
    
    # Analyze memory patterns
    return analyzeMemoryPatterns(episodes)
```

#### Template 2: Developmental Learning

```python
def developmental_learning_experiment():
    """Study how developmental stage affects learning"""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    learning_rates = []
    
    # Simulate through developmental stages
    for stage in [pynlm.DevelopmentalStage.Initial,
                  pynlm.DevelopmentalStage.CriticalPeriod,
                  pynlm.DevelopmentalStage.Maturation,
                  pynlm.DevelopmentalStage.Adult]:
        brain.setDevelopmentalStage(stage)
        
        # Run learning test
        for step in range(100):
            # Learning occurs...
            pass
            
        # Record learning rate
        learning_rates.append(brain.getLearningRate())
    
    return learning_rates
```

## Performance Optimization

### 1. For Large-Scale Simulations

```python
import multiprocessing
import time

# Use multiple processes for parallel simulation
def parallel_simulation(config, steps):
    with multiprocessing.Pool() as pool:
        results = pool.starmap(run_simulation, 
                              [(config, i*100, (i+1)*100) for i in range(10)])
    return combine_results(results)

# Optimized configuration
optimized_config = pynlm.createDefaultConfig()
optimized_config.set("brain.neuron_count", 5000)
optimized_config.set("brain.synapse_density", 0.05)
```

### 2. Memory Management

```python
class SimulationManager:
    def __init__(self, config):
        self.config = config
        self.checkpoint_interval = 1000
        self.step_counter = 0
    
    def run(self, max_steps):
        brain = pynlm.createBrain(self.config)
        
        try:
            for step in range(max_steps):
                # Simulation step...
                
                # Periodic checkpoint
                if step % self.checkpoint_interval == 0:
                    brain.save(f"checkpoint_{step}.bin")
                    # Clear working memory
                    brain.getWorkingMemory().clear()
                    
        finally:
            brain.reset()  # Clean up
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "Brain initialization failed"
```python
# Solution 1: Check configuration
config = pynlm.createDefaultConfig()
if not config.has("brain.neuron_count"):
    print("Missing required configuration")

# Solution 2: Use default config with valid parameters
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # Should work
```

**Issue: "Agent not responding to world"
```python
# Solution 1: Check initialization order
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Agent must be initialized after world
```

**Issue: "Simulation too slow"
```python
# Solutions:
# 1. Reduce neuron count
config.set("brain.neuron_count", 500)

# 2. Use batch operations
brain.batchInjectCurrents(injections)

# 3. Use efficient context manager
with pynlm.BrainContext(config) as brain:
    # Run simulation...
```

**Issue: "Memory leak"
```python
# Solution: Use context managers
with pynlm.BrainContext(config) as brain:
    with pynlm.WorldContext() as world:
        # Everything is automatically cleaned up
```

## Best Practices

### 1. Always Use Context Managers
```python
with pynlm.BrainContext(config) as brain:
    with pynlm.WorldContext() as world:
        # Your simulation code here
        # Resources are automatically cleaned up
```

### 2. Validate Inputs
```python
def validate_simulation_parameters(config):
    # Check required parameters
    required_params = ["brain.neuron_count", "random_seed"]
    for param in required_params:
        if not config.has(param):
            raise ValueError(f"Missing required parameter: {param}")
```

### 3. Handle Errors Gracefully
```python
def safe_simulation(brain, world, agent, max_steps):
    try:
        brain.initialize()
        for step in range(max_steps):
            # Simulation code...
    except Exception as e:
        print(f"Simulation failed: {e}")
        return None
    finally:
        brain.reset()  # Always reset
```

### 4. Monitor Performance
```python
def profile_simulation(brain, world, agent, steps):
    import time
    start_time = time.time()
    
    # Run simulation...
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Calculate performance metrics
    steps_per_second = steps / duration
    neurons_per_second = brain.getTotalNeuronCount() / duration
    
    return {
        "duration_seconds": duration,
        "steps_per_second": steps_per_second,
        "performance_score": steps_per_second * brain.getTotalNeuronCount()
    }
```

## Advanced Topics

### 1. Custom Neuron Types

```python
# Extend basic neuron types
class CustomNeuronType:
    def __init__(self, name, parameters):
        self.name = name
        self.parameters = parameters

# Use in population
population = pynlm.NeuralPopulation(pynlm.PopulationId(1), 100)
population.setNeuronType(CustomNeuronType("learning_neuron", {"plasticity": 0.5}))
```

### 2. Advanced Plasticity Rules

```python
class CustomPlasticityRule:
    def __init__(self, learning_rate, time_constant):
        self.learning_rate = learning_rate
        self.time_constant = time_constant
    
    def update(self, synapse, pre_spike, post_spike, dt):
        # Custom plasticity rule implementation
        weight_change = self.learning_rate * self.calculate_dependency(pre_spike, post_spike)
        return weight_change
```

### 3. Distributed Simulation

```python
import multiprocessing
import pynlm

def run_simulation_chunk(config, start_step, end_step):
    """Run simulation for a range of steps in a separate process"""
    brain = pynlm.createBrain(config)
    
    try:
        for step in range(start_step, end_step):
            brain.step(step)
        return brain.getTotalSpikeCount()
    finally:
        brain.reset()

# Run simulation in parallel
if __name__ == "__main__":
    import multiprocessing
    config = pynlm.createDefaultConfig()
    
    with multiprocessing.Pool() as pool:
        results = pool.map(run_simulation_chunk, 
                          [(config, i*1000, (i+1)*1000) for i in range(10)])
    
    total_spikes = sum(results)
```

## API Quick Reference

### Core Functions

| Function | Description |
|----------|-------------|
| `pynlm.createDefaultConfig()` | Create default configuration |
| `pynlm.createBrain(config)` | Create brain with configuration |
| `pynlm.createSimpleWorld()` | Create simple world |
| `pynlm.createAgentBrain(brain)` | Create agent brain interface |

### Context Managers

```python
with pynlm.BrainContext(config) as brain:
    # Your code

with pynlm.WorldContext() as world:
    # Your code
```

### Utility Functions

```python
pynlm.quickBrainSimulation(brain, steps)  # Quick simulation
pynlm.createBrainWithDefaultConfig()  # Easy brain creation
brain.batchInjectCurrents(injections)  # Batch operations
brain.getBrainStats()  # Comprehensive statistics
```

## License and Support

- **License**: MIT
- **Documentation**: Updated 2026-10-11
- **Support**: For issues, visit the NLM project repository

**For more advanced features and customization, refer to the C++ API documentation and experiment examples in the `src/experiments/` directory.**