# NLM User Guide: A Comprehensive Introduction to Neural Learning Machine

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain simulation platform. It enables researchers and developers to create, train, and study neural networks that learn and develop through interaction with environments.

### What Makes NLM Special

- **Brain-Inspired**: Uses Leaky Integrate-and-Fire (LIF) neurons with realistic synaptic dynamics
- **Developmental**: Brains grow, learn, and adapt over time through experience
- **Integrated Systems**: Combines memory, neuromodulation, prediction, and cognition
- **Interactive**: Can learn from environments through sensory input and motor output
- **Research-Focused**: Designed for studying neural computation and learning mechanisms

## Quick Start

### Step 1: Installation and Setup

#### Build from Source (Recommended)

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Install Python bindings
pip install scikit-build-core pybind11
pip install -e .  # Install in development mode
```

#### Simple Installation

If you just want the Python API:

```bash
pip install .
```

### Step 2: Basic Usage

#### Create a Simple Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
```

#### Run a Simple Simulation

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

### Step 3: Complete Agent Example

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
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
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
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
print("Agent simulation complete!")
```

## Core Concepts

### Neurons and Synapses

NLM uses **Leaky Integrate-and-Fire (LIF) neurons** that:

- Maintain a membrane potential that integrates inputs over time
- Leak toward resting potential between spikes
- Fire when threshold potential is reached
- Reset after firing and enter refractory period

**Synapses** connect neurons and:

- Transmit electrical signals (excitatory or inhibitory)
- Implement plasticity rules (STDP, Hebbian)
- Have configurable weights and delays

### Brain Regions

Neurons are organized into **regions**:

- **Sensory regions**: Process input from environment
- **Internal regions**: Handle cognitive processing
- **Motor regions**: Generate output actions

### Memory Systems

NLM includes multiple memory systems:

- **Working Memory**: Active maintenance of current patterns
- **Episodic Memory**: Storage of experienced events
- **Associative Memory**: Pattern completion and recall

### Neuromodulation

Biological neuromodulators:

- **Dopamine**: Reward prediction error, reinforcement learning
- **Curiosity**: Novelty detection, exploration drive
- **Novelty**: Surprise detection, learning trigger
- **Prediction Error**: Mismatch between expected and actual outcomes

### Development

Brains develop through **stages**:

- **Initial**: High plasticity, rapid learning
- **Critical Period**: Sensitive learning window
- **Maturation**: Specialization, refinement
- **Adult**: Stable, efficient operation

## Configuration Options

### Brain Configuration

```python
config = pynlm.createDefaultConfig()

# Configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)
```

### Plasticity Configuration

```python
# Enable and configure plasticity rules
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)

config.set("plasticity.hebbian.enable", true)
config.set("plasticity.structural.enable", true)
```

### Neuromodulation Configuration

```python
# Configure neuromodulation systems
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.novelty.enable", true)
```

## Advanced Usage

### Working with Different Environments

#### Grid World Environment

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)
```

#### Agent Interaction

```python
# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run interaction loop
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Experiment Configuration

#### Phase 6 Integration Experiment

```python
import pynlm

# Create brain with specific configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.region_count", 2)
config.set("learning.rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()

# Create experiment
experiment = pynlm.createPhase6Experiment(brain)

# Configure experiment
experiment.setNeuronCount(500)
experiment.setMaxSteps(2000)
experiment.enableCheckpointing(True)
experiment.enableDevelopment(True)

# Run experiment
result = experiment.run()
print(f"Experiment completed. Reward: {result.totalReward}")
print(f"Memory episodes: {result.memoryEpisodesStored}")
```

### Performance Tuning

#### Optimize for Speed

```python
# Configure for fast simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Moderate size
config.set("plasticity.stdp.enable", false)  # Disable costly plasticity
config.set("neuromod.dopamine.enable", false)  # Disable neuromodulation
config.set("brain.synapse_density", 0.05)  # Sparse connectivity
```

#### Optimize for Accuracy

```python
# Configure for high accuracy
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # Large network
config.set("plasticity.stdp.enable", true)  # Enable STDP
config.set("plasticity.hebbian.enable", true)  # Enable Hebbian learning
config.set("neuromod.curiosity.enable", true)  # Enable exploration
config.set("brain.synapse_density", 0.1)  # Dense connectivity
```

### Saving and Loading

#### Save Brain State

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation
for step in range(1000):
    brain.step(step)

# Save brain state
brain.save("my_brain_checkpoint.bin")
print("Brain saved successfully!")
```

#### Load Brain State

```python
import pynlm

# Create new brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Load saved state
brain.load("my_brain_checkpoint.bin")
print("Brain loaded successfully!")

# Continue simulation
for step in range(500):
    brain.step(step)
```

#### Save Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.01)

# Save configuration
config.saveToFile("my_config.cfg")
print("Configuration saved!")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.cfg")
```

## Common Patterns

### Pattern 1: Run a Complete Agent Simulation

```python
def run_agent_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with brain and world."""
    
    for step in range(num_steps):
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
    
    # Return final statistics
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'final_reward': world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    }
```

### Pattern 2: Get Brain Statistics

```python
# Add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("E/I ratio:", brain.getExcitationInhibitionRatio())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional and can be enabled as needed:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enablePrediction(True)            # Predict future events
agent.enableAttention(True)             # Focus on important stimuli
```

## Troubleshooting

### "My brain isn't doing anything"

**Solution:**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neurons have input to make them fire

### "The agent isn't moving"

**Solution:**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure motor neurons are receiving input

### "Everything is 0"

**Solution:**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that neurons have appropriate threshold and resting potential

### "Memory systems aren't working"

**Solution:**
- Ensure memory systems are enabled in configuration
- Check that episodes are being stored
- Verify that replay mechanisms are working

### "Performance is too slow"

**Solution:**
- Reduce neuron count
- Disable expensive plasticity rules
- Turn off neuromodulation
- Use smaller world sizes

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more detailed usage information
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Read `docs/SCIENCE.md`** for the scientific background
4. **Experiment with different configurations** and see what works best
5. **Run the experiments** in the `src/experiments/` directory to see real usage
6. **Join the NLM community** to share experiences and get help

That's it! You're now ready to use NLM and create your own artificial developmental brains!
