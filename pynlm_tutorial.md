# pynlm Tutorial: From Beginner to Advanced

This tutorial provides a comprehensive guide to using the pynlm Python bindings,
from basic usage to advanced features and techniques.

## Overview

This tutorial covers:

1. **Basic Usage** - Getting started with NLM
2. **Advanced Features** - Enhanced APIs and functionality
3. **Learning and Training** - Reinforcement learning and curiosity
4. **Performance Optimization** - Optimizing your simulations
5. **Real-World Applications** - Practical use cases
6. **Troubleshooting** - Common issues and solutions

## Installation

### Prerequisites

```bash
# Ensure you have Python 3.8 or higher
python --version
# Should output: Python 3.8.x or higher

# Install pip if not already installed
python -m ensurepip

# Install pynlm
pip install pynlm

# For development version
pip install -e .
```

### Quick Setup

```python
# After installation, you can start using pynlm
import pynlm

print("pynlm version:", pynlm.__version__ if hasattr(pynlm, '__version__') else 'Latest')
print("Setup complete!")
```

## Section 1: Basic Usage

### 1.1 Creating a Brain

The brain is the central neural simulation component. Let's start with the simplest possible brain.

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getTotalRegionCount()} regions")
```

**Key Concepts:**
- **Brain**: Virtual neural system that can think and learn
- **Config**: Configuration settings for the brain
- **Default Config**: Pre-defined settings for common use cases

### 1.2 Initializing the Brain

```python
import pynlm

# Create and initialize a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Brain initialized successfully!")
print(f"Ready to simulate with {brain.getTotalNeuronCount()} neurons")
```

**Note:** Always call `initialize()` before using the brain!

### 1.3 Running a Simple Simulation

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
print("Starting simulation...")
for step in range(10):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Total spikes after simulation: {brain.getTotalSpikeCount()}")
```

### 1.4 Working with World and Agent

```python
import pynlm

# Create the complete system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run the brain-world-agent loop
for step in range(20):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Make decision
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Step {step}: Action {action}, "
          f"Neurons firing: {brain.getFiringNeuronCount()}")

print("Simulation complete!")
```

## Section 2: Advanced Features

### 2.1 Enhanced Brain Creation

#### Using Configuration Dictionary

```python
import pynlm

# Create configuration using dictionary
config_dict = {
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.15,
    "random_seed": 42,
    "plasticity_learning_rate": 0.01
}

brain = pynlm.createBrainWithConfig(config_dict)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Using learning rate: {pynlm.getConfigValue('plasticity_learning_rate')}")
```

#### Parameterized Brain Creation

```python
import pynlm

# Create brain with specific parameters
brain = pynlm.createBrainWithParameters(
    neuronCount=1000,
    regionCount=3,
    connectionProb=0.12,
    seed=12345
)
brain.initialize()

print(f"Parameterized brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Regions: {brain.getRegionCount()}")
```

### 2.2 Convenience Functions

```python
import pynlm

# Use convenience functions for common scenarios
print("=== Demo Brain ===")
demo_brain = pynlm.createDemoBrain()
demo_brain.initialize()
for i in range(10):
    demo_brain.step(i)

print(f"Demo brain simulation completed with {demo_brain.getTotalSpikeCount()} spikes")

print("\n=== Test Brain ===")
test_brain = pynlm.createTestBrain()
test_brain.initialize()
for i in range(20):
    test_brain.step(i)

print(f"Test brain simulation completed with {test_brain.getTotalSpikeCount()} spikes")
```

### 2.3 Advanced Statistics

```python
import pynlm

# Create brain and collect comprehensive statistics
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(50):
    brain.step(i, i * 0.001)

# Get comprehensive statistics
stats = brain.getStatistics()
print("=== Brain Statistics ===")
for key, value in stats.items():
    if isinstance(value, dict):
        print(f"{key}:")
        for sub_key, sub_value in value.items():
            print(f"  {sub_key}: {sub_value}")
    else:
        print(f"{key}: {value}")

# Get regional statistics
region_stats = brain.getRegionStats()
print("\n=== Regional Statistics ===")
for region_id, region_info in region_stats.items():
    print(f"Region {region_id.value}:")
    for key, value in region_info.items():
        print(f"  {key}: {value}")
```

## Section 3: Learning and Training

### 3.1 Enabling Advanced Learning Features

```python
import pynlm

# Create complete system with advanced learning
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning features
print("Enabling advanced learning features...")
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

print("Learning features enabled successfully!")
```

### 3.2 Reinforcement Learning

```python
import pynlm

# Create learning environment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Add training resources
print("Adding training resources...")
pynlm.addResourceToWorld(world, 3.0, 3.0, 50.0)
pynlm.addResourceToWorld(world, 7.0, 7.0, 30.0)
pynlm.addResourceToWorld(world, 2.0, 8.0, 20.0)

pynlm.addHazardToWorld(world, 5.0, 5.0, -10.0)

# Run learning simulation
print("\nStarting reinforcement learning...")
for episode in range(10):
    world.reset()
    agent.reset()
    brain.reset()
    brain.initialize()
    
    episode_reward = 0.0
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward with prediction error
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        episode_reward += result.reward
        
        if step % 20 == 0:
            print(f"  Step {step}: Reward {result.reward:.2f}, "
                  f"Neuromodulation {agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}")
    
    print(f"Episode {episode}: Total reward = {episode_reward:.2f}")

print("\nLearning simulation complete!")
print(f"Average neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Average curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Current developmental stage: {agent.getDevelopmentalStage()}")
```

### 3.3 Curiosity-Driven Exploration

```python
import pynlm

# Create agent with curiosity for exploration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

print("=== Curiosity-Driven Exploration Test ===")
print("The agent will explore the environment using curiosity...")

# Add diverse resources to encourage exploration
resources = [(5, 5, 10), (15, 15, 15), (5, 15, 8), (15, 5, 12)]
hazards = [(10, 10, -20)]

for rx, ry, value in resources:
    pynlm.addResourceToWorld(world, rx, ry, value)
    print(f"Added resource at ({rx}, {ry}) with value {value}")

for hx, hy, damage in hazards:
    pynlm.addHazardToWorld(world, hx, hy, damage)
    print(f"Added hazard at ({hx}, {hy}) with damage {damage}")

# Run curiosity-driven simulation
for episode in range(5):
    world.reset()
    agent.reset()
    brain.reset()
    brain.initialize()
    
    print(f"\nEpisode {episode}:")
    
    for step in range(80):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        if step % 20 == 0:
            print(f"  Step {step}: {action}, "
                  f"Reward {result.reward:.2f}, "
                  f"Curiosity {agent.getCuriosityLevel():.3f}")

print("\nExploration complete!")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final neuromodulation: {agent.getNeuromodulationLevel():.3f}")
```

## Section 4: Performance Optimization

### 4.1 Performance Benchmarking

```python
import pynlm
import time

print("=== Performance Benchmarking ===")

# Create brain for benchmarking
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Warm-up phase
print("Warming up...")
for i in range(100):
    brain.step(i, i * 0.001)

# Benchmark phase
print("Running benchmark...")
start_time = time.time()

benchmark_steps = 10000
for i in range(benchmark_steps):
    brain.step(i, i * 0.001)

end_time = time.time()

duration_ms = (end_time - start_time) * 1000
steps_per_second = (benchmark_steps * 1000.0) / duration_ms

print(f"Benchmark Results:")
print(f"  Steps completed: {benchmark_steps}")
print(f"  Duration: {duration_ms:.2f} ms")
print(f"  Steps per second: {steps_per_second:.1f}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Neurons: {brain.getTotalNeuronCount()}")

# Run comprehensive performance test
print("\n=== Comprehensive Performance Test ===")
benchmark_results = pynlm.benchmarkBrain(brain, steps=5000)

print("Performance metrics:")
for key, value in benchmark_results.items():
    print(f"  {key}: {value}")
```

### 4.2 Multi-Step Simulation

```python
import pynlm

print("=== Multi-Step Simulation ===")

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run multiple simulation steps efficiently
print("Running 1000 simulation steps...")

# Using stepMultiple for efficiency
brain.stepMultiple(startStep=0, numSteps=1000, timestep=0.001)

print("Multi-step simulation completed!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Run with different parameters
print("\n=== Different Simulation Parameters ===")

# Fast simulation with fewer steps
fast_brain = pynlm.createBrain(pynlm.createDefaultConfig())
fast_brain.initialize()

for i in range(500):
    fast_brain.step(i, i * 0.001)

print(f"Fast simulation: {fast_brain.getTotalSpikeCount()} spikes in 500 steps")

# Slow simulation with detailed output
slow_brain = pynlm.createBrain(pynlm.createDefaultConfig())
slow_brain.initialize()

for i in range(1000):
    slow_brain.step(i, i * 0.0005)
    
    if i % 100 == 0:
        print(f"  Step {i}: {slow_brain.getFiringNeuronCount()} neurons firing, "
              f"{slow_brain.getTotalSpikeCount()} total spikes")
```

### 4.3 Memory Efficiency

```python
import pynlm
import sys

print("=== Memory Efficiency Test ===")

# Create multiple brains to test memory usage
brains = []
for i in range(5):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brains.append(brain)
    
    # Run some simulation
    for j in range(100):
        brain.step(j, j * 0.001)
    
    # Get memory usage
    mem_usage = sys.getsizeof(brain)
    print(f"Brain {i+1}: {mem_usage} bytes")

print(f"Total memory for 5 brains: {sum(sys.getsizeof(b) for b in brains)} bytes")

# Clean up
brains.clear()
print("Memory test completed and cleaned up!")
```

## Section 5: Real-World Applications

### 5.1 Simple Maze Navigation

```python
import pynlm

def navigate_maze(brain_size=200, max_steps=500):
    """Simulate a brain navigating through a simple maze"""
    print(f"=== Maze Navigation Simulation ===")
    print(f"Brain size: {brain_size}, Max steps: {max_steps}")
    
    # Create maze environment
    brain = pynlm.createBrainWithParameters(
        neuronCount=brain_size,
        regionCount=2,
        connectionProb=0.12,
        seed=42
    )
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create maze with walls and goal
    for x in range(0, 30, 3):
        pynlm.addHazardToWorld(world, x, 15, -100)
    
    for y in range(0, 30, 3):
        pynlm.addHazardToWorld(world, 15, y, -100)
    
    pynlm.addResourceToWorld(world, 5, 5, 50)  # Start position
    pynlm.addResourceToWorld(world, 25, 25, 100)  # Goal position
    
    print("Maze created with walls and goal!")
    
    # Run navigation simulation
    for step in range(max_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        if result.reward > 0:
            print(f"Step {step}: Found goal! Reward {result.reward}")
            break
        
        if step % 100 == 0:
            print(f"  Step {step}: Pos ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
                  f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    print(f"Navigation complete! Total spikes: {brain.getTotalSpikeCount()}")
    return brain.getTotalSpikeCount()

# Run maze navigation
spike_count = navigate_maze(brain_size=300, max_steps=1000)
print(f"Maze navigation simulation completed with {spike_count} total spikes")
```

### 5.2 Learning Environments

```python
import pynlm

def simulate_learning_environment(env_type="controlled", complexity="medium"):
    """Simulate different learning environments"""
    print(f"=== Learning Environment: {env_type} (complexity: {complexity}) ===")
    
    # Create environment-specific configuration
    if env_type == "controlled":
        config_dict = {
            "neuron_count": 500,
            "region_count": 2,
            "connection_probability": 0.1,
            "random_seed": 42,
            "plasticity_learning_rate": 0.01
        }
    elif env_type == "uncontrolled":
        config_dict = {
            "neuron_count": 1000,
            "region_count": 3,
            "connection_probability": 0.15,
            "random_seed": 123,
            "plasticity_learning_rate": 0.05
        }
    elif env_type == "dynamic":
        config_dict = {
            "neuron_count": 800,
            "region_count": 4,
            "connection_probability": 0.2,
            "random_seed": 456,
            "plasticity_learning_rate": 0.03
        }
    
    # Create brain and world
    brain = pynlm.createBrainWithConfig(config_dict)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning based on environment type
    if complexity == "high":
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
    else:
        agent.enableRewardModulation(False)
        agent.enableCuriosity(False)
        agent.enableDevelopment(False)
        agent.enableStructuralPlasticity(False)
    
    # Add environment-specific challenges
    print("Creating environment challenges...")
    if env_type == "controlled":
        # Simple, predictable environment
        for x in range(10, 30, 5):
            pynlm.addResourceToWorld(world, x, 20, 10)
        pynlm.addResourceToWorld(world, 35, 35, 100)  # Easy goal
    
    elif env_type == "uncontrolled":
        # Complex, unpredictable environment
        import random
        for _ in range(20):
            x = random.randint(5, 35)
            y = random.randint(5, 35)
            pynlm.addHazardToWorld(world, x, y, -20)
        pynlm.addResourceToWorld(world, 20, 20, 80)  # Hard-to-find goal
    
    elif env_type == "dynamic":
        # Changing environment over time
        pynlm.addResourceToWorld(world, 10, 10, 30)
        pynlm.addResourceToWorld(world, 30, 30, 50)
        pynlm.addHazardToWorld(world, 20, 20, -30)
    
    print(f"Environment created with {env_type} characteristics!")
    
    # Run simulation
    for episode in range(5):
        print(f"\nEpisode {episode}:")
        
        world.reset()
        agent.reset()
        brain.reset()
        brain.initialize()
        
        episode_reward = 0.0
        
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, step * 0.1)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
            
            episode_reward += result.reward
            
            if step % 50 == 0:
                print(f"  Step {step}: Reward {result.reward:.2f}, "
                      f"Stage {agent.getDevelopmentalStage()}")
        
        print(f"  Episode reward: {episode_reward:.2f}")
    
    return brain, world, agent

# Test different learning environments
environments = ["controlled", "uncontrolled", "dynamic"]
for env in environments:
    simulate_learning_environment(env, "high")
```

### 5.3 Performance Analysis

```python
import pynlm
import matplotlib.pyplot as plt

def analyze_performance():
    """Analyze performance across different configurations"""
    print("=== Performance Analysis ===")
    
    # Test different neuron counts
    neuron_counts = [100, 500, 1000, 2000]
    times = []
    memory_usages = []
    
    for neurons in neuron_counts:
        print(f"\nTesting with {neurons} neurons...")
        
        start_time = time.time()
        
        brain = pynlm.createBrainWithParameters(
            neuronCount=neurons,
            regionCount=2,
            connectionProb=0.15,
            seed=42
        )
        brain.initialize()
        
        # Run simulation
        for i in range(200):
            brain.step(i, i * 0.001)
        
        end_time = time.time()
        
        times.append(end_time - start_time)
        memory_usages.append(brain.getTotalNeuronCount())  # Rough memory estimate
        
        print(f"  Time: {times[-1]:.3f}s, Neurons: {neurons}")
    
    # Create performance comparison plot
    plt.figure(figsize=(12, 5))
    
    plt.subplot(1, 2, 1)
    plt.plot(neuron_counts, times, 'bo-')
    plt.xlabel('Neuron Count')
    plt.ylabel('Time (seconds)')
    plt.title('Performance vs. Neuron Count')
    plt.grid(True)
    
    plt.subplot(1, 2, 2)
    plt.plot(neuron_counts, memory_usages, 'ro-')
    plt.xlabel('Neuron Count')
    plt.ylabel('Neuron Count (memory proxy)')
    plt.title('Memory Usage vs. Neuron Count')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('performance_analysis.png')
    plt.show()
    
    print("\nPerformance analysis complete!")
    print("Results saved to 'performance_analysis.png'")
    
    return {
        'neuron_counts': neuron_counts,
        'times': times,
        'memory_usages': memory_usages
    }

if __name__ == "__main__":
    analyze_performance()
```

## Section 6: Troubleshooting

### 6.1 Common Issues

#### Issue 1: Brain Initialization Fails

**Symptoms:**
- Brain doesn't initialize
- Errors during setup
- Simulation crashes immediately

**Solutions:**

```python
import pynlm

# Solution 1: Check configuration
config = pynlm.createDefaultConfig()
if not config.has("neuron_count"):
    print("Error: neuron_count not set in configuration")
    exit(1)

# Solution 2: Use safe initialization
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Error: Brain initialization failed")
    print("Try using a smaller neuron count")
    exit(1)
```

#### Issue 2: Simulation Slow

**Symptoms:**
- Simulation takes too long
- High CPU usage
- Performance bottlenecks

**Solutions:**

```python
import pynlm
import time

# Solution 1: Use test brain for faster testing
test_brain = pynlm.createTestBrain()
start_time = time.time()
for i in range(100):
    test_brain.step(i)
end_time = time.time()
print(f"Test brain simulation: {end_time - start_time:.3f}s for 100 steps")

# Solution 2: Reduce neuron count
brain = pynlm.createBrainWithParameters(
    neuronCount=100,  # Reduced from 1000
    regionCount=1,
    connectionProb=0.1,
    seed=42
)
```

#### Issue 3: No Learning

**Symptoms:**
- Learning rates are zero
- No behavior changes
- Reward modulation not working

**Solutions:**

```python
import pynlm

# Solution: Enable learning features properly
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# CRITICAL: Enable all learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Learning features enabled!")
print(f"Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
```

### 6.2 Error Messages

#### Common Error: Configuration Not Found

```python
# Error: Configuration key 'neuron_count' not found
# Solution: Use createDefaultConfig() or set required keys
brain = pynlm.createBrain(pynlm.createDefaultConfig())
```

#### Common Error: Invalid Simulation Step

```python
# Error: Invalid simulation step
# Solution: Use stepMultiple for multiple steps or manage step count properly
brain.stepMultiple(startStep=0, numSteps=100)
```

#### Common Error: Memory Issues

```python
# Error: Memory allocation failed
# Solution: Use smaller configurations or process in batches
brain = pynlm.createBrainWithParameters(
    neuronCount=500,  # Start with smaller size
    regionCount=1,
    connectionProb=0.1,
    seed=42
)
```

### 6.3 Debugging Tools

```python
import pynlm

def debug_simulation(brain, world, agent, max_steps=100):
    """Debug simulation with detailed logging"""
    print("=== Debug Simulation ===")
    
    # Initial state check
    print(f"Initial state:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # Run debug simulation
    for step in range(max_steps):
        # Check world state
        print(f"\nStep {step}:")
        print(f"  World time: {world.getSimulationTime():.3f}")
        print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"  Vision size: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
        
        # Update world
        world.update(0.1)
        
        # Check percept
        percept = world.getSensoryPercept()
        print(f"  Vision intensity: {sum(percept.getVision()) / len(percept.getVision()):.3f}")
        
        # Process input
        agent.processSensoryInput(percept)
        brain.step(step, step * 0.1)
        
        # Check results
        action = agent.decodeMotorCommand()
        print(f"  Action: {action}")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Execute action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        print(f"  Result: Reward {result.reward:.2f}, Success {result.success}")
        
        # Apply learning
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        if step % 20 == 0:
            print(f"  --- Checkpoints at step {step} ---")

# Use debug simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

debug_simulation(brain, world, agent, max_steps=50)
```

### 6.4 Performance Monitoring

```python
import pynlm
import time

def monitor_performance(brain, world, agent, steps=100):
    """Monitor performance during simulation"""
    print("=== Performance Monitoring ===")
    
    step_times = []
    memory_usage = []
    
    for step in range(steps):
        # Start timing
        step_start = time.time()
        
        # Run one step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate step time
        step_time = time.time() - step_start
        step_times.append(step_time)
        
        if step % 20 == 0:
            print(f"Step {step}: {step_time*1000:.1f}ms, "
                  f"FPS: {1.0/step_time:.1f}, "
                  f"Neurons: {brain.getFiringNeuronCount()}")
    
    # Calculate statistics
    avg_step_time = sum(step_times) / len(step_times)
    min_step_time = min(step_times)
    max_step_time = max(step_times)
    fps = 1.0 / avg_step_time
    
    print(f"\nPerformance Statistics:")
    print(f"  Average step time: {avg_step_time*1000:.1f}ms")
    print(f"  Min step time: {min_step_time*1000:.1f}ms")
    print(f"  Max step time: {max_step_time*1000:.1f}ms")
    print(f"  Average FPS: {fps:.1f}")
    print(f"  Total steps: {steps}")
    
    return {
        'avg_step_time': avg_step_time,
        'min_step_time': min_step_time,
        'max_step_time': max_step_time,
        'fps': fps,
        'step_times': step_times
    }

# Use performance monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

performance_data = monitor_performance(brain, world, agent, steps=50)
```

## Section 7: Advanced Usage

### 7.1 Custom Configuration Factory

```python
import pynlm

class NLMConfigurator:
    """Factory class for creating optimized configurations"""
    
    @staticmethod
    def create_starting_config():
        """Create a configuration suitable for beginners"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
        config.set("random_seed", 42)
        config.set("plasticity_learning_rate", 0.005)
        return config
    
    @staticmethod
    def create_expert_config():
        """Create a configuration suitable for experts"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 5000)
        config.set("region_count", 5)
        config.set("connection_probability", 0.2)
        config.set("random_seed", 12345)
        config.set("plasticity_learning_rate", 0.02)
        return config
    
    @staticmethod
    def create_learning_config(learning_style="balanced"):
        """Create a configuration optimized for specific learning styles"""
        config = pynlm.createDefaultConfig()
        
        if learning_style == "fast":
            config.set("neuron_count", 1000)
            config.set("plasticity_learning_rate", 0.05)
        elif learning_style == "slow":
            config.set("neuron_count", 300)
            config.set("plasticity_learning_rate", 0.001)
        else:  # balanced
            config.set("neuron_count", 800)
            config.set("plasticity_learning_rate", 0.01)
        
        config.set("region_count", 2)
        config.set("connection_probability", 0.15)
        config.set("random_seed", 999)
        
        return config

# Usage examples
print("=== Configuration Factory Examples ===")

# Beginner configuration
beginner_config = NLMConfigurator.create_starting_config()
beginner_brain = pynlm.createBrain(beginner_config)
beginner_brain.initialize()

print(f"Beginner brain: {beginner_brain.getTotalNeuronCount()} neurons")

# Expert configuration
expert_config = NLMConfigurator.create_expert_config()
expert_brain = pynlm.createBrain(expert_config)
expert_brain.initialize()

print(f"Expert brain: {expert_brain.getTotalNeuronCount()} neurons")

# Learning style configurations
for style in ["fast", "balanced", "slow"]:
    style_config = NLMConfigurator.create_learning_config(style)
    style_brain = pynlm.createBrain(style_config)
    style_brain.initialize()
    print(f"{style.capitalize()} learning brain: {style_brain.getTotalNeuronCount()} neurons")
```

### 7.2 Simulation Orchestration

```python
import pynlm

class NLMSimulator:
    """Orchestrates complex NLM simulations"""
    
    def __init__(self, config):
        self.config = config
        self.history = []
        self.performance_data = []
    
    def create_system(self):
        """Create the brain-world-agent system"""
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world based on simulation type
        if hasattr(self.config, 'get'):
            world_size = self.config.getOr<int64_t>("world_width", 20)
        else:
            world_size = 20
        
        self.world.configure(world_size, world_size, 8, 8)
        
        # Initialize system
        self.world.reset()
        self.agent.initialize(self.world)
        
        return self.brain, self.world, self.agent
    
    def run_simulation(self, steps, learning_enabled=True, verbose=True):
        """Run a simulation with comprehensive monitoring"""
        if learning_enabled:
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
        
        print(f"Starting simulation: {steps} steps")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            start_time = time.time()
            self.brain.step(step, step * 0.1)
            step_time = time.time() - start_time
            
            # Make decision
            action = self.agent.decodeMotorCommand()
            
            # Execute action
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            self.agent.applyRewardModulation(result.reward, self.agent.getPredictionError())
            
            # Record history
            self.history.append({
                'step': step,
                'action': action,
                'reward': result.reward,
                'success': result.success,
                'neurons_firing': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'neuromodulation': self.agent.getNeuromodulationLevel(),
                'curiosity': self.agent.getCuriosityLevel(),
                'step_time': step_time
            })
            
            self.performance_data.append(step_time)
            
            if verbose and step % 50 == 0:
                print(f"Step {step}: Action {action}, Reward {result.reward:.2f}, "
                      f"Neurons {self.brain.getFiringNeuronCount()}, "
                      f"Time {step_time*1000:.1f}ms")
        
        print(f"Simulation completed: {steps} steps")
        return self.history
    
    def analyze_results(self):
        """Analyze simulation results"""
        if not self.history:
            print("No simulation history available")
            return
        
        total_steps = len(self.history)
        
        # Calculate statistics
        total_rewards = sum(h['reward'] for h in self.history)
        avg_reward = total_rewards / total_steps
        max_reward = max(h['reward'] for h in self.history)
        min_reward = min(h['reward'] for h in self.history)
        
        avg_neurons_firing = sum(h['neurons_firing'] for h in self.history) / total_steps
        max_neurons_firing = max(h['neurons_firing'] for h in self.history)
        
        avg_step_time = sum(h['step_time'] for h in self.history) / total_steps
        
        # Calculate learning indicators
        final_neuromodulation = self.history[-1]['neuromodulation']
        final_curiosity = self.history[-1]['curiosity']
        
        print("=== Simulation Analysis ===")
        print(f"Total steps: {total_steps}")
        print(f"Average reward: {avg_reward:.3f}")
        print(f"Max reward: {max_reward:.3f}")
        print(f"Min reward: {min_reward:.3f}")
        print(f"Average neurons firing: {avg_neurons_firing:.1f}")
        print(f"Max neurons firing: {max_neurons_firing}")
        print(f"Average step time: {avg_step_time*1000:.1f}ms")
        print(f"Average FPS: {1.0/avg_step_time:.1f}")
        print(f"Final neuromodulation: {final_neuromodulation:.3f}")
        print(f"Final curiosity: {final_curiosity:.3f}")
        
        return {
            'total_steps': total_steps,
            'avg_reward': avg_reward,
            'max_reward': max_reward,
            'min_reward': min_reward,
            'avg_neurons_firing': avg_neurons_firing,
            'max_neurons_firing': max_neurons_firing,
            'avg_step_time': avg_step_time,
            'final_neuromodulation': final_neuromodulation,
            'final_curiosity': final_curiosity
        }

# Use the simulator
print("=== Simulation Orchestration Example ===")

config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("random_seed", 42)

simulator = NLMSimulator(config)
brain, world, agent = simulator.create_system()

# Run simulation
simulation_history = simulator.run_simulation(steps=200, learning_enabled=True, verbose=True)

# Analyze results
results = simulator.analyze_results()
```

## Section 8: Appendices

### A. API Reference

#### Core Classes

| Class | Methods | Description |
|-------|---------|-------------|
| `Brain` | `initialize()`, `step()`, `getStatistics()` | Central neural simulation brain |
| `Config` | `loadFromFile()`, `saveToFile()`, `getKeys()` | Configuration management |
| `SimpleWorld` | `update()`, `getSensoryPercept()`, `applyMotorCommand()` | Virtual world simulation |
| `AgentBrain` | `initialize()`, `processSensoryInput()`, `decodeMotorCommand()` | Agent brain interface |

#### Factory Functions

| Function | Description |
|----------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `createBrain(config)` | Create brain with configuration |
| `createSimpleWorld()` | Create simple world |
| `createAgentBrain(brain)` | Create agent brain |
| `createDemoBrain()` | Create demo brain |
| `createTestBrain()` | Create test brain |

#### Enhanced Functions

| Function | Description |
|----------|-------------|
| `createBrainWithConfig(dict)` | Create brain from dictionary |
| `createBrainWithParameters(...)` | Create brain with specific parameters |
| `runSimulation(brain, steps)` | Run complete simulation |
| `benchmarkBrain(brain, steps)` | Benchmark brain performance |
| `trainWithRewards(...)` | Train agent with rewards |

### B. Common Parameters

#### Configuration Keys

| Key | Type | Default | Description |
|----|------|---------|-------------|
| `neuron_count` | int | 500 | Number of neurons |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | float | 0.15 | Connection probability |
| `random_seed` | int | 42 | Random seed for reproducibility |
| `plasticity_learning_rate` | float | 0.01 | Learning rate for plasticity |

#### Action Types

| Type | Value | Description |
|------|-------|-------------|
| `MoveForward` | 0 | Move forward |
| `MoveBackward` | 1 | Move backward |
| `TurnLeft` | 2 | Turn left |
| `TurnRight` | 3 | Turn right |
| `Look` | 4 | Look in current direction |
| `Interact` | 5 | Interact with environment |
| `Wait` | 6 | Do nothing |

### C. Performance Guidelines

#### Recommended Settings for Different Use Cases

| Use Case | Neuron Count | Regions | Learning Rate | Expected Performance |
|----------|--------------|---------|---------------|---------------------|
| Development | 200-500 | 1-2 | 0.001-0.01 | Fast, exploratory |
| Research | 1000-5000 | 3-10 | 0.01-0.05 | Comprehensive, detailed |
| Real-time | 100-300 | 1 | 0.005 | Fast, interactive |
| Batch Processing | 500-2000 | 2-5 | 0.02 | Balanced, efficient |

#### Memory Usage Estimates

- Small brain (100 neurons): ~10-20 MB
- Medium brain (500 neurons): ~50-100 MB
- Large brain (1000 neurons): ~100-200 MB
- Expert brain (5000 neurons): ~500-1000 MB

### D. Troubleshooting Table

| Symptom | Cause | Solution |
|---------|-------|----------|
| Brain doesn't initialize | Invalid configuration | Use createDefaultConfig() |
| Slow simulation | Too many neurons | Reduce neuron count |
| No learning | Learning features disabled | Enable with enable*() methods |
| Memory issues | Configuration not saved | Use saveToFile() |
| Crashes | Invalid parameters | Use validated factory functions |

### E. Example Scripts

#### Basic Simulation

```bash
# Save as: basic_simulation.py
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)
    print(f"Step {i}: {brain.getTotalSpikeCount()} spikes")

print("Basic simulation complete!")
```

#### Learning Simulation

```bash
# Save as: learning_simulation.py
import pynlm

# Create learning environment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run learning simulation
for episode in range(10):
    world.reset()
    agent.reset()
    brain.reset()
    brain.initialize()
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())

print("Learning simulation complete!")
```

#### Performance Test

```bash
# Save as: performance_test.py
import pynlm
import time

print("=== Performance Test ===")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

start_time = time.time()
for i in range(1000):
    brain.step(i, i * 0.001)
end_time = time.time()

duration = end_time - start_time
steps_per_second = 1000.0 / duration

print(f"Performance: {steps_per_second:.1f} steps/second")
print(f"Duration: {duration:.3f}s")
```

## Conclusion

This tutorial provides a comprehensive guide to using pynlm, from basic usage to advanced features. It covers:

1. **Basic Usage**: Getting started with brain, world, and agent creation
2. **Advanced Features**: Enhanced APIs, configuration management, and statistics
3. **Learning and Training**: Reinforcement learning, curiosity, and development
4. **Performance Optimization**: Benchmarking, monitoring, and optimization
5. **Real-World Applications**: Practical use cases and scenarios
6. **Troubleshooting**: Common issues and solutions
7. **Advanced Usage**: Custom configurations, simulation orchestration, and API references

The pynlm Python bindings provide a powerful, flexible framework for neural network simulation with comprehensive support for both beginners and advanced users. With the enhanced features and improved documentation, you can now create sophisticated neural simulations with ease.

**Happy simulating!** 🚀
