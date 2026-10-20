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

## Advanced Usage Examples

This section covers advanced usage patterns and examples for power users and researchers.

### 1. Research Experiment Configuration

```python
import pynlm

def run_development_experiment(neuron_count=1000, max_steps=5000):
    """Advanced development experiment with NLM"""
    
    # Create advanced configuration for research
    config = pynlm.createAdvancedConfig()
    
    # Override with research-specific parameters
    config.set("neuron_count", neuron_count)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    config.set("synaptogenesis_rate", 0.001)
    config.set("pruning_rate", 0.0001)
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run development experiment
    print(f"Starting development experiment with {neuron_count} neurons for {max_steps} steps")
    
    for step in range(max_steps):
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 500 == 0:
            metrics = agent.getBrainMetrics() if hasattr(agent, 'getBrainMetrics') else {}
            print(f"Step {step}: Development stage {agent.getDevelopmentalStage()}, " +
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Print final results
    print("\n=== Experiment Results ===")
    print(f"Final developmental stage: {agent.getDevelopmentalStage()}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'final_stage': agent.getDevelopmentalStage(),
        'final_firing_rate': brain.getAverageFiringRate()
    }
```

### 2. Configuration Management and Validation

```python
import pynlm

def manage_experiment_config(base_config_path, experiment_type):
    """Advanced configuration management for complex experiments"""
    
    # Load base configuration
    config = pynlm.createDefaultConfig()
    
    # Experiment-specific configuration
    if experiment_type == "development":
        config.set("neuron_count", 2000)
        config.set("region_count", 2)
        config.set("stdp_ltp_weight", 0.025)
        config.set("stdp_ltd_weight", 0.01)
        config.set("synaptogenesis_rate", 0.002)
        config.set("pruning_rate", 0.0002)
        
    elif experiment_type == "plasticity":
        config.set("neuron_count", 500)
        config.set("region_count", 1)
        config.set("stdp_ltp_weight", 0.01)
        config.set("stdp_ltd_weight", 0.015)
        
    elif experiment_type == "prediction":
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        config.set("prediction_confidence", 0.8)
    
    # Validate configuration
    if not pynlm.validateConfig(config):
        print("Configuration validation failed!")
        return None
    
    # Save configuration
    config_filename = f"config_{experiment_type}_{int(time.time())}.json"
    if config.saveToFile(config_filename):
        print(f"Configuration saved to {config_filename}")
    
    # Get suggestions for improvement
    suggestions = pynlm.getConfigSuggestions(experiment_type)
    print(f"Configuration suggestions for {experiment_type}:")
    for key, value in suggestions.items():
        print(f"  {key}: {value}")
    
    return config

def load_and_run_from_config(config_path, simulation_steps):
    """Load configuration from file and run simulation"""
    
    # Try different config formats
    config = pynlm.createDefaultConfig()
    
    # Try JSON first
    if config_path.endswith('.json'):
        # Load JSON config
        # Note: This would require implementing JSON loading in Python bindings
        print(f"Loading JSON config from {config_path} (JSON loading not yet implemented)")
    elif config_path.endswith('.cfg'):
        # For now, we need to use C++ to load .cfg files
        print(f"Loading legacy config from {config_path} (legacy format support not yet implemented)")
    
    print("Configuration loading from file requires C++ implementation")
    return config
```

### 3. Advanced Simulation Control

```python
import pynlm
import time

def advanced_simulation_control(config, world, agent, max_steps):
    """Advanced simulation control with pause, resume, and checkpoint functionality"""
    
    brain = agent.getBrain()
    
    # Create checkpoint directory
    checkpoint_dir = "./checkpoints"
    import os
    os.makedirs(checkpoint_dir, exist_ok=True)
    
    checkpoint_files = []
    
    for step in range(max_steps):
        # Check for pause signal (simulated by keyboard interrupt)
        if step % 100 == 0:
            print(f"Step {step} - Press Ctrl+C to pause, Ctrl+D to save checkpoint...")
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Save checkpoint periodically
        if step % 1000 == 0 and step > 0:
            checkpoint_path = f"{checkpoint_dir}/checkpoint_step_{step}.bin"
            if brain.save(checkpoint_path):
                checkpoint_files.append(checkpoint_path)
                print(f"Checkpoint saved: {checkpoint_path}")
        
        # Log detailed metrics
        if step % 100 == 0:
            metrics = brain.getBrainMetrics() if hasattr(brain, 'getBrainMetrics') else {}
            print(f"Step {step}: Firing rate {brain.getAverageFiringRate():.3f}, " +
                  f"Spikes {brain.getTotalSpikeCount()}, " +
                  f"Memory traces {metrics.get('working_memory_traces', 'N/A')}")
    
    # Load latest checkpoint if available
    if checkpoint_files:
        latest_checkpoint = max(checkpoint_files, key=os.path.getctime)
        print(f"Loading latest checkpoint: {latest_checkpoint}")
        brain.load(latest_checkpoint)
    
    return checkpoint_files

def run_multiple_episodes(config, world, agent, num_episodes, steps_per_episode):
    """Run multiple independent simulation episodes with different random seeds"""
    
    results = []
    
    for episode in range(num_episodes):
        print(f"\n=== Episode {episode + 1}/{num_episodes} ===")
        
        # Create new brain for each episode with different seed
        episode_config = pynlm.createDefaultConfig()
        episode_config.set("random_seed", 42 + episode)
        
        # Copy other config parameters from original config
        for key in config.getKeys():
            if key != "random_seed":
                value = config.get(key)
                if value:
                    episode_config.set(key, value)
        
        # Run episode
        brain = pynlm.createBrain(episode_config)
        brain.initialize()
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Enable features for this episode
        agent_brain.enableRewardModulation(True)
        agent_brain.enableCuriosity(True)
        
        for step in range(steps_per_episode):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            brain.step(step)
            action = agent_brain.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 200 == 0:
                print(f"  Step {step}")
        
        # Collect episode results
        episode_result = {
            'episode': episode + 1,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': agent_brain.getDevelopmentalStage(),
            'final_curiosity': agent_brain.getCuriosityLevel(),
            'final_novelty': agent_brain.getNoveltyLevel()
        }
        
        results.append(episode_result)
        print(f"Episode {episode + 1} completed: {episode_result}")
        
        # Reset world for next episode
        world.reset()
    
    # Print summary statistics
    print("\n=== Experiment Summary ===")
    for result in results:
        print(f"Episode {result['episode']}: Firing rate {result['final_firing_rate']:.3f}, " +
              f"Curiosity {result['final_curiosity']:.3f}")
    
    return results
```

### 4. Custom Extension Example

```python
import pynlm

def create_custom_extension():
    """Example of creating custom extensions for NLM"""
    
    # Custom brain subclass (requires C++ implementation)
    class CustomBrain(pynlm.Brain):
        def __init__(self, config):
            super().__init__(config)
            
        def custom_plasticity_rule(self, synapse, pre_neuron_activity, post_neuron_activity):
            """Custom plasticity rule implementation"""
            # This would require C++ extension to work properly
            weight = synapse.getWeight()
            # Custom plasticity calculation
            weight += (pre_neuron_activity * post_neuron_activity) * 0.001
            synapse.setWeight(weight)
            return weight
    
    # Custom configuration loader
    class CustomConfigLoader:
        @staticmethod
        def load_from_yaml(filepath):
            """Load configuration from YAML file"""
            try:
                import yaml
                with open(filepath, 'r') as f:
                    data = yaml.safe_load(f)
                
                # Convert YAML to Config
                config = pynlm.createDefaultConfig()
                
                for key, value in data.items():
                    # Convert value to appropriate type
                    if isinstance(value, bool):
                        config.set(key, value)
                    elif isinstance(value, int):
                        config.set(key, value)
                    elif isinstance(value, float):
                        config.set(key, value)
                    elif isinstance(value, str):
                        config.set(key, value)
                    elif isinstance(value, list):
                        # For now, convert lists to strings
                        config.set(key, str(value))
                
                return config
                
            except ImportError:
                print("PyYAML not installed. Install with: pip install pyyaml")
                return None
            except Exception as e:
                print(f"Error loading YAML config: {e}")
                return None
    
    return CustomBrain, CustomConfigLoader

# Usage example
CustomBrain, CustomConfigLoader = create_custom_extension()
```

---

## Performance and Optimization Tips

### 1. Performance Monitoring

```python
import pynlm

def profile_simulation(config, world, agent, steps):
    """Profile simulation performance and resource usage"""
    
    brain = agent.getBrain()
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Calculate performance metrics
    steps_per_second = steps / elapsed if elapsed > 0 else 0
    time_per_step = elapsed / steps if steps > 0 else 0
    
    print(f"\n=== Performance Profile ===")
    print(f"Total time: {elapsed:.2f} seconds")
    print(f"Steps per second: {steps_per_second:.2f}")
    print(f"Time per step: {time_per_step*1000:.2f} ms")
    
    # Get brain metrics if available
    if hasattr(brain, 'getBrainMetrics'):
        metrics = brain.getBrainMetrics()
        print(f"Peak memory traces: {metrics.get('peak_memory_traces', 'N/A')}")
    
    return {
        'total_time': elapsed,
        'steps_per_second': steps_per_second,
        'time_per_step_ms': time_per_step * 1000,
        'steps_completed': steps
    }
```

### 2. Memory Management

```python
import pynlm

def memory_optimized_simulation(config, world, agent, max_steps):
    """Memory-efficient simulation with cleanup"""
    
    brain = agent.getBrain()
    
    for step in range(max_steps):
        # Clear old sensory inputs periodically
        if step % 100 == 0:
            # Clear agent memory
            agent.reset()
            
            # Force garbage collection if needed
            import gc
            gc.collect()
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Monitor memory usage (basic example)
        if step % 500 == 0:
            # In a real implementation, you might use memory_profiler
            # or tracemalloc to track memory usage
            print(f"Step {step} completed")
    
    return brain
```

### 3. Parallel Processing (for future implementation)

```python
import pynlm
import multiprocessing as mp

def parallel_simulation_experiment(config, world, agent, steps_per_process):
    """Example of how parallel simulation could be implemented"""
    
    # This would require extending the C++ backend
    # to support parallel processing
    
    print("Parallel simulation requires C++ backend extensions")
    print("Current implementation runs simulations sequentially")
    
    # Run sequentially for now
    brain = agent.getBrain()
    for step in range(steps_per_process):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain
```

---

## API Reference

### Configuration Classes

- `Config`: Configuration management with JSON/YAML support
- `createDefaultConfig()`: Create basic configuration
- `createAdvancedConfig()`: Create research-ready configuration
- `createResearchConfig(neuronCount, maxSteps)`: Create specialized research configuration

### Agent Classes

- `AgentBrain`: Brain-world interface
- `enableRewardModulation(bool)`: Enable reward-based learning
- `enableStructuralPlasticity(bool)`: Enable structural plasticity
- `enableDevelopment(bool)`: Enable developmental processes
- `enableCuriosity(bool)`: Enable curiosity-driven exploration

### Simulation Control

- `runBrainSimulation(brain, world, agent, maxSteps)`: Run complete simulation
- `getBrainMetrics(brain)`: Get comprehensive brain metrics
- `validateConfig(config)`: Validate configuration
- `getConfigSuggestions(type)`: Get configuration suggestions

### Monitoring and Profiling

- `profile_simulation()`: Profile simulation performance
- `memory_optimized_simulation()`: Run memory-efficient simulations
- `advanced_simulation_control()`: Advanced simulation control with checkpoints

---

## Troubleshooting and Common Issues

### Common Configuration Issues

1. **Invalid configuration values**: Always validate configuration parameters before using them
2. **Missing configuration keys**: Use `config.has(key)` to check for required keys
3. **Type mismatches**: Ensure configuration values match expected types

### Common Simulation Issues

1. **Brain not firing**: Check that neurons have received sensory input
2. **Agent not learning**: Verify that reward modulation and plasticity are enabled
3. **World simulation stuck**: Check that world updates are being called

### Performance Issues

1. **Slow simulation**: Reduce neuron count or increase simulation timestep
2. **High memory usage**: Monitor memory usage and clear unused data
3. **Checkpoint failures**: Ensure checkpoint directory exists and is writable

---

## Performance Monitoring and Optimization

This section covers performance monitoring and optimization tools for advanced users.

### 1. Performance Profiling

```python
import pynlm
import time

def profile_simulation(config, world, agent, steps):
    """Profile simulation performance and measure execution time"""
    
    brain = agent.getBrain()
    
    print(f"Starting performance profile for {steps} steps...")
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"  Step {step} completed")
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Calculate performance metrics
    steps_per_second = steps / elapsed if elapsed > 0 else 0
    time_per_step_ms = (elapsed / steps) * 1000 if steps > 0 else 0
    
    print(f"\n=== Performance Profile ===")
    print(f"Total time: {elapsed:.2f} seconds")
    print(f"Steps per second: {steps_per_second:.2f}")
    print(f"Time per step: {time_per_step_ms:.2f} ms")
    
    # Get brain metrics
    if hasattr(brain, 'getBrainMetrics'):
        metrics = brain.getBrainMetrics()
        print(f"Peak memory traces: {metrics.get('working_memory_traces', 'N/A')}")
        print(f"Total spikes: {metrics.get('total_spikes', 'N/A')}")
    
    return {
        'total_time': elapsed,
        'steps_per_second': steps_per_second,
        'time_per_step_ms': time_per_step_ms,
        'steps_completed': steps,
        'average_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount()
    }
```

### 2. Benchmarking

```python
import pynlm
import time

def benchmark_brain(brain, steps, warmup_steps=100):
    """Benchmark brain performance with warmup and steady-state measurements"""
    
    print(f"Starting benchmark: {steps} steps with {warmup_steps} warmup steps")
    
    # Warmup phase
    print("Running warmup phase...")
    warmup_start = time.time()
    for step in range(warmup_steps):
        brain.step(step, step * 0.001)
    warmup_time = time.time() - warmup_start
    
    # Benchmark phase
    print("Running benchmark phase...")
    bench_start = time.time()
    for step in range(warmup_steps, warmup_steps + steps):
        brain.step(step, step * 0.001)
    bench_time = time.time() - bench_start
    
    total_time = warmup_time + bench_time
    
    print(f"\n=== Benchmark Results ===")
    print(f"Warmup time: {warmup_time:.2f} seconds")
    print(f"Benchmark time: {bench_time:.2f} seconds")
    print(f"Total time: {total_time:.2f} seconds")
    
    # Calculate metrics
    steps_per_second = steps / bench_time if bench_time > 0 else 0
    time_per_step_ms = (bench_time / steps) * 1000 if steps > 0 else 0
    warmup_steps_per_second = warmup_steps / warmup_time if warmup_time > 0 else 0
    
    print(f"Warmup: {warmup_steps_per_second:.2f} steps/second")
    print(f"Benchmark: {steps_per_second:.2f} steps/second")
    print(f"Time per step: {time_per_step_ms:.2f} ms")
    
    return {
        'warmup_time': warmup_time,
        'benchmark_time': bench_time,
        'total_time': total_time,
        'warmup_steps_per_second': warmup_steps_per_second,
        'benchmark_steps_per_second': steps_per_second,
        'time_per_step_ms': time_per_step_ms,
        'steps_benchmarked': steps,
        'warmup_steps': warmup_steps
    }
```

### 3. Memory Profiling

```python
import pynlm
import gc

def memory_profile(brain, monitor_interval=100):
    """Basic memory profiling for NLM brain"""
    
    print("Starting memory profile...")
    
    # Force garbage collection
    gc.collect()
    
    # Get basic brain metrics
    if hasattr(brain, 'getBrainMetrics'):
        metrics = brain.getBrainMetrics()
    else:
        metrics = {}
    
    # Estimate memory usage
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    
    # Rough memory estimates (in bytes)
    neuron_memory = neuron_count * 50  # Approximate memory per neuron
    synapse_memory = synapse_count * 8  # Approximate memory per synapse
    overhead_memory = 1024 * 1024  # 1MB overhead for Python objects
    
    total_memory_mb = (neuron_memory + synapse_memory + overhead_memory) / (1024 * 1024)
    
    print(f"\n=== Memory Profile ===")
    print(f"Total neurons: {neuron_count}")
    print(f"Total synapses: {synapse_count}")
    print(f"Estimated neuron memory: {neuron_memory / 1024:.2f} KB")
    print(f"Estimated synapse memory: {synapse_memory / 1024:.2f} KB")
    print(f"Estimated total memory: {total_memory_mb:.2f} MB")
    
    if hasattr(metrics, 'working_memory_traces'):
        print(f"Working memory traces: {metrics.working_memory_traces}")
    
    if hasattr(metrics, 'episodic_memory_episodes'):
        print(f"Episodic memory episodes: {metrics.episodic_memory_episodes}")
    
    # Memory warnings
    if total_memory_mb > 100:
        print(f"⚠️  WARNING: High memory usage ({total_memory_mb:.2f} MB)")
    
    if neuron_count > 5000:
        print("⚠️  WARNING: Large brain system - may require significant memory")
    
    return {
        'total_memory_mb': total_memory_mb,
        'neuron_count': neuron_count,
        'synapse_count': synapse_count,
        'working_memory_traces': getattr(metrics, 'working_memory_traces', None),
        'episodic_memory_episodes': getattr(metrics, 'episodic_memory_episodes', None),
        'memory_warnings': []
    }
```

### 4. Advanced Performance Monitoring

```python
import pynlm
import time
import threading
import queue

def monitor_performance(brain, sample_interval=100):
    """Real-time performance monitoring (requires C++ extensions)"""
    
    print(f"Starting performance monitoring with {sample_interval} step intervals")
    
    # This would require C++ backend extensions for real monitoring
    # For now, provide basic metrics and warnings
    
    if hasattr(brain, 'getBrainMetrics'):
        metrics = brain.getBrainMetrics()
    else:
        metrics = {}
    
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    
    print(f"\n=== Performance Monitoring ===")
    print(f"Monitored neurons: {neuron_count}")
    print(f"Monitored synapses: {synapse_count}")
    
    # Performance recommendations
    recommendations = []
    
    if neuron_count > 3000:
        recommendations.append("Consider reducing neuron count for better performance")
    
    if brain.getAverageFiringRate() > 10.0:
        recommendations.append("High firing rate detected - may indicate overactive neurons")
    
    if synapse_count > neuron_count * 5:
        recommendations.append("High synapse-to-neuron ratio - may impact performance")
    
    if recommendations:
        print("Performance recommendations:")
        for rec in recommendations:
            print(f"  • {rec}")
    
    return {
        'monitored': True,
        'sample_interval': sample_interval,
        'recommendations': recommendations,
        'brain_size_estimated': neuron_count + synapse_count,
        'note': "Real monitoring requires C++ backend extensions"
    }
```

### 5. Optimization Suggestions

```python
import pynlm

def get_optimization_suggestions(brain, world):
    """Get optimization suggestions for improving simulation performance"""
    
    suggestions = []
    
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    firing_rate = brain.getAverageFiringRate()
    
    # Neuron count optimization
    if neuron_count > 2000:
        suggestions.append({
            'category': 'neuron_count',
            'suggestion': 'Reduce neuron count to improve performance',
            'impact': 'high',
            'difficulty': 'easy'
        })
    
    # Synapse optimization
    if synapse_count > neuron_count * 10:
        suggestions.append({
            'category': 'connectivity',
            'suggestion': 'Reduce connection probability to decrease synapse count',
            'impact': 'medium',
            'difficulty': 'easy'
        })
    
    # Firing rate optimization
    if firing_rate > 15.0:
        suggestions.append({
            'category': 'neural_activity',
            'suggestion': 'Check for runaway excitation - may indicate parameter issues',
            'impact': 'high',
            'difficulty': 'hard'
        })
    
    # Memory optimization
    if neuron_count > 1000 and synapse_count > neuron_count * 3:
        suggestions.append({
            'category': 'memory',
            'suggestion': 'Implement synaptic compression or sparse connectivity',
            'impact': 'medium',
            'difficulty': 'medium'
        })
    
    # World optimization
    if world and hasattr(world, 'getWidth') and world.getWidth() > 100:
        suggestions.append({
            'category': 'world_size',
            'suggestion': 'Reduce world dimensions to improve spatial performance',
            'impact': 'low',
            'difficulty': 'easy'
        })
    
    print(f"\n=== Optimization Suggestions ({len(suggestions)}) ===")
    
    for i, suggestion in enumerate(suggestions, 1):
        impact_emoji = "🔥" if suggestion['impact'] == 'high' else "📊" if suggestion['impact'] == 'medium' else "💡"
        difficulty_emoji = "🔴" if suggestion['difficulty'] == 'hard' else "🟡" if suggestion['difficulty'] == 'medium' else "🟢"
        
        print(f"{i}. {impact_emoji} {suggestion['category'].replace('_', ' ').title()} ({suggestion['impact']} impact, {suggestion['difficulty']} difficulty)")
        print(f"   💡 {suggestion['suggestion']}")
        print()
    
    return suggestions
```

### 6. Complete Performance Analysis

```python
import pynlm
import time

def complete_performance_analysis(config, world, agent, steps):
    """Complete performance analysis including profiling, benchmarking, and optimization"""
    
    brain = agent.getBrain()
    
    print("=== Complete Performance Analysis ===")
    print(f"Brain configuration: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # 1. Memory profile
    print("\n1. Memory Profile:")
    memory_profile = memory_profile(brain)
    
    # 2. Performance profile
    print("\n2. Performance Profile:")
    performance_profile = profile_simulation(config, world, agent, steps)
    
    # 3. Optimization suggestions
    print("\n3. Optimization Suggestions:")
    optimizations = get_optimization_suggestions(brain, world)
    
    # 4. Benchmarking
    print("\n4. Benchmarking:")
    benchmark_results = benchmark_brain(brain, min(1000, steps), 100)
    
    # Summary
    print(f"\n=== Performance Analysis Summary ===")
    print(f"Total memory usage: {memory_profile['total_memory_mb']:.2f} MB")
    print(f"Performance: {performance_profile['steps_per_second']:.2f} steps/second")
    print(f"Benchmark: {benchmark_results['benchmark_steps_per_second']:.2f} steps/second")
    
    optimization_count = len(optimizations)
    print(f"Optimization recommendations: {optimization_count}")
    
    if optimization_count > 0:
        print(f"🚨 Action needed: Review optimization suggestions")
    else:
        print(f"✅ Performance looks good!")
    
    return {
        'memory_profile': memory_profile,
        'performance_profile': performance_profile,
        'benchmark_results': benchmark_results,
        'optimizations': optimizations,
        'summary': {
            'memory_mb': memory_profile['total_memory_mb'],
            'performance_pps': performance_profile['steps_per_second'],
            'benchmark_pps': benchmark_results['benchmark_steps_per_second'],
            'optimization_count': optimization_count
        }
    }
```

---

## Performance Monitoring API

### Performance Monitoring Functions

- `profile_simulation(brain, world, agent, steps)`: Profile simulation performance and return timing metrics
- `benchmark_brain(brain, steps, warmup_steps)`: Benchmark brain performance with warmup and steady-state measurements
- `memory_profile(brain)`: Get basic memory profile (requires C++ extensions for detailed analysis)
- `monitor_performance(brain, sample_interval)`: Start performance monitoring (requires C++ extensions)
- `get_optimization_suggestions(brain, world)`: Get optimization suggestions for improving simulation performance

### Performance Metrics

The performance monitoring system provides:

1. **Timing Metrics**: Steps per second, time per step
2. **Memory Usage**: Estimated memory consumption
3. **Optimization Suggestions**: Actionable recommendations for improvement
4. **Benchmarking**: Accurate performance measurements with warmup
5. **Real-time Monitoring**: Ongoing performance tracking (requires C++ extensions)

---

## Further Optimization Techniques

### 1. Neural Network Optimization

- **Synaptic Compression**: Use sparse connectivity for large networks
- **Weight Quantization**: Reduce precision for weights to save memory
- **Pruning**: Remove redundant connections for better performance
- **Distributed Computing**: Parallelize across multiple processors

### 2. Memory Optimization

- **Object Pooling**: Reuse neural and synaptic objects
- **Lazy Evaluation**: Defer computation until needed
- **Memory Mapping**: Use disk-based storage for large brains
- **Garbage Collection**: Optimize cleanup of unused objects

### 3. Simulation Optimization

- **Adaptive Timesteps**: Use variable time steps based on neural activity
- **Event-Driven Updates**: Only process active neurons and synapses
- **Batching**: Group related operations together
- **Caching**: Store frequently accessed data

### 4. Hardware Optimization

- **SIMD Instructions**: Use vectorized operations
- **GPU Acceleration**: Offload computation to graphics processors
- **NUMA Optimization**: Align memory with processor architecture
- **Networked Computing**: Distribute across multiple machines

---

## Troubleshooting Performance Issues

### Common Performance Problems

1. **Slow Simulation**: Increase timestep, reduce neuron count, or use sparse connectivity
2. **High Memory Usage**: Monitor memory usage and implement optimization strategies
3. **Checkpoint Performance**: Ensure checkpoint directories are writable and have space
4. **World Size Impact**: Large worlds can significantly impact performance

### Performance Testing

```python
# Simple performance test
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Testing performance with 1000 steps...")
start = time.time()
for step in range(1000):
    brain.step(step, step * 0.001)
end = time.time()

print(f"Completed 1000 steps in {end-start:.2f} seconds")
print(f"Performance: {1000/(end-start):.2f} steps/second")
```

---

## Future Performance Improvements

The NLM framework continues to evolve with:

1. **C++ Extensions**: Enhanced performance monitoring and optimization features
2. **GPU Support**: Graphics processing unit acceleration
3. **Distributed Computing**: Network-based parallel processing
4. **Advanced Algorithms**: Improved neural computation methods
5. **Real-time Optimization**: Dynamic performance tuning during simulation

Performance monitoring will be expanded through C++ backend extensions to provide:
- Real-time resource monitoring
- Advanced profiling tools
- Automated optimization suggestions
- Distributed computing support
- GPU acceleration capabilities

---

## Next Steps for Performance Optimization

For advanced performance optimization:

1. **Study the C++ backend**: Understand performance bottlenecks in the core implementation
2. **Implement extensions**: Add C++ performance monitoring features
3. **Hardware profiling**: Use tools like Intel VTune or AMD PROFILER
4. **Network analysis**: Identify communication bottlenecks in distributed systems
5. **Automated optimization**: Implement automatic performance tuning algorithms

Performance optimization is an ongoing process that requires understanding both the algorithmic complexity and hardware-specific optimizations.
