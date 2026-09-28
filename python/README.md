# NLM Python Bindings - User Guide

**Python API for the Neural Learning Machine (NLM)**

This document provides comprehensive documentation for the Python bindings of NLM, covering everything from basic usage to advanced features.

---

## What Are Python Bindings?

The Python bindings provide a high-level interface to the NLM C++ neural simulation framework. They expose the core functionality through a simplified, Pythonic API that makes it easy to create, simulate, and experiment with artificial brains.

## Quick Start Examples

### Example 1: Minimal Brain (Phase 6 Demo)

```python
"""Phase 6 Demo - Integrated Brain with All Systems"""
import pynlm

# Create brain with default configuration (includes all systems)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Set up the world environment
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems (Phase 6 integration)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)  # Update environment
    percept = world.getSensoryPercept()  # Get sensory input
    agent.processSensoryInput(percept)  # Process through brain
    brain.step(step)  # Brain computation
    action = agent.decodeMotorCommand()  # Select action
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute action

print(f"Simulation complete: {brain.getAverageFiringRate():.2f} Hz average firing rate")
print(f"Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Development Experiment

```python
"""Demonstrate brain development over time"""
import pynlm

# Create brain with development enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world for interaction
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Track development stages
print("Development Experiment")
print("Step | Stage | Firing Rate | Memory Used")
print("-----|-------|-------------|-------------")

for step in range(500):
    # Run world update
    world.update(0.05)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print development info every 50 steps
    if step % 50 == 0:
        print(f"{step:4d} | {brain.getDevelopmentalStage():5d} | {brain.getAverageFiringRate():10.2f} | {brain.getWorkingMemoryUsage():4.1f}")

print("Development complete!")
```

### Example 3: Memory Integration Experiment

```python
"""Demonstrate memory systems integration"""
import pynlm

# Create complete brain with all memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Set up world
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all memory systems
agent.enableWorkingMemory(True)
agent.enableEpisodicMemory(True)
agent.enableSemanticMemory(True)
agent.enableProceduralMemory(True)

print("Memory Integration Experiment")
print("Step | Encoding | Completion | Recall")
print("-----|---------|------------|---------")

for step in range(200):
    # Store experience in episodic memory
    experience = agent.getCurrentExperience()
    
    # Process world state
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check memory recall every 25 steps
    if step % 25 == 0:
        recall = agent.recallExperience(step - 50)
        print(f"{step:4d} | {len(experience):8d} | {agent.getCompletionConfidence():.3f} | {recall:.3f}")

print("Memory integration test complete!")
```

---

## Core API Reference

### Brain Creation and Initialization

```python
# Create brain with default configuration (recommended for beginners)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Alternative: Create brain with custom configuration
# brain = pynlm.createBrain(config)

# Always initialize before use
brain.initialize()

# Make brain compute one step
brain.step(step_number)
```

### World and Agent Interface

```python
# Create world environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()

# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete interaction loop
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world environment
        world.update(0.1)
        
        # 2. Get sensory percept
        percept = world.getSensoryPercept()
        
        # 3. Process through agent
        agent.processSensoryInput(percept)
        
        # 4. Brain computation
        brain.step(step)
        
        # 5. Decode action
        action = agent.decodeMotorCommand()
        
        # 6. Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Learning System Control

```python
# Enable specific learning systems
agent.enableRewardModulation(True)      # Reward-modulated STDP
agent.enableCuriosity(True)             # Novelty-driven exploration
agent.enableDevelopment(True)           # Age-dependent plasticity
agent.enableStructuralPlasticity(True)  # Synapse growth/removal

# Get system statistics
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Memory usage: {brain.getWorkingMemoryUsage():.2f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

---

## Phase 6 Integration Features

The Python bindings now include all Phase 6 integrated systems:

### Memory Systems Integration

```python
# Working Memory
print(f"Working Memory: {brain.getWorkingMemoryUsage():.2f} / {brain.getWorkingMemoryCapacity():.2f}")

# Episodic Memory  
brain.step(step)  # Automatically stores experiences
recall = agent.recallRecentExperiences(5)  # Retrieve recent memories

# Semantic Memory
agent.processSensoryInput(percept)  # Forms semantic associations
```

### Neuromodulation Integration

```python
# Dopamine affects learning strength
agent.enableRewardModulation(True)

# Curiosity drives exploration
agent.enableCuriosity(True)

# Novelty detection
novelty = agent.getNoveltyDetection()
if novelty > agent.getCuriosityThreshold():
    # Explore new areas
    action = agent.exploreRandomly()
```

### Prediction System Integration

```python
# Prediction influences attention
agent.enablePrediction(True)
prediction = agent.getCurrentPrediction()

# Planning based on predictions
plan = agent.planActions(prediction, time_horizon=10)
```

### Cognitive Integration

```python
# Concept formation from experience
agent.enableConceptFormation(True)
concepts = agent.getCurrentConcepts()

# Attention selection
agent.enableAttention(True)
attended_features = agent.getAttendedFeatures()
```

---

## Advanced Usage Patterns

### 1. Custom Configuration

```python
import pynlm

# Get default configuration and modify it
config = pynlm.createDefaultConfig()
config.simulation.timestep = 0.01
config.simulation.maxSteps = 10000
config.brain.neuronCount = 1000

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Experiment Monitoring

```python
import pynlm
import time

class NLMExperiment:
    def __init__(self):
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.metrics = []
        
    def run_trial(self, duration, reward_function=None):
        self.world.reset()
        self.agent.initialize(self.world)
        self.brain.initialize()
        
        for step in range(duration):
            start_time = time.time()
            
            # World update
            self.world.update(0.1)
            
            # Perception-action cycle
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Record metrics
            metrics = {
                'step': step,
                'firing_rate': self.brain.getAverageFiringRate(),
                'memory_usage': self.brain.getWorkingMemoryUsage(),
                'development': self.brain.getDevelopmentalStage(),
                'reward': self.agent.getCurrentReward() if reward_function else 0.0
            }
            self.metrics.append(metrics)
            
            # Small delay for timing
            elapsed = time.time() - start_time
            if elapsed < 0.1:
                import time
                time.sleep(0.1 - elapsed)
                
        return self.metrics
```

### 3. Multi-Agent Simulation

```python
import pynlm

class MultiAgentSimulation:
    def __init__(self, num_agents):
        self.agents = []
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30)
        self.world.reset()
        
        for i in range(num_agents):
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            self.agents.append({'brain': brain, 'agent': agent})
            
    def run_step(self):
        # All agents sense the same world
        percept = self.world.getSensoryPercept()
        
        # Process simultaneously
        for i, agent_data in enumerate(self.agents):
            agent_data['agent'].processSensoryInput(percept)
            agent_data['brain'].step(i)
            
        # Agents decide actions (could coordinate or compete)
        actions = []
        for agent_data in self.agents:
            action = agent_data['agent'].decodeMotorCommand()
            actions.append(action)
            
        # Apply combined actions to world
        for i, action in enumerate(actions):
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
        return actions
```

---

## Debugging and Monitoring

### Brain State Monitoring

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor brain state throughout simulation
for step in range(1000):
    # Check for potential issues
    firing_rate = brain.getAverageFiringRate()
    if firing_rate > 100:  # Too high
        print(f"Warning: High firing rate at step {step}: {firing_rate}")
    
    if firing_rate < 0.1:  # Too low
        print(f"Warning: Low firing rate at step {step}: {firing_rate}")
    
    # Check memory limits
    mem_usage = brain.getWorkingMemoryUsage()
    if mem_usage > brain.getWorkingMemoryCapacity() * 0.9:
        print(f"Warning: Memory near capacity: {mem_usage:.2f}")
    
    brain.step(step)
```

### Learning Progress Tracking

```python
experiment = {
    'steps': [],
    'firing_rates': [],
    'memory_usage': [],
    'development_stages': [],
    'curiosity_levels': []
}

for step in range(1000):
    # Run simulation step
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record metrics
    experiment['steps'].append(step)
    experiment['firing_rates'].append(brain.getAverageFiringRate())
    experiment['memory_usage'].append(brain.getWorkingMemoryUsage())
    experiment['development_stages'].append(brain.getDevelopmentalStage())
    experiment['curiosity_levels'].append(agent.getCuriosityLevel())
```

---

## Error Handling and Best Practices

### Common Issues and Solutions

```python
# Issue: Brain not firing
if brain.getAverageFiringRate() < 0.1:
    print("Brain not firing. Check:")
    print("1. Did you call brain.initialize()?")
    print("2. Is there sufficient sensory input?")
    print("3. Are synapses properly connected?")

# Issue: Memory overflow
if brain.getWorkingMemoryUsage() > brain.getWorkingMemoryCapacity():
    print("Memory capacity exceeded!")
    print("Consider reducing neural population size or increasing capacity")

# Issue: Development stuck
if brain.getDevelopmentalStage() == 0:
    print("Brain stuck in developmental stage 0")
    print("Check development system integration")
```

### Proper Initialization Order

```python
# CORRECT order:
1. Create configuration
2. Create brain
3. Initialize brain
4. Create world
5. Create agent
6. Initialize agent
7. Run simulation loop

# INCORRECT order:
brain.initialize()  # OK
world = pynlm.createSimpleWorld()  # OK
agent = pynlm.createAgentBrain(brain)  # OK
brain.initialize()  # DANGER: Double initialization! Causes undefined behavior
```

---

## Performance Optimization

### Batch Processing

```python
# For large simulations, use batch processing
import pynlm

def batch_simulation(num_brains, steps_per_brain):
    brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(num_brains)]
    worlds = [pynlm.createSimpleWorld() for _ in range(num_brains)]
    agents = [pynlm.createAgentBrain(brain) for brain, world in zip(brains, worlds)]
    
    for brain, world, agent in zip(brains, worlds, agents):
        brain.initialize()
        world.initialize()
        agent.initialize(world)
        
        for step in range(steps_per_brain):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Installation and Setup

### Installing from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
pip install -e .
```

### Using Pre-built Packages

```bash
# Install from PyPI (if available)
pip install pynlm

# Or use conda
conda install -c nlm-repo pynlm
```

---

## Advanced Configuration Options

### Custom Brain Configuration

```python
import pynlm
import json

# Create and save configuration
config = pynlm.createDefaultConfig()
config.brain.neuronCount = 500
config.brain.regionCount = 3
config.simulation.dt = 0.01
config.plasticity.stdpEnabled = True
config.plasticity.hebbianEnabled = True
config.memory.workingMemoryCapacity = 1000

# Save configuration
with open('my_config.json', 'w') as f:
    json.dump(config.to_dict(), f)

# Load configuration
config2 = pynlm.createConfigFromFile('my_config.json')
brain = pynlm.createBrain(config2)
```

---

## Examples and Tutorials

For more examples and detailed tutorials, see:

- `examples/` directory for practical applications
- `docs/` directory for architectural explanations  
- `tests/` directory for unit tests and benchmarks

---

## API Reference

Complete API documentation is available via:

```python
import pynlm
help(pynlm)  # Python help system
pynlm.__doc__  # Module documentation
```

For C++ API documentation, see the `docs/` directory.

---

## Contributing

When contributing to the Python bindings:

1. Write comprehensive tests for new functionality
2. Update this documentation with examples
3. Ensure compatibility with all supported Python versions
4. Test with both CPU and GPU (if available)

---

**Last Updated:** Phase 6 Integration (September 2026)
**Python Compatibility:** 3.8+ (requires Python 3.8 or higher)
**License:** MIT
