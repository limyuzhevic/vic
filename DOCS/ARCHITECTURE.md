# NLM - Improved Documentation and Examples
# 
# This document provides comprehensive documentation for the NLM (Neural Learning Machine)
# project, including architecture, configuration, usage examples, and troubleshooting.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Configuration Guide](#configuration-guide)
3. [Python API Reference](#python-api-reference)
4. [Usage Examples](#usage-examples)
   - [Basic Simulation](#basic-simulation)
   - [Brain in World Environment](#brain-in-world-environment)
   - [Complete Agent Simulation](#complete-agent-simulation)
   - [Advanced Configuration](#advanced-configuration)
5. [Troubleshooting Guide](#troubleshooting-guide)
6. [Performance Tips](#performance-tips)
7. [Extending NLM](#extending-nlm)
8. [Advanced Features](#advanced-features)

---

## Architecture Overview

NLM is a brain-inspired spiking neural network simulator that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation. The system is designed as a modular, extensible artificial brain that can learn and adapt through experience.

### Core Components

#### 1. Neural Architecture
- **Neurons**: Leaky Integrate-and-Fire (LIF) neurons with biological parameters
- **Synapses**: Event-driven synaptic connections with multiple plasticity rules
- **Brain Regions**: Hierarchical organization of neural populations
- **Populations**: Functional groups of neurons (Sensory, Internal, Motor, Modulatory)

#### 2. Memory Systems
- **Working Memory**: Short-term memory for active maintenance and competition
- **Episodic Memory**: Long-term memory for experience recording and replay
- **Associative Memory**: Pattern completion and recognition
- **Semantic Memory**: Knowledge representation and conceptual learning

#### 3. Neuromodulation
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration drive and novelty detection
- **Prediction Error**: Bayesian learning and prediction accuracy
- **Novelty**: Pattern recognition and surprise detection

#### 4. Cognitive Systems
- **Prediction System**: Forward models and expectation generation
- **Cognition**: Planning, attention, and concept formation
- **Development**: Growth and maturation processes

#### 5. Learning and Plasticity
- **STDP**: Spike-Timing-Dependent Plasticity
- **Hebbian Learning**: Correlation-based synaptic strengthening
- **Structural Plasticity**: Formation and elimination of connections
- **Reward-Modulated Learning**: Credit assignment and reinforcement

### System Integration

The NLM brain operates as a closed-loop system:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
                                      ↓              ↓
                             MEMORY/PREDICTION ← MOTIVATION/NEUROMODULATION
                                      ↓              ↓
                            ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE
                                      ↓              ↓
                            REWARD/SURPRISE/ERROR → PLASTICITY → MEMORY/DEVELOPMENT
```

### Development Phases

The project is organized into progressive development phases:

- **Phase 2**: Real neural computation (LIF neurons, STDP, Hebbian plasticity)
- **Phase 3**: World interaction, learning from experience
- **Phase 4**: Advanced cognition, planning, and concept formation
- **Phase 5**: Performance optimizations and parallel processing
- **Phase 6**: Final integration - all systems working together coherently

---

## Configuration Guide

NLM supports multiple configuration methods:

### 1. Default Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
```

### 2. Configuration File

Create a `config.json` file:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        }
    }
}
```

### 3. Command Line Configuration

```bash
nlm --neuron-count 1000 --timestep 0.001 --brain-type phase6 \
    --enable-stdp --enable-hebbian --enable-curiosity \
    --experiment basic --output results.json
```

### 4. Advanced Configuration Methods

#### Programmatic Configuration

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 1.5)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Configuration Validation

```python
# Validate configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

if not config.isValid():
    errors = config.getValidationErrors()
    for error in errors:
        print(f"Error: {error}")
```

#### Configuration Serialization

```python
# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
config.loadFromFile("my_config.json")
```

---

## Python API Reference

### Core Classes

#### Brain Class

**Creation and Initialization:**

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create brain with custom configuration
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.learning_rate", 0.002)
brain = pynlm.createBrain(config)
brain.initialize()
```

**Simulation:**

```python
# Run single simulation step
brain.step(step_number)

# Run multiple steps
for step in range(1000):
    brain.step(step)
    
    # Optional: get statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

**State Management:**

```python
# Reset brain to initial state
brain.reset()

# Save brain state to checkpoint
brain.save("checkpoint.bin")

# Load brain state from checkpoint
brain.load("checkpoint.bin")
```

**Input/Output:**

```python
# Receive sensory input
from pynlm import SensoryInput
sensory_input = SensoryInput([0.1, 0.5, 0.9, ...])
brain.receiveSensoryInput(sensory_input)

# Get motor action
action = brain.produceAction()
```

**Statistics and Monitoring:**

```python
# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

#### Config Class

**Query Parameters:**

```python
config = pynlm.createDefaultConfig()

# Check if parameter exists
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")
    print(f"Neuron count: {value}")

# Get with default value
default_neurons = config.getOr("brain.neuron_count", 1000)
```

**List Keys:**

```python
config = pynlm.createDefaultConfig()
all_keys = config.getKeys()
print("All configuration keys:", all_keys)
```

**Enumerations:**

```python
# Neuron types
print(pynlm.NeuronType.Excitatory)
print(pynlm.NeuronType.Inhibitory)
print(pynlm.NeuronType.Sensory)
print(pynlm.NeuronType.Motor)
print(pynlm.NeuronType.Modulatory)
print(pynlm.NeuronType.Internal)

# Developmental stages
print(pynlm.DevelopmentalStage.Initial)
print(pynlm.DevelopmentalStage.CriticalPeriod)
print(pynlm.DevelopmentalStage.Maturation)
print(pynlm.DevelopmentalStage.Adult)
print(pynlm.DevelopmentalStage.Aging)

# Action types
print(pynlm.ActionType.MoveForward)
print(pynlm.ActionType.TurnLeft)
print(pynlm.ActionType.Interact)
print(pynlm.ActionType.Eat)
```

#### AgentBrain Class

**Sensory Processing:**

```python
import pynlm

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

**Motor Decoding:**

```python
# Decode motor command from brain activity
motor_cmd = agent.decodeMotorCommand()
print(f"Action: {motor_cmd.getType()}")
```

**Neuromodulation:**

```python
# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
```

**Development:**

```python
# Update development
agent.updateDevelopment(timestep)

# Get developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")
```

#### SimpleWorld Class

**Configuration:**

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()
world.setAgentStart(10.0, 10.0)
```

**Control:**

```python
# Update world
world.update(timestep=0.1)

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent body
body = world.getAgentBody()

# Apply motor command
world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

**World Objects:**

```python
# Add resource
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))

# Remove object
world.removeObject(x, y)

# Check valid position
is_valid = world.isValidPosition(x, y)
```

---

## Usage Examples

### Basic Simulation

```python
import pynlm

def basic_simulation():
    """Run a simple neural simulation without world interaction."""
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        # Optional: log statistics
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    print("Basic simulation completed!")

# Run the simulation
basic_simulation()
```

### Brain in World Environment

```python
import pynlm

def brain_world_simulation():
    """Run a complete brain-in-world simulation."""
    # Setup brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation loop
    for step in range(500):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("World simulation completed!")

# Run the simulation
brain_world_simulation()
```

### Complete Agent

```python
import pynlm

def run_agent_simulation(num_steps=1000):
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
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
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
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # 8. Print final results
    print("Agent simulation complete!")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(1000)
```

### Advanced Configuration

```python
import pynlm

def advanced_simulation():
    """Run simulation with advanced configuration and monitoring."""
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run with advanced monitoring
    stats = {
        "steps": 0,
        "max_firing": 0,
        "total_spikes": 0,
        "reward_sum": 0.0,
        "curiosity_sum": 0.0
    }
    
    for step in range(2000):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        stats["steps"] += 1
        stats["max_firing"] = max(stats["max_firing"], brain.getFiringNeuronCount())
        stats["total_spikes"] += brain.getTotalSpikeCount()
        stats["reward_sum"] += reward
        stats["curiosity_sum"] += agent.getCuriosityLevel()
        
        # Periodic detailed output
        if step % 200 == 0:
            print(f"Step {step}: Advanced simulation running...")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # Print final statistics
    print("\n=== Advanced Simulation Results ===")
    print(f"Total steps: {stats['steps']}")
    print(f"Maximum firing neurons: {stats['max_firing']}")
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Average firing rate: {stats['total_spikes']/stats['steps']:.2f}")
    print(f"Average reward: {stats['reward_sum']/stats['steps']:.3f}")
    print(f"Average curiosity: {stats['curiosity_sum']/stats['steps']:.3f}")
    
    return stats

# Run advanced simulation
advanced_stats = advanced_simulation()
```

---

## Troubleshooting Guide

### Common Issues and Solutions

#### 1. "My brain isn't doing anything"

**Problem:** Brain seems inactive during simulation.

**Solutions:**
- Ensure `brain.initialize()` is called before `brain.step()`
- Try running more simulation steps (minimum 10)
- Check that neurons have input: `brain.injectCurrentToNeurons(NeuronType.Sensory, 30.0f)`
- Enable curiosity or neuromodulation for more activity: `agent.enableCuriosity(True)`

#### 2. "The agent isn't moving"

**Problem:** Agent produces no motor actions.

**Solutions:**
- Ensure `world.applyMotorCommand()` is called after `agent.decodeMotorCommand()`
- Check that `world.update()` is being called in the simulation loop
- Verify that motor neurons are firing: `print(f"Motor neurons: {len(pImpl->motorNeurons)}")`
- Check action selection logic in `AgentBrain.cpp`

#### 3. "Everything is 0"

**Problem:** All simulation values are zero.

**Solutions:**
- Brains need time to "warm up" - try more steps (minimum 100)
- Some neurons need input to fire - make sure sensory input is connected
- Check that `agent.enableRewardModulation(True)` is called for learning
- Verify that `config->set("random_seed", 42, ConfigSource::Default)` is set

#### 4. "Memory systems not working"

**Problem:** Memory systems (working, episodic, associative) not functioning.

**Solutions:**
- Ensure memory systems are initialized in the brain
- Check that memory capacity is sufficient
- Verify that `agent.enableDevelopment(True)` is called
- Look for memory-related log messages in the output

#### 5. "Dopamine system not working"

**Problem:** Dopamine-based reward prediction and learning not functioning.

**Solutions:**
- Ensure dopamine is enabled: `agent.enableRewardModulation(True)`
- Check dopamine configuration in `config.set("neuromod.dopamine.scale", 1.0)`
- Verify that reward signals are being applied: `agent.applyRewardModulation(reward, predicted)`
- Look for dopamine-related log messages

### Debug Strategies

#### 1. Enable Verbose Logging

```bash
# Enable debug logging
nlm --verbose --log-level DEBUG

# Or in Python
config = pynlm.createDefaultConfig()
config.set("logging.level", "DEBUG")
```

#### 2. Check Log Files

```bash
# View log file
tail -f nlm.log

# Search for specific events
grep "ERROR" nlm.log | head -20
grep "Dopamine" nlm.log | head -20
grep "Learning" nlm.log | head -20
```

#### 3. Use Simple World

For debugging, use a simple world with known behavior:

```python
# Create simple world
world = pynlm.createSimpleWorld()
world.configure(width=5, height=5, visionWidth=3, visionHeight=3)
world.reset()
world.setAgentStart(2.0, 2.0)

# Add visible objects
world.addObject(pynlm.WorldObject(1.0, 1.0, pynlm.WorldObjectType.Resource, value=1.0))
```

#### 4. Step-by-Step Debugging

```python
# Run one step at a time for debugging
for step in range(10):
    print(f"\n=== Step {step} ===")
    world.update(0.1)
    percept = world.getSensoryPercept()
    print(f"Percept: {percept}")
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    print(f"Action: {action.getType()}")
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### 5. Configuration Validation

```python
# Validate configuration before running
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

if not config.isValid():
    print("Configuration errors:")
    for error in config.getValidationErrors():
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### Common Error Messages and Solutions

| Error Message | Likely Cause | Solution |
|---------------|--------------|----------|
| "Failed to initialize brain" | Invalid configuration | Check configuration parameters | |
| "No sensory neurons found" | Brain not properly initialized | Ensure `brain.initialize()` is called | |
| "Reward system not working" | Dopamine not enabled | Call `agent.enableRewardModulation(True)` | |
| "Memory capacity exceeded" | Memory system not configured | Increase memory capacity or disable memory | |
| "Checkpoint save failed" | Invalid checkpoint path | Ensure directory exists and is writable | |

### Performance Troubleshooting

#### 1. Simulation Too Slow

**Solutions:**
- Reduce number of neurons
- Disable expensive features: `agent.enableCuriosity(False)`
- Reduce simulation steps
- Use smaller world size

#### 2. Memory Usage High

**Solutions:**
- Disable episodic memory: `agent.enableDevelopment(False)`
- Reduce memory capacity
- Enable checkpointing to save memory

#### 3. Visualization Too Slow

**Solutions:**
- Disable visualization: `--disable-visualization`
- Reduce visualization complexity
- Use simpler visualization type

---

## Performance Tips

### 1. Optimize Configuration

```python
# Optimized configuration for best performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Moderate size
config.set("brain.synapse_density", 0.1)  # Reasonable connectivity
config.set("plasticity.stdp.learning_rate", 0.001)  # Appropriate learning rate
config.set("neuromod.dopamine.scale", 1.0)  # Default scale
```

### 2. Enable Efficient Features

```python
# Enable features that improve performance
agent.enableDevelopment(True)  # Better memory management
agent.enableCuriosity(True)   # Better exploration
agent.enableRewardModulation(True)  # Better learning efficiency
```

### 3. Monitor Performance

```python
import time

# Time the simulation
start_time = time.time()

# Run simulation
for step in range(1000):
    brain.step(step)

end_time = time.time()
elapsed = end_time - start_time
print(f"Simulation completed in {elapsed:.2f} seconds")
print(f"Rate: {1000/elapsed:.0f} steps/second")
```

### 4. Use Appropriate World Size

```python
# For different performance levels
# High performance (fast)
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)

# Moderate performance (balanced)
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Detailed performance (slower but more detailed)
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
```

### 5. Memory Management

```python
# Enable checkpointing to save memory
agent.enableDevelopment(True)  # Enables checkpointing

# Monitor memory usage
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

---

## Extending NLM

### 1. Adding Custom Plasticity Rules

```cpp
// Create a custom plasticity rule
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Custom learning rule implementation
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        // Custom weight change application
    }
    
    const char* getName() const override {
        return "CustomPlasticity";
    }
};
```

### 2. Adding Custom Neuromodulators

```cpp
// Create a custom neuromodulator
class CustomNeuromodulator : public Neuromodulator {
public:
    void update(TimestepDuration dt) override {
        // Custom neuromodulation dynamics
    }
    
    float getPlasticityFactor() const override {
        // Custom plasticity factor calculation
        return 1.0f;
    }
    
    const char* getName() const override {
        return "CustomNeuromodulator";
    }
};
```

### 3. Adding Custom Brain Regions

```cpp
// Create a custom brain region
class CustomBrainRegion : public NeuralRegion {
public:
    CustomBrainRegion(RegionId id, const std::string& name) : NeuralRegion(id, name) {}
    
    void initialize() override {
        // Custom region initialization
        NeuralRegion::initialize();
    }
};
```

### 4. Adding Custom Neuron Types

```cpp
// Create a custom neuron type
class CustomNeuron : public Neuron {
public:
    CustomNeuron(NeuronId id) : Neuron(id) {}
    
    void stepLIF(Timestamp currentTime, TimestepDuration dt) override {
        // Custom LIF dynamics
        Neuron::stepLIF(currentTime, dt);
    }
};
```

### 5. Integration with External Systems

```cpp
// Integrate with external reinforcement learning
class ExternalRLIntegration {
public:
    void applyExternalReward(float reward, float prediction_error) {
        // Apply external reward signals to the brain
        brain->applyNeuromodulation(reward, prediction_error);
    }
    
    void updatePolicy(float gradient) {
        // Update external policy based on brain activity
        // Policy gradient calculations
    }
};
```

---

## Advanced Features

### 1. Reward Prediction Error

The dopamine system implements reward prediction error (RPE) learning:

```python
# RPE-based learning
agent.applyRewardModulation(reward, predicted_reward)

# Positive RPE (better than expected)
# Strengthens associations and increases dopamine

# Negative RPE (worse than expected)
# Weakens associations and decreases dopamine
```

### 2. Curiosity-Driven Exploration

```python
# Enable curiosity-driven exploration
agent.enableCuriosity(True)

# Curiosity increases with novelty and prediction error
# Leading to exploration of uncertain states
```

### 3. Structural Plasticity

```python
# Enable structural plasticity (growing new connections)
agent.enableStructuralPlasticity(True)

# Brain can form new synaptic connections over time
# Based on activity and developmental signals
```

### 4. Development and Maturation

```python
# Enable developmental processes
agent.enableDevelopment(True)

# Brain undergoes maturation over time
# Plasticity rates change with developmental stage
# Synaptogenesis and pruning occur
```

### 5. Episodic Memory

```python
# Enable episodic memory
agent.enableDevelopment(True)  # Enables episodic memory

# Brain records experiences as episodes
# Can replay important memories during rest
# Consolidates memories over time
```

### 6. Replay and Consolidation

```python
# Enable memory replay
agent.enableDevelopment(True)

# During rest periods, important memories are replayed
# This strengthens and consolidates learning
# Occurs automatically during simulation
```

---

## Conclusion

This comprehensive documentation provides everything you need to:

1. **Get Started Quickly** with basic simulations
2. **Understand the Architecture** of the NLM brain
3. **Configure and Optimize** your simulations
4. **Debug and Troubleshoot** issues effectively
5. **Extend the System** with custom components
6. **Utilize Advanced Features** for sophisticated learning

The NLM represents a significant step forward in artificial intelligence, combining:

- **Biological plausibility** with computational efficiency
- **Modular design** with comprehensive integration
- **Performance** with scalability
- **Flexibility** with robustness

Whether you're a researcher, developer, or student, NLM provides the tools to explore the frontiers of artificial neural systems. The combination of biological inspiration and computational innovation makes NLM a powerful platform for developing intelligent, adaptive systems.

---

*Document Version: 1.0*
*Last Updated: $(date)*
*For more information, visit: https://github.com/nlm-project/nlm*