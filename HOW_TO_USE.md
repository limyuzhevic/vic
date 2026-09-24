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

    # High-Level API

The NLM Python bindings include a comprehensive high-level API in `python/nlm_python.py` that provides:

## What the High-Level API Provides

1. **Easy Simulation Setup**: One-line creation of complete simulation environments
2. **High-Level Abstractions**: Concept-based APIs instead of low-level operations
3. **Built-in Monitoring**: Progress tracking, statistics collection, and reporting
4. **Experiment Management**: Run and compare multiple experiments
5. **Progress Tracking**: Callback systems and progress reporting
6. **Configuration Templates**: Pre-configured settings for common scenarios

## Quick Start with High-Level API

```python
import nlm_python
from nlm_python import create_simulation, SimulationRunner, ExperimentManager

# Create a simulation with simple configuration
sim = create_simulation(neuron_count=1000, max_steps=5000)

# Run the simulation
runner = SimulationRunner(sim)
results = runner.run(num_episodes=10, steps_per_episode=500)

# View results
print(f"Learning achieved: {results.get('learning_detected', False)}")
print(f"Total reward: {results.get('total_reward', 0):.2f}")
```

## Key Classes

### NLMConfig
Configuration manager for NLM simulations with sensible defaults:

```python
# Basic configuration
config = nlm_python.NLMConfig(neuron_count=500)

# Enable advanced features
config.enable_features(
    reward_modulation=True,
    structural_plasticity=True,
    development=True,
    curiosity=True
)

# Configure plasticity rules
config.with_plasticity_rules(hebbian=True, stdp=True)

# Set development stages
config.with_development_stages(stages=5)
```

### NeuralNetworkSimulator
High-level interface for running neural network simulations:

```python
# Create simulator
simulator = nlm_python.NeuralNetworkSimulator(config)

# Run simulation
simulator.run_episode(1000, verbose=True)

# Get statistics
stats = simulator.get_simulation_stats()
print(f"Firing rate: {stats['avg_firing_rate']:.3f}")
print(f"Curiosity: {stats['curiosity']:.3f}")
```

### SimulationRunner
Simulation execution with monitoring and reporting:

```python
# Create runner with simulation
runner = nlm_python.SimulationRunner(simulator)

# Run with monitoring
results = runner.run(
    num_episodes=5,
    steps_per_episode=200,
    verbose=True
)

# Results include:
print(f"Learning achieved: {results.get('learning_detected', False)}")
print(f"Average reward: {results.get('avg_reward', 0):.3f}")
print(f"Final firing rate: {results.get('final_firing_rate', 0):.3f}")
```

### ExperimentManager
Manage and compare multiple experiments:

```python
# Create experiment manager
manager = nlm_python.ExperimentManager()

# Create different experiments
config1 = nlm_python.NLMConfig(neuron_count=100, connection_probability=0.1)
config2 = nlm_python.NLMConfig(neuron_count=200, connection_probability=0.2)

exp1 = manager.create_experiment("Small Network", config1, num_episodes=3, steps_per_episode=100)
exp2 = manager.create_experiment("Large Network", config2, num_episodes=3, steps_per_episode=100)

# Run experiments
results1 = exp1.run()
results2 = exp2.run()

# Compare experiments
comparison = manager.compare_experiments(["Small Network", "Large Network"])
```

## Common Use Case Patterns

### 1. Learning Simulation
```python
def run_learning_simulation(neuron_count=500, episodes=10):
    """Run a learning simulation with reward modulation."""
    
    # Create configuration optimized for learning
    config = (nlm_python.NLMConfig(neuron_count=neuron_count)
              .enable_features(
                  reward_modulation=True,
                  structural_plasticity=True,
                  development=True,
                  curiosity=True
              )
              .with_plasticity_rules(hebbian=True, stdp=True, reward_modulated=True))
    
    # Create simulation
    sim = nlm_python.NeuralNetworkSimulator(config)
    
    # Run simulation
    runner = nlm_python.SimulationRunner(sim)
    results = runner.run(num_episodes=episodes, steps_per_episode=500, verbose=True)
    
    # Analyze learning
    learning_detected = (results.get('learning_detected', False) and 
                        results.get('avg_reward', 0) > 0)
    
    return {
        'learning_detected': learning_detected,
        'total_reward': results.get('total_reward', 0),
        'final_firing_rate': results.get('final_firing_rate', 0),
        'config': config.__dict__
    }
```

### 2. Development Simulation
```python
def run_development_simulation(start_stage="Initial", stages=5):
    """Run a simulation focused on developmental stages."""
    
    # Create configuration for development
    config = (nlm_python.NLMConfig(neuron_count=300)
              .enable_features(
                  reward_modulation=False,
                  structural_plasticity=True,
                  development=True,
                  curiosity=True
              )
              .with_development_stages(stages=stages))
    
    # Create and run simulation
    sim = nlm_python.NeuralNetworkSimulator(config)
    
    # Custom runner with development tracking
    def development_callback(episode, episode_metrics):
        stage = sim.agent.getDevelopmentalStage()
        print(f"Episode {episode}: Development stage = {stage}")
    
    runner = nlm_python.SimulationRunner(sim)
    results = runner.run(
        num_episodes=10,
        steps_per_episode=300,
        verbose=False,
        callback=development_callback
    )
    
    return results
```

### 3. Comparison Experiment
```python
def compare_configurations():
    """Compare different network configurations."""
    
    # Different configurations to compare
    configs = [
        nlm_python.NLMConfig(neuron_count=100, connection_probability=0.05)
            .enable_features(reward_modulation=True, development=True),
        nlm_python.NLMConfig(neuron_count=500, connection_probability=0.15)
            .enable_features(reward_modulation=True, development=True),
        nlm_python.NLMConfig(neuron_count=1000, connection_probability=0.25)
            .enable_features(reward_modulation=True, development=True)
    ]
    
    # Run all experiments
    results = []
    for i, config in enumerate(configs):
        print(f"Running configuration {i+1}/3...")
        sim = nlm_python.NeuralNetworkSimulator(config)
        runner = nlm_python.SimulationRunner(sim)
        result = runner.run(num_episodes=2, steps_per_episode=200, verbose=False)
        results.append({
            'config_index': i,
            'neuron_count': config.neuron_count,
            'connection_probability': config.connection_probability,
            'result': result
        })
    
    # Analyze and compare
    print("\n=== Configuration Comparison ===")
    for result in results:
        print(f"Config {result['config_index']}: {result['neuron_count']} neurons, "
              f"conn_prob={result['connection_probability']}")
        print(f"  Learning: {result['result'].get('learning_detected', False)}")
        print(f"  Total reward: {result['result'].get('total_reward', 0):.2f}")
        print(f"  Final firing rate: {result['result'].get('final_firing_rate', 0):.3f}")
        print()
    
    return results
```

### 4. Simple Simulation Template
```python
def create_simple_simulation(**kwargs):
    """Create a simple simulation with minimal configuration."""
    
    # Use high-level create_simulation function
    return nlm_python.create_simulation(**kwargs)

def run_simple_test(neurons=100, steps=50):
    """Run a simple test for verification."""
    
    return nlm_python.run_simple_test(neurons, steps)

# Usage example
if __name__ == "__main__":
    # Create simple simulation
    sim = nlm_python.create_simulation(neuron_count=200, timestep=0.01)
    
    # Run steps
    for i in range(100):
        metrics = sim.step()
        if i % 25 == 0:
            print(f"Step {i}: Reward={metrics['reward']:.3f}")
    
    # Get final statistics
    stats = sim.get_simulation_stats()
    print(f"\nFinal statistics:")
    print(f"Firing neurons: {stats['neurons_firing']}")
    print(f"Average firing rate: {stats['avg_firing_rate']:.3f}")
    print(f"Curiosity level: {stats['curiosity']:.3f}")
    print(f"Development stage: {stats['development_stage']}")
```

## Integration with Low-Level API

The high-level API seamlessly integrates with the low-level `pynlm` bindings. You can:

1. **Access low-level components**: Use `sim.brain`, `sim.agent`, `sim.world` to access low-level functionality
2. **Mix high-level and low-level APIs**: Use high-level setup but low-level control where needed
3. **Custom experiments**: Build custom simulation scenarios using both APIs

```python
# High-level setup, low-level control
sim = nlm_python.create_simulation(neuron_count=500)

# Access low-level components
brain = sim.brain
agent = sim.agent
world = sim.world

# Use low-level API directly
brain.step(0)
agent.processSensoryInput(world.getSensoryPercept())
```

## Performance Considerations

### For Small Networks (< 500 neurons)
```python
sim = nlm_python.create_simulation(neuron_count=100)
```

### For Medium Networks (500-2000 neurons)
```python
sim = nlm_python.create_simulation(
    neuron_count=1000,
    connection_probability=0.1
)
```

### For Large Networks (> 2000 neurons)
```python
sim = nlm_python.create_simulation(
    neuron_count=5000,
    connection_probability=0.05,
    enable_features(
        reward_modulation=False,  # Can be expensive
        development=True
    )
)
```

### For Real-time Applications
```python
sim = nlm_python.create_simulation(
    neuron_count=200,
    timestep=0.001
)
# Run with short episodes and frequent checkpoints
```

## Error Handling

The high-level API includes comprehensive error handling:

```python
try:
    sim = nlm_python.create_simulation(neuron_count=500)
    results = runner.run(num_episodes=10)
except RuntimeError as e:
    print(f"Simulation error: {e}")
except ValueError as e:
    print(f"Configuration error: {e}")
```

## Best Practices

1. **Always validate configuration**: Use the built-in validation
2. **Monitor progress**: Use callbacks for long-running simulations
3. **Use appropriate timestep**: Smaller timesteps are more accurate but slower
4. **Enable features judiciously**: Some features can significantly impact performance
5. **Use simulation checkpoints**: For long-running experiments
6. **Analyze results systematically**: Compare multiple configurations

The high-level API makes it easy to create, run, and analyze NLM simulations while maintaining access to low-level control when needed.

## Further Examples

See the `python/` directory for additional example scripts and documentation.

---

## Next Steps

1. **Read the examples**: Check the `python/` directory for additional use case patterns
2. **Try the demonstrations**: Run the example scripts to see the API in action
3. **Experiment**: Modify configurations and observe effects on learning and behavior
4. **Integrate**: Combine the high-level API with custom simulation scenarios
5. **Performance tune**: Adjust parameters for your specific use case

The high-level Python API provides a powerful, intuitive interface for working with NLM simulations while maintaining flexibility for advanced use cases.

## References

- [Low-Level API Documentation](https://github.com/nlm-project/nlm/blob/main/HOW_TO_USE.md) - For advanced usage
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [Phase 6 Integration Demo](src/experiments/Phase6Demo.cpp) - Real-world integration example
- [Example Scripts](python/) - Additional examples and templates

---

When you're comfortable with the basics, consider exploring:

1. **Creating custom environments** using the low-level world API
2. **Implementing custom plasticity rules** for specialized learning
3. **Running Phase 6 integration experiments** using the high-level API
4. **Building advanced agent behaviors** with neuromodulation
5. **Performance optimization** for large-scale simulations

The high-level Python API is designed to scale from simple tutorials to complex research experiments while maintaining a consistent, easy-to-use interface.


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
