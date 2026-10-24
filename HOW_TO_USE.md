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

### Advanced Configuration

#### Custom Configurations

```python
import pynlm

# Create configuration from preset
config = pynlm.createPresetConfig("development")

# Modify configuration
config.set("brain.neuron_count", 800)
config.set("plasticity.structural.enable", True)
config.set("neuromod.curiosity.enable", True)

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.cfg")
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

### Advanced Training Examples

#### High-Level Training Functions

```python
import pynlm

def run_advanced_training(num_episodes=100, verbose=False):
    """Run advanced training with monitoring and metrics."""
    
    # Setup with development preset
    brain = pynlm.createBrain(pynlm.createPresetConfig("development"))
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize
    world.reset()
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track performance
    monitor = pynlm.TrainingMonitor()
    profiler = pynlm.Profiler()
    
    # Run training episodes
    for episode in range(num_episodes):
        profiler.start("episode")
        
        # Run single episode
        episode_metrics = pynlm.runTrainingEpisode(
            brain, world, agent, 
            maxSteps=1500, 
            verbose=verbose
        )
        
        profiler.record("episode", episode_metrics.get("total_reward", 0.0))
        monitor.recordEpisode(episode_metrics)
        
        if verbose:
            print(f"Episode {episode}: Reward = {episode_metrics['total_reward']:.2f}, "
                  f"Firing Rate = {episode_metrics['avg_firing_rate']:.2f}")
    
    # Generate final report
    status_report = monitor.getStatusReport()
    performance_profile = profiler.report()
    
    return {
        "metrics": status_report,
        "performance": performance_profile,
        "brain_stats": {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount()
        }
    }

# Run advanced training
results = run_advanced_training(num_episodes=100, verbose=True)
print(results["brain_stats"])
```

#### Real-time Monitoring

```python
import pynlm
import time

def run_real_time_simulation(duration_seconds=300, timestep=0.1):
    """Run simulation with real-time monitoring and visualization."""
    
    # Setup
    config = pynlm.createPresetConfig("standard")
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    agent = pynlm.createAgentBrain(brain)
    
    world.reset()
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    start_time = time.time()
    last_stats_time = start_time
    
    print("=== Real-time NLM Simulation ===")
    print(f"Duration: {duration_seconds} seconds")
    print(f"Timestep: {timestep}")
    print("Press Ctrl+C to stop early")
    print()
    
    while time.time() - start_time < duration_seconds:
        current_time = time.time()
        
        # Update world
        world.update(timestep)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process and act
        agent.processSensoryInput(percept)
        brain.step(current_time)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, current_time)
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Print real-time stats every 5 seconds
        if current_time - last_stats_time >= 5.0:
            print(f"Time: {current_time - start_time:.1f}s | "
                  f"Firing: {brain.getFiringNeuronCount()} | "
                  f"Spikes: {brain.getTotalSpikeCount()} | "
                  f"Curiosity: {agent.getCuriosityLevel():.3f} | "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
            
            # Generate brain activity visualization
            visualization = pynlm.visualizeBrainActivity(brain)
            print(visualization)
            
            last_stats_time = current_time
    
    total_time = time.time() - start_time
    print(f"\n=== Simulation Complete ===")
    print(f"Total duration: {total_time:.1f} seconds")
    print(f"Average steps per second: {brain.getTotalSpikeCount() / total_time:.1f}")
    
    return {
        "total_spikes": brain.getTotalSpikeCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "duration": total_time
    }

# Run real-time simulation (uncomment to use)
# results = run_real_time_simulation(duration_seconds=60)
```

### Experiment Management

#### Experiment Runner

```python
import pynlm
from typing import Dict, List, Any

class NLMTrainingExperiment:
    """Manage complex NLM training experiments with multiple conditions."""
    
    def __init__(self, name: str, config_preset: str = "standard"):
        self.name = name
        self.config_preset = config_preset
        self.results: List[Dict[str, Any]] = []
    
    def run_condition(self, condition_name: str, 
                     overrides: Dict[str, Any] = None,
                     num_episodes: int = 20) -> Dict[str, Any]:
        """Run a single experimental condition."""
        
        # Setup
        config = pynlm.createPresetConfig(self.config_preset)
        
        # Apply condition-specific overrides
        if overrides:
            for key, value in overrides.items():
                config.set(key, value)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionWidth=8)
        agent = pynlm.createAgentBrain(brain)
        
        world.reset()
        agent.initialize(world)
        
        # Enable relevant subsystems based on condition
        agent.enableRewardModulation(True)
        agent.enableCuriosity(overrides.get("enable_curiosity", True) if overrides else True)
        
        # Run training
        metrics = pynlm.runTraining(
            brain, world, agent,
            numEpisodes=num_episodes,
            verbose=False
        )
        
        # Store results
        result = {
            "condition": condition_name,
            "metrics": metrics,
            "config": config.summary()
        }
        self.results.append(result)
        
        return result
    
    def compare_conditions(self, conditions: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare results across multiple conditions."""
        comparison = {"conditions": [], "summary": {}}
        
        for condition in conditions:
            result = self.run_condition(**condition)
            comparison["conditions"].append(result)
        
        # Calculate comparisons
        if len(comparison["conditions"]) >= 2:
            for i in range(len(comparison["conditions"]) - 1):
                cond1 = comparison["conditions"][i]
                cond2 = comparison["conditions"][i + 1]
                
                comparison["summary"][f"{cond1['condition']}_vs_{cond2['condition']}"] = {
                    "reward_difference": cond2["metrics"]["total_reward"] - cond1["metrics"]["total_reward"],
                    "performance_ratio": cond2["metrics"]["avg_reward_per_step"] / cond1["metrics"]["avg_reward_per_step"]
                }
        
        return comparison
    
    def save_results(self, filepath: str):
        """Save experiment results to file."""
        import json
        
        # Convert to JSON-serializable format
        json_data = {
            "experiment_name": self.name,
            "conditions": []
        }
        
        for result in self.results:
            json_data["conditions"].append({
                "name": result["condition"],
                "metrics": result["metrics"],
                "config": result["config"]
            })
        
        with open(filepath, 'w') as f:
            json.dump(json_data, f, indent=2)
    
    def get_summary_report(self) -> str:
        """Generate a summary report of all conditions."""
        report = f"=== Experiment: {self.name} ===\n\n"
        
        for i, result in enumerate(self.results):
            report += f"Condition {i+1}: {result['condition']}\n"
            report += f"  Total Reward: {result['metrics']['total_reward']:.2f}\n"
            report += f"  Average Firing Rate: {result['metrics']['avg_firing_rate']:.2f}\n"
            report += f"  Max Curiosity: {result['metrics']['max_curiosity']:.3f}\n"
            report += f"  Steps Completed: {result['metrics']['steps_completed']}\n\n"
        
        return report

# Example usage
experiment = NLMTrainingExperiment("Curiosity_Analysis")

# Run different conditions
experiment.run_condition("No_Curiosity", {"enable_curiosity": False})
experiment.run_condition("High_Curiosity", {"enable_curiosity": True})

print(experiment.get_summary_report())
experiment.save_results("experiment_results.json")
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
