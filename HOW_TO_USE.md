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

### Advanced Usage (New Section)

#### Advanced Python Examples

##### 1. Performance Monitoring and Optimization

```python
import pynlm
import time

# Configure for high-performance simulation
config = pynlm.createDefaultConfig()
config.set("performance.optimization_level", 3)
config.set("neuron_count", 5000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.0005)

# Create and monitor brain
brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
step_times = []
firing_rates = []
memory_usage = []

start_time = time.time()
for step in range(5000):
    step_start = time.time()
    
    brain.step(step)
    
    step_time = time.time() - step_start
    step_times.append(step_time)
    
    if step % 100 == 0:
        firing_rates.append(brain.getAverageFiringRate())
        memory_usage.append(brain.getWorkingMemory().size() if brain.getWorkingMemory() else 0)
        
        # Adaptive optimization based on performance
        if step_time > 0.1:  # Slow step
            config.set("performance.optimization_level", min(3, config.get("performance.optimization_level", 0) + 1))
            brain.reconfigure(config)

end_time = time.time()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average step time: {sum(step_times)/len(step_times):.4f} seconds")
print(f"Min step time: {min(step_times):.4f} seconds")
print(f"Max step time: {max(step_times):.4f} seconds")
print(f"Avg firing rate: {sum(firing_rates)/len(firing_rates):.2f} Hz")
```

##### 2. Batch Processing for Experiment Repeats

```python
import pynlm
from typing import List, Dict
import json
import numpy as np

class ExperimentRunner:
    def __init__(self, base_config: pynlm.Config):
        self.base_config = base_config
        self.results = []
    
    def run_experiment(self, 
                      config_modifications: Dict[str, any],
                      num_steps: int = 1000,
                      seed: int = 42) -> Dict[str, any]:
        # Create configuration copy with modifications
        config = pynlm.createDefaultConfig()
        
        # Apply base config
        for key in self.base_config.getKeys():
            config.set(key, self.base_config.get(key))
        
        # Apply modifications
        for key, value in config_modifications.items():
            config.set(key, value)
        
        # Set random seed for reproducibility
        config.set("random.seed", seed)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Collect metrics
        metrics = {
            'seed': seed,
            'final_spikes': 0,
            'final_firing_rate': 0.0,
            'max_firing_rate': 0.0,
            'total_plasticity': 0
        }
        
        for step in range(num_steps):
            brain.step(step)
            
            if step == num_steps - 1:
                metrics['final_spikes'] = brain.getTotalSpikeCount()
                metrics['final_firing_rate'] = brain.getAverageFiringRate()
                metrics['max_firing_rate'] = brain.getMaxFiringRate()
                metrics['total_plasticity'] = brain.getTotalPlasticityApplied()
        
        return metrics
    
    def run_batch(self, 
                  parameter_grid: List[Dict[str, any]],
                  num_steps: int = 1000) -> List[Dict[str, any]]:
        results = []
        
        for i, params in enumerate(parameter_grid):
            print(f"Running experiment {i+1}/{len(parameter_grid)}: {params}")
            
            # Test multiple random seeds for each parameter set
            for seed in [42, 123, 456]:
                result = self.run_experiment(params, num_steps, seed)
                result.update(params)
                results.append(result)
        
        return results

# Example usage
if __name__ == "__main__":
    # Base configuration
    base_config = pynlm.createDefaultConfig()
    base_config.set("performance.optimization_level", 2)
    
    # Create experiment runner
    runner = ExperimentRunner(base_config)
    
    # Define parameter grid for systematic experiments
    parameter_grid = [
        {"neuron_count": 1000, "plasticity.stdp.learning_rate": 0.001},
        {"neuron_count": 1000, "plasticity.stdp.learning_rate": 0.01},
        {"neuron_count": 5000, "plasticity.stdp.learning_rate": 0.001},
        {"neuron_count": 5000, "plasticity.stdp.learning_rate": 0.01},
    ]
    
    # Run batch experiment
    results = runner.run_batch(parameter_grid, num_steps=2000)
    
    # Analyze results
    print("\nExperiment Results Summary:")
    for result in results:
        print(f"Seed {result['seed']}, Neurons {result['neuron_count']}, LR {result['plasticity.stdp.learning_rate']} "
              f"-> Final firing rate: {result['final_firing_rate']:.2f} Hz")
    
    # Save results to file
    with open("experiment_results.json", "w") as f:
        json.dump(results, f, indent=2)
```

##### 3. Experiment Management Framework

```python
import pynlm
from typing import List, Dict, Any
import time
import json
from datetime import datetime

class NLMExperiment:
    def __init__(self, name: str, config: pynlm.Config):
        self.name = name
        self.config = config
        self.world = pynlm.createSimpleWorld()
        self.agent = None
        self.brain = None
        self.results = {}
        self.log = []
    
    def setup(self, world_config: Dict[str, any]):
        """Setup the experiment with world configuration."""
        self.world.configure(**world_config)
        self.world.reset()
        
        # Create brain with experiment-specific config
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable key subsystems based on experiment goals
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        self.log.append(f"Experiment '{self.name}' setup complete")
    
    def run(self, num_steps: int, collect_metrics: bool = True) -> Dict[str, Any]:
        """Run the experiment for specified number of steps."""
        start_time = time.time()
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = self._calculate_reward(percept)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Collect metrics periodically
            if collect_metrics and step % 100 == 0:
                self._collect_metrics(step)
        
        end_time = time.time()
        
        # Compile results
        results = {
            'experiment_name': self.name,
            'duration': end_time - start_time,
            'steps_completed': num_steps,
            'final_metrics': self.results.get('final', {}),
            'metrics_history': self.results.get('history', [])
        }
        
        self.log.append(f"Experiment '{self.name}' completed in {end_time - start_time:.2f} seconds")
        return results
    
    def _calculate_reward(self, percept) -> float:
        """Calculate reward based on current percept."""
        # Custom reward function - can be modified per experiment
        internal = percept.getInternal() if percept.getInternal() else []
        return internal[0] if internal else 0.0
    
    def _collect_metrics(self, step: int):
        """Collect metrics at specified step."""
        metrics = {
            'step': step,
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'simulation_time': self.world.getSimulationTime()
        }
        
        if 'history' not in self.results:
            self.results['history'] = []
        
        self.results['history'].append(metrics)
    
    def save_results(self, filepath: str):
        """Save experiment results to file."""
        data = {
            'experiment': self.name,
            'config': dict(self.config.getKeys()),
            'results': self.results,
            'log': self.log,
            'timestamp': datetime.now().isoformat()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        self.log.append(f"Results saved to {filepath}")
    
    def get_summary(self) -> str:
        """Get a summary of the experiment."""
        if not self.results:
            return "Experiment not run yet"
        
        final = self.results.get('final', {})
        summary = f"\n=== Experiment Summary: {self.name} ===\n"
        summary += f"Duration: {final.get('duration', 'N/A'):.2f} seconds\n"
        summary += f"Final firing rate: {final.get('firing_rate', 'N/A'):.2f} Hz\n"
        summary += f"Final curiosity: {final.get('curiosity', 'N/A'):.3f}\n"
        summary += f"Final developmental stage: {final.get('developmental_stage', 'N/A')}\n"
        
        return summary

# Example usage for different experiment types
class MemoryExperiment(NLMExperiment):
    """Experiment focused on memory formation and recall."""
    
    def __init__(self, config: pynlm.Config):
        super().__init__("Memory_Formation", config)
        self.target_pattern = None
        self.memory_traces = []
    
    def setup(self, world_config: Dict[str, any]):
        super().setup(world_config)
        # Enable episodic memory for this experiment
        if hasattr(self.brain, 'enableEpisodicMemory'):
            self.brain.enableEpisodicMemory(True)
    
    def run(self, num_steps: int, target_step: int = 500) -> Dict[str, Any]:
        """Run memory experiment with specific target."""
        results = super().run(num_steps)
        
        # Analyze memory formation
        if 'history' in self.results:
            for i, metrics in enumerate(self.results['history']):
                if metrics['step'] == target_step:
                    self.target_pattern = metrics['firing_neuron_count']
                    break
        
        # Calculate memory retention
        if self.target_pattern:
            final_firing = self.results['final']['firing_neuron_count'] if 'final' in self.results else 0
            retention = final_firing / self.target_pattern if self.target_pattern > 0 else 0
            results['memory_retention'] = retention
        
        return results

class LearningExperiment(NLMExperiment):
    """Experiment focused on learning and adaptation."""
    
    def __init__(self, config: pynlm.Config):
        super().__init__("Skill_Learning", config)
        self.performance_history = []
    
    def run(self, num_steps: int, adaptation_window: int = 100) -> Dict[str, Any]:
        """Run learning experiment with adaptation analysis."""
        results = super().run(num_steps)
        
        # Analyze learning curve
        if 'history' in self.results:
            for i in range(0, len(self.results['history']), adaptation_window):
                window_data = self.results['history'][i:i+adaptation_window]
                if window_data:
                    firing_rates = [m['firing_rate'] for m in window_data]
                    avg_firing = sum(firing_rates) / len(firing_rates)
                    self.performance_history.append({
                        'window': i,
                        'avg_firing_rate': avg_firing,
                        'trend': 'improving' if len(self.performance_history) > 0 and avg_firing > self.performance_history[-1]['avg_firing_rate'] else 'stable'
                    })
        
        return results

# Example usage
if __name__ == "__main__":
    # Create experiment configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("performance.optimization_level", 2)
    
    # Run memory experiment
    memory_exp = MemoryExperiment(config)
    memory_exp.setup({
        'width': 20, 'height': 20,
        'visionWidth': 8, 'visionHeight': 8
    })
    
    memory_results = memory_exp.run(2000, target_step=1000)
    memory_exp.save_results("memory_experiment.json")
    
    # Run learning experiment
    learning_exp = LearningExperiment(config)
    learning_exp.setup({
        'width': 15, 'height': 15,
        'visionWidth': 6, 'visionHeight': 6
    })
    
    learning_results = learning_exp.run(3000)
    learning_exp.save_results("learning_experiment.json")
    
    print(memory_exp.get_summary())
    print(learning_exp.get_summary())
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
