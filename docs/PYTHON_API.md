# NLM Python API Reference

This document provides comprehensive documentation for the NLM Python bindings.
It expands on the easy_usage.md guide with more detailed examples, error handling guidance, and advanced usage patterns.

## Introduction

The NLM (Neural Learning Machine) Python API provides full access to the C++ neural simulation framework with a clean, Pythonic interface. The API is designed to be both simple for beginners and powerful for advanced users.

## Quick Start Examples

### Basic Brain Simulation

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### Complete Agent with World

```python
import pynlm

# Setup the brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)  # Update world physics
    percept = world.getSensoryPercept()  # Get sensory input
    agent.processSensoryInput(percept)  # Process input
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Select action
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute action
    
    if step % 20 == 0:
        print(f"Step {step}: Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")

print("Simulation completed!")
```

### Error Handling Examples

```python
import pynlm

# Example 1: Graceful error handling with try-except
try:
    config = pynlm.createConfigFromFile("config.json")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain created successfully")
except (pynlm.InvalidArgumentError, ValueError) as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# Example 2: Validating configuration before use
config = pynlm.createDefaultConfig()
config.set("neuronCount", 1000)
config.set("learningRate", 0.01)

# Validate configuration
errors = pynlm.validateConfigFromDict(config)
if errors:
    print("Configuration validation failed:")
    for error in errors:
        print(f"  - {error}")
else:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Valid configuration used successfully")

# Example 3: Checking for specific configuration keys
if config.has("neuronCount") and config.has("learningRate"):
    neuron_count = config.get("neuronCount")
    learning_rate = config.get("learningRate")
    print(f"Configured with {neuron_count} neurons and learning rate {learning_rate}")
```

### Advanced Configuration

```python
import pynlm

# Create a custom configuration
default_config = pynlm.createDefaultConfig()
custom_config = pynlm.createConfigFromDict({
    "neuronCount": 2000,
    "connectionProbability": 0.15,
    "learningRate": 0.005,
    "initialExcitatoryWeight": 0.4,
    "initialInhibitoryWeight": -0.2,
    "world.width": 30,
    "world.height": 30,
    "world.visionWidth": 10,
    "world.visionHeight": 10,
    "development.enabled": True,
    "neuromodulation.rewardModulation": True,
    "neuromodulation.curiosity": True,
    "memory.workingMemorySize": 128,
    "memory.episodicMemoryCapacity": 1000
})

# Compare configurations
brain1 = pynlm.createBrain(default_config)
brain2 = pynlm.createBrain(custom_config)

# Check differences
differences = pynlm.compareBrains(brain1, brain2)
if differences:
    print("Differences found:")
    for diff in differences:
        print(f"  - {diff}")
else:
    print("Configurations produce equivalent brains")

# Save and load brain state
brain.save("my_brain.nnet")
loaded_brain = pynlm.importBrainState("my_brain.nnet")
print(f"Loaded brain has {loaded_brain.getTotalNeuronCount()} neurons")
```

### Developmental Learning

```python
import pynlm

# Create brain with development enabled
config = pynlm.createDefaultConfig()
config.set("development.enabled", True)
config.set("development.plasticity.initial", 1.0)
config.set("development.plasticity.adult", 0.2)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Starting developmental learning simulation...")
for step in range(300):
    world.update(0.1)
    
    # Random action selection to explore
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide random feedback
    reward = 1.0 if step % 50 == 0 else 0.0
    predicted = 0.0
    agent.applyRewardModulation(reward, predicted)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 60 == 0:
        print(f"Step {step}: Developmental stage = {agent.getDevelopmentalStage()}")
        print(f"  Plasticity modifier: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")

print("Developmental simulation completed!")
```

### Memory Systems

```python
import pynlm

# Create configuration with enhanced memory systems
config = pynlm.createDefaultConfig()
config.set("memory.workingMemorySize", 256)
config.set("memory.episodicMemoryCapacity", 5000)
config.set("memory.associativeMemorySize", 128)

brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Demonstrate memory operations
print("Working memory size:", working_memory.getSize())
print("Episodic memory capacity:", episodic_memory.getCapacity())
print("Associative memory patterns:", associative_memory.getPatternCount())

# Store and retrieve episodic memory
import time
brain.receiveSensoryInput(world.getSensoryPercept())
episodic_memory.storeExperience("exploration", percept, time.time())

# Get related experiences
related = episodic_memory.recallExperience("exploration")
print(f"Recalled {len(related)} related experiences")
```

## Error Handling Guide

### Common Exceptions

The NLM Python bindings can raise several types of exceptions:

1. **`pynlm.InvalidArgumentError`**: Thrown when configuration files cannot be read or parsed
2. **`pynlm.RuntimeError`**: Thrown for runtime errors during brain operations
3. **`KeyError`**: Thrown when accessing non-existent configuration keys
4. **`TypeError`**: Thrown when type conversions fail

### Best Practices

```python
import pynlm

def safe_simulation(steps=100, config_file=None):
    """Safely run a simulation with proper error handling."""
    try:
        # Load configuration
        if config_file:
            config = pynlm.createConfigFromFile(config_file)
        else:
            config = pynlm.createDefaultConfig()
        
        # Validate configuration
        errors = config.validate()
        if errors:
            raise pynlm.InvalidArgumentError(
                f"Configuration validation failed: {errors[0]}"
            )
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(steps):
            try:
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
            except Exception as e:
                print(f"Warning: Step {step} failed: {e}")
                # Continue simulation despite individual step failures
                continue
        
        return brain, world, agent
        
    except pynlm.InvalidArgumentError as e:
        print(f"Configuration error: {e}")
        return None, None, None
    except pynlm.RuntimeError as e:
        print(f"Runtime error: {e}")
        return None, None, None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None, None, None

# Usage example
brain, world, agent = safe_simulation(steps=200, config_file="my_config.json")
if brain is not None:
    print("Simulation completed successfully!")
else:
    print("Simulation failed - check error messages above")
```

## Advanced Usage Patterns

### 1. Custom Agent Behaviors

```python
import pynlm
import random

class CustomAgent:
    def __init__(self, brain, world):
        self.agent_brain = pynlm.createAgentBrain(brain)
        self.world = world
        self.agent_brain.initialize(world)
        
        # Enable specific capabilities
        self.agent_brain.enableRewardModulation(True)
        self.agent_brain.enableCuriosity(True)
        self.agent_brain.enableDevelopment(True)
        
        # Configure custom parameters
        self.agent_brain.setVisionScale(3.0)  # Different sensitivity
        self.agent_brain.setActivityThreshold(0.3)  # Lower activation threshold
        self.agent_brain.setExplorationChance(0.5)  # Higher exploration
    
    def run_episode(self, max_steps=100):
        """Run a single episode with custom logic."""
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get percept
            percept = self.world.getSensoryPercept()
            
            # Custom preprocessing (e.g., noise injection)
            if random.random() < 0.1:  # 10% chance of adding noise
                percept = self.add_noise_to_percept(percept)
            
            # Process input
            self.agent_brain.processSensoryInput(percept)
            
            # Custom action selection (e.g., curiosity-based)
            action = self.custom_action_selection()
            
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Custom reward calculation
            reward = self.calculate_custom_reward()
            predicted = self.agent_brain.getPredictionError()
            self.agent_brain.applyRewardModulation(reward, predicted)
            
            # Update development
            self.agent_brain.updateDevelopment(0.1)
            
            # Check termination conditions
            if self.world.getAgentBody().health <= 0:
                break
                
            if step % 20 == 0:
                self.log_episode_progress(step)
        
        return step
    
    def add_noise_to_percept(self, percept):
        """Add controlled noise to percept for exploration."""
        # This is just an example - modify as needed
        return percept  # Placeholder
    
    def custom_action_selection(self):
        """Custom action selection logic."""
        # This is just an example - modify as needed
        return self.agent_brain.decodeMotorCommand()
    
    def calculate_custom_reward(self):
        """Custom reward calculation."""
        # This is just an example - modify as needed
        return 1.0  # Fixed reward
    
    def log_episode_progress(self, step):
        """Log episode progress."""
        agent_body = self.world.getAgentBody()
        print(f"Step {step}: Pos({agent_body.x:.1f},{agent_body.y:.1f}), "
              f"Health:{agent_body.health:.1f}, "
              f"Curiosity:{self.agent_brain.getCuriosityLevel():.3f}")

# Usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.setAgentStart(12, 12)

custom_agent = CustomAgent(brain, world)
custom_agent.run_episode(max_steps=150)
```

### 2. Configuration Templates

```python
import pynlm

# Predefined configuration templates
CONFIG_TEMPLATES = {
    "explorer": {
        "neuronCount": 1000,
        "learningRate": 0.01,
        "connectionProbability": 0.12,
        "development.enabled": True,
        "neuromodulation.curiosity": True,
        "neuromodulation.rewardModulation": True,
        "memory.workingMemorySize": 128,
    },
    "exploiter": {
        "neuronCount": 500,
        "learningRate": 0.02,
        "connectionProbability": 0.08,
        "development.enabled": False,
        "neuromodulation.curiosity": False,
        "neuromodulation.rewardModulation": True,
        "memory.workingMemorySize": 64,
    },
    "balanced": {
        "neuronCount": 1500,
        "learningRate": 0.008,
        "connectionProbability": 0.15,
        "development.enabled": True,
        "neuromodulation.curiosity": True,
        "neuromodulation.rewardModulation": True,
        "memory.workingMemorySize": 256,
        "memory.episodicMemoryCapacity": 2000,
    },
    "high_performance": {
        "neuronCount": 2000,
        "learningRate": 0.005,
        "connectionProbability": 0.18,
        "development.enabled": True,
        "neuromodulation.curiosity": True,
        "neuromodulation.rewardModulation": True,
        "memory.workingMemorySize": 512,
        "memory.episodicMemoryCapacity": 10000,
        "plasticity.enabled": True,
        "plasticity.rate": 1.0,
    }
}

def create_config_from_template(template_name):
    """Create configuration from a named template."""
    template = CONFIG_TEMPLATES.get(template_name.lower())
    if not template:
        available = list(CONFIG_TEMPLATES.keys())
        raise ValueError(f"Unknown template '{template_name}'. Available templates: {available}")
    
    config = pynlm.createDefaultConfig()
    for key, value in template.items():
        config.set(key, value)
    
    return config

# Usage
exploration_config = create_config_from_template("explorer")
brain = pynlm.createBrain(exploration_config)
brain.initialize()
```

### 3. Performance Monitoring

```python
import pynlm
import time
import psutil
import os

class PerformanceMonitor:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.step_times = []
        self.memory_usage = []
        self.cpu_usage = []
        self.start_time = time.time()
        self.step_count = 0
    
    def start_step(self):
        """Mark the beginning of a simulation step."""
        self.step_start_time = time.time()
        self.process = psutil.Process(os.getpid())
    
    def end_step(self):
        """Mark the end of a simulation step and record metrics."""
        step_time = time.time() - self.step_start_time
        self.step_times.append(step_time)
        
        # Record memory usage
        memory_mb = self.process.memory_info().rss / 1024 / 1024
        self.memory_usage.append(memory_mb)
        
        # Record CPU usage
        cpu_percent = self.process.cpu_percent()
        self.cpu_usage.append(cpu_percent)
        
        self.step_count += 1
    
    def get_statistics(self):
        """Get performance statistics."""
        if not self.step_times:
            return {}
        
        return {
            'total_steps': self.step_count,
            'total_time': time.time() - self.start_time,
            'avg_step_time': sum(self.step_times) / len(self.step_times),
            'min_step_time': min(self.step_times),
            'max_step_time': max(self.step_times),
            'avg_memory_mb': sum(self.memory_usage) / len(self.memory_usage),
            'max_memory_mb': max(self.memory_usage),
            'avg_cpu_percent': sum(self.cpu_usage) / len(self.cpu_usage),
            'neurons_count': self.brain.getTotalNeuronCount(),
            'synapses_count': self.brain.getTotalSynapseCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
        }
    
    def print_report(self):
        """Print performance report."""
        stats = self.get_statistics()
        
        print("\n" + "="*50)
        print("PERFORMANCE MONITORING REPORT")
        print("="*50)
        print(f"Total steps executed: {stats['total_steps']}")
        print(f"Total simulation time: {stats['total_time']:.2f}s")
        print(f"Average step time: {stats['avg_step_time']*1000:.2f}ms")
        print(f"Step time range: {stats['min_step_time']*1000:.2f} - {stats['max_step_time']*1000:.2f}ms")
        print(f"Average memory usage: {stats['avg_memory_mb']:.1f} MB")
        print(f"Peak memory usage: {stats['max_memory_mb']:.1f} MB")
        print(f"Average CPU usage: {stats['avg_cpu_percent']:.1f}%")
        print(f"Neural network size: {stats['neurons_count']} neurons, {stats['synapses_count']} synapses")
        print(f"Average firing rate: {stats['firing_rate']:.2f}")
        print("="*50)

# Usage example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

monitor = PerformanceMonitor(brain, world, agent)

# Run simulation with monitoring
for step in range(100):
    monitor.start_step()
    
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    monitor.end_step()
    
    if step % 20 == 0:
        monitor.print_report()

monitor.print_report()
```

## Troubleshooting Guide

### Common Issues and Solutions

1. **Brain not firing neurons**
   - **Cause**: Insufficient input or incorrect configuration
   - **Solution**: Check configuration parameters, verify sensory input is being processed

2. **Simulation too slow**
   - **Cause**: Large brain network or inefficient implementation
   - **Solution**: Reduce neuron count, use development stages to reduce plasticity, profile performance

3. **Agent not taking actions**
   - **Cause**: Motor neuron activity below threshold
   - **Solution**: Adjust activity threshold, enable curiosity, check motor neuron groups

4. **Memory systems not working**
   - **Cause**: Configuration missing memory system settings
   - **Solution**: Set memory-related configuration parameters

5. **Configuration validation errors**
   - **Cause**: Invalid or inconsistent configuration values
   - **Solution**: Check configuration files, use validateConfig() function

### Diagnostic Commands

```python
import pynlm

# System diagnostic
print("Running NLM system diagnostics...")
pynlm.printSystemInfo()

# Check available features
features = pynlm.getFeatureInfo()
print("\nAvailable features:")
for feature, available in features.items():
    status = "✓" if available else "✗"
    print(f"  {status} {feature}")

# Test configuration
print("\nTesting default configuration...")
config = pynlm.createDefaultConfig()
errors = config.validate()
if errors:
    print("Configuration validation failed:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Default configuration is valid")

# Test brain creation
print("\nTesting brain creation...")
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created successfully with {brain.getTotalNeuronCount()} neurons")
except Exception as e:
    print(f"✗ Brain creation failed: {e}")
```

## Migration Guide

### From C++ to Python

If you're migrating from the C++ API to Python:

1. **Configuration**: Replace `Config::loadFromFile()` with `pynlm.createConfigFromFile()`
2. **Brain Creation**: Replace `std::make_shared<Brain>(config)` with `pynlm.createBrain(config)`
3. **Agent Creation**: Replace custom agent brain setup with `pynlm.createAgentBrain(brain)`
4. **World Setup**: Replace `SimpleWorld()` constructor with `pynlm.createSimpleWorld()`
5. **Error Handling**: Use Python try-except blocks with specific exception types
6. **Type Safety**: Python handles type conversions automatically

### Key Differences

| C++ Feature | Python Equivalent |
|-------------|------------------|
| `std::shared_ptr<Config>` | `std::shared_ptr<Config>` (Python wrapper) |
| Manual memory management | Automatic garbage collection |
| `Brain::step(step)` | `brain.step(step)` |
| `AgentBrain::decodeMotorCommand()` | `agent.decodeMotorCommand()` |
| `Config::set(key, value)` | `config.set(key, value)` |
| `Config::get<T>(key)` | `config.get(key)` (type inference) |
| C++ exceptions | Python exceptions |

## Conclusion

The NLM Python API provides a comprehensive, well-documented interface for neural simulation. This reference guide covers the essential functionality, error handling, and advanced usage patterns to help you get the most out of the NLM framework.

For the most up-to-date information, always refer to the official NLM documentation and the pybind11 module documentation.
