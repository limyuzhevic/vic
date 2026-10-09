# NLM Python Bindings - Advanced Usage Guide

## Overview

This guide provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings, including all the improvements and new features added in this version.

## Key Improvements

### 1. Enhanced Configuration System

#### New Configuration Methods

**Config.set()** - Set configuration values with type safety:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set configuration values with type hints
config.set("brain.neuron_count", 2000)              # Integer
config.set("brain.synapse_density", 0.15)           # Float  
config.set("brain.enable_curiosity", True)          # Boolean
config.set("brain.description", "My neural network") # String

# Set with specific source (advanced usage)
config.set("brain.learning_rate", 0.001, "runtime")
```

**Config.get()** - Get configuration values with type safety:

```python
# Get configuration values with automatic type conversion
neuron_count = config.get("brain.neuron_count", 1000)      # Returns int
learning_rate = config.get("brain.learning_rate", 0.001)  # Returns float
enable_curiosity = config.get("brain.enable_curiosity", True)  # Returns bool
description = config.get("brain.description", "")            # Returns string
```

**Config.getOr()** - Get configuration values with fallback defaults:

```python
# Get values with fallback defaults
max_neurons = config.getOr("brain.neuron_count", 1000)
learning_rate = config.getOr("brain.learning_rate", 0.001)
enable_features = config.getOr("brain.enable_features", True)
```

**Config.remove()** - Remove configuration keys:

```python
config.remove("brain.temporary_setting")
```

### 2. Enhanced AgentBrain State Access

#### New State Access Methods

```python
import pynlm

# Create agent with default settings
agent = pynlm.createAgentWithDefaults()
world = pynlm.createStandardWorld()
agent.initialize(world)

# Access detailed system state
stats = agent.getAllStats()
print(f"Curiosity level: {stats['curiosity']:.3f}")
print(f"Novelty level: {stats['novelty']:.3f}")
print(f"Prediction error: {stats['prediction_error']:.3f}")
print(f"Neuromodulation level: {stats['neuromodulation']:.3f}")
print(f"Developmental stage: {stats['developmental_stage']}")
print(f"Features enabled: reward={stats['reward_enabled']}, "
      f"plasticity={stats['plasticity_enabled']}, "
      f"development={stats['development_enabled']}, "
      f"curiosity={stats['curiosity_enabled']}")

# Enable/disable all features at once
agent.enableAllFeatures(True)  # Enable all
agent.enableAllFeatures(False) # Disable all
```

### 3. Convenience Factory Functions

#### Pre-configured World Creation

```python
import pynlm

# Create a standard world with common settings
world = pynlm.createStandardWorld()
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Vision size: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
```

#### Pre-configured Agent Creation

```python
import pynlm

# Create agent with default settings
agent = pynlm.createAgentWithDefaults()
brain = agent.getBrain()

# Configure brain with common settings
brain_config = pynlm.createDefaultConfig()
brain_config.set("brain.neuron_count", 3000)
brain_config.set("brain.synapse_density", 0.2)
brain_config.set("brain.enable_curiosity", True)
brain_config.set("brain.enable_reward_modulation", True)

brain = pynlm.createBrain(brain_config)
agent = pynlm.createAgentBrain(brain)
```

### 4. Complete Episode Management

#### High-level episode running

```python
import pynlm

# Create all components
def create_complete_agent(num_neurons=2000):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", num_neurons)
    config.set("brain.enable_curiosity", True)
    config.set("brain.enable_reward_modulation", True)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createStandardWorld()
    
    agent.initialize(world)
    return agent, world

# Run complete episode
agent, world = create_complete_agent(3000)
pynlm.runEpisode(agent, world, 1000)

print(f"Episode completed!")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
```

### 5. Configuration Validation and Error Handling

#### Safe configuration access

```python
import pynlm

config = pynlm.createDefaultConfig()

# Safe access with validation
def safe_get_int(config, key, default=0):
    if not config.has(key):
        return default
    try:
        return config.get(key)
    except (ValueError, TypeError):
        return default

# Example usage
max_energy = safe_get_int(config, "world.max_energy", 100)
print(f"Max energy: {max_energy}")

# Configuration validation
def validate_config(config):
    errors = []
    
    if config.has("brain.neuron_count"):
        neuron_count = config.get("brain.neuron_count")
        if neuron_count < 100 or neuron_count > 100000:
            errors.append(f"Neuron count {neuron_count} out of valid range [100, 100000]")
    
    if config.has("brain.synapse_density"):
        density = config.get("brain.synapse_density")
        if density < 0.01 or density > 0.5:
            errors.append(f"Synapse density {density} out of valid range [0.01, 0.5]")
    
    return errors

config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
validation_errors = validate_config(config)
if validation_errors:
    print("Configuration errors:", validation_errors)
else:
    print("Configuration is valid")
```

## Advanced Usage Patterns

### Pattern 1: Multi-agent Training

```python
import pynlm

class NLMTrainer:
    def __init__(self, num_agents=5, num_neurons=1500):
        self.agents = []
        self.worlds = []
        
        for i in range(num_agents):
            # Create agent with slight variations
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", num_neurons + i * 200)
            config.set("brain.synapse_density", 0.1 + i * 0.02)
            config.set("brain.enable_curiosity", i % 2 == 0)
            
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createStandardWorld()
            
            agent.initialize(world)
            self.agents.append(agent)
            self.worlds.append(world)
    
    def train_agents(self, num_steps=500):
        """Train all agents in parallel"""
        for step in range(num_steps):
            for agent, world in zip(self.agents, self.worlds):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                agent.getBrain()->step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                agent.applyRewardModulation(0.0f, 0.0f)
    
    def get_agent_stats(self):
        """Get statistics for all agents"""
        all_stats = []
        for agent in self.agents:
            all_stats.append(agent.getAllStats())
        return all_stats

# Usage
trainer = NLMTrainer(num_agents=3)
trainer.train_agents(num_steps=200)
print("Training completed!")
```

### Pattern 2: Configuration Management

```python
import pynlm
import json
import os

class ConfigurationManager:
    def __init__(self, config_file="nlm_config.json"):
        self.config_file = config_file
        self.current_config = pynlm.createDefaultConfig()
    
    def load_config(self, file_path=None):
        """Load configuration from file"""
        path = file_path or self.config_file
        if os.path.exists(path):
            self.current_config.loadFromFile(path)
            return True
        return False
    
    def save_config(self, file_path=None):
        """Save configuration to file"""
        path = file_path or self.config_file
        self.current_config.saveToFile(path)
        return True
    
    def get_config_summary(self):
        """Get human-readable configuration summary"""
        return self.current_config.summary()
    
    def apply_preset(self, preset_name):
        """Apply a predefined configuration preset"""
        presets = {
            "minimal": {
                "brain.neuron_count": 500,
                "brain.synapse_density": 0.05,
                "brain.enable_curiosity": False,
                "brain.enable_reward_modulation": False
            },
            "balanced": {
                "brain.neuron_count": 2000,
                "brain.synapse_density": 0.15,
                "brain.enable_curiosity": True,
                "brain.enable_reward_modulation": True
            },
            "advanced": {
                "brain.neuron_count": 5000,
                "brain.synapse_density": 0.25,
                "brain.enable_curiosity": True,
                "brain.enable_reward_modulation": True,
                "brain.enable_structural_plasticity": True
            }
        }
        
        if preset_name in presets:
            for key, value in presets[preset_name].items():
                self.current_config.set(key, value)
            return True
        return False

# Usage
config_manager = ConfigurationManager()
config_manager.apply_preset("balanced")
config_manager.save_config("my_config.json")

# Load and use
config_manager.load_config("my_config.json")
print(config_manager.get_config_summary())
```

### Pattern 3: Simulation Monitoring

```python
import pynlm
import time

class SimulationMonitor:
    def __init__(self):
        self.history = []
        self.start_time = None
    
    def record_step(self, agent, world, step):
        """Record simulation state at each step"""
        record = {
            "step": step,
            "time": world.getSimulationTime(),
            "neurons_firing": agent.getBrain()->getFiringNeuronCount(),
            "total_spikes": agent.getBrain()->getTotalSpikeCount(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "neuromodulation": agent.getNeuromodulationLevel(),
            "developmental_stage": agent.getDevelopmentalStage(),
            "timestamp": time.time()
        }
        self.history.append(record)
    
    def get_summary(self):
        """Get summary of simulation"""
        if not self.history:
            return {}
        
        return {
            "total_steps": len(self.history),
            "final_curiosity": self.history[-1]["curiosity"],
            "final_novelty": self.history[-1]["novelty"],
            "max_curiosity": max(r["curiosity"] for r in self.history),
            "max_novelty": max(r["novelty"] for r in self.history),
            "final_development_stage": self.history[-1]["developmental_stage"]
        }
    
    def save_history(self, filename):
        """Save simulation history to file"""
        import json
        with open(filename, 'w') as f:
            json.dump(self.history, f, indent=2)
    
    def load_history(self, filename):
        """Load simulation history from file"""
        import json
        with open(filename, 'r') as f:
            self.history = json.load(f)

# Usage
monitor = SimulationMonitor()

# Example simulation loop
agent = pynlm.createAgentWithDefaults()
world = pynlm.createStandardWorld()
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    agent.getBrain()->step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    monitor.record_step(agent, world, step)

# Get results
summary = monitor.get_summary()
print(f"Simulation summary: {summary}")

# Save for later analysis
monitor.save_history("simulation_data.json")
```

## Migration Guide

### From Old API to New API

#### Configuration Changes

**Old:**
```python
# Only had load/save methods
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
config.saveToFile("config.json")
# No way to set/get individual values
```

**New:**
```python
# Full configuration API
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.enable_curiosity", True)
value = config.get("brain.neuron_count")
config.saveToFile("config.json")
```

#### AgentBrain Changes

**Old:**
```python
# Limited state access
stats = {
    "curiosity": agent.getCuriosityLevel(),
    "novelty": agent.getNoveltyLevel(),
    "prediction_error": agent.getPredictionError(),
    "neuromodulation": agent.getNeuromodulationLevel()
}
# No easy way to enable/disable all features
```

**New:**
```python
# Comprehensive state access and management
stats = agent.getAllStats()
print(f"Development stage: {stats['developmental_stage']}")
print(f"All features enabled: {stats['reward_enabled']} "
      f"{stats['plasticity_enabled']} "
      f"{stats['development_enabled']} "
      f"{stats['curiosity_enabled']}")

# Enable/disable all features at once
agent.enableAllFeatures(True)
```

#### New Convenience Functions

**New utility functions:**
- `pynlm.createStandardWorld()` - Create pre-configured world
- `pynlm.createBrainWithDefaults()` - Create brain with default settings
- `pynlm.createAgentWithDefaults()` - Create agent with default settings
- `pynlm.runEpisode(agent, world, num_steps)` - Run complete episode

## Performance Considerations

### Memory Management

The Python bindings use pybind11's smart pointer integration for automatic memory management. Be mindful of:

1. **Object Lifetime**: Python objects automatically manage C++ object lifetimes
2. **Return Values**: Some methods return references to internal objects; avoid holding references for long periods
3. **Configuration Copying**: Configuration objects are lightweight and can be copied freely

### Simulation Performance

```python
import pynlm
import time

# Optimized simulation setup
agent = pynlm.createAgentWithDefaults()
world = pynlm.createStandardWorld()
agent.initialize(world)

# Time simulation
start_time = time.time()
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    agent.getBrain()->step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

elapsed = time.time() - start_time
print(f"1000 steps in {elapsed:.2f} seconds ({1000/elapsed:.1f} steps/sec)")

# Profile specific operations
import cProfile, pstats

pr = cProfile.Profile()
pr.enable()

for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)

pr.disable()
stats = pstats.Stats(pr).sort_stats('cumulative')
stats.print_stats(10)
```

## Troubleshooting

### Common Issues

**1. Configuration Errors**
```python
# Check for configuration errors
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", -100)  # Invalid

if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")
    if value < 0:
        print("Warning: Invalid neuron count")
```

**2. Memory Issues**
```python
# Monitor memory usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

**3. Simulation Stability**
```python
# Add error handling to simulation loop
for step in range(1000):
    try:
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        agent.getBrain()->step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    except Exception as e:
        print(f"Error at step {step}: {e}")
        agent.reset()
        world.reset()
```

## Future Enhancements

### Planned Features

1. **Advanced Configuration Validation**: Built-in schema validation for configuration values
2. **Parallel Simulation Support**: Multi-threaded simulation capabilities
3. **Visualization Integration**: Built-in plotting and visualization tools
4. **Distributed Computing**: Support for distributed NLM simulations
5. **Hardware Acceleration**: GPU support for large-scale simulations
6. **Advanced Debugging**: Real-time simulation monitoring and debugging tools

## Conclusion

The enhanced NLM Python bindings now provide:

- **Complete Configuration API**: Full read/write access to configuration values
- **Comprehensive State Monitoring**: Easy access to all system state variables
- **Convenience Utilities**: Pre-configured factory functions for common use cases
- **Robust Error Handling**: Better validation and error reporting
- **Advanced Features**: Support for complex simulation scenarios

These improvements make the NLM framework more accessible to Python developers while maintaining the powerful computational capabilities of the C++ core.

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM Architecture](docs/ARCHITECTURE.md)
- [SCIENCE.md](docs/SCIENCE.md) - Scientific Background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment Descriptions