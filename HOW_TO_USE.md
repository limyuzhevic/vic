# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a **brain-inspired artificial developmental brain** simulator written in C++20. It implements **Phase 6: Final Integration** - a complete closed-loop brain system where all components work together cohesively.

**Key Features (Phase 6 Complete):**
- Memory systems fully integrated with neural processing
- Neuromodulation affecting plasticity and dynamics  
- Prediction integrated as central organizing principle
- Developmental stages modulating learning rates
- Checkpoint save/load and replay functionality
- Complete brain loop: WORLD → SENSORY → NEURAL → MEMORY/PREDICTION → MOTIVATION → ACTION → WORLD

**Project Architecture:**
```
                 WORLD CONSEQUENCE
                       ↓
               REWARD / SURPRISE / ERROR
                       ↓
                 PLASTICITY (STDP, Hebbian, structural)
                       ↓
                 MEMORY / DEVELOPMENT
                       ↓
                CHANGED BRAIN
                       ↓
               CHANGED FUTURE BEHAVIOR
```

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

## Advanced Usage (Phase 6 Features)

### 1. Phase 6 Integration Tests

```python
import pynlm

# Run comprehensive Phase 6 integration test
print("Running Phase 6 integration test...")
results = pynlm.run_phase6_integration_test(episodes=5, steps_per_episode=200)
print(f"Integration results: {results}")
```

### 2. Advanced Configuration Management

```python
import pynlm

# Create advanced configuration
config = pynlm.create_default_config()
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("dopamine_baseline", 0.2)
config.set("curiosity_baseline", 0.1)

# Convert to dictionary and back
config_dict = pynlm.config_to_dict(config)
print(f"Configuration: {config_dict}")

# Create from dictionary
custom_config = pynlm.create_config_from_dict(config_dict)
```

### 3. Brain Validation and Health Checks

```python
import pynlm

# Create and validate brain
brain = pynlm.createBrain(config)
valid, issues = pynlm.validate_brain(brain)
print(f"Brain valid: {valid}")
print(f"Issues: {issues}")

# Health check
health = pynlm.brain_health_check(brain)
print(f"Health status: {health}")
```

### 4. Performance and Benchmarking

```python
import pynlm

# Run brain benchmark
brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()

metrics = pynlm.run_brain_benchmark(brain, num_steps=1000)
print(f"Benchmark results: {metrics}")

# Memory analysis
memory_usage = pynlm.estimate_memory_usage(brain)
print(f"Memory usage: {memory_usage} bytes")

# Efficiency metrics
efficiency = pynlm.get_brain_efficiency_metrics(brain)
print(f"Efficiency metrics: {efficiency}")
```

### 5. Research Tools and Workflows

```python
import pynlm

# Create research workspace
research = pynlm.create_research_workspace()

# Run research experiment
brain = research["brain"]
world = research["world"]
agent = research["agent"]

results = pynlm.run_research_experiment(
    brain, world, agent, 
    experiment_type="integration_test",
    trials=10
)

# Analyze results
analysis = pynlm.analyze_research_data([results])
print(f"Analysis: {analysis}")
```

### 6. Learning and Optimization

```python
import pynlm

# Enable adaptive learning
brain = pynlm.createBrain(pynlm.create_default_config())
brain = pynlm.enable_adaptive_learning(brain, "balanced")

# Create learning environment
env = pynlm.create_learning_environment(episodes=20, steps_per_episode=300)
brain = env["brain"]
world = env["world"]
agent = env["agent"]

# Custom reward function
def curiosity_reward():
    return agent.getCuriosityLevel() * 10.0

# Run learning session
results = pynlm.learning_session(brain, world, agent, episodes=10)
print(f"Learning results: {results}")
```

### 7. Simulation Control and Monitoring

```python
import pynlm

# Create simulation controller
brain = pynlm.createBrain(pynlm.create_default_config())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

controller = pynlm.create_simulation_controller(brain, world, agent)

# Control simulation
print(controller["reset"]())
print(controller["status"]())

# Step through simulation
for i in range(10):
    controller["step"](1)
    status = controller["status"]()
    print(f"Step {i+1}: {status['spikes']} spikes")
```

### 8. Export and Serialization

```python
import pynlm

# Create and save brain
brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()

# Save brain state
pynlm.save_brain_state(brain, "my_brain.nlm")

# Load brain state
loaded_brain = pynlm.load_brain_state("my_brain.nlm")
print(f"Loaded brain: {loaded_brain.getTotalNeuronCount()} neurons")

# Export research data
results = {"experiment": "demo", "score": 0.85}
pynlm.export_results(results, "research_data.json", "json")
```

### 9. Phase 6 Specific Functions

```python
import pynlm

# Run Phase 6 demo
brain, world, agent, result = pynlm.create_phase6_demo()
print(f"Phase 6 demo completed: {result}")

# Validate Phase 6 system
is_valid, issues = pynlm.validate_phase6_system(brain)
print(f"Phase 6 validation: {is_valid}")
print(f"Issues: {issues}")

# Benchmark Phase 6 system
benchmark_results = pynlm.benchmark_phase6_system(num_steps=500)
print(f"Benchmark: {benchmark_results}")
```

### 10. Python Bindings Help and Examples

```python
import pynlm

# Get help information
nlm.print_help()

# Get usage examples
examples = nlm.get_examples()
print(f"Example categories: {list(examples.keys())}")

# Show advanced help
nlm.print_advanced_help()

# Show migration guide
nlm.legacy_mode()
```

## Performance Considerations (Phase 6 Optimized)

### Memory Requirements

```python
# Estimate memory for different brain sizes
memory_1k = nlm.estimate_simulation_memory(neurons=1000, regions=2, connections=5000)
memory_10k = nlm.estimate_simulation_memory(neurons=10000, regions=5, connections=50000)
memory_100k = nlm.estimate_simulation_memory(neurons=100000, regions=10, connections=500000)
```

### Recommendations

1. **Small networks (< 1000 neurons)**: Enable visualization for debugging
2. **Medium networks (1k-10k neurons)**: Use standard simulation settings
3. **Large networks (>10k neurons)**: Enable performance optimizations
4. **Research workloads**: Use parallel simulation for multiple experiments

### Advanced Configuration

```python
# Optimized configuration for research
research_config = nlm.create_advanced_config(
    neurons=2000,
    regions=3,
    connection_prob=0.1,
    dopamine_level=0.15
)

# Run optimization study
study_config = {
    "learning_rate": 0.01,
    "plasticity_type": "stdp",
    "development_enabled": True
}

study = nlm.create_research_experiment(
    name="brain_optimization",
    description="Optimizing neural parameters for learning",
    parameters=study_config
)
```

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
