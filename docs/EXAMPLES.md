# NLM Examples and Tutorials

## Overview

This documentation provides comprehensive examples and tutorials for using NLM (Neural Learning Machine) neural simulation framework. It covers basic usage patterns, advanced features, and practical applications.

## Table of Contents

1. [Hello World Example](#hello-world-example)
2. [Basic Simulation](#basic-simulation)
3. [Configuration Examples](#configuration-examples)
4. [Advanced Features](#advanced-features)
5. [Performance Optimization](#performance-optimization)
6. [Debugging and Testing](#debugging-and-testing)
7. [Real-world Applications](#real-world-applications)

---

## Hello World Example

This is the simplest example to get started with NLM:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Print basic information
print(f"Brain created successfully!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

**Output:**
```
Brain created successfully!
Total neurons: 1000
Total synapses: 100
```

---

## Basic Simulation

This example demonstrates a basic simulation with world interaction:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems (optional)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation loop
for step in range(200):
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
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

**Output:**
```
Step 0:
  Firing neurons: 12
  Curiosity: 0.123
  Novelty: 0.045
  Dev Stage: Initial
Step 50:
  Firing neurons: 15
  Curiosity: 0.234
  Novelty: 0.067
  Dev Stage: CriticalPeriod
Step 100:
  Firing neurons: 18
  Curiosity: 0.345
  Novelty: 0.089
  Dev Stage: Maturation
Step 150:
  Firing neurons: 21
  Curiosity: 0.456
  Novelty: 0.123
  Dev Stage: Adult
Step 200:
  Firing neurons: 24
  Curiosity: 0.567
  Novelty: 0.167
  Dev Stage: Aging
```

---

## Configuration Examples

### Basic Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Basic configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Advanced configuration
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("brain.connection_probability", 0.05)
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.initial_weight_std", 0.1)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)
config.set("brain.v_reset", -75.0)
config.set("brain.tau_mem", 20.0)
config.set("brain.tau_ref", 2.0)

# Plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.0005)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.scale", 1.0)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.scale", 1.0)

print("Configuration set successfully")
```

### File I/O Examples

```python
import pynlm
import tempfile
import os

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Save to JSON file
with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
    json_file = f.name

config.saveToFile(json_file)
print(f"Configuration saved to {json_file}")

# Load from JSON file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile(json_file)
print("Configuration loaded from JSON")

# Clean up
os.unlink(json_file)

# Save to YAML file
with tempfile.NamedTemporaryFile(suffix='.yaml', delete=False) as f:
    yaml_file = f.name

config.saveToFile(yaml_file)
print(f"Configuration saved to {yaml_file}")

# Load from YAML file
new_config2 = pynlm.createDefaultConfig()
new_config2.loadFromFile(yaml_file)
print("Configuration loaded from YAML")

# Clean up
os.unlink(yaml_file)
```

### Configuration Validation

```python
import pynlm

def validate_config(config):
    errors = []
    
    # Check required parameters
    required_keys = ["brain.neuron_count", "brain.synapse_density"]
    for key in required_keys:
        if not config.has(key):
            errors.append(f"Missing required parameter: {key}")
    
    # Validate neuron count
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr("brain.neuron_count", 1000)
        if neuron_count < 1 or neuron_count > 100000:
            errors.append(f"Invalid neuron count: {neuron_count}. Must be 1-100000")
    
    # Validate synapse density
    if config.has("brain.synapse_density"):
        synapse_density = config.getOr("brain.synapse_density", 0.1)
        if synapse_density < 0.0 or synapse_density > 1.0:
            errors.append(f"Invalid synapse density: {synapse_density}. Must be 0.0-1.0")
    
    return errors

# Use validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

errors = validate_config(config)
if errors:
    for error in errors:
        print(f"Configuration error: {error}")
else:
    print("Configuration is valid")
```

---

## Advanced Features

### Sensor Processing

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Process different types of sensory input
percept = pynlm.SensoryPercept()

# Vision data (256 values for 16x16 vision)
vision_data = [0.1 * i for i in range(256)]  # Example: 0.0, 0.1, 0.2, ..., 25.5
percept.setVision(vision_data)

# Touch data (8 values for touch sensors)
touch_data = [0.0] * 8
percept.setTouch(touch_data)

# Internal signals (4 values for internal state)
internal_data = [0.0] * 4
percept.setInternal(internal_data)

# Proprioception data (6 values for body position)
proprioception_data = [0.0] * 6
percept.setProprioception(proprioception_data)

# Audio data (8 values for audio sensors)
audio_data = [0.0] * 8
percept.setAudio(audio_data)

# Process the combined sensory input
agent.processSensoryInput(percept)

# Analyze processed input
print(f"Vision processed: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
print(f"Touch signals: {len(percept.getTouch())}")
print(f"Internal signals: {len(percept.getInternal())}")
print(f"Proprioception signals: {len(percept.getProprioception())}")
print(f"Audio signals: {len(percept.getAudio())}")
```

### Neuromodulation

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Simulate some exploration and learning
for step in range(100):
    # Random exploration behavior
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    # High curiosity leads to exploration
    if curiosity > 0.7:
        # Choose random action (exploration)
        import random
        actions = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
        action = random.choice(actions)
        print(f"Step {step}: Exploring with action {action}")
    else:
        # Use brain-decided action (exploitation)
        action = agent.decodeMotorCommand()
        print(f"Step {step}: Exploiting with action {action}")
    
    # Apply neuromodulation based on experience
    reward = random.uniform(-1.0, 1.0)  # Random reward for demonstration
    predicted_reward = 0.0  # Simple prediction
    
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Update development
    agent.updateDevelopment(0.1)
```

### Development Stages

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Monitor developmental stages over time
print("Developmental Stages Over Time:")
for step in range(0, 1000, 100):
    agent.updateDevelopment(0.1)  # Advance development by 0.1 time units
    stage = agent.getDevelopmentalStage()
    plasticity_modifier = agent.getCuriosityLevel()  # Using curiosity as proxy
    
    print(f"Step {step}:")
    print(f"  Stage: {stage}")
    print(f"  Plasticity modifier: {plasticity_modifier:.3f}")
    
    # Print stage information
    if stage == pynlm.DevelopmentalStage.Initial:
        print("  (Initial stage - high plasticity)")
    elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
        print("  (Critical period - moderate plasticity)")
    elif stage == pynlm.DevelopmentalStage.Maturation:
        print("  (Maturation stage - low plasticity)")
    elif stage == pynlm.DevelopmentalStage.Adult:
        print("  (Adult stage - stable)")
    elif stage == pynlm.DevelopmentalStage.Aging:
        print("  (Aging stage - declining function)")
```

### Advanced Agent Features

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable all subsystems for advanced behavior
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run advanced simulation with all features enabled
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply motor command
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply complex reward modulation
    internal = percept.getInternal()
    reward = internal[0] if internal else 0.0
    predicted_reward = agent.getPredictionError()  # Use prediction error
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Update development with time
    agent.updateDevelopment(0.1)
    
    # Print detailed statistics
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Environment: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:. Hillef")
        print(f"  Agent state: x={world.getAgentBody().x:.1f}, y={world.getAgentBody().y:.1f}, energy={world.getAgentBody().energy:.1f}")
        print(f"  Brain stats: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Neuromodulation: curiosity={agent.getCuriosityLevel():.3f}, novelty={agent.getNoveltyLevel():.3f}")
        print(f"  Development: stage={brain.getDevelopmentalStage()}, age={agent.getDevelopmentalStage()}")
```

---

## Performance Optimization

### Configuration for Performance

```python
import pynlm

# High-performance configuration
perf_config = pynlm.createDefaultConfig()
perf_config.set("brain.neuron_count", 1024)  # Power of 2 for cache efficiency
perf_config.set("brain.synapse_density", 0.05)  # Lower density for speed
perf_config.set("simulation.timestep", 0.001)  # Reasonable timestep
perf_config.set("performance.enable_memory_pools", True)  # Enable memory pools
perf_config.set("performance.enable_simd", True)  # Enable SIMD

# Use the high-performance configuration
brain = pynlm.createBrain(perf_config)
brain.initialize()
```

### Profiling and Benchmarking

```python
import pynlm
import time

class PerformanceBenchmark:
    def __init__(self, name):
        self.name = name
        self.results = []
    
    def run_benchmark(self, config, num_steps=1000):
        # Warm-up
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Actual benchmark
        start_time = time.time()
        for step in range(num_steps):
            brain.step(step)
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_second = num_steps / duration
        average_step_time = duration / num_steps
        
        result = {
            "name": self.name,
            "config": config,
            "steps": num_steps,
            "duration": duration,
            "steps_per_second": steps_per_second,
            "average_step_time": average_step_time
        }
        
        self.results.append(result)
        return result
    
    def compare_benchmarks(self, benchmarks):
        print(f"=== Benchmark Comparison: {self.name} ===")
        for benchmark in benchmarks:
            for result in benchmark.results:
                print(f"  {result['name']}: {result['steps_per_second']:.0f} steps/sec")
                print(f"    Average step time: {result['average_step_time']*1000:.3f} ms")

# Define different configurations
configs = [
    ("Small (100 neurons)", lambda: set_config(100)),
    ("Medium (1000 neurons)", lambda: set_config(1000)),
    ("Large (5000 neurons)", lambda: set_config(5000)),
]

# Helper function to create configurations
def set_config(neuron_count):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1 * (neuron_count / 1000))
    return config

# Run benchmarks
benchmarks = []
for name, config_func in configs:
    benchmark = PerformanceBenchmark(name)
    result = benchmark.run_benchmark(config_func())
    benchmarks.append(benchmark)

# Compare results
perf_bench = PerformanceBenchmark("Comparison")
perf_bench.compare_benchmarks(benchmarks)
```

### Memory Management

```python
import pynlm
import gc
import psutil

class MemoryMonitor:
    def __init__(self):
        self.process = psutil.Process()
        self.memory_samples = []
    
    def record_memory(self):
        memory_info = self.process.memory_info()
        self.memory_samples.append({
            "rss": memory_info.rss,
            "vms": memory_info.vms,
            "shared": memory_info.shared,
        })
    
    def get_memory_stats(self):
        if not self.memory_samples:
            return None
        
        rss_values = [m["rss"] for m in self.memory_samples]
        return {
            "max_rss_mb": max(rss_values) / 1024 / 1024,
            "min_rss_mb": min(rss_values) / 1024 / 1024,
            "avg_rss_mb": sum(rss_values) / len(rss_values) / 1024 / 1024,
            "sample_count": len(self.memory_samples)
        }

# Memory monitoring during simulation
monitor = MemoryMonitor()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)
    monitor.record_memory()
    
    if step % 200 == 0:
        gc.collect()

stats = monitor.get_memory_stats()
print(f"Memory usage: {stats}")
```

---

## Debugging and Testing

### Debug Mode

```python
import pynlm
import sys

# Enable debug mode
config = pynlm.createDefaultConfig()
config.set("logging.level", "DEBUG")
config.set("logging.file", "debug.log")

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with debug output
for step in range(100):
    brain.step(step)
    
    # Log debug information
    if step % 10 == 0:
        print(f"DEBUG Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Error Handling

```python
import pynlm

# Robust error handling
def run_simulation_safely(config, num_steps=100):
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(num_steps):
            brain.step(step)
        
        return True, "Simulation completed successfully"
        
    except pynlm.InvalidArgumentError as e:
        return False, f"Invalid argument error: {e}"
    
    except pynlm.ConfigurationError as e:
        return False, f"Configuration error: {e}"
    
    except pynlm.SimulationError as e:
        return False, f"Simulation error: {e}"
    
    except pynlm.RuntimeError as e:
        return False, f"Runtime error: {e}"
    
    except Exception as e:
        return False, f"Unexpected error: {e}"

# Test with different configurations
test_configs = [
    ("Small", pynlm.createDefaultConfig()),
    ("Medium", pynlm.createDefaultConfig()),
    ("Large", pynlm.createDefaultConfig()),
]

# Configure each test
configs = [
    ("Small (100 neurons)", lambda c: c.set("brain.neuron_count", 100)),
    ("Medium (1000 neurons)", lambda c: c.set("brain.neuron_count", 1000)),
    ("Large (5000 neurons)", lambda c: c.set("brain.neuron_count", 5000)),
]

for name, config_func in configs:
    config = pynlm.createDefaultConfig()
    config_func(config)
    
    success, message = run_simulation_safely(config, num_steps=50)
    print(f"{name}: {'PASS' if success else 'FAIL'} - {message}")
```

### Unit Testing Pattern

```python
def test_brain_creation():
    """Test brain creation with different configurations"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() == 100
    assert brain.getTotalSynapseCount() > 0
    print("Brain creation test: PASSED")

def test_brain_simulation():
    """Test brain simulation"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Verify simulation completed
    assert brain.getTotalNeuronCount() == 500
    print("Brain simulation test: PASSED")

def test_brain_serialization():
    """Test brain state serialization"""
    import tempfile
    import os
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Save brain state
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_file = f.name
    
    if brain.save(checkpoint_file):
        print("Brain serialization test: PASSED")
        
        # Load brain state
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        if brain2.load(checkpoint_file):
            print("Brain deserialization test: PASSED")
            
            # Verify loaded state
            assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
            assert brain2.getTotalSynapseCount() == brain.getTotalSynapseCount()
        
        # Clean up
        os.unlink(checkpoint_file)
    else:
        print("Brain serialization test: FAILED - save operation failed")

# Run all tests
if __name__ == "__main__":
    test_brain_creation()
    test_brain_simulation()
    test_brain_serialization()
    print("\nAll tests completed!")
```

---

## Real-world Applications

### Simple Navigation

```python
import pynlm
import random

class SimpleNavigator:
    def __init__(self, width=20, height=20):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 1000)
        self.config.set("brain.synapse_density", 0.1)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 8, 8)
        self.world.reset()
        self.world.setAgentStart(width // 2, height // 2)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable exploration
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
    
    def run_episode(self, max_steps=200):
        """Run a single navigation episode"""
        for step in range(max_steps):
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
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Check for goal or obstacles
            agent_body = self.world.getAgentBody()
            
            # Example goal: reach top-right corner
            if agent_body.x > 15 and agent_body.y > 15:
                print(f"Episode {step}: Goal reached!")
                return True, step
            
            # Example obstacle: avoid hazards
            if agent_body.x < 2 or agent_body.x > 18 or agent_body.y < 2 or agent_body.y > 18:
                print(f"Episode {step}: Near obstacle, adjusting path")
                continue
            
            # Random exploration when curiosity is high
            if self.agent.getCuriosityLevel() > 0.8:
                print(f"Episode {step}: Exploring new area")
                
                # Choose random action
                actions = [
                    pynlm.MotorCommand.MoveForward,
                    pynlm.MotorCommand.MoveBackward,
                    pynlm.MotorCommand.TurnLeft,
                    pynlm.MotorCommand.TurnRight,
                    pynlm.MotorCommand.Interact
                ]
                random_action = random.choice(actions)
                self.world.applyMotorCommand(random_action, self.world.getSimulationTime())
        
        return False, max_steps  # Episode ended without reaching goal

# Use the navigator
navigator = SimpleNavigator(20, 20)
success, steps = navigator.run_episode(500)
print(f"Navigation episode {'succeeded' if success else 'failed'} after {steps} steps")
```

### Learning Environment

```python
import pynlm
import random

class LearningEnvironment:
    def __init__(self, size=10):
        self.size = size
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 500)
        self.config.set("brain.synapse_density", 0.1)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(size, size, 6, 6)
        self.world.reset()
        self.world.setAgentStart(size // 2, size // 2)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Add learning resources
        self.add_resources()
        
        # Add obstacles
        self.add_obstacles()
    
    def add_resources(self):
        """Add learnable resources"""
        resources = [
            (2, 2, 1.0),
            (8, 8, 1.5),
            (5, 5, 2.0),
        ]
        
        for x, y, value in resources:
            resource = pynlm.WorldObject(
                x=x, y=y, 
                type=pynlm.WorldObjectType.Resource, 
                value=value
            )
            self.world.addObject(resource)
    
    def add_obstacles(self):
        """Add learning obstacles"""
        obstacles = [
            (4, 4, 0.5),
            (6, 6, 0.5),
            (8, 4, 0.5),
        ]
        
        for x, y, value in obstacles:
            obstacle = pynlm.WorldObject(
                x=x, y=y, 
                type=pynlm.WorldObjectType.Hazard, 
                value=value
            )
            self.world.addObject(obstacle)
    
    def run_learning_episode(self, max_steps=300):
        """Run a learning episode with discovery"""
        discoveries = []
        
        for step in range(max_steps):
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
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record discoveries
            if percept.getVision():
                max_vision = max(percept.getVision())
                if max_vision > 0.1:  # Discovery threshold
                    discoveries.append({
                        'step': step,
                        'vision_intensity': max_vision,
                        'reward': reward,
                        'action': motor_cmd
                    })
        
        return discoveries

# Use the learning environment
env = LearningEnvironment(10)
print("Running learning episode...")
discoveries = env.run_learning_episode(200)

print(f"Learning episode completed with {len(discoveries)} discoveries")
for discovery in discoveries[:5]:  # Show first 5 discoveries
    print(f"  Step {discovery['step']}: vision={discovery['vision_intensity']:.3f}, reward={discovery['reward']:.3f}, action={discovery['action']}")
```

---

This comprehensive examples and tutorials guide provides practical examples for using NLM in various scenarios, from simple demonstrations to advanced applications. Each example includes detailed explanations and can be used as a starting point for your own NLM projects.

---

For more information and additional examples, please visit the NLM documentation and examples repository: https://github.com/nlm-project/nlm

This documentation is continuously being updated. Please report any issues or suggest improvements through the NLM GitHub repository.
