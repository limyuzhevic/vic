# NLM - How to Use the Neural Learning Machine

## Quick Start

This document provides comprehensive instructions for using the NLM (Neural Learning Machine) framework. It covers everything from basic setup to advanced usage patterns.

## Table of Contents

1. [Installation and Setup](#installation-and-setup)
2. [Basic Usage](#basic-usage)
3. [Brain Configuration](#brain-configuration)
4. [Agent Configuration](#agent-configuration)
5. [World Setup](#world-setup)
6. [Simulation Control](#simulation-control)
7. [Learning and Training](#learning-and-training)
8. [Memory Management](#memory-management)
9. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
10. [Performance Optimization](#performance-optimization)
11. [Advanced Features](#advanced-features)
12. [API Reference](#api-reference)
13. [Example Projects](#example-projects)

## Installation and Setup

### System Requirements
- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++17 compatible compiler (g++, clang++, MSVC)
- **Python**: 3.7 or higher (for Python bindings)
- **Memory**: At least 1GB RAM for basic usage
- **Disk Space**: 500MB for complete installation

### Building from Source

#### Linux/macOS
```bash
# Clone the repository
git clone <repository-url> nlm
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
           -DCMAKE_CXX_STANDARD=17 \
          
# Build
make -j4
```

#### Windows (Visual Studio)
```powershell
# Clone the repository
git clone <repository-url> nlm
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
# Use Developer Command Prompt for VS
cmake .. -G "Visual Studio 16 2019" -A x64

# Build
cmake --build . --config Release
```

### Installing Python Bindings

#### Python Package Installation
```bash
# Install via pip (if package is published)
pip install nlm

# Install from source
cd <nlm-root>/python
pip install .
```

#### Python Dependencies
```python
# Required packages
import nlm

# Check installation
print("NLM version:", nlm.__version__ if hasattr(nlm, '__version__') else 'Unknown')
print("NLM is ready for use")
```

## Basic Usage

### Creating a Simple Brain

The NLM framework allows you to create and simulate artificial brains with neural components, memory systems, and learning capabilities.

#### Basic Brain Example
```python
import nlm

# Create a brain with default configuration
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a few simulation steps
for i in range(10):
    brain.step(i)

# Check brain status
print(f"Brain initialized successfully")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Brain Configuration

#### Understanding Brain Parameters
```python
import nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Key configuration parameters
parameters = {
    "neuron_count": "Number of neurons in the brain (default: 1000)",
    "region_count": "Number of brain regions (default: 1)",
    "simulation_timestep": "Time step duration in seconds (default: 0.001)",
    "random_seed": "Random seed for reproducible results (default: 42)",
    "connection_probability": "Probability of synaptic connections (default: 0.1)",
    "stdp_ltp_weight": "STDP LTP weight (default: 0.01)",
    "stdp_ltd_weight": "STDP LTD weight (default: 0.012)",
    "stdp_tau": "STDP time constant (default: 20.0)",
}

# Modify configuration
config.set("neuron_count", 2000)
config.set("simulation_timestep", 0.0005)
config.set("random_seed", 12345)

# Create brain with custom configuration
brain = nlm.createBrain(config)
```

### Agent Configuration

#### Creating and Configuring an Agent
```python
import nlm

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Create world
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Configure agent with learning capabilities
agent.enableRewardModulation(True)      # Enable reward-based learning
agent.enableCuriosity(True)             # Enable curiosity-driven exploration
agent.enableStructuralPlasticity(True)   # Enable structural changes
agent.enableDevelopment(True)            # Enable developmental progression

# Verify configuration
print(f"Agent sensory input size: {agent.getSensoryInputSize()}")
print(f"Agent motor output size: {agent.getMotorOutputSize()}")
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
```

## Brain Configuration

### Core Brain Parameters

#### Neural Network Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |
| `simulation_timestep` | float | 0.001 | Simulation time step in seconds |
| `random_seed` | int64 | 42 | Random seed for reproducibility |

#### Plasticity Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |

#### Memory Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | int | 100 | Working memory capacity |
| `episodic_memory_capacity` | int | 1000 | Episodic memory capacity |
| `associative_memory_capacity` | int | 500 | Associative memory capacity |

### Configuration Builder Pattern
```python
import nlm

class NLMConfigurator:
    def __init__(self):
        self.config = nlm.createDefaultConfig()
    
    def basic_brain(self, neuron_count=1000):
        self.config.set("neuron_count", neuron_count)
        return self
    
    def performance_optimized(self, use_simd=True):
        self.config.set("use_simd", use_simd)
        self.config.set("spike_queue_capacity", 32768)
        return self
    
    def learning_enabled(self, reward_strength=0.8, curiosity_strength=0.5):
        self.config.set("reward_modulation_strength", reward_strength)
        self.config.set("curiosity_strength", curiosity_strength)
        return self
    
    def memory_enhanced(self, working_capacity=500, episodic_capacity=2000):
        self.config.set("working_memory_capacity", working_capacity)
        self.config.set("episodic_memory_capacity", episodic_capacity)
        return self
    
    def create(self):
        return nlm.createBrain(self.config)

# Usage
builder = NLMConfigurator()
brain = builder.basic_brain(2000).learning_enabled().create()
brain.initialize()
```

## Agent Configuration

### Agent Configuration Options

#### Sensory Processing Configuration
```python
class SensoryConfig:
    def __init__(self):
        self.input_size = 256  # Default vision input size
        self.encoding_strength = 5.0  # Signal encoding strength
        self.noise_level = 0.1  # Sensory noise
    
    def configure_vision(self, width, height):
        self.input_size = width * height
        return self
    
    def set_encoding(self, strength, noise=0.0):
        self.encoding_strength = strength
        self.noise_level = noise
        return self
```

#### Motor Output Configuration
```python
class MotorConfig:
    def __init__(self):
        self.output_size = 6  # Default action space size
        self.action_threshold = 0.5  # Action selection threshold
        self.exploration_rate = 0.1  # Random action probability
    
    def set_action_space(self, actions):
        self.output_size = len(actions)
        return self
    
    def configure_exploration(self, rate, method="epsilon_greedy"):
        self.exploration_rate = rate
        self.exploration_method = method
        return self
```

#### Neuromodulation Configuration
```python
class NeuromodulationConfig:
    def __init__(self):
        self.reward_modulation = True
        self.reward_strength = 0.8
        self.reward_delay = 1.0
        
        self.curiosity = True
        self.curiosity_strength = 0.5
        self.curiosity_decay = 0.95
        
        self.novelty = True
        self.novelty_sensitivity = 0.7
        
        self.ac acetylcholine = False
        self.acetylcholine_strength = 0.5
        
        self.norepinephrine = False
        self.norepinephrine_strength = 0.5
        
        self.serotonin = False
        self.serotonin_strength = 0.5
    
    def enable_reward(self, enabled=True, strength=0.8, delay=1.0):
        self.reward_modulation = enabled
        self.reward_strength = strength
        self.reward_delay = delay
        return self
    
    def enable_curiosity(self, enabled=True, strength=0.5, decay=0.95):
        self.curiosity = enabled
        self.curiosity_strength = strength
        self.curiosity_decay = decay
        return self
```

### Applying Configuration to Agent
```python
import nlm

# Create configuration
class CompleteConfig:
    def build(self):
        config = nlm.createDefaultConfig()
        
        # Brain configuration
        config.set("neuron_count", 3000)
        config.set("simulation_timestep", 0.001)
        config.set("random_seed", 12345)
        
        # Performance configuration
        config.set("use_simd", True)
        config.set("spike_queue_capacity", 65536)
        
        # Learning configuration
        config.set("reward_modulation_enabled", True)
        config.set("reward_strength", 0.8)
        config.set("curiosity_enabled", True)
        config.set("curiosity_strength", 0.6)
        
        # Memory configuration
        config.set("working_memory_capacity", 500)
        config.set("episodic_memory_capacity", 2000)
        
        return config

# Apply configuration
config = CompleteConfig().build()
brain = nlm.createBrain(config)
brain.initialize()

# Create and configure agent
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Apply neuromodulation configuration
agent.enableRewardModulation(True, 0.8)
agent.enableCuriosity(True, 0.6)
agent.enableNoveltyDetection(True, 0.7)
```

## World Setup

### Environment Configuration

#### Basic World Configuration
```python
import nlm

# Create simple world
world = nlm.createSimpleWorld()

# Configure world dimensions and perception
world.configure(
    width=30,              # World width in units
    height=30,             # World height in units
    visionWidth=8,         # Vision sensor width
    visionHeight=8         # Vision sensor height
)

# Set agent starting position
world.setAgentStart(5, 5)

# Configure world properties
world.setMaxEnergy(100.0)           # Maximum energy
world.setEnergyDecayRate(0.01)      # Energy consumption rate
world.setRandomSeed(42)             # Reproducible environment
```

#### World Object Management
```python
# Add objects to the world

# Resource objects (positive for agent)
world.addObject(10, 10, nlm.WorldObjectType.Resource, 5.0)
world.addObject(20, 20, nlm.WorldObjectType.Resource, 10.0)
world.addObject(30, 30, nlm.WorldObjectType.Resource, 15.0)

# Hazard objects (negative for agent)
world.addObject(15, 15, nlm.WorldObjectType.Hazard, -5.0)
world.addObject(25, 25, nlm.WorldObjectType.Hazard, -10.0)

# Wall boundaries
for x in range(0, 30, 2):
    world.addObject(x, 0, nlm.WorldObjectType.Wall, 0.5)    # North wall
    world.addObject(x, 30, nlm.WorldObjectType.Wall, 0.5)    # South wall

for y in range(0, 30, 2):
    world.addObject(0, y, nlm.WorldObjectType.Wall, 0.5)    # West wall
    world.addObject(30, y, nlm.WorldObjectType.Wall, 0.5)    # East wall

# Markers for special locations
world.addObject(5, 5, nlm.WorldObjectType.Marker, 1.0)    # Start point
world.addObject(25, 25, nlm.WorldObjectType.Marker, 2.0)  # Goal point
```

#### World Sensor Configuration
```python
# Configure sensory systems
world.setVisionRange(8)              # Vision detection range
world.setTouchRange(4)               # Touch detection range
world.setAudioRange(6)               # Audio detection range
world.setProprioceptionEnabled(True)  # Proprioception enabled

# Set sensory noise levels
world.setVisionNoise(0.1)
world.setTouchNoise(0.05)
world.setAudioNoise(0.1)
```

### Simulation Control

#### Simulation Management
```python
import nlm
import time

# Create complete simulation setup
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Run simulation with control
print("Starting simulation...")
simulation_start = time.time()

step_count = 0
max_steps = 1000
target_time = 10.0  # Target simulation time in seconds

while step_count < max_steps:
    current_time = step_count * 0.1  # 0.1 time units per step
    
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain computation
    brain.step(step_count)
    
    # Generate action
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    result = world.applyMotorCommand(action, current_time)
    
    # Monitor progress
    if step_count % 100 == 0:
        elapsed = time.time() - simulation_start
        print(f"Step {step_count}, Time: {current_time:.1f}s, "
              f"Elapsed: {elapsed:.1f}s, "
              f"Action: {action}, Reward: {result.reward}")
    
    step_count += 1

print(f"Simulation completed: {step_count} steps")
elapsed = time.time() - simulation_start
print(f"Total elapsed time: {elapsed:.1f}s")
```

#### Controlled Simulation with Events
```python
import nlm

# Setup simulation with events
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Add event callback
step_events = []
def on_step_completed(step_num):
    step_events.append({
        'step': step_num,
        'spikes': brain.getTotalSpikeCount(),
        'neurons_firing': brain.getFiringNeuronCount(),
        'curiosity': agent.getCuriosityLevel()
    })

# Run with event tracking
for step in range(500):
    # Update world and process as before
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.1)
    
    # Record event
    on_step_completed(step)

# Analyze events
print("Event analysis:")
for i, event in enumerate(step_events[::100]):  # Every 100 steps
    print(f"  Step {event['step']}: "
          f"Spikes={event['spikes']}, "
          f"Firing={event['neurons_firing']}, "
          f"Curiosity={event['curiosity']:.3f}")
```

## Learning and Training

### Reward-Based Learning
```python
import nlm

# Setup agent with reward learning
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable reward modulation with specific parameters
agent.enableRewardModulation(True, 0.8)  # Enable with 80% strength

# Track learning progress
learning_data = []
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, step * 0.1)
    
    # Record learning data
    learning_data.append({
        'step': step,
        'action': action,
        'reward': result.reward,
        'success': result.success,
        'dopamine': brain.getDopamine() and brain.getDopamine()->getLevel() or 0.0,
        'curiosity': agent.getCuriosityLevel()
    })
    
    # Apply reward modulation
    predicted_reward = 0.5  # Simple prediction
    agent.applyRewardModulation(result.reward, predicted_reward)

# Analyze learning progress
print("Learning analysis:")
print(f"Total steps: {len(learning_data)}")
print(f"Average reward: {sum(d['reward'] for d in learning_data) / len(learning_data):.3f}")
print(f"Success rate: {sum(1 for d in learning_data if d['success']) / len(learning_data):.3f}")
print(f"Final curiosity: {learning_data[-1]['curiosity']:.3f}")
print(f"Final dopamine: {learning_data[-1]['dopamine']:.3f}")
```

### Curiosity-Driven Exploration
```python
import nlm

# Setup agent with curiosity-driven exploration
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable curiosity-driven learning
agent.enableCuriosity(True, 0.6)  # 60% curiosity strength

# Track exploration behavior
exploration_data = []
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Record exploration data
    exploration_data.append({
        'step': step,
        'action': action,
        'novelty': agent.getNoveltyLevel(),
        'curiosity': agent.getCuriosityLevel(),
        'reward': brain.getDopamine() and brain.getDopamine()->getLevel() or 0.0
    })
    
    # Apply action
    world.applyMotorCommand(action, step * 0.1)

# Analyze exploration patterns
print("Exploration analysis:")
print(f"Total steps: {len(exploration_data)}")

# Count different actions
action_counts = {}
for d in exploration_data:
    action = d['action']
    action_counts[action] = action_counts.get(action, 0) + 1

print("Action distribution:")
for action, count in action_counts.items():
    percentage = count / len(exploration_data) * 100
    print(f"  {action}: {count} times ({percentage:.1f}%)")

print(f"Final curiosity: {exploration_data[-1]['curiosity']:.3f}")
print(f"Final novelty: {exploration_data[-1]['novelty']:.3f}")
```

## Memory Management

### Memory Monitoring
```python
import nlm

# Create agent with memory monitoring
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Configure memory systems
if brain.getWorkingMemory():
    working_memory = brain.getWorkingMemory()
    working_memory.setCapacity(1000)
    working_memory.setDecayRate(0.02)

if brain.getEpisodicMemory():
    episodic_memory = brain.getEpisodicMemory()
    episodic_memory.setMaxEpisodes(2000)
    episodic_memory.setReplayInterval(100)

# Monitor memory usage
memory_stats = []
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Record memory statistics
    stats = {
        'step': step,
        'working_memory_load': brain.getWorkingMemory() and brain.getWorkingMemory()->getCurrentLoad() or 0.0,
        'episodic_memory_count': brain.getEpisodicMemory() and brain.getEpisodicMemory()->getEpisodeCount() or 0,
        'total_spikes': brain.getTotalSpikeCount(),
        'neurons_firing': brain.getFiringNeuronCount()
    }
    memory_stats.append(stats)
    
    # Apply action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.1)

# Analyze memory usage
print("Memory usage analysis:")
print(f"Average working memory load: {sum(s['working_memory_load'] for s in memory_stats) / len(memory_stats):.3f}")
print(f"Total episodes stored: {memory_stats[-1]['episodic_memory_count']}")
print(f"Total spikes processed: {memory_stats[-1]['total_spikes']}")
print(f"Average firing rate: {memory_stats[-1]['neurons_firing'] / len(memory_stats):.3f}")
```

### Memory Replay
```python
import nlm

# Create agent with memory replay
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Configure memory for replay
if brain.getEpisodicMemory():
    episodic_memory = brain.getEpisodicMemory()
    episodic_memory.setReplayInterval(50)  # Replay every 50 steps
    episodic_memory.setReplayDepth(3)     # Replay 3 episodes
    episodic_memory.setConsolidationStrength(0.8)  # 80% consolidation

# Run simulation with memory replay
print("Running simulation with memory replay...")
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Check for replay trigger
    if step % episodic_memory.getReplayInterval() == 0:
        # System will automatically replay important memories
        print(f"Memory replay triggered at step {step}")
    
    world.applyMotorCommand(action, step * 0.1)

# Analyze memory after simulation
if brain.getEpisodicMemory():
    memory = brain.getEpisodicMemory()
    episodes = memory.getAllEpisodes()
    
    print(f"Total episodes stored: {len(episodes)}")
    print(f"Replay events: {memory.getReplayCount()}")
    print(f"Consolidation strength: {memory.getConsolidationStrength():.3f}")
```

## Debugging and Troubleshooting

### Common Issues and Solutions

#### "Brain not initialized"
```python
# Problem: Calling brain methods before initialization
# Solution:
brain = nlm.createBrain(config)
brain.initialize()  # Initialize first
brain.step(0)       # Then use
```

#### "Agent not properly configured"
```python
# Problem: Using agent without proper initialization
# Solution:
brain = nlm.createBrain(config)
brain.initialize()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)  # Initialize agent with world
```

#### "Memory system not available"
```python
# Problem: Accessing memory systems before they're initialized
# Solution:
# Check if memory systems are available
if brain.getWorkingMemory():
    memory = brain.getWorkingMemory()
    # Use memory systems
else:
    print("Working memory not available")
```

#### "Event queue full"
```python
# Problem: Event queue overflow
# Solution:
# Configure larger event queues in config
config.set("spike_queue_capacity", 65536)
config.set("delayed_queue_capacity", 4096)
```

### Debugging Tools
```python
import nlm

# Debug function to diagnose common issues
def debug_system(brain, agent, world):
    print("=== System Debug Information ===")
    
    # Basic brain status
    print(f"Brain initialized: {brain.getTotalNeuronCount() > 0}")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Memory status
    if brain.getWorkingMemory():
        print(f"Working memory active: {brain.getWorkingMemory()->isActive()}")
        print(f"Working memory load: {brain.getWorkingMemory()->getCurrentLoad()}%")
    
    if brain.getEpisodicMemory():
        print(f"Episodic memory active: {brain.getEpisodicMemory()->isActive()}")
        print(f"Episode count: {brain.getEpisodicMemory()->getEpisodeCount()}")
    
    # Neuromodulation status
    if brain.getDopamine():
        print(f"Dopamine level: {brain.getDopamine()->getLevel():.3f}")
    
    if agent.getCuriosityLevel():
        print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    
    if agent.getNoveltyLevel():
        print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Event system status
    if hasattr(brain, 'getSpikeSystem'):
        spike_system = brain.getSpikeSystem()
        print(f"Pending spikes: {spike_system.getPendingSpikeCount()}")
        print(f"Pending delayed: {spike_system.getPendingDelayedCount()}")
    
    # Action system status
    print(f"Agent sensory size: {agent.getSensoryInputSize()}")
    print(f"Agent motor size: {agent.getMotorOutputSize()}")
    print(f"Reward modulation: {agent.isRewardModulationEnabled()}")

# Usage
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

debug_system(brain, agent, world)
```

## Performance Optimization

### Memory Optimization
```python
import nlm

# Memory-optimized brain configuration
config = nlm.createDefaultConfig()

# Optimize for memory usage
config.set("neuron_count", 2000)  # Reduce neuron count
config.set("use_simd", True)       # Enable SIMD for better cache usage
config.set("spike_queue_capacity", 16384)  # Reasonable queue size

# Memory pool configuration
config.set("neuron_pool_block_size", 512)
config.set("synapse_pool_block_size", 2048)

brain = nlm.createBrain(config)
brain.initialize()

# Monitor memory during simulation
import tracemalloc
tracemalloc.start()

for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        current, peak = tracemalloc.get_traced_memory()
        print(f"Step {step}: Current memory = {current / 1024:.1f} KB, Peak = {peak / 1024:.1f} KB")

tracemalloc.stop()
```

### Simulation Performance
```python
import nlm
import time

# Performance-optimized brain configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("use_simd", True)

# Run performance test
brain = nlm.createBrain(config)
brain.initialize()

# Measure simulation performance
start_time = time.time()
step_count = 0
max_steps = 1000

while step_count < max_steps:
    brain.step(step_count)
    step_count += 1

end_time = time.time()
elapsed = end_time - start_time
steps_per_second = step_count / elapsed

print(f"Performance Results:")
print(f"  Steps executed: {step_count}")
print(f"  Elapsed time: {elapsed:.2f}s")
print(f"  Steps per second: {steps_per_second:.0f}")
print(f"  Time per step: {elapsed / step_count * 1000:.2f}ms")

# Performance analysis
if steps_per_second > 10000:
    print("Performance: EXCELLENT (>10,000 steps/sec)")
elif steps_per_second > 5000:
    print("Performance: GOOD (5,000-10,000 steps/sec)")
elif steps_per_second > 1000:
    print("Performance: ACCEPTABLE (1,000-5,000 steps/sec)")
else:
    print("Performance: NEEDS OPTIMIZATION (<1,000 steps/sec)")
```

## Advanced Features

### Event-Driven Processing
```python
import nlm

# Configure brain for event-driven processing
config = nlm.createDefaultConfig()
config.set("use_event_queue", True)
config.set("spike_queue_capacity", 32768)
config.set("delayed_queue_capacity", 2048)

brain = nlm.createBrain(config)
brain.initialize()

# Enable event-driven mode
if hasattr(brain, 'configureForEventDrivenProcessing'):
    brain.configureForEventDrivenProcessing()

# Verify configuration
if hasattr(brain, 'getEventQueue') and brain.getEventQueue():
    event_queue = brain.getEventQueue()
    print(f"Event queue capacity: {event_queue.capacity()}")
    print(f"Event queue type: {type(event_queue).__name__}")
```

### SIMD Vectorization
```python
import nlm

# Check SIMD availability
print(f"SIMD available: {nlm.Neuron.isSIMDAvailable()}")
print(f"SIMD type: {nlm.Neuron.getSIMDType()}")
print(f"SIMD width: {nlm.Neuron.getOptimalSIMDBlockSize()} elements")

# Create brain with SIMD support
config = nlm.createDefaultConfig()
config.set("use_simd", True)
config.set("simd_width", 16)  # AVX-512

brain = nlm.createBrain(config)
brain.initialize()

# Process neurons with SIMD if available
if nlm.Neuron.isSIMDAvailable():
    neurons = [brain]  # This is simplified - actual implementation would have neuron collection
    success = nlm.Neuron.processBatchSIMD(neurons, 0.0, 0.001)
    print(f"SIMD batch processing: {'SUCCESS' if success else 'FAILED'}")
```

### Parallel Processing
```python
import nlm
import threading

# Parallel processing configuration
config = nlm.createDefaultConfig()
config.set("use_parallel", True)
config.set("max_threads", 4)

brain = nlm.createBrain(config)
brain.initialize()

# Check parallel capabilities
if hasattr(brain, 'isParallelProcessingAvailable'):
    print(f"Parallel processing available: {brain.isParallelProcessingAvailable()}")
    print(f"Max threads: {brain.getMaxThreads()}")

# Process in parallel
if hasattr(brain, 'processParallel'):
    result = brain.processParallel(step_count=100)
    print(f"Parallel processing result: {result}")
```

## Python API Reference

### Core Classes

#### Brain Class
| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the brain |
| `step(step)` | Process one simulation step |
| `receiveSensoryInput(input)` | Process sensory data |
| `produceAction()` | Generate motor command |
| `save(filepath)` | Save brain state |
| `load(filepath)` | Load brain state |
| `getTotalNeuronCount()` | Get total neuron count |
| `getTotalSynapseCount()` | Get total synapse count |
| `getTotalSpikeCount()` | Get total spike count |
| `getFiringNeuronCount()` | Get firing neuron count |

#### AgentBrain Class
| Method | Description |
|--------|-------------|
| `initialize(world)` | Initialize with world |
| `processSensoryInput(percept)` | Process environmental input |
| `decodeMotorCommand()` | Convert neural activity to action |
| `applyRewardModulation(reward, predicted)` | Update based on outcomes |
| `updateDevelopment(time)` | Update developmental stage |
| `getSensoryInputSize()` | Get expected sensory input size |
| `getMotorOutputSize()` | Get expected motor output size |
| `getCuriosityLevel()` | Get curiosity level |
| `getNoveltyLevel()` | Get novelty level |

#### Configuration Class
| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `set(key, value)` | Set configuration parameter |
| `getOr(key, default)` | Get configuration value with fallback |

### Constants
- `NeuronType.Excitatory` - Excitatory neuron
- `NeuronType.Inhibitory` - Inhibitory neuron
- `NeuronType.Sensory` - Sensory neuron
- `NeuronType.Motor` - Motor neuron
- `NeuronType.Internal` - Internal neuron

- `ActionType.MoveForward` - Move forward action
- `ActionType.MoveBackward` - Move backward action
- `ActionType.TurnLeft` - Turn left action
- `ActionType.TurnRight` - Turn right action
- `ActionType.Look` - Look action
- `ActionType.Interact` - Interact action
- `ActionType.Wait` - Wait action

- `WorldObjectType.Empty` - Empty world object
- `WorldObjectType.Resource` - Resource object
- `WorldObjectType.Hazard` - Hazard object
- `WorldObjectType.Wall` - Wall object
- `WorldObjectType.Marker` - Marker object

## Example Projects

### 1. Random Walk Environment
See Python usage section for complete example.

### 2. Maze Navigation
See Python usage section for complete example.

### 3. Multi-Agent System
See Python usage section for complete example.

## Troubleshooting Guide

### Common Issues and Solutions

#### "Brain not responding"
- **Problem**: Brain methods not working as expected
- **Solution**: Check initialization, timestep settings, and event queue status

#### "Agent not moving"
- **Problem**: Agent not generating motor commands
- **Solution**: Verify agent initialization, sensory processing, and action decoding

#### "Memory not being stored"
- **Problem**: Memory systems not storing experiences
- **Solution**: Check memory system initialization and configuration

#### "Performance issues"
- **Problem**: Slow simulation performance
- **Solution**: Optimize timestep, enable SIMD, monitor memory usage

### Debugging Tools

#### System Information
```python
import nlm

# Print system information
print("=== NLM System Information ===")
print(f"NLM version: {nlm.__version__ if hasattr(nlm, '__version__') else 'Unknown'}")
print(f"SIMD available: {nlm.Neuron.isSIMDAvailable()}")
print(f"Python version: {__import__('sys').version}")
print(f"Available memory: {nlm.getSystemMemory() if hasattr(nlm, 'getSystemMemory') else 'Unknown'} MB")
```

#### Performance Monitoring
```python
import nlm
import time
import psutil

def monitor_performance(brain, agent, duration_seconds=60):
    start_time = time.time()
    step_count = 0
    
    print(f"Starting performance monitoring for {duration_seconds} seconds...")
    
    while time.time() - start_time < duration_seconds:
        brain.step(step_count)
        step_count += 1
        
        if step_count % 100 == 0:
            # Get system metrics
            cpu_percent = psutil.cpu_percent()
            memory = psutil.virtual_memory()
            
            print(f"Step {step_count}: CPU={cpu_percent}%, "
                  f"Memory={memory.percent}%, "
                  f"Spikes={brain.getTotalSpikeCount()}")
    
    elapsed = time.time() - start_time
    print(f"Monitoring completed: {step_count} steps in {elapsed:.1f}s")
    print(f"Performance: {step_count/elapsed:.0f} steps/second")

# Usage
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
monitor_performance(brain, None)  # Pass agent if needed
```

## Support and Community

### For Questions and Issues
- **Documentation**: Check this file for comprehensive usage instructions
- **Examples**: Review example projects for complete working code
- **Troubleshooting**: Use debugging tools and common solutions
- **Performance**: Monitor system metrics and optimize configuration

### Research and Development
- **Open-source**: Contribute to the NLM project
- **Benchmarks**: Compare performance across different configurations
- **Collaboration**: Join community discussions and development
- **Documentation**: Help improve and expand documentation

---

**Note:** This is an experimental system. Features are continuously improved. Some components may require specific configuration or have limited functionality. Always test configurations thoroughly before deployment.

**Getting Started:** Begin with the [Quick Start](#quick-start) section for basic usage, then explore [Advanced Features](#advanced-features) for more complex scenarios.

**For Support:** Refer to the [Troubleshooting Guide](#troubleshooting-guide) for common issues and solutions, or use the debugging tools provided.
