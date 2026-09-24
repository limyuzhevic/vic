# NLM Complete Beginner's Tutorial

## Welcome to NLM!

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator that allows you to create virtual brains that can learn, remember things, and make decisions - just like how real brains work!

This tutorial will guide you through everything you need to know to start using NLM effectively. We'll cover:

1. Setting up your environment
2. Creating your first brain
3. Understanding the core concepts
4. Building complete simulations
5. Exploring advanced features
6. Troubleshooting common issues

Let's get started!

## Chapter 1: Setting Up Your Environment

### 1.1 Installing NLM

NLM comes with both C++ core and Python bindings. Here's how to install:

#### Option 1: Install from Package (Recommended)

```bash
# Install using pip (includes pre-built binaries)
pip install pynlm
```

#### Option 2: Build from Source

```bash
# Clone the repository
1. Clone repository:
   git clone https://github.com/nlm-project/nlm.git
   cd nlm

2. Install Python dependencies:
   pip install scikit-build-core pybind11 numpy

3. Build and install:
   pip install .

# Or in development mode:
# pip install -e .
```

#### Option 3: Build with CMake

```bash
# Clone the repository
1. Clone repository:
   git clone https://github.com/nlm-project/nlm.git
   cd nlm

2. Create build directory:
   mkdir build
   cd build

3. Configure CMake:
   cmake .. -DCMAKE_BUILD_TYPE=Release

4. Build:
   make -j$(nproc)

# Install Python bindings:
cd ../python
pip install .
```

### 1.2 Verifying Your Installation

Let's test that everything is working:

```python
import pynlm
print("NLM version:", pynlm.__version__)
print("NLM description:", pynlm.__description__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Brain initialized successfully: {brain.getTotalNeuronCount()} neurons")
```

### 1.3 Running Example Scripts

NLM comes with example scripts. Let's look at what's available:

```bash
# Find example scripts
find . -name "*.py" -path "*/examples/*" -o -name "*example*.py"

# Run some examples
python examples/advanced_examples.py --demo-mode
```

## Chapter 2: Understanding NLM Architecture

### 2.1 The Three Core Components

NLM is built around three main concepts:

#### 1. Brain - The Virtual Brain
A brain is a collection of neurons that can think, learn, and make decisions.

**Key properties:**
- Millions of virtual neurons that fire like real brain cells
- Complex connections (synapses) between neurons
- Learning mechanisms (plasticity) that strengthen/weaken connections
- Memory systems for storing experiences
- Development stages that affect learning capacity

#### 2. World - The Environment
The world is the environment where the brain lives and interacts with.

**Key properties:**
- 2D simulation space with objects and resources
- Sensory input (vision, touch, internal signals)
- Motor output (actions the agent can take)
- Physics and dynamics for interaction

#### 3. Agent - The Bridge
The agent connects the brain to the world, handling:

**Key properties:**
- Receives sensory input from the world
- Processes sensory information and passes it to the brain
- Decodes brain activity to determine actions
- Applies learning signals (rewards) from the world

### 2.2 The Brain-World Interaction Loop

```
WORLD
  ↓ (Sensory Input)
SENSORY PERCEPT
  ↓ (Processing)
BRAIN (Neural Processing)
  ↓ (Action Selection)
MOTOR COMMAND
  ↓ (Execution)
WORLD CONSEQUENCE
  ↓ (Reward/Feedback)
BRAIN (Learning)
```

This is the fundamental loop that drives all NLM simulations!

### 2.3 Key Terminology

- **Neuron**: Virtual brain cell that fires electrical signals
- **Synapse**: Connection between neurons that transmits signals
- **Spike**: A single electrical signal from a neuron
- **Firing Rate**: How often neurons fire (spikes per second)
- **Plativity**: Learning mechanism that changes connection strengths
- **Neuromodulation**: Chemicals that affect learning and behavior
- **Development**: Brain maturation over time
- **Memory**: Storing and recalling experiences
- **Prediction**: Anticipating future events

## Chapter 3: Your First Brain

### 3.1 Creating a Simple Brain

Let's start with the simplest possible brain:

```python
import pynlm

# Step 1: Create a configuration
config = pynlm.createDefaultConfig()

# Step 2: Create the brain
brain = pynlm.createBrain(config)

# Step 3: Initialize the brain
brain.initialize()

# Step 4: Check brain properties
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Brain initialized successfully!")
```

### 3.2 Running a Brain Simulation

Now let's make the brain think:

```python
import pynlm
import time

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Starting brain simulation...")
start_time = time.time()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Print progress every 200 steps
    if step % 200 == 0:
        firing_rate = brain.getAverageFiringRate()
        firing_neu = brain.getFiringNeuronCount()
        print(f"Step {step}: Firing rate={firing_rate:.1f} Hz, "
              f"Firing neurons={firing_neu}")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 3.3 Understanding Brain Statistics

Here are the key statistics you should understand:

```python
# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Key statistics
print("=== Brain Statistics ===")
print(f"Total Neurons: {brain.getTotalNeuronCount()}")
print(f"Total Synapses: {brain.getTotalSynapseCount()}")
print(f"Active Neurons: {brain.getActiveNeuronCount()}")
print(f"Firing Neurons: {brain.getFiringNeuronCount()}")
print(f"Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitatory/Inhibitory Ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Total Spikes: {brain.getTotalSpikeCount()}")
print(f"Pending Spike Events: {brain.getPendingSpikeEventCount()}")

# Development stage
print(f"Developmental Stage: {brain.getDevelopmentalStage()}")
```

## Chapter 4: Adding a World and Agent

### 4.1 Creating a World

Now let's add an environment for the brain to interact with:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,          # World width
    height=20,         # World height
    visionWidth=8,     # Agent's vision width
    visionHeight=8     # Agent's vision height
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

print("World created successfully!")
print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Agent starts at: (10.0, 10.0)")
```

### 4.2 Creating an Agent

The agent connects the brain to the world:

```python
import pynlm

# Create brain and world (from previous example)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create agent
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)

print("Agent created and initialized!")
print(f"Agent sensory input size: {agent.getSensoryInputSize()}")
print(f"Agent motor output size: {agent.getMotorOutputSize()}")
```

### 4.3 Complete Simulation Loop

Now let's put it all together:

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.5, 7.5)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Starting complete simulation...")
print("Press Ctrl+C to stop")

# Main simulation loop
try:
    for step in range(1000):
        # 1. Update world
        world.update(timestep=0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Avg firing rate: {brain.getAverageFiringRate():.1f} Hz")
            print(f"  Action: {action.getType()}")
            
except KeyboardInterrupt:
    print("\nSimulation stopped by user")

print("Simulation complete!")
```

## Chapter 5: Enabling Learning

### 5.1 Enabling Reward-Based Learning

Let's enable the agent's ability to learn from rewards:

```python
import pynlm

# Create complete setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)    # Learn from rewards
agent.enableCuriosity(True)           # Explore new things
agent.enableDevelopment(True)         # Brain matures over time
agent.enableStructuralPlasticity(True) # Grow new connections

print("Learning subsystems enabled:")
print(f"  Reward Modulation: {agent.isRewardModulationEnabled()}")
print(f"  Curiosity: {agent.isCuriosityEnabled()}")
print(f"  Development: {agent.isDevelopmentEnabled()}")
print(f"  Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
```

### 5.2 Applying Reward Learning

Now let's make the agent learn from its actions:

```python
import pynlm

# Setup (same as before)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.5, 7.5)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)

print("Running simulation with reward learning...")

# Run simulation with learning
for step in range(500):
    world.update(0.1)
    
    # Get sensory input and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply learning
    # In this simple world, getting food gives positive reward
    reward = 1.0 if action.getType() == pynlm.MotorCommand.Interact else 0.0
    
    # Apply reward modulation (this is where learning happens!)
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Monitor learning
    if step % 100 == 0:
        curiosity = agent.getCuriosityLevel()
        prediction_error = agent.getPredictionError()
        print(f"Step {step}: Reward={reward}, Curiosity={curiosity:.3f}, "
              f"Prediction Error={prediction_error:.3f}")
```

## Chapter 6: Exploring NLM Features

### 6.1 Brain Regions

Brains can have different regions for different functions:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add different regions
cortex_id = brain.addRegion("Cortex")
hippocampus_id = brain.addRegion("Hippocampus")
amygdala_id = brain.addRegion("Amygdala")

print(f"Created regions: Cortex={cortex_id}, Hippocampus={hippocampus_id}, "
      f"Amygdala={amygdala_id}")
print(f"Total regions: {brain.getRegionCount()}")
```

### 6.2 Memory Systems

NLM has multiple memory systems:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access memory systems
working_mem = brain.getWorkingMemory()
episodic_mem = brain.getEpisodicMemory()
associative_mem = brain.getAssociativeMemory()

print("Memory systems accessible:")
print(f"  Working Memory: {working_mem is not None}")
print(f"  Episodic Memory: {episodic_mem is not None}")
print(f"  Associative Memory: {associative_mem is not None}")
```

### 6.3 Neuromodulation

The brain has multiple neuromodulators that affect behavior:

```python
def explore_neuromodulation(brain):
    """Explore different neuromodulators"""
    
    # Access neuromodulators
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    # Get agent for neuromodulation control
    config = pynlm.createDefaultConfig()
    agent_brain = pynlm.createAgentBrain(brain)
    
    print("Neuromodulation systems:")
    print(f"  Dopamine: {dopamine is not None}")
    print(f"  Curiosity: {curiosity is not None}")
    print(f"  Novelty: {novelty is not None}")
    print(f"  Prediction Error: {prediction_error is not None}")
    
    # Enable different neuromodulators
    agent_brain.enable_curiosity(True)
    agent_brain.enable_reward_modulation(True)
    
    return agent_brain

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = explore_neuromodulation(brain)
```

## Chapter 7: Saving and Loading

### 7.1 Saving Brain State

Save your trained brain for later use:

```python
import pynlm

# Create and train brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Train for some time
for step in range(1000):
    brain.step(step)

# Save brain state
checkpoint_file = "my_brain.bin"
if brain.save(checkpoint_file):
    print(f"✓ Brain saved to {checkpoint_file}")
else:
    print("✗ Failed to save brain")
```

### 7.2 Loading Brain State

Load a previously saved brain:

```python
import pynlm

# Load previously saved brain
checkpoint_file = "my_brain.bin"
brain = pynlm.createBrain(pynlm.createDefaultConfig())

if brain.load(checkpoint_file):
    print(f"✓ Brain loaded from {checkpoint_file}")
    print(f"Loaded brain has {brain.getTotalNeuronCount()} neurons")
else:
    print("✗ Failed to load brain")
```

## Chapter 8: Common Patterns and Best Practices

### 8.1 Pattern: Complete Agent Simulation

Here's a reusable template for complete agent simulations:

```python
def run_complete_agent_simulation(
    num_steps=1000,
    neuron_count=500,
    enable_learning=True,
    render_progress=True
):
    """Run a complete agent simulation with brain and world."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems if requested
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(timestep=0.1)
        
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
        
        # Apply reward modulation if enabled
        if enable_learning:
            reward = world.getSensoryPercept().getInternal()[0] or 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Print progress
        if render_progress and step % 100 == 0:
            print(f"Step {step}/{num_steps}: "
                  f"Neurons firing={brain.getFiringNeuronCount()}, "
                  f"Firing rate={brain.getAverageFiringRate():.1f} Hz")
    
    # Return final results
    return {
        'final_reward': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'neurons_firing': brain.getFiringNeuronCount(),
        'development_stage': brain.getDevelopmentalStage(),
        'curiosity_level': agent.getCuriosityLevel() if enable_learning else 0.0
    }

# Usage example
results = run_complete_agent_simulation(
    num_steps=500,
    neuron_count=300,
    enable_learning=True
)

print(f"Simulation completed!")
print(f"Results: {results}")
```

### 8.2 Best Practices

#### 1. Always Initialize Before Use
```python
# ✓ Good
brain = pynlm.createBrain(config)
brain.initialize()

# ✗ Bad - This will crash or behave unpredictably
brain = pynlm.createBrain(config)
```

#### 2. Reset Between Simulations
```python
# ✓ Good - Reset for clean state
brain.reset()

# Then re-initialize
brain.initialize()
```

#### 3. Monitor Performance
```python
# ✓ Good - Monitor your simulation
start_time = time.time()
for step in range(num_steps):
    brain.step(step)
end_time = time.time()
print(f"Average step time: {(end_time - start_time)/num_steps*1000:.2f} ms")
```

#### 4. Use Checkpoints
```python
# ✓ Good - Save progress regularly
if step % 1000 == 0:
    brain.save(f"checkpoint_{step}.bin")
```

#### 5. Handle Errors Gracefully
```python
# ✓ Good - Handle errors
for step in range(num_steps):
    try:
        brain.step(step)
    except Exception as e:
        print(f"Error at step {step}: {e}")
        brain.reset()
        brain.initialize()
```

## Chapter 9: Troubleshooting

### 9.1 Common Issues and Solutions

#### Issue 1: "My brain isn't doing anything"
**Problem**: Brain seems inactive
**Solution**:
```python
# Ensure initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Check if neurons are firing
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Try running more steps
for i in range(1000):
    brain.step(i)
```

#### Issue 2: "The agent isn't moving"
**Problem**: Agent actions aren't affecting the world
**Solution**:
```python
# Ensure world updates are happening
world.update(0.1)

# Check if action is being applied
action = agent.decodeMotorCommand()
print(f"Action: {action.getType()}")

# Ensure motor command is valid
result = world.applyMotorCommand(action, world.getSimulationTime())
print(f"Action result: Reward={result.reward}, Success={result.success}")
```

#### Issue 3: "Everything is 0"
**Problem**: All values are zero
**Solution**:
```python
# Brains need time to "warm up"
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### Issue 4: "ImportError: No module named 'pynlm'"
**Problem**: Module not found
**Solution**:
```bash
# Install or reinstall
pip install --force-reinstall pynlm

# Or build from source
pip install -e .
```

### 9.2 Debugging Tips

#### 1. Print Debug Information
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Debug info
print("=== DEBUG INFO ===")
print(f"Config keys: {config.getKeys()}")
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Synapse count: {brain.getTotalSynapseCount()}")
print(f"Region count: {brain.getRegionCount()}")
```

#### 2. Step-by-Step Debugging
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Debug single step
print("=== DEBUG SINGLE STEP ===")
print(f"Before step: Firing neurons = {brain.getFiringNeuronCount()}")
print(f"Before step: Total spikes = {brain.getTotalSpikeCount()}")

brain.step(0)

print(f"After step: Firing neurons = {brain.getFiringNeuronCount()}")
print(f"After step: Total spikes = {brain.getTotalSpikeCount()}")
```

## Chapter 10: Next Steps

### 10.1 Continue Learning

#### 1. Read Advanced Documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit

#### 2. Explore Advanced Examples
```bash
# Look for advanced examples
find examples -name "*.py" | head -10

# Run example scripts
python examples/advanced_examples.py --help
```

#### 3. Join the Community
- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues and discussions on GitHub

### 10.2 Experiment with NLM

#### 1. Modify Configurations
Try different brain configurations:

```python
# Experiment 1: Large brain
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 2000)

# Experiment 2: High connectivity
config2 = pynlm.createDefaultConfig()
config2.set("brain.synapse_density", 0.2)

# Experiment 3: Fast learning
config3 = pynlm.createDefaultConfig()
config3.set("plasticity.stdp.learning_rate", 0.01)
```

#### 2. Test Different Environments
```python
# Modify world settings
world.configure(
    width=30, height=30,      # Larger world
    visionWidth=12, visionHeight=12,  # Better vision
)
world.setMaxEnergy(200.0)       # More energy
```

#### 3. Combine Features
```python
# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation with all features
results = run_complete_agent_simulation(
    num_steps=2000,
    neuron_count=1000,
    enable_learning=True
)
```

### 10.3 Contribute to NLM

If you're interested in contributing to NLM development:

#### 1. Report Issues
- File bugs on GitHub
- Provide detailed reproduction steps
- Include minimal reproduction code

#### 2. Submit Pull Requests
- Fork the repository
- Make your changes
- Submit a pull request
- Include tests and documentation

#### 3. Improve Documentation
- Write tutorials and examples
- Improve existing documentation
- Add code comments and docstrings

#### 4. Help Others
- Answer questions on GitHub
- Help with debugging
- Share your experiments

## Quick Reference Guide

### Core Classes and Methods

| Class | Method | Description |
|-------|--------|-------------|
| Brain | `initialize()` | Initialize brain with configuration |
| Brain | `step(step_number)` | Run one simulation step |
| Brain | `getTotalNeuronCount()` | Get total neuron count |
| Brain | `getFiringNeuronCount()` | Get currently firing neurons |
| Brain | `getAverageFiringRate()` | Get average firing rate |
| Config | `loadFromFile(filepath)` | Load configuration from file |
| Config | `saveToFile(filepath)` | Save configuration to file |
| SimpleWorld | `configure(width, height, visionWidth, visionHeight)` | Configure world |
| SimpleWorld | `update(timestep)` | Update world state |
| SimpleWorld | `applyMotorCommand(cmd, time)` | Apply motor command |
| AgentBrain | `initialize(world)` | Initialize with world |
| AgentBrain | `processSensoryInput(percept)` | Process sensory input |
| AgentBrain | `decodeMotorCommand()` | Decode motor command |
| Action | `getType()` | Get action type |
| Action | `getParameters()` | Get action parameters |

### Common Patterns

```python
# Pattern 1: Basic simulation
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(1000):
    brain.step(step)

# Pattern 2: Complete agent
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Pattern 3: Learning agent
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
for step in range(1000):
    # ... simulation loop ...
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

### Frequently Used Constants

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory

# Action types  
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact

# World object types
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.Adult
```

## Final Tips

1. **Start Simple**: Begin with basic examples and gradually add complexity
2. **Document Your Work**: Keep notes on experiments and parameters
3. **Be Patient**: Neural simulations can take time to run
4. **Debug Systematically**: Isolate issues step by step
5. **Explore Documentation**: Use the built-in documentation and examples
6. **Join the Community**: Get help from other users

Congratulations! You've completed the NLM Beginner's Tutorial! 🎉

You now have all the knowledge needed to:
- Set up NLM environment
- Create and run brain simulations
- Build complete agent-world systems
- Enable learning and development
- Save and load brain states
- Troubleshoot common issues

You're ready to start your own neural learning experiments! 🚀

For more advanced topics and features, check out the [ARCHITECTURE.md](docs/ARCHITECTURE.md) and other documentation files in the docs directory.

Happy neural simulation! 🐭🧠✨