# Python Integration Guide

## Installation and Setup

### Quick Start

```bash
# Install dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build and install the Python module
pip install .

# Or install in development mode
pip install -e .
```

### Building from Source

```bash
# Full C++ build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Python bindings build
pip install .
```

## Python Module Overview

The `pynlm` module provides Python bindings for the NLM C++ neural simulation framework, including:

- **Brain**: Core neural computation with LIF neurons, plasticity, and memory systems
- **Config**: Configuration management for brain parameters
- **AgentBrain**: Interface connecting brain to world environment
- **SimpleWorld**: 2D world simulation with sensory input and motor output

## Basic Usage Examples

### 1. Hello World - Create and Run a Brain

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Interactive Agent in a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.get('SimpleWorld').configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run interactive session
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### 3. Learning with Reward

```python
import pynlm

# Create brain with learning capabilities
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(width=30, height=30, visionWidth=12, visionHeight= ANNUAL_VALUE(0.1)]
agent.initialize(world)

# Run learning simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getAgentBody().energy / 100.0
    agent.applyRewardModulation(reward, 0.0)

print(f"Learning simulation complete. Firing rate: {brain.getAverageFiringRate()}")
```

## Installation and Setup

### Quick Start

```bash
# Install dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build and install the Python module
pip install .

# Or install in development mode
pip install -e .
```

### Building from Source

```bash
# Full C++ build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Python bindings build
pip install .
```

## Python Module Overview

The `pynlm` module provides Python bindings for the NLM C++ neural simulation framework, including:

- **Brain**: Core neural computation with LIF neurons, plasticity, and memory systems
- **Config**: Configuration management for brain parameters
- **AgentBrain**: Interface connecting brain to world environment
- **SimpleWorld**: 2D world simulation with sensory input and motor output

## Basic Usage Examples

### 1. Hello World - Create and Run a Brain

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Interactive Agent in a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run interactive session
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### 3. Learning with Reward

```python
import pynlm

# Create brain with learning capabilities
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(width=30, height=30, visionWidth=12, visionHeight=8)
agent.initialize(world)

# Run learning simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getAgentBody().energy / 100.0
    agent.applyRewardModulation(reward, 0.0)

print(f"Learning simulation complete. Firing rate: {brain.getAverageFiringRate()}")
```

## Advanced Usage Examples

### 4. Complete Agent Lifecycle

```python
import pynlm

def run_agent_simulation(num_steps=500):
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
    agent.initialize(world)
    
    # 5. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 6. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
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
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

# Run the simulation
run_agent_simulation(500)
```

### 5. Configuration Management

```python
import pynlm

# Create and configure a brain
config = pynlm.createDefaultConfig()

# Customize configuration for research
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.scale", 0.5)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 1.0)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Research brain created: {brain.getTotalNeuronCount()} neurons")
```

### 6. Batch Simulation Management

```python
import pynlm
from typing import List, Dict, Any

class NLMExperiment:
    """Manage multiple NLM experiments with consistent configuration."""
    
    def __init__(self, config: pynlm.Config):
        self.config = config
        self.experiments = []
    
    def run_experiment(self, num_steps: int, description: str = "") -> Dict[str, Any]:
        """Run a single experiment and return results."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        agent = pynlm.createAgentBrain(brain)
        
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect results
        results = {
            "description": description,
            "total_steps": num_steps,
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "energy": world.getAgentBody().energy,
            "developmental_stage": str(brain.getDevelopmentalStage())
        }
        
        self.experiments.append(results)
        return results
    
    def run_batch(self, experiments: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Run multiple experiments with different configurations."""
        results = []
        
        for exp_config in experiments:
            # Create copy of config with experiment-specific settings
            exp_config = self.config.copy()
            exp_config.update(exp_config.get("brain_settings", {}))
            
            result = self.run_experiment(
                num_steps=exp_config.get("steps", 100),
                description=exp_config.get("description", "")
            )
            result.update(exp_config.get("additional_results", {}))
            results.append(result)
        
        return results

# Usage example
config = pynlm.createDefaultConfig()
experiment = NLMExperiment(config)

# Run single experiment
result1 = experiment.run_experiment(200, "Baseline experiment")
print(f"Experiment result: {result1['description']}")

# Run batch of experiments
experiments = [
    {"steps": 100, "brain_settings": {"brain.neuron_count": 1000}, "description": "Small brain"},
    {"steps": 200, "brain_settings": {"brain.neuron_count": 5000}, "description": "Medium brain"},
    {"steps": 300, "brain_settings": {"brain.neuron_count": 10000}, "description": "Large brain"}
]

batch_results = experiment.run_batch(experiments)
for result in batch_results:
    print(f"{result['description']}: {result['total_neurons']} neurons, "
          f"{result['average_firing_rate']:.2f} Hz firing rate")
```

## Error Handling and Debugging

### 7. Basic Error Handling

```python
import pynlm

def safe_brain_simulation(config, num_steps):
    """Safe brain simulation with error handling."""
    try:
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with progress monitoring
        for step in range(num_steps):
            brain.step(step)
            
            # Check for errors
            if step % 100 == 0:
                stats = pynlm.getBrainStats(brain)
                print(f"Step {step}: {stats['total_neurons']} neurons")
                
    except Exception as e:
        print(f"Error during simulation: {e}")
        raise

# Usage
config = pynlm.createDefaultConfig()
safe_brain_simulation(config, 1000)
```

### 8. Debugging Helper Functions

```python
def debug_brain(brain, label="Brain"):
    """Print detailed debug information about a brain."""
    print(f"\n=== {label} Debug Information ===")
    
    # Basic statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Developmental stage
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Check for learning indicators
    if brain.getAverageFiringRate() > 10.0:
        print("Learning indicator: High firing rate detected")
    
    if brain.getExcitationInhibitionRatio() > 1.0:
        print("Learning indicator: Excitatory dominance detected")
    
    # Memory system status
    if hasattr(brain, 'getWorkingMemory') and brain.getWorkingMemory():
        print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
debug_brain(brain, "Simulation Brain")
```

## Performance Optimization

### 9. Optimized Simulation Loop

```python
import pynlm
import time

def optimized_simulation(config, num_steps, world_config=None):
    """Optimized simulation loop for better performance."""
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    if world_config:
        for key, value in world_config.items():
            world.configure(key, value)
    else:
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Performance monitoring
    start_time = time.time()
    steps_per_second = []
    
    # Optimized simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Periodic performance monitoring
        if step % 100 == 0:
            current_time = time.time()
            elapsed = current_time - start_time
            steps_per_sec = num_steps / elapsed
            steps_per_second.append(steps_per_sec)
            
            # Progress output
            print(f"Step {step}: {steps_per_sec:.0f} steps/sec, "
                  f"{brain.getAverageFiringRate():.2f} Hz firing rate")
    
    # Performance summary
    total_time = time.time() - start_time
    avg_steps_per_sec = sum(steps_per_second) / len(steps_per_second) if steps_per_second else 0
    
    print(f"\nSimulation Complete:")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Average performance: {avg_steps_per_sec:.0f} steps/sec")
    print(f"  Total steps executed: {num_steps}")
    
    return {
        "total_time": total_time,
        "average_performance": avg_steps_per_sec,
        "final_stats": pynlm.getBrainStats(brain)
    }

# Usage with performance optimization
config = pynlm.createDefaultConfig()
results = optimized_simulation(config, 1000)
```

## Quick Reference

### Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Brain` | Neural simulation | `initialize()`, `step()`, `receiveSensoryInput()` |
| `Config` | Brain configuration | `set()`, `get()`, `saveToFile()` |
| `AgentBrain` | Brain-world interface | `processSensoryInput()`, `decodeMotorCommand()` |
| `SimpleWorld` | World simulation | `update()`, `getSensoryPercept()` |

### Common Patterns

```python
# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize
brain.initialize()
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Common Configuration Options

```python
# Brain size and connectivity
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)

# Learning mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Development
config.set("development.enabled", True)
config.set("development.plasticity_modifier", 1.0)
```

## Troubleshooting

### Common Issues and Solutions

#### "Brain not firing"
```python
# Check if brain is initialized
brain = pynlm.createBrain(config)
brain.initialize()  # Must be called!
```

#### "Agent not moving"
```python
# Ensure world is being updated
for step in range(100):
    world.update(0.1)  # This must be called!
    # ... other steps
```

#### "Simulation too slow"
```python
# Optimization tips
# 1. Reduce neuron count for testing
config.set("brain.neuron_count", 100)

# 2. Increase simulation timestep
world.update(0.5)  # Slower but faster simulation

# 3. Use smaller world
world.configure(width=10, height=10)
```

## Best Practices

1. **Always initialize** the brain before using it
2. **Configure** the brain with appropriate parameters for your use case
3. **Monitor** simulation progress for debugging
4. **Use** reward modulation for learning behaviors
5. **Enable** curiosity for exploration behaviors
6. **Check** developmental stages for age-appropriate behaviors
7. **Save** checkpoints for long-running simulations
8. **Use** batch processing for parameter sweeps

## Example: Research Workflow

```python
import pynlm

# 1. Define research parameters
research_config = pynlm.createDefaultConfig()
research_config.set("brain.neuron_count", 2000)
research_config.set("plasticity.stdp.learning_rate", 0.002)
research_config.set("neuromod.curiosity.scale", 0.8)

# 2. Run baseline experiment
baseline_results = run_agent_simulation(research_config, 500, "Baseline")

# 3. Test with different parameters
research_config.set("brain.neuron_count", 5000)
hypothesis_results = run_agent_simulation(research_config, 500, "Hypothesis: Large brain")

# 4. Compare results
compare_results(baseline_results, hypothesis_results)
```

This comprehensive guide provides everything needed to get started with NLM Python development, from basic usage to advanced research workflows. The examples cover common patterns, error handling, performance optimization, and best practices for building robust NLM applications.

For the most up-to-date documentation and additional examples, visit the NLM documentation website or refer to the source code comments.