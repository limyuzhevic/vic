# NLM — 熙然: Easy Usage Guide

**Phase 6: Final Integration - Quick Start**

This guide provides simple, beginner-friendly examples for getting started with the enhanced NLM Python bindings.

## Table of Contents

1. [What You Can Do With NLM](#1-what-you-can-do-with-nlm)
2. [Quick Start Examples](#2-quick-start-examples)
3. [Installation](#3-installation)
4. [Basic Concepts](#4-basic-concepts)
5. [Common Tasks](#5-common-tasks)
6. [Troubleshooting](#6-troubleshooting)
7. [Next Steps](#7-next-steps)

## 1. What You Can Do With NLM

### 🚀 Quick Overview

NLM lets you:

- **Create neural networks** that can learn from experience
- **Simulate intelligent agents** that interact with environments
- **Run experiments** with measurable results
- **Analyze brain activity** with detailed metrics
- **Export/import brain states** for research
- **Build custom scenarios** for testing

### 🎯 Common Use Cases

1. **Research Simulation**: Study how neural networks learn and adapt
2. **Agent Development**: Create intelligent agents that explore and learn
3. **Performance Testing**: Benchmark different neural architectures
4. **Education**: Teach neural network concepts with visual simulations
5. **Prototyping**: Quickly test neural algorithms before implementing in other languages

## 2. Quick Start Examples

### Example 1: "Hello World" of NLM

```python
import pynlm

print("Creating your first NLM simulation...")

# Create the complete simulation ecosystem
ecosystem = pynlm.Factory.createSimulationEcosystem({
    'neuron_count': 100,  # Number of neurons
    'region_count': 1     # Number of brain regions
})

# Get the main components
brain = ecosystem['brain']
world = ecosystem['world']
agent = ecosystem['agent']

print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"✓ Created world {world.getWidth()}x{world.getHeight()} pixels")
print(f"✓ Created agent with curiosity enabled")

# Run a short simulation
brain.initialize()
for step in range(10):
    brain.step(step, step * 0.001)

print("✓ Simulation completed successfully!")
```

### Example 2: Learning Agent

```python
import pynlm

print("Building a learning agent...")

# Create agent with curiosity drive (will explore new things)
agent = pynlm.Factory.createSimulationEcosystem({
    'neuron_count': 200,
    'region_count': 2,
    'curiosity_enabled': True
})

brain = agent['brain']
world = agent['world']
agent_brain = agent['agent']

# Initialize the agent
agent_brain.initialize(world)

# Run training episode
results = agent_brain.runEpisodeWithMetrics(world, steps=100)

print(f"✓ Agent completed {results['steps']} steps")
print(f"✓ Final curiosity level: {results['curiosity']:.2f}")
print(f"✓ Final novelty level: {results['novelty']:.2f}")
print(f"✓ Neuromodulation level: {results['final_reward']:.2f}")
```

### Example 3: World Exploration

```python
import pynlm

print("Exploring different world scenarios...")

# Create different types of environments
worlds = {
    'simple': pynlm.Factory.createWorldTemplate("empty"),
    'complex': pynlm.Factory.createWorldTemplate("maze"),
    'rich': pynlm.Factory.createWorldTemplate("resource_harvest")
}

# Test each world
for name, world in worlds.items():
    print(f"\n--- Testing {name} world ---")
    
    analysis = world.analyzeEnvironment()
    print(f"Size: {analysis['width']}x{analysis['height']} pixels")
    print(f"Objects: {analysis['total_objects']} total")
    print(f"Resources: {analysis['resources']} available")
    print(f"Hazards: {analysis['hazards']} present")
    
    # Run quick simulation
    results = world.runEpisode(steps=50)
    print(f"Simulation: {results['steps_per_second']:.0f} steps/sec")
```

### Example 4: Brain Analysis

```python
import pynlm

print("Analyzing brain activity...")

# Create and run brain
brain = pynlm.Brain()
brain.initialize()

# Run simulation to generate activity
for step in range(500):
    brain.step(step, step * 0.001)

# Get comprehensive analysis
metrics = brain.collectMetrics()
connectivity = brain.analyzeConnectivity()

print(f"\n=== Brain Analysis ===")
print(f"Total neurons: {metrics['total_neurons']}")
print(f"Total synapses: {metrics['total_synapses']}")
print(f"Active neurons: {metrics['active_neurons']}")
print(f"Average firing rate: {metrics['average_firing_rate']:.3f}")
print(f"Total spikes generated: {metrics['total_spikes']}")
print(f"Strong connections: {connectivity.get('strong_ratio', 0):.3f}")
print(f"Weak connections: {connectivity.get('weak_ratio', 0):.3f}")
```

### Example 5: Complete Research Experiment

```python
import pnlkm

print("Running a complete research experiment...")

# Setup experiment
world = pynlm.Factory.createWorldTemplate("resource_harvest")
brain = pynlm.Brain()
agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)

# Run experiment with multiple trials
experiment = pynlm.SimulationManager.runExperiment(
    world, brain, agent,
    trials=3,                    # 3 trials
    stepsPerTrial=1000,        # 1000 steps each
    randomizeSeeds=True         # Different random seeds
)

print(f"✓ Experiment completed with {len(experiment['trials'])} trials")

# Get results
trials = experiment['trials']
for i, trial in enumerate(trials):
    brain_metrics = trial['brain_metrics']
    print(f"\nTrial {i + 1}:")
    print(f"  Time: {brain_metrics['total_time_ms']:.1f} ms")
    print(f"  Steps/sec: {brain_metrics['steps_per_second']:.0f}")
    print(f"  Spikes: {brain_metrics['spikes_generated']}")
```

## 3. Installation

### One-Command Installation

```bash
pip install pynlm
```

### Verify Installation

```python
import pynlm

print(f"NLM Version: {pynlm.get_version()}")

# Test all major components
print("\nTesting components...")

# Test 1: Factory creation
ecosystem = pynlm.Factory.createSimulationEcosystem()
print("✓ Factory functions work")

# Test 2: Configuration
config = pynlm.Config().withNeuronCount(50)
print("✓ Configuration builder works")

# Test 3: Brain
brain = pynlm.Brain(config)
print("✓ Brain creation works")

# Test 4: World
world = pynlm.SimpleWorld.create(100, 100)
print("✓ World creation works")

# Test 5: Agent
agent = pynlm.AgentBrain.createCuriosityDrivenAgent(brain)
print("✓ Agent creation works")

print("\n🎉 All tests passed! Installation successful.")
```

## 4. Basic Concepts

### Core Components

#### 1. Brain (`pynlm.Brain`)
The central neural simulation engine that:
- Contains neurons and connections
- Processes sensory input
- Produces motor output
- Learns through plasticity

#### 2. World (`pynlm.SimpleWorld`)
The environment where the agent lives:
- 2D grid world
- Contains objects (resources, hazards, etc.)
- Provides sensory input to brain
- Receives motor commands from brain

#### 3. Agent (`pynlm.AgentBrain`)
Connects brain and world:
- Translates world sensory input to brain
- Translates brain motor output to world
- Applies reward signals for learning
- Manages curiosity and exploration

#### 4. Configuration (`pynlm.Config`)
Settings for the simulation:
- Number of neurons and regions
- Simulation timestep
- Random seed
- Various parameters

### Key Concepts

#### 🔄 Simulation Loop
```
World → Sensory Input → Brain → Motor Output → World
   ↑                   ↓
   └─── Reward Signal ←─┘
```

#### 🧠 Learning Process
1. **Sensory Input**: World provides sensory data to brain
2. **Neural Processing**: Brain processes input and produces action
3. **Motor Output**: Brain action affects world
4. **Reward**: World provides reward signal based on outcome
5. **Plasticity**: Brain adjusts connections based on reward

#### 📊 What You Get
After running simulations, you receive:
- **Metrics**: Statistical information about brain activity
- **Connectivity**: Analysis of neural connections
- **Diagnostics**: Detailed information for debugging
- **Export Data**: Data for further analysis

## 5. Common Tasks

### Task 1: Simple Neural Network

```python
import pynlm

# Create simple network
brain = pynlm.Brain().withNeuronCount(100).withRegionCount(1)
brain.initialize()

# Run test
for step in range(100):
    brain.step(step, step * 0.001)

# Check results
metrics = brain.collectMetrics()
print(f"Network produced {metrics['total_spikes']} spikes")
print(f"Average firing rate: {metrics['average_firing_rate']:.3f}")
```

### Task 2: Create Custom Environment

```python
import pynlm

# Create custom world
world = pynlm.SimpleWorld.create(200, 200, 10, 10)

# Add custom objects
world.addObjectWithValidation(50, 50, pynlm.WorldObjectType.Resource, 1.0, 0.5)
world.addObjectWithValidation(150, 150, pynlm.WorldObjectType.Hazard, -1.0, 0.8)
world.setAgentStart(10, 10)  # Where agent starts

# Run simulation
results = world.runEpisode(steps=200)
print(f"Custom world simulation completed")
```

### Task 3: Compare Different Agent Types

```python
import pynlm

brain = pynlm.Brain().withNeuronCount(300)

# Create different agent types
agents = {
    'curiosity': pynlm.AgentBrain.createCuriosityDrivenAgent(brain),
    'reward': pynlm.AgentBrain.createRewardBasedAgent(brain),
    'exploration': pynlm.AgentBrain.createExplorationAgent(brain)
}

world = pynlm.Factory.createWorldTemplate("empty")

# Test each agent
for name, agent in agents.items():
    agent.initialize(world)
    results = agent.runEpisodeWithMetrics(world, steps=100)
    print(f"{name.capitalize()} agent: curiosity={results['curiosity']:.2f}")
```

### Task 4: Performance Testing

```python
import pynlm

# Test different network sizes
sizes = [50, 100, 200, 500]

for size in sizes:
    brain = pynlm.Brain().withNeuronCount(size)
    
    # Run benchmark
    results = brain.benchmark(steps=500, warmupSteps=50)
    
    print(f"Size {size}: {results['steps_per_second']:.0f} steps/sec, "
          f"~{results['spikes_generated']} spikes")
```

### Task 5: Export Analysis Data

```python
import pynlm
import json

# Create and run brain
brain = pynlm.Brain()
brain.initialize()
brain.runEpisode(1000)

# Export for analysis
analysis_data = brain.exportForAnalysis()

# Save to file
with open('my_analysis.json', 'w') as f:
    json.dump(analysis_data, f, indent=2)

print("✓ Analysis data saved to my_analysis.json")
```

## 6. Troubleshooting

### Problem: Installation Issues

**Error**: "ModuleNotFoundError: No module named 'pynlm'"

**Solution**:
```bash
# Check if pip is working
pip list | grep pynlm

# Reinstall if needed
pip install --force-reinstall pynlm

# Or install from source
cd ~/projects
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install -e .
```

### Problem: Brain Initialization Fails

**Error**: "Failed to initialize brain!"

**Solution**:
```python
import pynlm

# Check configuration
config = pynlm.Config()
print("Default config:", config.getSummary())

# Create brain and check configuration
brain = pynlm.Brain(config)
print("Brain configuration:", brain.getConfig().getSummary())

# Try again
try:
    brain.initialize()
    print("✓ Brain initialized")
except Exception as e:
    print(f"✗ Brain initialization failed: {e}")
```

### Problem: High Memory Usage

**Problem**: Memory usage is higher than expected

**Solution**:
```python
import pynlm

# Check memory usage
brain = pynlm.Brain().withNeuronCount(200)  # Start smaller
memory_info = brain.getMemoryInfo()

print(f"Memory estimate: ~{memory_info['memory_usage_estimate_kb']} KB")
print(f"Neurons: {memory_info['total_neurons']}")
print(f"Active neurons: {memory_info['active_neurons']}")

# Try with even smaller network
brain_small = pynlm.Brain().withNeuronCount(50)
results = brain_small.benchmark(steps=100, warmupSteps=10)
print(f"Small network: {results['steps_per_second']:.0f} steps/sec")
```

### Problem: Slow Performance

**Problem**: Simulation is slower than expected

**Solution**:
```python
import pynlm

# Check performance baseline
brain = pynlm.Brain().withNeuronCount(100)
results = brain.benchmark(steps=1000, warmupSteps=100)

print(f"Performance: {results['steps_per_second']:.0f} steps/sec")
print(f"Time per step: {1000/results['steps_per_second']:.3f} ms")

# Compare with different sizes
for size in [50, 100, 200]:
    brain = pynlm.Brain().withNeuronCount(size)
    results = brain.benchmark(steps=500, warmupSteps=50)
    print(f"Size {size}: {results['steps_per_second']:.0f} steps/sec")
```

### Problem: World Validation Fails

**Problem**: World validation fails with "Object collision detected"

**Solution**:
```python
import pynlm

# Create world
world = pynlm.SimpleWorld.create(100, 100)

# Add objects with proper spacing
world.addObjectWithValidation(10, 10, pynlm.WorldObjectType.Resource, 1.0, 0.5)
world.addObjectWithValidation(90, 90, pynlm.WorldObjectType.Resource, 1.0, 0.5)

# Check validation
validation = world.validateWorld()
print(f"World valid: {validation['valid']}")
if not validation['valid']:
    print(f"Errors: {validation['errors']}")
```

## 7. Next Steps

### 🎯 Beginner Path

1. **Master the basics**: Run the quick start examples
2. **Understand the components**: Learn how Brain, World, and Agent work
3. **Try common tasks**: Practice the 5 common task examples
4. **Experiment**: Modify parameters and see what happens
5. **Analyze results**: Use the metrics and diagnostics

### 🚀 Intermediate Path

1. **Create custom scenarios**: Use world templates and configuration
2. **Run experiments**: Use the experiment manager
3. **Performance optimization**: Benchmark and profile your simulations
4. **Data analysis**: Export and analyze simulation data
5. **Error handling**: Robust error handling and debugging

### 💡 Advanced Path

1. **Research integration**: Export data for external analysis
2. **Custom agent types**: Create your own agent behaviors
3. **Performance tuning**: Optimize for specific use cases
4. **Visualization**: Create visual representations of results
5. **Documentation**: Document your findings and methods

## 📚 Resources

### Documentation
- **Main Guide**: `HOW_TO_USE.md` - Comprehensive guide
- **Quick Start**: `README.md` - Quick reference
- **Beginner Guide**: This guide - Simple examples

### Code Examples
- **Repository**: All examples are in the main repository
- **GitHub**: [nlm-project/nlm](https://github.com/nlm-project/nlm)
- **Documentation**: Available at [nlm.readthedocs.io](https://nlm.readthedocs.io)

### Getting Help

#### Stack Overflow
Ask questions with tags `nlm` or `neural-learning-machine`

#### GitHub Issues
Report bugs and request features

#### Community
- **Discord**: NLM Community Server
- **Reddit**: r/neurallearningmachine
- **Email**: contact@nlm-project.org

## 🔧 Command Line Tools

### Build Commands

```bash
# Build from source
mkdir build && cd build
cmake ..
make -j4

# Install locally
pip install -e .
```

### Test Commands

```bash
# Run Python tests
pytest

# Run specific test file
test_path = "tests/test_brain.py"
pytest test_path -v
```

## 📖 Version History

### v2.0.0 (Current)
- Enhanced Python bindings with custom exceptions
- Context managers for automatic cleanup
- Advanced configuration with builder pattern
- High-level simulation utilities
- Factory functions for complex objects
- Comprehensive error handling

### v1.0.0 (Original)
- Basic Python bindings
- Simple API for common use cases
- Basic error handling

## ✅ Installation Checklist

```python
import pynlm

# 1. Import the module
print("✓ Imported pynlm")

# 2. Check version
print(f"✓ Version: {pynlm.get_version()}")

# 3. Create ecosystem
ecosystem = pynlm.Factory.createSimulationEcosystem()
print("✓ Created simulation ecosystem")

# 4. Test components
brain = ecosystem['brain']
world = ecosystem['world']
agent = ecosystem['agent']

print("✓ Brain created")
print("✓ World created")
print("✓ Agent created")

# 5. Initialize and run
brain.initialize()
for step in range(10):
    brain.step(step, step * 0.001)

print("✓ Simulation completed")

print("\n🎉 All checks passed! Ready to start using NLM.")
```

## 🚀 Start Now!

You're ready to start using the enhanced NLM Python bindings!

**First Steps:**
1. Run the "Hello World" example
2. Try the learning agent example
3. Explore the world templates
4. Create your first experiment

**Remember:**
- Use the enhanced configuration builder pattern
- Handle errors gracefully with custom exceptions
- Use context managers for automatic cleanup
- Monitor performance with benchmarks
- Export data for analysis

Happy simulating! 🧠✨