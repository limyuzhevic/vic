# NLM - Enhanced Usage Guide for Beginners and Advanced Users

## Quick Start: The 3 Essential Concepts

Before diving in, understand NLM's philosophy:

1. **Brain** - The virtual brain that thinks (neurons + synapses + learning)
2. **World** - The environment the brain lives in (where actions have consequences)
3. **Agent** - The bridge connecting brain to world (sensory input + motor output)

**Core Idea**: Brains connected to worlds through agents create intelligent behavior through interaction, not preprogramming.

---

## Example 1: Simplest Brain (1 line)

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it think
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

## Example 2: Complete Learning Agent

```python
import pynlm

# Setup with advanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)      # More neurons = more complex behavior
config.set("random_seed", 12345)     # Reproducible experiments
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(5, 5)  # Where agent starts
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore new things
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

# Run learning simulation
print("Starting agent learning...")
for step in range(1000):
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides action
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Position ({agent.getBrainBody().x}, {agent.getBrainBody().y}), "
              f"Firing neurons: {brain.getFiringNeuronCount()}")

print("Learning complete!")
print("Final stats:")
print("  - Total spikes:", brain.getTotalSpikeCount())
print("  - Avg firing rate:", brain.getAverageFiringRate())
print("  - Brain developed to:", brain.getDevelopmentalStage().name)
```

---

## Advanced Configuration Patterns

### 1. Custom Neural Architecture

```python
import pynlm
import json

# Create custom configuration
config = pynlm.createDefaultConfig()

# Set architecture parameters
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.15)

# Learning parameters
config.set("plasticity_learning_rate", 0.05)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.018)

# Development parameters
config.set("development_synaptogenesis_rate", 0.002)
config.set("development_pruning_rate", 0.0002)

# Neuromodulation
config.set("dopamine_baseline", 0.2)
config.set("reward_discount_factor", 0.95)

# Create and save custom config
custom_config = pynlm.createBrain(config)
custom_config.save("my_custom_brain.json")

# Load later
custom_config = pynlm.createBrain(pynlm.Config())
custom_config.loadFromFile("my_custom_brain.json")
```

### 2. World Configuration

```python
import pynlm

# Advanced world setup
world = pynlm.createSimpleWorld()
world.configure(
    width=50,                    # Grid size
    height=50,
    visionWidth=12,              # Agent vision range
    visionHeight=12,
    maxEnergy=100.0,              # Energy capacity
    energyDecayRate=0.1,          # Energy loss per step
    randomSeed=42                # Reproducible world
)

# Add objects
from pynlm import WorldObject, WorldObjectType

# Add resources
for i in range(10):
    obj = WorldObject()
    obj.x = random.uniform(5, 45)
    obj.y = random.uniform(5, 45)
    obj.type = WorldObjectType.Resource
    obj.value = random.uniform(10, 50)
    world.addObject(obj)

# Add hazards
for i in range(3):
    obj = WorldObject()
    obj.x = random.uniform(10, 40)
    obj.y = random.uniform(10, 40)
    obj.type = WorldObjectType.Hazard
    obj.value = -1.0
    world.addObject(obj)
```

### 3. Agent Configuration

```python
import pynlm

# Create agent with custom settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure agent behavior
agent.enableRewardModulation(True, 0.9, 0.1)      # Enable with custom parameters
agent.enableCuriosity(True, 0.5, 2.0)           # Novelty detection
agent.enableDevelopment(True, 0.1, 0.01)        # Growth parameters
agent.enableStructuralPlasticity(True, 0.001)    # Connection growth

# Get agent capabilities
print(f"Sensory input size: {agent.getSensoryInputSize()}")
print(f"Motor output size: {agent.getMotorOutputSize()}")
```

---

## Command Line Tools (Advanced Usage)

### 1. Build and Run Scripts

```python
# Create build script for advanced users
#!/usr/bin/env python3
import subprocess
import sys
import os

def build_nlm():
    """Build NLM with custom options"""
    cmd = ["cmake", "-B", "build", "-DCMAKE_BUILD_TYPE=Release"]
    result = subprocess.run(cmd)
    if result.returncode != 0:
        return False
    
    cmd = ["cmake", "--build", "build", "--config", "Release", "-j4"]
    result = subprocess.run(cmd)
    return result.returncode == 0

def run_experiment(config_file="configs/advanced.cfg"):
    """Run NLM experiment with custom config"""
    cmd = ["./build/nlm", "--config", config_file]
    subprocess.run(cmd)

if __name__ == "__main__":
    if build_nlm():
        print("Build successful!")
        if len(sys.argv) > 1 and sys.argv[1] == "run":
            run_experiment()
    else:
        print("Build failed!")
        sys.exit(1)
```

### 2. Experiment Manager

```python
import pynlm
from datetime import datetime
import json

class NLMExperimentManager:
    """Manage multiple NLM experiments with tracking"""
    
    def __init__(self):
        self.experiments = []
    
    def run_experiment(self, name, config, world_size=(10, 10), steps=500):
        """Run a single experiment"""
        print(f"Running experiment: {name}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=world_size[0], height=world_size[1], 
                       visionWidth=5, visionHeight=5)
        world.setAgentStart(2, 2)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        results = {
            'name': name,
            'timestamp': datetime.now().isoformat(),
            'config': config,
            'total_reward': 0,
            'final_firing_rate': 0,
            'steps': steps
        }
        
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect results
        results['total_reward'] = brain.getTotalSpikeCount() * 0.1
        results['final_firing_rate'] = brain.getAverageFiringRate()
        
        self.experiments.append(results)
        return results
    
    def save_results(self, filepath="experiments/results.json"):
        """Save experiment results to file"""
        with open(filepath, 'w') as f:
            json.dump(self.experiments, f, indent=2)
        print(f"Results saved to {filepath}")
    
    def load_results(self, filepath="experiments/results.json"):
        """Load experiment results from file"""
        with open(filepath, 'r') as f:
            self.experiments = json.load(f)
        print(f"Loaded {len(self.experiments)} experiments from {filepath}")

# Usage example
manager = NLMExperimentManager()

# Run multiple experiments
config1 = pynlm.createDefaultConfig()
config1.set("neuron_count", 100)
manager.run_experiment("Simple", config1, (15, 15), 300)

config2 = pynlm.createDefaultConfig()
config2.set("neuron_count", 500)
config2.set("plasticity_learning_rate", 0.1)
manager.run_experiment("Complex", config2, (20, 20), 500)

# Save results
manager.save_results()
```

---

## Visualization and Analysis Tools

### 1. Simple Visualization

```python
try:
    import matplotlib.pyplot as plt
    import numpy as np
    import pynlm
    
    # Create and run brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Collect spike data
    spikes = []
    for step in range(500):
        # Inject sensory input
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
        brain.step(step)
        spikes.append(brain.getFiringNeuronCount())
    
    # Create visualization
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
    
    # Spike count over time
    ax1.plot(spikes)
    ax1.set_title('Neuron Firing Activity')
    ax1.set_xlabel('Simulation Step')
    ax1.set_ylabel('Firing Neurons')
    ax1.grid(True, alpha=0.3)
    
    # Firing rate distribution
    ax2.hist(brain.getRegions()[0].getAllNeurons(), bins=50, alpha=0.7, edgecolor='black')
    ax2.set_title('Neuron Membrane Potential Distribution')
    ax2.set_xlabel('Membrane Potential (mV)')
    ax2.set_ylabel('Count')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('nlm_activity.png', dpi=150)
    plt.show()
    
    print("Visualization saved as 'nlm_activity.png'")
    
except ImportError:
    print("Matplotlib not available. Install with: pip install matplotlib")
```

### 2. Performance Monitoring

```python
import time
import pynlm

class NLMMonitor:
    """Monitor NLM performance and resource usage"""
    
    def __init__(self):
        self.metrics = []
    
    def run_monitored(self, brain, world, agent, steps):
        """Run simulation with detailed monitoring"""
        start_time = time.time()
        
        for step in range(steps):
            step_start = time.time()
            
            # Record before state
            neurons_before = brain.getTotalNeuronCount()
            spikes_before = brain.getTotalSpikeCount()
            
            # Run step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record after state
            neurons_after = brain.getTotalNeuronCount()
            spikes_after = brain.getTotalSpikeCount()
            
            # Calculate metrics
            step_time = time.time() - step_start
            firing_rate = brain.getAverageFiringRate()
            memory_usage = brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0
            
            self.metrics.append({
                'step': step,
                'step_time': step_time,
                'neurons': neurons_after,
                'spikes': spikes_after,
                'firing_rate': firing_rate,
                'memory_usage': memory_usage,
                'total_reward': brain.getTotalSpikeCount() * 0.1
            })
            
            if step % 100 == 0:
                print(f"Step {step}: {step_time*1000:.1f}ms, "
                      f"Firing: {firing_rate:.2f}, "
                      f"Memory: {memory_usage}")
        
        total_time = time.time() - start_time
        return total_time
    
    def generate_report(self):
        """Generate performance report"""
        if not self.metrics:
            return "No metrics collected"
        
        total_steps = len(self.metrics)
        avg_step_time = sum(m['step_time'] for m in self.metrics) / total_steps
        avg_firing_rate = sum(m['firing_rate'] for m in self.metrics) / total_steps
        
        report = f"""
=== NLM Performance Report ===
Total Steps: {total_steps}
Average Step Time: {avg_step_time*1000:.2f} ms
Average Firing Rate: {avg_firing_rate:.2f} Hz
Total Spikes Generated: {sum(m['spikes'] for m in self.metrics)}
Peak Memory Usage: {max(m['memory_usage'] for m in self.metrics)}
Total Simulation Time: {sum(m['step_time'] for m in self.metrics)*1000:.0f} ms

Performance Summary:
- Steps per second: {total_steps/(sum(m['step_time'] for m in self.metrics) or 1):.1f}
- Efficiency rating: {'Excellent' if avg_step_time < 0.01 else 'Good' if avg_step_time < 0.1 else 'Needs Optimization'}
"""
        return report

# Usage
monitor = NLMMonitor()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run with monitoring
total_time = monitor.run_monitored(brain, world, agent, 200)
print(monitor.generate_report())
```

---

## API Reference (Advanced Usage)

### Enhanced Configuration

```python
# Configuration operations
config = pynlm.createDefaultConfig()

# Get various types
neuron_count = config.getOr<int64_t>("neuron_count", 1000)
connection_prob = config.getOr<double>("connection_probability", 0.1)
learning_rate = config.getOr<double>("plasticity_learning_rate", 0.01)

# Set with validation
success = config.setValidated("neuron_count", "500")
if success:
    print("Config updated successfully")
else:
    print("Invalid configuration value")

# Export/Import
json_str = config.saveToJson()
config2 = pynlm.createDefaultConfig()
config2.loadFromJson(json_str)
```

### Agent Brain Extensions

```python
# Enhanced agent capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning with parameters
agent.enableRewardModulation(True, lambda r: r * 1.1, 0.1)
agent.enableCuriosity(True, 0.3, 2.0)
agent.enableDevelopment(True, 0.05)

# Get advanced metrics
print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")

# Control development
agent.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
print(f"Development stage: {agent.getDevelopmentalStage().name}")
```

### World Extensions

```python
# Enhanced world capabilities
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)

# Add objects with parameters
obj = pynlm.WorldObject()
obj.x = 15
obj.y = 15
obj.type = pynlm.WorldObjectType.Resource
obj.value = 100.0
obj.radius = 1.0
world.addObject(obj)

# Object interactions
print(f"Object at ({obj.x}, {obj.y}) is {obj.type.name}")
print(f"Is valid position: {world.isValidPosition(20, 20)}")

# Get world state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent energy: {body.energy}")
```

---

## Next Steps for Advanced Users

### 1. Read Architecture Documentation

```bash
# After installation
pynlm documentation
# Or browse online at: https://nlm.readthedocs.io
```

### 2. Explore Advanced Examples

```python
# Look for examples in the repository
import pynlm

# Check available examples
examples = [
    "examples/learning_agent.py",
    "examples/visualization.py",
    "examples/experiment_runner.py",
    "examples/performance_monitor.py"
]

for example in examples:
    print(f"Example: {example}")
```

### 3. Extend the Framework

```python
# Create custom neuron types
class CustomNeuron(pynlm.Neuron):
    def __init__(self, neuron_id):
        super().__init__(neuron_id)
        self.custom_property = 0.0
    
    def custom_update(self):
        # Add custom learning rules
        self.custom_property += 0.01

# Create custom world objects
class InteractiveObject(pynlm.WorldObject):
    def __init__(self, x, y, value):
        super().__init__(x, y, pynlm.WorldObjectType.Resource, value)
        self.interaction_count = 0
    
    def interact(self):
        self.interaction_count += 1
        return self.interaction_count * self.value
```

---

## Troubleshooting Guide

### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure motor commands are being decoded properly

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters

**"Performance is slow"**
- Reduce neuron count for testing
- Decrease simulation steps
- Disable expensive features like visualization
- Use smaller world size

### Installation Issues

**Missing dependencies**
```bash
# Install core dependencies
pip install numpy scipy

# Install development dependencies
pip install pytest pytest-cov black flake8 mypy

# Install advanced features
pip install torch matplotlib jupyter ipython
```

**Build issues**
```bash
# Rebuild with verbose output
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
make -j4
```

---

## Quick Command Reference

| Task | Command |
|------|---------|
| Create simple brain | `brain = pynlm.createBrain(pynlm.createDefaultConfig())` |
| Initialize brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get stats | `brain.getTotalSpikeCount()` |
| See world | `percept = world.getSensoryPercept()` |
| Make action | `action = agent.decodeMotorCommand()` |
| Do action | `world.applyMotorCommand(action, time)` |
| Enable learning | `agent.enableRewardModulation(True)` |
| Save config | `config.saveToFile("myconfig.json")` |
| Load config | `config.loadFromFile("myconfig.json")` |

---

**Enjoy building intelligent agents with NLM!**

For advanced usage, explore the architecture documentation and experiment with the examples provided.
