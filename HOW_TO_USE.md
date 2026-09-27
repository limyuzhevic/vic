# NLM (Neural Learning Machine) - Enhanced HOW TO USE Guide

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This guide covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API (enhanced with new features)
4. Advanced usage patterns and best practices
5. Troubleshooting and common issues

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
| `pynlm` | Python Module | Python bindings |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with Python bindings disabled
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3 -DBUILD_PYTHON_BINDINGS=OFF
```

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__); print(pynlm.__description__)"
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

# Install with all optional dependencies
pip install .[dev]
```

## Part 3: Using the Enhanced NLM Python API

### Basic Usage

#### Creating a Brain with Configuration

```python
import pynlm

# Method 1: Using default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Method 2: Using configuration from dictionary
config_dict = {
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.1,
    "brain.connection_probability": 0.05,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.0
}
config = pynlm.createConfigFromDict(config_dict)
brain = pynlm.createBrain(config)
brain.initialize()

# Method 3: Using preset configurations
config = pynlm.createDefaultConfigWithPreset("medium")
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Configuration validated: {pynlm.validateConfig(config)}")
```

#### Running a Simple Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfigWithPreset("small")
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    
    # Log statistics periodically
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Simulation complete!")
```

#### Complete Agent Example with Enhanced Features

```python
import pynlm

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with all enhanced features."""
    
    # 1. Create configuration with presets
    config = pynlm.createDefaultConfigWithPreset("medium")
    
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
    
    # 6. Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop with enhanced logging
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
        
        # Apply reward modulation based on internal signals
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development system
        agent.updateDevelopment(0.1)
        
        # Enhanced logging with new API functions
        if step % 100 == 0:
            print(f"\n=== Step {step} ===")
            print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"Prediction error: {agent.getPredictionError():.3f}")
            print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"Development stage: {brain.getDevelopmentalStage()}")
            print(f"E/I balance: {brain.getExcitationInhibitionRatio():.3f}")
            
            # Get current configuration as dictionary
            config_dict = pynlm.exportConfigToDict(config)
            print(f"Key configuration: {config_dict.get('brain.neuron_count', 'N/A')} neurons")
    
    print("\n=== Simulation Complete ===")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Total developmental time: {brain.getDevelopmentalStage()}")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'config': config,
        'total_steps': num_steps
    }

# Run the enhanced simulation
result = run_enhanced_agent_simulation(500)
```

#### Configuration Management with Enhanced API

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Get available configuration keys
all_keys = pynlm.list_available_configs()
print("Available configuration keys:", all_keys)

# Set various configuration values
config.set("brain.neuron_count", 8000)
config.set("brain.synapse_density", 0.12f)
config.set("brain.connection_probability", 0.06f)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Save configuration to file
config.saveToFile("my_config.json")
print("Configuration saved to my_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
if loaded_config.loadFromFile("my_config.json"):
    print("Configuration loaded successfully")
    print(f"Loaded neuron count: {loaded_config.getOr<long>('brain.neuron_count', 0)}")

# Validate configuration
if pynlm.validateConfig(loaded_config):
    print("Configuration is valid")
else:
    print("Configuration has issues")

# Get configuration as dictionary
config_dict = pynlm.exportConfigToDict(loaded_config)
print("Configuration as dictionary:", config_dict)

# Create new config from dictionary
new_config = pynlm.createConfigFromDict(config_dict)
print("New configuration created from dictionary")
```

#### Utility Functions for Advanced Usage

```python
import pynlm

# Get module information
version, author, license, description = pynlm.get_version()
print(f"NLM Python Bindings v{version}")
print(f"Author: {author}")
print(f"License: {license}")
print(f"Description: {description}")

# Estimate simulation time and resource requirements
config = pynlm.createDefaultConfigWithPreset("large")
estimation = pynlm.estimate_simulation_time(config, durationSeconds=10.0)
neurons, synapses, est_seconds, total_seconds = estimation

print(f"Simulation estimates:")
print(f"  Neurons: {neurons:,}")
print(f"  Synapses: {synapses:,}")
print(f"  Estimated runtime for 1 step: {est_seconds:.2f}s")
print(f"  Total estimated runtime for 10s simulation: {total_seconds:.2f}s")

# Create configuration presets
config_small = pynlm.createDefaultConfigWithPreset("small")
config_medium = pynlm.createDefaultConfigWithPreset("medium")
config_large = pynlm.createDefaultConfigWithPreset("large")
config_debug = pynlm.createDefaultConfigWithPreset("debug")

print("Presets available: small, medium, large, debug")
```

## Part 4: Advanced Usage Patterns

### 1. Batch Processing Multiple Simulations

```python
import pynlm

def run_batch_simulation(config, num_simulations, steps_per_simulation):
    """Run multiple simulations with the same configuration."""
    results = []
    
    for sim in range(num_simulations):
        print(f"Running simulation {sim + 1}/{num_simulations}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps_per_simulation):
            brain.step(step)
        
        stats = {
            'simulation_id': sim,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'final_stage': brain.getDevelopmentalStage()
        }
        results.append(stats)
    
    return results

# Run batch simulation
config = pynlm.createDefaultConfigWithPreset("small")
batch_results = run_batch_simulation(config, num_simulations=5, steps_per_simulation=100)

# Analyze results
for result in batch_results:
    print(f"Simulation {result['simulation_id']}: {result['total_spikes']} spikes, "
          f"firing rate {result['firing_rate']:.3f}")
```

### 2. Error Handling and Robustness

```python
import pynlm

def safe_simulation_run(config, max_steps=1000):
    """Run simulation with robust error handling."""
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(max_steps):
            try:
                brain.step(step)
                
                # Check for stability issues
                firing_rate = brain.getAverageFiringRate()
                if firing_rate > 100.0:  # Too high firing rate
                    print(f"Warning: High firing rate at step {step}: {firing_rate:.3f}")
                elif firing_rate < 0.01:  # Very low firing rate
                    print(f"Warning: Low firing rate at step {step}: {firing_rate:.3f}")
                    
            except pynlm.NLMError as e:
                print(f"Simulation error at step {step}: {e}")
                break
            except Exception as e:
                print(f"Unexpected error at step {step}: {e}")
                continue
        
        return brain
        
    except pynlm.NLMError as e:
        print(f"Failed to initialize brain: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

# Use with error handling
config = pynlm.createDefaultConfigWithPreset("debug")
brain = safe_simulation_run(config, max_steps=500)
if brain:
    print(f"Simulation completed successfully with {brain.getTotalSpikeCount()} spikes")
```

### 3. Configuration Comparison and Analysis

```python
import pynlm

def compare_configurations(config1, config2, name1="Config 1", name2="Config 2"):
    """Compare two configurations and show differences."""
    keys1 = config1.getKeys()
    keys2 = config2.getKeys()
    
    all_keys = set(keys1) | set(keys2)
    
    print(f"Comparing {name1} vs {name2}:")
    print("-" * 60)
    
    for key in sorted(all_keys):
        val1 = config1.getOr<std::string>(key, "(not set)")
        val2 = config2.getOr<std::string>(key, "(not set)")
        
        if (val1 == "(not set)" and val2 == "(not set)") or val1 == val2:
            status = "="
        else:
            status = "≠"
        
        print(f"{status} {key}")
        print(f"  {name1}: {val1}")
        print(f"  {name2}: {val2}")
        print()

# Compare different presets
config_small = pynlm.createDefaultConfigWithPreset("small")
config_large = pynlm.createDefaultConfigWithPreset("large")
config_debug = pynlm.createDefaultConfigWithPreset("debug")

compare_configurations(config_small, config_large, "Small Preset", "Large Preset")
compare_configurations(config_debug, config_medium, "Debug Preset", "Medium Preset")
```

## Part 5: Troubleshooting

### Common Issues and Solutions

#### 1. "My brain isn't doing anything"

**Symptoms**: No spikes, low activity, silent network

**Solutions**:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Solution 1: Check if initialization worked
if brain.getTotalNeuronCount() == 0:
    print("ERROR: No neurons created!")
    print("Check configuration: brain.neuron_count")
else:
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")

# Solution 2: Try with a preset that has neurons
config = pynlm.createDefaultConfigWithPreset("medium")
brain = pynlm.createBrain(config)
brain.initialize()

# Solution 3: Add sensory input to stimulate neurons
# (Requires world integration)
```

#### 2. "The agent isn't moving"

**Symptoms**: Agent stays in place, no actions taken

**Solutions**:

```python
import pynlm

# Create complete agent setup
config = pynlm.createDefaultConfigWithPreset("debug")
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)

# Check if agent can see
percept = world.getSensoryPercept()
vision = percept.getVision()
if vision and len(vision) > 0:
    print(f"Agent sees vision data with {len(vision)} pixels")
    
    # Solution: Ensure motor commands are being generated
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        print(f"Step {step}: Action = {action}")
        world.applyMotorCommand(action, world.getSimulationTime())
else:
    print("Agent has no vision data")
```

#### 3. "Everything is 0"

**Symptoms**: All values are zero, no activity

**Solutions**:

```python
import pynlm

# Solution 1: Ensure proper initialization and reset
config = pynlm.createDefaultConfigWithPreset("medium")
brain = pynlm.createBrain(config)

# Always call reset before using brain
brain.reset()
brain.initialize()

# Solution 2: Allow time for "warmup"
print("Warming up brain...")
for step in range(50):
    brain.step(step)

# Check if neurons are firing
if brain.getTotalSpikeCount() == 0:
    print("Brain still not firing after warmup")
    print("Try increasing neuron count or adding sensory input")
else:
    print(f"Brain is firing: {brain.getTotalSpikeCount()} spikes after warmup")

# Solution 3: Check configuration
config_dict = pynlm.exportConfigToDict(config)
print("Current configuration:", config_dict)
```

#### 4. Build and Installation Issues

**Common Solutions**:

```bash
# Issue: CMake cannot find Python
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)

# Issue: ImportError: No module named 'pynlm'
pip install --force-reinstall .

# Issue: Segmentation faults
# 1. Ensure initialize() is called before step()
# 2. Call reset() before re-running simulation
# 3. Check for invalid configuration values

# Issue: Compilation errors
# 1. Ensure C++20 compiler support
# 2. Check CMake version >= 3.16
# 3. Verify all source files are present
```

## Part 6: Advanced Examples

### Example 1: Curiosity-Driven Exploration

```python
import pynlm
import random

def curiosity_driven_exploration(world_size=20, num_steps=1000):
    """Create an agent that explores based on curiosity."""
    
    # Create curious agent
    config = pynlm.createDefaultConfigWithPreset("small")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Random starting position
    world.setAgentStart(random.uniform(2, world_size-2), random.uniform(2, world_size-2))
    agent.initialize(world)
    
    print(f"Curiosity-driven exploration in {world_size}x{world_size} world")
    print(f"Starting position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
    
    exploration_positions = [(world.getAgentBody().x, world.getAgentBody().y)]
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log exploration behavior
        if step % 100 == 0:
            body = world.getAgentBody()
            exploration_positions.append((body.x, body.y))
            print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f}), "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Calculate exploration metrics
    unique_positions = len(set(exploration_positions))
    coverage = unique_positions / (world_size * world_size)
    
    print(f"\nExploration completed!")
    print(f"Unique positions visited: {unique_positions}")
    print(f"Coverage: {coverage:.1%}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world, exploration_positions

# Run exploration experiment
brain, agent, world, positions = curiosity_driven_exploration(30, 1000)
```

### Example 2: Reward-Based Learning

```python
import pynlm

def reward_based_learning(num_episodes=50, max_steps=200):
    """Train an agent using reward signals."""
    
    # Create agent with reward modulation
    config = pynlm.createDefaultConfigWithPreset("medium")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    rewards_per_episode = []
    
    for episode in range(num_episodes):
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        episode_reward = 0.0
        
        for step in range(max_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward from world internal signals
            internal = percept.getInternal()
            if internal and len(internal) > 0:
                reward = internal[0]
                episode_reward += reward
                agent.applyRewardModulation(reward, 0.0)
        
        rewards_per_episode.append(episode_reward)
        
        if episode % 10 == 0:
            avg_reward = sum(rewards_per_episode[-10:]) / 10
            print(f"Episode {episode}: Average reward = {avg_reward:.3f}")
    
    # Analyze learning curve
    import statistics
    mean_reward = statistics.mean(rewards_per_episode)
    std_reward = statistics.stdev(rewards_per_episode) if len(rewards_per_episode) > 1 else 0
    
    print(f"\nReward-based learning completed!")
    print(f"Episodes: {num_episodes}")
    print(f"Mean reward: {mean_reward:.3f} (±{std_reward:.3f})")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent, world, rewards_per_episode

# Run reward learning experiment
brain, agent, world, rewards = reward_based_learning(100, 300)
```

### Example 3: Development and Aging

```python
import pynlm

def development_and_aging_simulation(duration_steps=1000):
    """Simulate brain development and aging over time."""
    
    config = pynlm.createDefaultConfigWithPreset("debug")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableDevelopment(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    agent.initialize(world)
    
    print("Development and Aging Simulation")
    print("=" * 40)
    
    development_stages = set()
    performance_metrics = []
    
    for step in range(duration_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.updateDevelopment(0.1)
        
        # Record development stage
        stage = brain.getDevelopmentalStage()
        development_stages.add(stage)
        
        # Record performance metrics
        if step % 100 == 0:
            metrics = {
                'step': step,
                'stage': stage,
                'firing_rate': brain.getAverageFiringRate(),
                'spike_count': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError()
            }
            performance_metrics.append(metrics)
            
            print(f"Step {step}: Stage {stage}, "
                  f"Firing: {metrics['firing_rate']:.3f}, "
                  f"Curiosity: {metrics['curiosity']:.3f}")
    
    print(f"\nDevelopment stages observed: {sorted(development_stages)}")
    print(f"Total simulation steps: {duration_steps}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world, performance_metrics

# Run development simulation
brain, agent, world, metrics = development_and_aging_simulation(1000)
```

## Part 7: Quick Reference

### Core Classes and Methods

#### Brain Class

| Method | Description |
|--------|-------------|
| `brain = pynlm.createBrain(config)` | Create new brain |
| `brain.initialize()` | Initialize brain |
| `brain.step(step_number)` | Run one simulation step |
| `brain.reset()` | Reset brain state |
| `brain.getTotalNeuronCount()` | Get total neuron count |
| `brain.getTotalSynapseCount()` | Get total synapse count |
| `brain.getFiringNeuronCount()` | Get currently firing neurons |
| `brain.getAverageFiringRate()` | Get average firing rate |
| `brain.getExcitationInhibitionRatio()` | Get E/I balance |
| `brain.getTotalSpikeCount()` | Get total spike count |
| `brain.getDevelopmentalStage()` | Get developmental stage |
| `brain.setDevelopmentalStage(stage)` | Set developmental stage |
| `brain.save("checkpoint.bin")` | Save brain state |
| `brain.load("checkpoint.bin")` | Load brain state |

#### Config Class

| Method | Description |
|--------|-------------|
| `config = pynlm.createDefaultConfig()` | Create default config |
| `config = pynlm.createConfigFromDict(dict)` | Create config from dict |
| `config = pynlm.createDefaultConfigWithPreset(name)` | Create config with preset |
| `config.has(key)` | Check if key exists |
| `config.set(key, value)` | Set configuration value |
| `config.get(key)` | Get configuration value |
| `config.getOr(key, default)` | Get with default value |
| `config.loadFromFile(filepath)` | Load from JSON file |
| `config.saveToFile(filepath)` | Save to JSON file |
| `pynlm.validateConfig(config)` | Validate configuration |

#### AgentBrain Class

| Method | Description |
|--------|-------------|
| `agent = pynlm.createAgentBrain(brain)` | Create agent brain |
| `agent.initialize(world)` | Initialize with world |
| `agent.processSensoryInput(percept)` | Process sensory input |
| `agent.decodeMotorCommand()` | Decode motor command |
| `agent.applyRewardModulation(reward, predicted)` | Apply reward modulation |
| `agent.updateDevelopment(timestep)` | Update development system |
| `agent.getCuriosityLevel()` | Get curiosity level |
| `agent.getNoveltyLevel()` | Get novelty level |
| `agent.getPredictionError()` | Get prediction error |
| `agent.getNeuromodulationLevel()` | Get neuromodulation level |

#### SimpleWorld Class

| Method | Description |
|--------|-------------|
| `world = pynlm.createSimpleWorld()` | Create simple world |
| `world.configure(width, height, visionW, visionH)` | Configure world dimensions |
| `world.reset()` | Reset world state |
| `world.update(timestep)` | Update world by timestep |
| `world.setAgentStart(x, y)` | Set agent starting position |
| `world.applyMotorCommand(cmd, time)` | Apply motor command |
| `world.getSensoryPercept()` | Get sensory percept |
| `world.getAgentBody()` | Get agent body state |

### Enumerations

#### Neuron Types

- `pynlm.NeuronType.Excitatory`
- `pynlm.NeuronType.Inhibitory`
- `pynlm.NeuronType.Modulatory`
- `pynlm.NeuronType.Sensory`
- `pynlm.NeuronType.Motor`
- `pynlm.NeuronType.Internal`

#### Developmental Stages

- `pynlm.DevelopmentalStage.Initial`
- `pynlm.DevelopmentalStage.CriticalPeriod`
- `pynlm.DevelopmentalStage.Maturation`
- `pynlm.DevelopmentalStage.Adult`
- `pynlm.DevelopmentalStage.Aging`

#### Action Types

- `pynlm.ActionType.MoveForward`
- `pynlm.ActionType.MoveBackward`
- `pynlm.ActionType.MoveLeft`
- `pynlm.ActionType.MoveRight`
- `pynlm.ActionType.TurnLeft`
- `pynlm.ActionType.TurnRight`
- `pynlm.ActionType.Look`
- `pynlm.ActionType.LookUp`
- `pynlm.ActionType.LookDown`
- `pynlm.ActionType.Interact`
- `pynlm.ActionType.Eat`
- `pynlm.ActionType.Drink`
- `pynlm.ActionType.Rest`
- `pynlm.ActionType.Wait`
- `pynlm.ActionType.Custom`

### Error Types

- `pynlm.NLMError` - Base NLM exception
- `pynlm.RuntimeError` - Runtime error
- `pynlm.ValueError` - Invalid value
- `pynlm.IndexError` - Index out of range
- `pynlm.LogicError` - Logical error

### Common Configuration Keys

| Key | Type | Default | Description |
|----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## Conclusion

NLM (熙然) provides a comprehensive framework for building artificial neural systems with advanced features including:

- **Real spiking neurons** with LIF dynamics
- **Multiple plasticity mechanisms** (STDP, Hebbian, structural)
- **Memory systems** (working, episodic, associative)
- **Neuromodulation** (dopamine, curiosity, novelty)
- **Prediction systems** for temporal sequence learning
- **Cognitive mechanisms** (attention, planning, concept formation)
- **Developmental processes** for maturation and aging

The enhanced Python API provides powerful tools for rapid prototyping, experimentation, and research while maintaining the flexibility of the underlying C++ implementation.

### Next Steps

1. **Read `docs/ARCHITECTURE.md`** for detailed system architecture
2. **Read `docs/SCIENCE.md`** for scientific background
3. **Read `docs/EXPERIMENTS.md`** for experiment descriptions
4. **Experiment with different configuration presets** (`small`, `medium`, `large`, `debug`)
5. **Create your own experiments** using the examples provided
6. **Contribute to the project** by adding new features or improving existing ones

That's it! You're now ready to use NLM for advanced neural simulation and research.