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

## Part 5: Advanced Example Scripts

### Advanced Complete Agent Example

```python
import pynlm

def run_advanced_agent_simulation(num_steps=2000):
    """Run a complete agent simulation with brain and world, including advanced features."""
    
    # 1. Create configuration with custom parameters
    config = pynlm.createDefaultConfig()
    
    # Configure brain parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.08)
    
    # Configure plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Configure neuromodulation
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Configure development
    config.set("development.enable", True)
    config.set("development.critical_period_duration", 500)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,
        height=30,
        visionWidth=12,
        visionHeight=12
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)
    world.setRandomSeed(42)  # For reproducible experiments
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Create performance tracking
    metrics = {
        'steps_completed': 0,
        'total_reward': 0.0,
        'max_firing_rate': 0.0,
        'min_firing_rate': float('inf'),
        'episodes_learned': 0,
        'curiosity_peak': 0.0,
        'novelty_peak': 0.0
    }
    
    print(f"Starting advanced agent simulation for {num_steps} steps...")
    print("Brain configuration:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print()
    
    # 8. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Update metrics
        metrics['steps_completed'] = step + 1
        metrics['total_reward'] += reward
        
        current_firing = brain.getAverageFiringRate()
        metrics['max_firing_rate'] = max(metrics['max_firing_rate'], current_firing)
        metrics['min_firing_rate'] = min(metrics['min_firing_rate'], current_firing)
        
        metrics['curiosity_peak'] = max(metrics['curiosity_peak'], agent.getCuriosityLevel())
        metrics['novelty_peak'] = max(metrics['novelty_peak'], agent.getNoveltyLevel())
        
        if step % 100 == 0:
            print(f"Step {step:4d}: "
                  f"Reward: {reward:6.3f} | "
                  f"Firing: {current_firing:5.2f} | "
                  f"Curiosity: {agent.getCuriosityLevel():5.3f} | "
                  f"Novelty: {agent.getNoveltyLevel():5.3f} | "
                  f"Dev Stage: {brain.getDevelopmentalStage()}")
    
    # 9. Finalize metrics
    if metrics['steps_completed'] > 0:
        metrics['avg_firing_rate'] = (metrics['max_firing_rate'] + metrics['min_firing_rate']) / 2
        metrics['avg_reward'] = metrics['total_reward'] / metrics['steps_completed']
    
    print()
    print("=== Advanced Agent Simulation Complete ===")
    print(f"Total steps completed: {metrics['steps_completed']}")
    print(f"Average reward per step: {metrics.get('avg_reward', 0.0):.3f}")
    print(f"Firing rate range: {metrics['min_firing_rate']:.2f} - {metrics['max_firing_rate']:.2f}")
    print(f"Curiosity peak: {metrics['curiosity_peak']:.3f}")
    print(f"Novelty peak: {metrics['novelty_peak']:.3f}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    
    return metrics

# Run the advanced simulation
if __name__ == "__main__":
    results = run_advanced_agent_simulation(2000)
```

### Performance Benchmarking

```python
import pynlm
import time

def benchmark_performance():
    """Benchmark different aspects of NLM performance."""
    
    print("=== NLM Performance Benchmark ===")
    
    # Create configurations for different benchmarks
    configs = {
        'small': pynlm.createDefaultConfig(),
        'medium': pynlm.createDefaultConfig(),
        'large': pynlm.createDefaultConfig()
    }
    
    configs['small'].set("brain.neuron_count", 500)
    configs['medium'].set("brain.neuron_count", 2000)
    configs['large'].set("brain.neuron_count", 5000)
    
    benchmarks = ['plasticity', 'stdp', 'all']
    
    for size, config in configs.items():
        print(f"\n--- {size.capitalize()} Network ({config.getOr<int64_t>('brain.neuron_count', 0)} neurons) ---")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for benchmark in benchmarks:
            # Reset brain for each benchmark
            brain.reset()
            brain.initialize()
            
            # Enable appropriate plasticity
            config.set("plasticity.stdp.enable", benchmark in ['stdp', 'all'])
            config.set("plasticity.hebbian.enable", benchmark in ['plasticity', 'all'])
            config.set("plasticity.structural.enable", benchmark in ['plasticity', 'all'])
            
            # Run benchmark
            steps = 1000 if size == 'small' else 500 if size == 'medium' else 200
            
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            end_time = time.time()
            
            duration = end_time - start_time
            spikes_per_second = brain.getTotalSpikeCount() / duration
            
            print(f"  {benchmark:10s}: {duration:.3f}s, {spikes_per_second:.0f} spikes/sec")

if __name__ == "__main__":
    benchmark_performance()
```

### Configuration Management

```python
import pynlm
import json

def advanced_config_management():
    """Demonstrate advanced configuration management features."""
    
    print("=== Advanced Configuration Management ===")
    
    # Create base configuration
    base_config = pynlm.createDefaultConfig()
    base_config.set("brain.neuron_count", 1000)
    base_config.set("brain.synapse_density", 0.1)
    
    # Create specialized configurations
    research_config = pynlm.createDefaultConfig()
    research_config.set("brain.neuron_count", 5000)
    research_config.set("brain.synapse_density", 0.05)
    research_config.set("plasticity.stdp.enable", True)
    research_config.set("plasticity.stdp.learning_rate", 0.001)
    
    production_config = pynlm.createDefaultConfig()
    production_config.set("brain.neuron_count", 2000)
    production_config.set("brain.synapse_density", 0.12)
    production_config.set("plasticity.stdp.enable", True)
    production_config.set("plasticity.stdp.learning_rate", 0.01)
    
    # Save configurations to files
    base_config.saveToFile("base_config.json")
    research_config.saveToFile("research_config.json")
    production_config.saveToFile("production_config.json")
    
    print("Saved configurations to files:")
    print("  - base_config.json")
    print("  - research_config.json") 
    print("  - production_config.json")
    
    # Load and compare configurations
    loaded_config = pynlm.Config()
    loaded_config.loadFromFile("research_config.json")
    
    print(f"\nLoaded configuration:")
    print(f"  Neurons: {loaded_config.getOr<int64_t>('brain.neuron_count', 0)}")
    print(f"  Synapse density: {loaded_config.getOr<float>('brain.synapse_density', 0.0)}")
    print(f"  STDP enabled: {loaded_config.getOr<bool>('plasticity.stdp.enable', False)}")
    
    # Configuration validation
    print(f"\nConfiguration validation:")
    is_valid = loaded_config.has("brain.neuron_count") and \
               loaded_config.has("brain.synapse_density") and \
               loaded_config.getOr<int64_t>("brain.neuron_count", 0) > 0 and \
               0.0 <= loaded_config.getOr<float>("brain.synapse_density", 0.0) <= 1.0
    
    print(f"  Valid: {'✓' if is_valid else '✗'}")
    
    # Configuration comparison
    print(f"\nConfiguration comparison:")
    print(f"  Base vs Research: {base_config.getOr<int64_t>('brain.neuron_count', 0)} vs "
          f"{research_config.getOr<int64_t>('brain.neuron_count', 0)} neurons")
    
    # Configuration merging
    merged_config = pynlm.createDefaultConfig()
    merged_config.merge(base_config)
    merged_config.set("custom.setting", "custom_value")
    
    print(f"  Merged config has custom setting: {merged_config.has('custom.setting')}")

if __name__ == "__main__":
    advanced_config_management()
```

### Multi-Agent Simulation

```python
import pynlm

def multi_agent_simulation(num_agents=3, steps=1000):
    """Simulate multiple interacting agents."""
    
    print(f"=== Multi-Agent Simulation ({num_agents} agents) ===")
    
    # Create shared configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.08)
    
    # Create agents
    agents = []
    brains = []
    
    for i in range(num_agents):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
        brains.append(brain)
        
        # Each agent gets different random seed
        brain.getConfig().set("random_seed", 42 + i)
        
        print(f"Agent {i}: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSynapseCount()} synapses")
    
    # Run multi-agent simulation
    for step in range(steps):
        # Each agent operates independently
        for agent_idx, agent in enumerate(agents):
            # Create a simple world for each agent
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
            world.reset()
            world.setAgentStart(5.0, 5.0)
            
            agent.initialize(world)
            agent.enableCuriosity(True)
            agent.enableRewardModulation(True)
            
            # Run agent episode
            for sub_step in range(50):  # Short episodes for multi-agent
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brains[agent_idx].step(sub_step)
                
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        if step % 200 == 0:
            print(f"Step {step}/{steps} completed")
    
    # Aggregate results
    print(f"\n=== Multi-Agent Results ===")
    total_spikes = sum(brain.getTotalSpikeCount() for brain in brains)
    total_neurons = sum(brain.getTotalNeuronCount() for brain in brains)
    
    print(f"Total spikes across all agents: {total_spikes}")
    print(f"Total neurons across all agents: {total_neurons}")
    print(f"Average spikes per neuron: {total_spikes / max(total_neurons, 1)}")

if __name__ == "__main__":
    multi_agent_simulation(3, 1000)
```

### Error Handling and Robustness

```python
import pynlm

def robust_simulation():
    """Demonstrate robust error handling in simulations."""
    
    print("=== Robust Simulation with Error Handling ===")
    
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        
        # Check if initialization was successful
        if not brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        print("Brain initialized successfully")
        
        # Simulate with error checking
        for step in range(100):
            try:
                brain.step(step)
                
                # Check for abnormal conditions
                firing_rate = brain.getAverageFiringRate()
                if firing_rate > 100:  # Arbitrary high firing threshold
                    print(f"Warning: High firing rate detected at step {step}: {firing_rate}")
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Continue with next step rather than stopping
                continue
        
        print("Simulation completed with error handling")
        
    except RuntimeError as e:
        print(f"Configuration error: {e}")
    except MemoryError as e:
        print(f"Memory error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")

if __name__ == "__main__":
    robust_simulation()
```

### Advanced Python Features

```python
import pynlm
import numpy as np

def advanced_features_demo():
    """Demonstrate advanced Python features with NLM."""
    
    print("=== Advanced Python Features Demo ===")
    
    # 1. NumPy integration for large-scale simulations
    print("\n1. NumPy integration for batch operations:")
    
    def batch_simulation(config, num_sims=5):
        """Run multiple simulations in batch."""
        
        results = []
        
        for sim in range(num_sims):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
            
            results.append({
                'simulation': sim,
                'neurons': brain.getTotalNeuronCount(),
                'synapses': brain.getTotalSynapseCount(),
                'spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            })
        
        return results
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    batch_results = batch_simulation(config, 3)
    print(f"Batch results: {len(batch_results)} simulations completed")
    
    # Convert to NumPy for analysis
    results_array = np.array([[
        r['neurons'], r['synapses'], r['spikes'], r['firing_rate']
    ] for r in batch_results])
    
    print(f"Average neurons: {np.mean(results_array[:, 0]):.0f}")
    print(f"Average firing rate: {np.mean(results_array[:, 3]):.2f}")
    
    # 2. Generator pattern for memory-efficient processing
    print("\n2. Generator pattern for memory-efficient processing:")
    
    def step_generator(brain, num_steps):
        """Generator that yields brain state at each step."""
        for step in range(num_steps):
            brain.step(step)
            yield {
                'step': step,
                'neurons_firing': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            }
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    step_gen = step_generator(brain, 20)
    
    # Process steps one at a time
    for i, state in enumerate(step_gen):
        if i % 5 == 0:  # Print every 5th step
            print(f"Step {state['step']}: {state['neurons_firing']} neurons firing, "
                  f"{state['firing_rate']:.2f} firing rate")
    
    # 3. Context manager for resource management
    print("\n3. Context manager pattern:")
    
    class NLMSession:
        """Context manager for managing NLM sessions."""
        
        def __init__(self, config):
            self.config = config
            self.brain = None
        
        def __enter__(self):
            print("  Entering NLM session...")
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            return self.brain
        
        def __exit__(self, exc_type, exc_val, exc_tb):
            print("  Exiting NLM session...")
            if self.brain:
                self.brain.reset()
            if exc_type:
                print(f"  Session ended with exception: {exc_val}")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    
    with NLMSession(config) as brain:
        print(f"  Brain initialized with {brain.getTotalNeuronCount()} neurons")
        brain.step(10)
        print("  Step completed")
    
    # 4. Callback pattern for progress monitoring
    print("\n4. Callback pattern for progress monitoring:")
    
    def run_with_progress_callback(brain, num_steps, progress_callback=None):
        """Run simulation with progress callback."""
        
        for step in range(num_steps):
            brain.step(step)
            
            if progress_callback:
                progress_callback(step, num_steps)
        
        return brain.getTotalSpikeCount()
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 150)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    def progress_callback(current, total):
        if current % 20 == 0 or current == total:
            print(f"  Progress: {current}/{total} steps ({current*100/total:.1f}%)")
    
    print("  Running simulation with progress monitoring...")
    total_spikes = run_with_progress_callback(brain, 60, progress_callback)
    print(f"  Total spikes: {total_spikes}")

if __name__ == "__main__":
    advanced_features_demo()
```

---

## Advanced Python Integration

### 1. NumPy Integration for Large-Scale Simulations

NLM integrates seamlessly with NumPy for scientific computing workflows:

```python
import pynlm
import numpy as np

# Batch processing for parameter sweeps
config = pynlm.createDefaultConfig()
neuron_counts = np.arange(500, 5000, 500)

results = []
for neurons in neuron_counts:
    config.set("brain.neuron_count", neurons)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
    
    results.append({
        'neurons': neurons,
        'spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate()
    })

# Analyze results with NumPy
results_array = np.array([[r['neurons'], r['spikes'], r['firing_rate']] 
                          for r in results])

print(f"Neuron-Spike correlation: {np.corrcoef(results_array[:, 0], results_array[:, 1])[0, 1]:.3f}")
```

### 2. Machine Learning Workflow Integration

```python
import pynlm
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt

def create_training_dataset(num_samples=100, max_neurons=2000):
    """Create a dataset for machine learning applications."""
    
    datasets = []
    targets = []
    
    for i in range(num_samples):
        config = pynlm.createDefaultConfig()
        
        # Generate random parameters
        neurons = np.random.randint(100, max_neurons)
        density = np.random.uniform(0.05, 0.2)
        plasticity = np.random.choice([True, False])
        
        config.set("brain.neuron_count", neurons)
        config.set("brain.synapse_density", density)
        config.set("plasticity.stdp.enable", plasticity)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(200):
            brain.step(step)
        
        # Extract features
        features = {
            'neuron_count': neurons,
            'synapse_density': density,
            'plasticity_enabled': plasticity,
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'firing_ratio': brain.getFiringNeuronCount() / max(1, brain.getTotalNeuronCount())
        }
        
        datasets.append(features)
        
        # Simple target: binary classification based on plasticity
        targets.append(1 if plasticity else 0)
    
    return datasets, targets

# Create training data
datasets, targets = create_training_dataset(50)

# Split data
X_train, X_test, y_train, y_test = train_test_split(
    datasets, targets, test_size=0.2, random_state=42
)

print(f"Created dataset with {len(datasets)} samples")
print(f"Training set: {len(X_train)} samples, Test set: {len(X_test)} samples")
```

### 3. Visualization and Analysis

```python
import pynlm
import matplotlib.pyplot as plt
import seaborn as sns

def analyze_brain_dynamics(brain, num_steps=500):
    """Analyze brain dynamics and create visualizations."""
    
    # Collect time series data
    steps = list(range(num_steps))
    firing_rates = []
    spike_counts = []
    neuron_counts = []
    
    for step in range(num_steps):
        brain.step(step)
        firing_rates.append(brain.getAverageFiringRate())
        spike_counts.append(brain.getTotalSpikeCount())
        neuron_counts.append(brain.getFiringNeuronCount())
    
    # Create analysis plots
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Firing rate over time
    axes[0, 0].plot(steps, firing_rates)
    axes[0, 0].set_title('Average Firing Rate Over Time')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Firing Rate')
    
    # Spike count over time
    axes[0, 1].plot(steps, spike_counts)
    axes[0, 1].set_title('Total Spike Count Over Time')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Total Spikes')
    
    # Neuron activity distribution
    axes[1, 0].hist(neuron_counts, bins=20, alpha=0.7)
    axes[1, 0].set_title('Neuron Activity Distribution')
    axes[1, 0].set_xlabel('Active Neurons')
    axes[1, 0].set_ylabel('Frequency')
    
    # Statistical summary
    axes[1, 1].text(0.1, 0.5, f'Statistics:\n'
                   f'Mean firing rate: {np.mean(firing_rates):.2f}\n'
                   f'Mean spikes: {np.mean(spike_counts):.0f}\n'
                   f'Max firing rate: {np.max(firing_rates):.2f}\n'
                   f'Firing rate std: {np.std(firing_rates):.2f}',
                   fontsize=12, fontfamily='monospace')
    axes[1, 1].set_title('Statistical Summary')
    axes[1, 1].axis('off')
    
    plt.tight_layout()
    plt.show()
    
    return {
        'firing_rates': firing_rates,
        'spike_counts': spike_counts,
        'neuron_counts': neuron_counts,
        'mean_firing_rate': np.mean(firing_rates),
        'std_firing_rate': np.std(firing_rates)
    }
```

### 4. Parallel Processing for Multi-Instance Testing

```python
import pynlm
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor

def simulate_brain(config_dict, steps=200):
    """Simulate a single brain - can be run in parallel."""
    
    config = pynlm.createDefaultConfig()
    for key, value in config_dict.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        'config': config_dict,
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate()
    }

def parallel_simulation_sweeps(param_combinations, num_processes=None):
    """Run multiple simulations in parallel."""
    
    if num_processes is None:
        num_processes = mp.cpu_count()
    
    print(f"Running {len(param_combinations)} simulations using {num_processes} processes...")
    
    with ProcessPoolExecutor(max_workers=num_processes) as executor:
        results = list(executor.map(simulate_brain, param_combinations))
    
    return results

# Define parameter combinations for parallel testing
param_combinations = []
for neurons in [100, 500, 1000, 2000]:
    for density in [0.05, 0.1, 0.15]:
        for plasticity in [True, False]:
            param_combinations.append({
                'brain.neuron_count': neurons,
                'brain.synapse_density': density,
                'plasticity.stdp.enable': plasticity
            })

# Run parallel simulations
parallel_results = parallel_simulation_sweeps(param_combinations[:8])  # Test with subset

print(f"Completed {len(parallel_results)} parallel simulations")
```

## Performance Optimization

### 1. Memory Management

```python
import pynlm
import gc

def optimized_simulation(brain, num_steps, memory_efficient=True):
    """Run simulation with memory optimization."""
    
    if memory_efficient:
        # Force garbage collection periodically
        for step in range(num_steps):
            brain.step(step)
            
            if step % 100 == 0:
                gc.collect()  # Help with memory management
                
                # Get memory usage if available
                if hasattr(brain, 'getMemoryUsage'):
                    memory_mb = brain.getMemoryUsage() / (1024 * 1024)
                    print(f"Step {step}: Memory usage: {memory_mb:.1f} MB")
    else:
        # Standard simulation
        for step in range(num_steps):
            brain.step(step)
    
    return brain.getTotalSpikeCount()
```

### 2. Step-size Optimization

```python
import pynlm
import time

def optimized_step_simulation(brain, num_steps, timestep=0.001):
    """Optimize simulation with variable timesteps."""
    
    # Test different timestep sizes
    timesteps = [0.001, 0.005, 0.01, 0.05]
    
    for ts in timesteps:
        print(f"\nTesting timestep: {ts}")
        
        brain.reset()
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(num_steps):
            brain.step(step, step * ts)
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"  Duration: {duration:.3f}s")
        print(f"  Steps/sec: {num_steps / duration:.0f}")
        print(f"  Final firing rate: {brain.getAverageFiringRate():.2f}")
```

## Integration with Scientific Computing

### 1. Jupyter Notebook Support

```python
# In Jupyter notebooks, you can use:
import pynlm
%matplotlib inline

# Create interactive brain visualization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Display real-time updates
from IPython.display import display, HTML
import time

for i in range(100):
    brain.step(i)
    
    # Update display
    display(HTML(f"""
    <div style="font-family: monospace;">
        <h3>Brain State Update {i}</h3>
        <p>Neurons: {brain.getTotalNeuronCount()}</p>
        <p>Synapses: {brain.getTotalSynapseCount()}</p>
        <p>Firing Neurons: {brain.getFiringNeuronCount()}</p>
        <p>Average Firing Rate: {brain.getAverageFiringRate():.2f}</p>
        <p>Total Spikes: {brain.getTotalSpikeCount()}</p>
    </div>
    """))
    time.sleep(0.1)
```

### 2. Integration with Data Science Libraries

```python
import pynlm
import pandas as pd
import plotly.graph_objects as go

def create_interactive_dashboard(brain, num_steps=200):
    """Create an interactive dashboard for brain simulation."""
    
    # Collect data
    data = {
        'step': list(range(num_steps)),
        'firing_rate': [],
        'total_spikes': [],
        'firing_neurons': []
    }
    
    for step in range(num_steps):
        brain.step(step)
        data['firing_rate'].append(brain.getAverageFiringRate())
        data['total_spikes'].append(brain.getTotalSpikeCount())
        data['firing_neurons'].append(brain.getFiringNeuronCount())
    
    # Create DataFrame
    df = pd.DataFrame(data)
    
    # Create interactive plots
    fig = go.Figure()
    
    # Add firing rate trace
    fig.add_trace(go.Scatter(
        x=df['step'],
        y=df['firing_rate'],
        mode='lines',
        name='Firing Rate',
        line=dict(color='blue')
    ))
    
    # Add spike count trace (secondary axis)
    fig.add_trace(go.Scatter(
        x=df['step'],
        y=df['total_spikes'],
        mode='lines',
        name='Total Spikes',
        line=dict(color='red', dash='dash'),
        yaxis='y2'
    ))
    
    # Update layout
    fig.update_layout(
        title='NLM Brain Simulation Dashboard',
        xaxis_title='Step',
        yaxis_title='Firing Rate',
        yaxis2=dict(
            title='Total Spikes',
            overlaying='y',
            side='right'
        ),
        hovermode='x unified'
    )
    
    fig.show()
    
    return df
```

## Advanced Configuration Options

### 1. Development Configuration

For development purposes, consider these configuration settings:

```python
import pynlm

# Development configuration for debugging and experimentation
config = pynlm.createDefaultConfig()

# Large network for complex behaviors
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)

# Enable all plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Enable all neuromodulation systems
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Enable development and learning
config.set("development.enable", True)
config.set("development.critical_period_duration", 1000)

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Research Configuration

For research experiments, use these settings:

```python
# Research configuration for scientific studies
config = pynlm.createDefaultConfig()

# Medium-sized network for controlled experiments
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# STDP with specific parameters for research
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 15.0)
config.set("plasticity.stdp.tau_minus", 15.0)

# Control experiments
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", False)  # Disable for clean experiments

# Novelty detection for exploration studies
config.set("neuromod.novelty.enable", True)
config.set("neuromod.curiosity.enable", False)  # Control curiosity

brain = pynlm.createBrain(config)
brain.initialize()
```

### 3. Production Configuration

For production systems, use these optimized settings:

```python
# Production configuration for stable operation
config = pynlm.createDefaultConfig()

# Performance-optimized network
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Balanced plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.enable", True)

# Novelty-driven exploration for adaptive behavior
config.set("neuromod.novelty.enable", True)
config.set("neuromod.curiosity.enable", True)

# Regular development
config.set("development.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
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
