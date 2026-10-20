# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library  
3. Using the enhanced NLM Python API

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

## Part 3: Using the Enhanced Python Library

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

### Enhanced Usage with Agent Brain

#### Complete Agent Example

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

### Enhanced API Reference

#### Brain Class (Enhanced)

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)     # Run step with timestamp
brain.reset()                      # Reset brain state

# Input/Output
brain.receiveSensoryInput(input)  # Inject sensory data
action = brain.produceAction()    # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()       # Total synapses
brain.getFiringNeuronCount()      # Currently firing
brain.getAverageFiringRate()       # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class (Enhanced)

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

# Pythonic convenience methods
config.set_int("neuron_count", 1000)
config.set_float("connection_probability", 0.1)
config.set_bool("enable_structural_plasticity", True)
config.get_int("neuron_count", 1000)      # With default
config.get_float("connection_probability", 0.1)  # With default
config.get_bool("enable_structural_plasticity", True)  # With default
```

#### AgentBrain Class (Enhanced)

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

# Advanced convenience
agent.enableAllFeatures()  # Enable all advanced features
```

#### SimpleWorld Class (Enhanced)

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

### Convenience Functions (Enhanced)

#### Enhanced Convenience Functions

| Function | Description |
|----------|-------------|
| `create_brain_with_config(profile)` | Create brain with preset configuration |
| `create_agent_setup(config_name, vision_width, vision_height)` | Create complete agent system |
| `run_simulation(brain, world, agent, steps, track_firing_rate)` | Run simulation with metrics |
| `get_system_info()` | Get system capabilities and information |
| `create_research_brain(neuron_count, region_count)` | Create research-ready brain |
| `create_test_world(width, height, vision_width, vision_height)` | Create test world |
| `create_test_agent(brain, world)` | Create research-ready agent |
| `save_experiment_config(config, filename)` | Save configuration to file |
| `load_experiment_config(filename)` | Load configuration from file |
| `compare_configurations(config1, config2)` | Compare two configurations |

#### Research Profiles

| Profile | Description |
|---------|-------------|
| `development` | Basic development for testing and exploration |
| `small_scale` | Small network for rapid prototyping |
| `medium_scale` | Medium network for detailed studies |
| `large_scale` | Large network for comprehensive research |
| `cognitive_development` | Focused on cognitive development processes |
| `neuromodulation_study` | Specialized for neuromodulation research |
| `memory_integration` | Focus on memory system integration |
| `learning_dynamics` | Study of learning dynamics and plasticity |
| `behavioral_modeling` | Behavioral modeling and action selection |

### Advanced Configuration Management

#### Using AdvancedConfigManager

```python
import pynlm
from pynlm import AdvancedConfigManager

# Create configuration manager
config_manager = AdvancedConfigManager()

# Apply research profile
config = pynlm.createDefaultConfig()
config_manager.applyProfile(config, config_manager.ResearchProfile.CognitiveDevelopment)

# Validate configuration
errors = []
if config_manager.validateConfiguration(config, errors):
    print("Configuration is valid")
else:
    for error in errors:
        print(f"Validation error: {error}")

# Save as research scene
config_manager.saveResearchScene(
    config,
    "my_research_scene.scene",
    "My Cognitive Development Study",
    "A study of cognitive development in artificial neural networks",
    "NLM Research Team"
)

# Load research scene
loaded_config = pynlm.createDefaultConfig()
config_manager.loadResearchScene(loaded_config, "my_research_scene.scene")

# Compare configurations
comparison = config_manager.compareConfigurations(config, loaded_config)
print("Configuration differences:")
print(comparison)

# Export to JSON
json_config = config_manager.exportToJSON(config)
print("Exported configuration:")
print(json_config)

# Import from JSON
new_config = pynlm.createDefaultConfig()
config_manager.importFromJSON(new_config, json_config)
```

### Complete Research Example

#### Research Experiment with Configuration Management

```python
import pynlm
from pynlm import AdvancedConfigManager
import datetime

# Create research manager
config_manager = AdvancedConfigManager()

# Create experiment identifier
experiment_id = f"nlm_exp_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}"

# Create and configure brain for cognitive development study
config = pynlm.createDefaultConfig()
config_manager.applyProfile(config, config_manager.ResearchProfile.CognitiveDevelopment)

# Save initial configuration
config_manager.saveResearchScene(
    config,
    f"{experiment_id}_initial.scene",
    "Initial Cognitive Development Configuration",
    "Baseline configuration for cognitive development experiment"
)

# Create agent system
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.create_test_world(32, 32, 16, 16)
agent = pynlm.create_test_agent(brain, world)

# Enable research features
agent.enableAllFeatures()

# Run experiment
print(f"=== {experiment_id} ===")
print(f"Running cognitive development experiment...")

result = pynlm.run_simulation(brain, world, agent, 2000)

# Save final configuration
config_manager.saveResearchScene(
    brain.getConfig(),
    f"{experiment_id}_final.scene",
    f"Final Configuration - {experiment_id}",
    "Configuration after experiment run"
)

# Create experiment summary
experiment_summary = {
    'experiment_id': experiment_id,
    'start_time': datetime.datetime.now().isoformat(),
    'end_time': datetime.datetime.now().isoformat(),
    'configuration_profile': 'cognitive_development',
    'results': {
        'total_reward': result.totalReward,
        'average_firing_rate': result.avgFiringRate,
        'final_spike_count': result.finalSpikeCount,
        'simulation_steps': result.totalSteps
    },
    'behavioral_metrics': {
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'final_prediction_error': agent.getPredictionError(),
        'developmental_stage': str(agent.getDevelopmentalStage())
    }
}

# Save experiment summary
with open(f"{experiment_id}_summary.json", 'w') as f:
    json.dump(experiment_summary, f, indent=2)

print(f"Experiment complete!")
print(f"Results saved to {experiment_id}_summary.json")
print(f"Configuration saved to {experiment_id}_final.scene")
print(f"Final metrics:")
print(f"  Total reward: {result.totalReward:.2f}")
print(f"  Average firing rate: {result.avgFiringRate:.2f}")
print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Developmental stage: {agent.getDevelopmentalStage()}")
```

### Research-Ready Examples

#### Example 1: Cognitive Development Study

```python
import pynlm

# Setup for cognitive development research
brain = pynlm.create_brain_with_config("cognitive_development")
world = pynlm.create_test_world(32, 32, 16, 16)
agent = pynlm.create_test_agent(brain, world)

# Enable development features
agent.enable_development(True)
agent.enable_curiosity(True)
agent.enable_planning(True)

# Run development study
print("=== Cognitive Development Study ===")
for step in range(1500):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step, step * 0.001)
    
    if step % 300 == 0:
        print(f"Step {step}: Stage={agent.get_developmental_stage()}, "
              f"Curiosity={agent.get_curiosity_level():.3f}, "
              f"Planning={agent.get_planning_enabled()}")

print(f"=== Development Study Complete ===")
print(f"Final developmental stage: {agent.get_developmental_stage()}")
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f}")
```

#### Example 2: Neuromodulation Learning

```python
import pynlm

# Neuromodulation learning experiment
brain = pynlm.create_brain_with_config("neuromodulation_study")
world = pynlm.create_test_world(20, 20, 10, 10)
agent = pynlm.create_test_agent(brain, world)

# Enable neuromodulation
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_novelty_detection(True)
agent.enable_prediction(True)

# Run learning with neuromodulation
print("=== Neuromodulation Learning Study ===")
for step in range(800):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step, step * 0.001)
    
    if step % 200 == 0:
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        reward = 0.0
        if world.get_sensory_percept().get_internal():
            reward = world.get_sensory_percept().get_internal()[0]
        agent.apply_reward_modulation(reward, 0.0)
        
        print(f"Step {step}: Reward={reward:.2f}, "
              f"Dopamine={agent.get_neuromodulation_level():.3f}, "
              f"PredictionError={agent.get_prediction_error():.3f}")

print(f"=== Neuromodulation Study Complete ===")
print(f"Final dopamine level: {agent.get_neuromodulation_level():.3f}")
print(f"Final prediction error: {agent.get_prediction_error():.3f}")
```

#### Example 3: Memory Integration

```python
import pynlm

# Memory integration study
brain = pynlm.create_brain_with_config("memory_integration")
world = pynlm.create_test_world(24, 24, 12, 12)
agent = pynlm.create_test_agent(brain, world)

# Enable all memory systems
agent.enable_working_memory(True)
agent.enable_episodic_memory(True)
agent.enable_semantic_memory(True)
agent.enable_development(True)

# Run memory integration experiment
print("=== Memory Integration Study ===")
result = pynlm.run_simulation(brain, world, agent, 1500)

print(f"Memory metrics:")
print(f"  Working memory traces: {brain.get_working_memory() and brain.get_working_memory().get_active_traces()}")
print(f"  Episodic memory episodes: {brain.get_episodic_memory() and brain.get_episodic_memory().get_episode_count()}")
print(f"  Semantic memory concepts: {brain.get_semantic_memory() and brain.get_semantic_memory().get_concept_count()}")
print(f"  Prediction error: {agent.get_prediction_error():.3f}")

print(f"\nSimulation results:")
print(f"  Total reward: {result.total_reward:.2f}")
print(f"  Average firing rate: {result.avgFiringRate:.2f}")
print(f"  Episodes stored: {result.memoryEpisodesStored}")
```

### Troubleshooting

#### Common Issues and Solutions

**"My simulation isn't converging"**
- Ensure you call `brain.initialize()` before `brain.step()`
- Call `brain.reset()` between experiments
- Check configuration parameters
- Reduce network size for debugging

**"Agent isn't responding to environment"**
- Ensure `agent.initialize(world)` is called
- Call `agent.decodeMotorCommand()` in simulation loop
- Ensure `world.update()` is called before getting percepts
- Enable agent features with `agent.enableAllFeatures()`

**"Memory systems not working"**
- Enable memory with `agent.enableWorkingMemory(True)`
- Call `agent.enableDevelopment(True)` (memory requires development)
- Allow sufficient simulation time for memory consolidation
- Check configuration memory parameters

**"Configuration not loading properly"**
- Check file format and syntax
- Use `config.getKeys()` to verify available parameters
- Ensure correct data types for parameters
- Call `config.clear()` before loading new configuration

### Performance Tips

#### Optimize Simulation Performance

1. **Use appropriate network sizes** for your research question
2. **Disable unused features** to reduce computational load
3. **Use research profiles** for optimized configurations
4. **Monitor performance** with timing and memory metrics
5. **Use checkpointing** for long-running experiments

#### Configuration Management Best Practices

1. **Use research profiles** for consistent experimental setups
2. **Document all parameters** used in experiments
3. **Save and load configurations** to avoid manual parameter tuning
4. **Compare configurations** to track changes
5. **Export configurations** to JSON for documentation

### Integration with External Tools

#### Visualization

```python
import pynlm
import matplotlib.pyplot as plt

# Create simulation and collect data
brain = pynlm.create_brain_with_config("small")
world = pynlm.create_test_world(16, 16, 8, 8)
agent = pynlm.create_test_agent(brain, world)

# Run simulation and collect metrics
firing_rates = []
curiosity_levels = []

for step in range(200):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step, step * 0.001)
    
    firing_rates.append(brain.get_average_firing_rate())
    curiosity_levels.append(agent.get_curiosity_level())

# Create plots
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

ax1.plot(firing_rates)
ax1.set_title('Firing Rate Over Time')
ax1.set_ylabel('Firing Rate')

ax2.plot(curiosity_levels)
ax2.set_title('Curiosity Level Over Time')
ax2.set_ylabel('Curiosity Level')
ax2.set_xlabel('Simulation Step')

plt.tight_layout()
plt.savefig('simulation_results.png')
print("Visualization saved to simulation_results.png")
```

#### Data Analysis

```python
import pynlm
import pandas as pd
import json

# Run experiment and collect data
brain = pynlm.create_brain_with_config("medium")
world = pynlm.create_test_world(20, 20, 10, 10)
agent = pynlm.create_test_agent(brain, world)

# Collect detailed metrics
experiment_data = {
    'step': [],
    'firing_rate': [],
    'curiosity': [],
    'novelty': [],
    'dopamine': [],
    'reward': [],
    'action': []
}

for step in range(500):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step, step * 0.001)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    # Collect metrics
    experiment_data['step'].append(step)
    experiment_data['firing_rate'].append(brain.get_average_firing_rate())
    experiment_data['curiosity'].append(agent.get_curiosity_level())
    experiment_data['novelty'].append(agent.get_novelty_level())
    experiment_data['dopamine'].append(agent.get_neuromodulation_level())
    
    reward = 0.0
    if world.get_sensory_percept().get_internal():
        reward = world.get_sensory_percept().get_internal()[0]
    experiment_data['reward'].append(reward)
    experiment_data['action'].append(str(action))

# Save to CSV for analysis
df = pd.DataFrame(experiment_data)
df.to_csv('experiment_data.csv', index=False)
print("Experiment data saved to experiment_data.csv")

# Create analysis report
analysis = {
    'experiment_id': 'exp_001',
    'parameters': {
        'neuron_count': brain.get_config().get_int('neuron_count'),
        'connection_probability': brain.get_config().get_float('connection_probability')
    },
    'results': {
        'total_steps': len(df),
        'avg_firing_rate': df['firing_rate'].mean(),
        'max_firing_rate': df['firing_rate'].max(),
        'avg_curiosity': df['curiosity'].mean(),
        'avg_reward': df['reward'].mean()
    }
}

with open('experiment_analysis.json', 'w') as f:
    json.dump(analysis, f, indent=2)

print("Experiment analysis saved to experiment_analysis.json")
```

### Next Steps for Advanced Users

1. **Explore Research Profiles**: Try different research profiles to understand their effects
2. **Configuration Management**: Master the AdvancedConfigManager for complex experiments
3. **Parameter Sensitivity**: Analyze how parameter changes affect behavior
4. **Cross-Validation**: Run experiments with different configurations for robust results
5. **Integration**: Combine NLM with other research tools for comprehensive analysis
6. **Custom Research**: Create your own research scenarios using the enhanced API

The enhanced NLM Python interface provides powerful tools for advanced neural network research. Start with the basic examples, then gradually explore the more complex features and research profiles.

---

## Mini Projects

### Project 1: Development Study

```python
import pynlm

# Development study
brain = pynlm.create_brain_with_config("cognitive_development")
world = pynlm.create_test_world(32, 32, 16, 16)
agent = pynlm.create_test_agent(brain, world)

for step in range(1000):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step, step * 0.001)
    
    if step % 200 == 0:
        print(f"Step {step}: Stage={agent.get_developmental_stage()}, "
              f"Curiosity={agent.get_curiosity_level():.3f}")

print(f"=== Development Study Complete ===")
print(f"Final stage: {agent.get_developmental_stage()}")
```

### Project 2: Neuromodulation Learning

```python
import pynlm

# Neuromodulation study
brain = pynlm.create_brain_with_config("neuromodulation_study")
world = pynlm.create_test_world(20, 20, 10, 10)
agent = pynlm.create_test_agent(brain, world)

agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

for step in range(800):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step, step * 0.001)
    
    if step % 200 == 0:
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        reward = 0.0
        if world.get_sensory_percept().get_internal():
            reward = world.get_sensory_percept().get_internal()[0]
        agent.apply_reward_modulation(reward, 0.0)
        
        print(f"Step {step}: Reward={reward:.2f}, "
              f"Dopamine={agent.get_neuromodulation_level():.3f}")
```

### Project 3: Memory Integration

```python
import pynlm

# Memory integration study
brain = pynlm.create_brain_with_config("memory_integration")
world = pynlm.create_test_world(24, 24, 12, 12)
agent = pynlm.create_test_agent(brain, world)

agent.enable_working_memory(True)
agent.enable_episodic_memory(True)
agent.enable_semantic_memory(True)

result = pynlm.run_simulation(brain, world, agent, 1500)

print(f"=== Memory Integration Study ===")
print(f"Working memory traces: {brain.get_working_memory() and brain.get_working_memory().get_active_traces()}")
print(f"Episodic memory episodes: {brain.get_episodic_memory() and brain.get_episodic_memory().get_episode_count()}")
```

### Project 4: Configuration Management

```python
import pynlm
from pynlm import AdvancedConfigManager

# Configuration management example
config_manager = AdvancedConfigManager()

# Create and modify configurations
config1 = pynlm.create_default_config()
config2 = pynlm.create_default_config()

config1.set_int("neuron_count", 500)
config1.set_float("connection_probability", 0.1)

config2.set_int("neuron_count", 1000)
config2.set_float("connection_probability", 0.15)

# Compare configurations
comparison = config_manager.compareConfigurations(config1, config2)
print("Configuration differences:")
print(comparison)

# Export/import
json_config = config_manager.exportToJSON(config2)
print(f"Exported configuration: {json_config}")

new_config = pynlm.create_default_config()
config_manager.importFromJSON(new_config, json_config)
print(f"Successfully imported: {new_config.get_int('neuron_count')} neurons")
```

### Project 5: Cross-Validation

```python
import pynlm

# Cross-validation experiment
configs = []
for i in range(3):
    config = pynlm.create_default_config()
    config.set_int("random_seed", 42 + i)
    config.set_int("neuron_count", 500 + i * 200)
    configs.append(config)

results = []
for i, config in enumerate(configs):
    brain = pynlm.createBrain(config)
    world = pynlm.create_test_world(16, 16, 8, 8)
    agent = pynlm.create_test_agent(brain, world)
    
    result = pynlm.runSimulation(brain, world, agent, 300)
    results.append(result)
    
    print(f"Configuration {i+1}: {config.get_int('neuron_count')} neurons, "
          f"Final reward={result.total_reward:.2f}")

print("\n=== Cross-Validation Results ===")
for i, result in enumerate(results):
    print(f"Configuration {i+1}: Avg firing rate={result.avg_firing_rate:.2f}, "
          f"Final spikes={result.final_spike_count}")
```

---

## Comprehensive Troubleshooting Guide

### Common Issues and Solutions

#### Issue 1: "Brain is not firing neurons"

**Symptoms:** All neurons remain at resting potential, no spikes detected.

**Solutions:**
1. Ensure `brain.initialize()` is called before `brain.step()`
2. Add `brain.injectCurrentToNeurons(NeuronType.Sensory, 10.0f)` in simulation loop
3. Verify neuron threshold parameters in configuration
4. Start with smaller networks for debugging

**Debug script:**
```python
brain = pynlm.create_brain_with_config("small")
print(f"Neurons: {brain.get_total_neuron_count()}")

brain.initialize()
for i in range(50):
    brain.inject_current_to_neurons(NeuronType.Sensory, 20.0f)
    brain.step(i, i * 0.001)

print(f"Spike count: {brain.get_total_spike_count()}")
```

#### Issue 2: "Agent not responding to world"

**Symptoms:** Agent doesn't move, no actions detected.

**Solutions:**
1. Ensure `agent.initialize(world)` is called
2. Call `agent.decodeMotorCommand()` in simulation loop
3. Ensure `world.update()` is called before getting percepts
4. Use `agent.enableAllFeatures()` for complete functionality

**Debug script:**
```python
brain = pynlm.create_brain_with_config("small")
world = pynlm.create_test_world(10, 10, 5, 5)
agent = pynlm.create_test_agent(brain, world)

print(f"Sensory input size: {agent.get_sensory_input_size()}")
print(f"Motor output size: {agent.get_motor_output_size()}")

for step in range(20):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step, step * 0.001)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    print(f"Step {step}: Action={action}, Position={brain.get_regions()[0].get_neurons()[0]->getMembranePotential():.2f}")
```

#### Issue 3: "Memory systems not working"

**Symptoms:** Memory metrics show zero traces, no episodes stored.

**Solutions:**
1. Enable memory with `agent.enable_working_memory(True)`
2. Call `agent.enable_development(True)` (memory requires development)
3. Allow sufficient simulation time for consolidation
4. Check configuration memory parameters

**Debug script:**
```python
brain = pynlm.create_brain_with_config("medium")
world = pynlm.create_test_world(16, 16, 8, 8)
agent = pynlm.create_test_agent(brain, world)

agent.enable_working_memory(True)
agent.enable_episodic_memory(True)
agent.enable_semantic_memory(True)
agent.enable_development(True)

for step in range(300):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step, step * 0.001)
    
    if step % 100 == 0:
        wm = brain.get_working_memory()
        em = brain.get_episodic_memory()
        
        print(f"Step {step}:")
        print(f"  Working Memory: {wm and wm.get_active_traces()} traces")
        print(f"  Episodic Memory: {em and em.get_episode_count()} episodes")
```

### Performance Optimization Tips

#### Simulation Performance

1. Use appropriate network sizes for your research
2. Disable unused features to reduce computational load
3. Monitor performance with timing metrics
4. Use checkpointing for long-running experiments

#### Configuration Management

1. Use research profiles for consistent setups
2. Document all parameters used in experiments
3. Save and load configurations to avoid manual tuning
4. Compare configurations to track changes

### Best Practices for Research

#### Experimental Design

**Do:**
- Use research profiles for consistent experimental setups
- Document all configuration parameters
- Run multiple trials with different random seeds
- Save intermediate checkpoints during long simulations
- Use parameter sensitivity analysis

**Don't:**
- Don't run simulations with unrealistic network sizes
- Don't modify configurations mid-experiment without documentation
- Don't rely on single random seeds for conclusive results
- Don't skip initialization or reset calls

#### Code Organization

**Good practice:**
```python
# Use organized naming
brain = pynlm.create_brain_with_config("cognitive_development")
world = pynlm.create_test_world(32, 32)
agent = pynlm.create_test_agent(brain, world)

# Document parameters
experiment_params = {
    'config_profile': 'cognitive_development',
    'world_dimensions': (32, 32),
    'simulation_steps': 1000
}
```

### Integration with External Tools

#### Visualization

```python
import pynlm
import matplotlib.pyplot as plt

# Create simulation and collect data
brain = pynlm.create_brain_with_config("small")
world = pynlm.create_test_world(16, 16, 8, 8)
agent = pynlm.create_test_agent(brain, world)

# Run simulation and collect metrics
firing_rates = []
curiosity_levels = []

for step in range(200):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step, step * 0.001)
    
    firing_rates.append(brain.get_average_firing_rate())
    curiosity_levels.append(agent.get_curiosity_level())

# Create plots
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
ax1.plot(firing_rates)
ax1.set_title('Firing Rate')
ax2.plot(curiosity_levels)
ax2.set_title('Curiosity Level')
plt.tight_layout()
plt.savefig('results.png')
```

#### Data Analysis

```python
import pynlm
import pandas as pd

# Run experiment and collect data
brain = pynlm.create_brain_with_config("medium")
world = pynlm.create_test_world(20, 20, 10, 10)
agent = pynlm.create_test_agent(brain, world)

# Collect metrics
experiment_data = {
    'step': [],
    'firing_rate': [],
    'curiosity': [],
    'reward': []
}

for step in range(500):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step, step * 0.001)
    
    experiment_data['step'].append(step)
    experiment_data['firing_rate'].append(brain.get_average_firing_rate())
    experiment_data['curiosity'].append(agent.get_curiosity_level())
    
    reward = 0.0
    if world.get_sensory_percept().get_internal():
        reward = world.get_sensory_percept().get_internal()[0]
    experiment_data['reward'].append(reward)

# Analyze with pandas
df = pd.DataFrame(experiment_data)
print(f"Average firing rate: {df['firing_rate'].mean():.2f}")
print(f"Average curiosity: {df['curiosity'].mean():.3f}")
print(f"Average reward: {df['reward'].mean():.2f}")
```

### Next Steps for Advanced Users

1. **Explore Research Profiles**: Try different profiles to understand their effects
2. **Configuration Management**: Master AdvancedConfigManager for complex experiments
3. **Parameter Sensitivity**: Analyze parameter changes on behavior
4. **Cross-Validation**: Run experiments with different configurations
5. **Integration**: Combine NLM with other research tools
6. **Custom Research**: Create your own research scenarios

The enhanced NLM Python interface provides powerful tools for advanced neural network research. Start with the basic examples, then gradually explore the more complex features and research profiles.

---

**Key Features:**
- Enhanced Python bindings with better error handling
- Research profiles for common experimental setups
- Advanced configuration management
- Convenience functions for rapid prototyping
- Comprehensive documentation and examples
- Integration with visualization and data analysis tools

**Research Profiles Available:**
- Development (for testing and exploration)
- Small/Medium/Large Scale (for different network sizes)
- Cognitive Development (for studying cognitive processes)
- Neuromodulation Study (for reward and curiosity research)
- Memory Integration (for memory system research)
- Learning Dynamics (for plasticity studies)
- Behavioral Modeling (for action selection research)

The enhanced NLM framework is now ready for advanced neural network research and experimentation!

---

### Table of Contents

1. Overview
2. Building from Source
3. Python Installation
4. Basic Usage
5. Enhanced Agent Brain Interface
6. Convenience Functions
7. Research Profiles
8. Advanced Configuration Management
9. Complete Research Examples
10. Research-Ready Examples
11. Troubleshooting Guide
12. Performance Optimization
13. Best Practices
14. Integration with External Tools
15. Next Steps for Advanced Users
16. Mini Projects

---

*Version 2.0.0 - Enhanced NLM Python Interface*
*Ready for advanced neural network research and experimentation!*