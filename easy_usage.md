# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
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

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Example 4: Advanced Agent with Learning and Exploration

```python
import pynlm

# Setup a learning agent with advanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore new things
agent.enableDevelopment(True)           # Brain matures over time
agent.setMaxExploration(0.7)            # Maximum exploration level
agent.setTemperature(0.6)               # Exploration temperature
agent.setEpsilon(0.2)                   # Epsilon-greedy exploration

# Run simulation with learning
print("Running advanced agent simulation with learning...")
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide some reward signal
    if step % 50 == 0:
        reward = 1.0 if world.getAgentBody().health > 0.5 else 0.0
        agent.applyRewardModulation(reward, 0.5)

print("Advanced simulation complete!")
print("Final stats:")
print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
print(f"  - Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  - Dopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  - Developmental stage: {agent.getDevelopmentalStage()}")
print(f"  - Snapshots created: {len([s for s in dir(agent) if 'snapshot' in s])}")
```

### Example 5: Brain Analysis and Visualization

```python
import pynlm

# Create a brain and analyze its properties
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a few steps to let the brain settle
for i in range(50):
    brain.step(i)

# Analyze brain connectivity
print("=== Brain Connectivity Analysis ===")
analysis = pynlm.analyzeBrainConnectivity(brain)
print(analysis)

# Visualize network state
print("\n=== Network State Visualization ===")
visualization = pynlm.visualizeNetworkState(brain)
print(visualization)

# Export brain data
print("\n=== Brain Data Export ===")
dataExport = pynlm.exportBrainData(brain, "brain_analysis")
print(dataExport[:500] + "..." if len(dataExport) > 500 else dataExport)

print("\nBrain analysis complete!")
```

### Example 6: State Management and Snapshot

```python
import pynlm

# Create and simulate a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

# Create snapshots at different stages
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create snapshots
early_snapshot = agent.createSnapshot()
print(f"Early snapshot: {early_snapshot}")

# Process some input and create another snapshot
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(50)
late_snapshot = agent.createSnapshot()
print(f"Late snapshot: {late_snapshot}")

# Compare snapshots
similarity = agent.compareSnapshots(early_snapshot, late_snapshot)
print(f"Snapshot similarity (0.0 = identical, 1.0 = completely different): {similarity:.3f}")

# Apply snapshot (reset brain to earlier state)
agent.applySnapshot(early_snapshot)
print("Brain reset to early snapshot state")
print(f"Total spikes after reset: {brain.getTotalSpikeCount()}")
```

### Example 7: Simulation Control and Monitoring

```python
import pynlm

# Create agent with monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run controlled simulation
print("Starting controlled simulation...")
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

steps_completed = 0
max_steps = 150
timestep = 0.1

for step in range(max_steps):
    if steps_completed >= max_steps:
        break
        
    world.update(timestep)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step, step * timestep)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * timestep)
    
    steps_completed += 1
    
    # Log progress every 25 steps
    if step % 25 == 0:
        print(f"Step {step}/{max_steps}: "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Spikes: {brain.getTotalSpikeCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Dopamine: {agent.getNeuromodulationLevel():.3f}")

print("Simulation complete!")
print(f"\nFinal Performance Summary:")
print(f"  - Steps completed: {steps_completed}")
print(f"  - Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Performance analysis
performance = pynlm.analyzeAgentPerformance(agent, world, steps_completed)
print(f"\nAgent Performance Analysis:")
print(performance)
```

---

## Advanced Features and Best Practices

### 1. Learning and Adaptation

- **Reward Modulation**: Enable learning from environmental feedback
- **Curiosity-Driven Exploration**: Balance exploitation with exploration
- **Development System**: Simulate brain maturation and changing plasticity

### 2. State Management

- **Snapshots**: Create and compare brain states for experimentation
- **Save/Load**: Persist brain state for later use
- **Reset**: Return to initial state for controlled experiments

### 3. Advanced Configuration

- **Exploration Parameters**: Control how the agent explores its environment
- **Neuromodulation**: Direct control over dopamine, curiosity, novelty
- **Development Control**: Fine-tune developmental stages and timing

### 4. Performance Optimization

- **Batch Processing**: Run multiple simulation steps efficiently
- **Analysis Tools**: Built-in connectivity and performance analysis
- **Visualization**: Generate ASCII visualizations of network states

### 5. Best Practices

#### For Beginners:
```python
# Start simple and build up
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

#### For Researchers:
```python
# Use snapshots for experimental control
brain1 = create_brain()
snapshot1 = agent.createSnapshot()
# Run experiment...
brain2 = create_brain()
snapshot2 = agent.createSnapshot()
# Compare results
```

#### For Production:
```python
# Monitor and log performance
for step in range(10000):
    # Run simulation
    # Log metrics periodically
    # Analyze performance
```

---

## Quick Reference

### Core Operations

| Operation | Function | Description |
|-----------|----------|-------------|
| **Create Brain** | `pynlm.createBrain(config)` | Creates a new neural brain |
| **Initialize** | `brain.initialize()` | Sets up initial brain state |
| **Step Forward** | `brain.step(step)` | Advances simulation by one time step |
| **Create World** | `pynlm.createSimpleWorld()` | Creates simulation environment |
| **Create Agent** | `pynlm.createAgentBrain(brain)` | Creates agent-brain interface |
| **Sensory Input** | `agent.processSensoryInput(percept)` | Injects environmental data |
| **Motor Output** | `agent.decodeMotorCommand()` | Generates actions from neural activity |

### Advanced Operations

| Feature | Function | Description |
|---------|----------|-------------|
| **Enable Learning** | `agent.enableRewardModulation(True)` | Turns on reward-based learning |
| **Set Exploration** | `agent.setTemperature(0.5)` | Controls exploration-exploitation balance |
| **Create Snapshot** | `agent.createSnapshot()` | Captures current brain state |
| **Compare States** | `agent.compareSnapshots(snap1, snap2)` | Measures state similarity |
| **Analyze Network** | `pynlm.analyzeBrainConnectivity(brain)` | Analyzes connectivity patterns |
| **Visualize State** | `pynlm.visualizeNetworkState(brain)` | Generates network visualization |

### Configuration Management

| Operation | Function | Description |
|-----------|----------|-------------|
| **Create Config** | `pynlm.createDefaultConfig()` | Creates default configuration |
| **Save Config** | `pynlm.saveConfig(cfg, "file.cfg")` | Saves to file |
| **Load Config** | `pynlm.loadConfig(cfg, "file.cfg")` | Loads from file |
| **Get Summary** | `pynlm.getConfigSummary(cfg)` | Gets config description |

### State Management

| Function | Description |
|----------|-------------|
| `brain.getTotalNeuronCount()` | Number of neurons in brain |
| `brain.getTotalSynapseCount()` | Number of connections |
| `brain.getAverageFiringRate()` | Average firing frequency |
| `agent.getNeuromodulationLevel()` | Current neuromodulation level |
| `agent.getCuriosityLevel()` | Current curiosity level |
| `agent.getDevelopmentalStage()` | Current developmental stage |

---

## Troubleshooting and Common Issues

### Performance Issues

**Problem**: Simulation is too slow

**Solution**:
- Reduce number of neurons (`config.set("neuron_count", 100)`)
- Decrease simulation steps
- Use more efficient algorithms

**Problem**: Memory usage is high

**Solution**:
- Monitor with `agent.getTotalNeuronCount()` and `brain.getTotalSynapseCount()`
- Use snapshots to manage memory efficiently
- Clear history with `agent.reset()`

### Learning Issues

**Problem**: Agent isn't learning

**Solution**:
- Verify `agent.enableRewardModulation(True)` is called
- Check that rewards are being applied: `agent.applyRewardModulation(reward, predicted)`
- Monitor neuromodulation levels: `agent.getNeuromodulationLevel()`

**Problem**: Agent gets stuck in local optima

**Solution**:
- Increase exploration: `agent.setTemperature(0.8)`
- Enable curiosity: `agent.enableCuriosity(True)`
- Use snapshot comparison for diversity

### Configuration Issues

**Problem**: Configuration not loading

**Solution**:
- Verify file path is correct
- Use `pynlm.getConfigSummary(cfg)` to check loaded values
- Try default config: `config = pynlm.createDefaultConfig()`

**Problem**: Unexpected behavior

**Solution**:
- Use analysis tools to debug
- Create snapshots before/after problematic operations
- Check agent state with `agent.getDevelopmentalStage()`

---

## Next Steps

When you're comfortable with the basics:

### 1. Deepen Your Understanding
- Read `HOW_TO_USE.md` for technical details
- Study `docs/ARCHITECTURE.md` for implementation details
- Explore the source code for advanced features

### 2. Research Applications
- Implement custom learning algorithms
- Create specialized environments
- Design novel neural architectures

### 3. Production Deployment
- Batch process large simulations
- Implement monitoring and logging
- Optimize performance for scale

### 4. Contribution
- Add new Python bindings
- Improve documentation
- Report issues and suggest features


That's it! You're now fully equipped to use NLM with both basic and advanced features. The Python API provides powerful tools for neural simulation, learning, and experimentation.

Happy brain hacking! 🧠✨

