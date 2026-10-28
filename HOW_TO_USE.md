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

### Python Binding Enhancements

Since Phase 6, the NLM project has added comprehensive Python bindings with support for all Phase 6 systems. Here are the Python-specific features:

#### New Python API Features

**1. Convenience Constructors for Different Brain Types**
```python
import pynlm

# Research brain - optimized for studying learning dynamics
brain = pynlm.createResearchBrain(neuron_count=5000, learning_rate=0.001)
brain.initialize()

# Development brain - with developmental plasticity
brain = pynlm.createDevelopmentBrain(1000, enable_development=True)
brain.initialize()

# Performance brain - optimized for speed testing
brain = pynlm.createPerformanceBrain(2000, optimized_for_speed=True)
brain.initialize()

# Simple brain - basic configuration for quick testing
brain = pynlm.createSimpleBrain(100)
brain.initialize()
```

**2. Configuration Presets**
```python
# Research configuration
research_config = pynlm.createResearchConfig()

# Development configuration  
dev_config = pynlm.createDevelopmentConfig()

# Performance configuration
perf_config = pynlm.createPerformanceConfig()

# Simple configuration
simple_config = pynlm.createSimpleConfig()
```

**3. Batch Operations**
```python
# Process multiple sensory inputs at once
percepts = [world.getSensoryPercept() for _ in range(10)]
pynlm.receiveSensoryInputs(brain, percepts)

# Process batch with timing
percepts = [world.getSensoryPercept() for _ in range(5)]
timestamps = [t * 0.001 for t in range(5)]
pynlm.receiveSensoryInputs(brain, percepts, timestamps)
```

**4. Performance Monitoring**
```python
# Detailed benchmarking
result = pynlm.benchmark(brain, num_steps=1000, detailed=True)
print(f"Steps completed: {result.stepsCompleted}")
print(f"Average firing rate: {result.averageFiringRate:.2f} Hz")
print(f"Total time: {result.totalTime:.3f}s")

# Get memory usage
usage = pynlm.getMemoryUsage(brain)
print(f"Memory utilization: {usage.memoryUtilization:.2%}")
print(f"Working memory capacity: {usage.workingMemoryCapacity} traces")
print(f"Episodic memory episodes: {usage.episodicMemoryCount}")
```

**5. Phase 6 System Access**
```python
# Memory systems access
working_memory = brain.getBrainWorkingMemory()
working_memory.store([0.1, 0.2, 0.3, 0.4])
memory_activity = working_memory.getMemoryActivity()

# Prediction system access
prediction = brain.getBrainPredictionSystem()
if prediction:
    error = prediction.getPredictionError()
    confidence = prediction.getPredictionConfidence()

# Cognition systems access
planner = brain.getBrainPlanner()
if planner:
    action = planner.planAction([0.1, 0.2, 0.3], 0.5)

attention = brain.getBrainAttentionalSelection()
if attention:
    winners = attention.processCompetition([1, 2, 3, 4, 5])

# Neuromodulation systems access
curiosity = brain.getBrainCuriosity()
if curiosity:
    level = curiosity.getCuriosityLevel()

novelty = brain.getBrainNovelty()
if novelty:
    test_input = pynlm.Vision(8, 8)
    novelty_level = novelty.computeNovelty(test_input)
```

**6. Simulation Utilities**
```python
# Run simulation for fixed number of steps
pynlm.runForSteps(brain, num_steps=100, log_progress=True)

# Run until condition is met
def stop_condition(brain):
    return brain.getTotalSpikeCount() > 1000

pynlm.runUntil(brain, stop_condition, max_steps=10000, log_progress=True)

# Save checkpoint with backup
pynlm.saveCheckpoint(brain, "brain_state.json", create_backup=True)
```

**7. Validation and Error Checking**
```python
# Validate sensory input before processing
percept = world.getSensoryPercept()
if pynlm.validateSensoryInput(brain, percept):
    agent.processSensoryInput(percept)

# Validate configuration
is_valid, errors = pynlm.validateConfig(config)
if not is_valid:
    print("Configuration errors:", errors)
```

**8. Random Number Generation**
```python
import pynlm

# Create random generator with specific seed
rng = pynlm.RandomGenerator(42)
print(f"Seed: {rng.getSeed()}")

# Generate random values
float_val = rng.randomFloat()
int_val = rng.randomInt(1, 10)
normal_val = rng.randomNormal(0.0, 1.0)
bool_val = rng.randomBool(0.7)
```

### Complete Python Example - Agent Simulation with Phase 6 Systems

This example demonstrates using the enhanced Python API with all Phase 6 systems:

```python
import pynlm

def run_complete_agent_simulation():
    # 1. Create configuration and brain
    config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable Phase 6 systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 4. Get system references for monitoring
    working_memory = brain.getBrainWorkingMemory()
    planner = brain.getBrainPlanner()
    curiosity = brain.getBrainCuriosity()
    
    # 5. Main simulation loop
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Store experience in working memory
        working_memory.store([percept.getVisionWidth(), percept.getVisionHeight()])
        
        # Brain processes
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {curiosity.getCuriosityLevel():.3f}")
            print(f"  Memory activity: {working_memory.getMemoryActivity():.3f}")
            if planner:
                print(f"  Planning confidence: {planner.getPlanningConfidence():.3f}")
    
    print("\nSimulation complete!")
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Run the simulation
run_complete_agent_simulation()
```

### Performance Monitoring Example

```python
import pynlm
import time

def benchmark_brain_performance():
    # Create different brain types for comparison
    brains = {
        "simple": pynlm.createSimpleBrain(100),
        "research": pynlm.createResearchBrain(1000, 0.001),
        "development": pynlm.createDevelopmentBrain(500, True),
        "performance": pynlm.createPerformanceBrain(2000, True),
    }
    
    results = {}
    
    for name, brain in brains.items():
        print(f"Benchmarking {name} brain...")
        brain.initialize()
        
        start_time = time.time()
        result = pynlm.benchmark(brain, 500, True)
        end_time = time.time()
        
        results[name] = {
            "benchmark": result,
            "wall_time": end_time - start_time,
            "speedup": result.totalTime / (end_time - start_time) if (end_time - start_time) > 0 else 0
        }
        
        print(f"  Steps: {result.stepsCompleted}")
        print(f"  Total spikes: {result.totalSpikes}")
        print(f"  Average firing rate: {result.averageFiringRate:.2f} Hz")
        print(f"  Execution time: {result.totalTime:.3f}s")
    
    print("\nPerformance Comparison:")
    for name, result in results.items():
        print(f"{name:12s}: {result['benchmark'].averageFiringRate:8.2f} Hz/s, "
              f"{result['benchmark'].totalTime:6.3f}s total")

benchmark_brain_performance()
```

### Error Handling and Validation Example

```python
import pynlm

def robust_simulation_example():
    # Create configuration
    config = pynlm.createResearchConfig()
    
    # Validate configuration
    is_valid, errors = pynlm.validateConfig(config)
    if not is_valid:
        print("Configuration errors found:")
        for error in errors:
            print(f"  - {error}")
        return False
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 4, 4)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Main simulation loop with error checking
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get and validate sensory input
        percept = world.getSensoryPercept()
        if not pynlm.validateSensoryInput(brain, percept):
            print(f"Warning: Invalid sensory input at step {step}")
            continue
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Check for emergency conditions
        if brain.getTotalSpikeCount() > 10000:
            print(f"Emergency shutdown at step {step}: spike count too high")
            break
        
        # Save checkpoint periodically
        if step % 50 == 0:
            success = pynlm.saveCheckpoint(brain, f"checkpoint_step_{step}.bin", create_backup=False)
            if success:
                print(f"Checkpoint saved at step {step}")
            else:
                print(f"Warning: Failed to save checkpoint at step {step}")
    
    print("Robust simulation completed successfully")
    return True

robust_simulation_example()
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
