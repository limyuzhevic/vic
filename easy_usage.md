# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is a brain simulator for computers that creates virtual brains capable of learning, memory, and decision-making. It uses spiking neural networks inspired by biological brains.

### Core Concepts:

| Concept | Explanation | Analogy |
|---------|-------------|---------|
| **Neurons** | Brain cells that process and transmit information | Electrons in a circuit |
| **Synapses** | Connections between neurons that carry signals | Wires between circuit components |
| **Spiking** | Neurons send electrical pulses when activated | Digital signals |
| **Learning** | Connections strengthen/weakness based on activity | Rewiring a circuit |
| **Memory** | Persistent patterns of neural activity | Stored data |

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks and processes information
2. **World** - The environment the brain interacts with
3. **Agent** - The interface connecting brain to world (senses and acts)

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize the brain (required before use)
brain.initialize()

# Step 3: Run thinking for 100 simulation steps
for i in range(100):
    brain.step(i)  # Each step processes neural activity

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Complete Agent in World

```python
import pynlm

# 1. Create and initialize the brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# 2. Create a 2D world for the agent to explore
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# 3. Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable advanced capabilities (optional but recommended)
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore novel things

# 5. Run complete simulation
for step in range(500):
    # Update world state
    world.update(0.1)  # 0.1 second timestep
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Send sensory data to brain
    agent.processSensoryInput(percept)
    
    # Run neural processing
    brain.step(step)
    
    # Brain decides on action
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Understanding the Brain Architecture

### How Neural Processing Works

1. **Input** → Sensory neurons receive world information
2. **Processing** → Neurons integrate and fire based on inputs
3. **Communication** → Spike signals propagate through synapses
4. **Output** → Motor neurons send action commands
5. **Feedback** → Results modify connection strengths (learning)

### Key Components:

| Component | Purpose | Typical Usage |
|-----------|---------|---------------|
| `pynlm.createBrain(config)` | Creates neural network | Every simulation |
| `brain.initialize()` | Sets up initial state | Before first use |
| `brain.step(step_number)` | Runs one processing cycle | Inside simulation loop |
| `pynlm.createSimpleWorld()` | Creates simulation environment | Optional for basic use |
| `pynlm.createAgentBrain(brain)` | Connects brain to world | Required for interaction |

---

## Mini Projects

### Project 1: Brain Development Study

```python
import pynlm
import time

# Create brain with development enabled
config = pynlm.createDefaultConfig()
# Enable developmental plasticity
config.set("brain.development.enable", True)
config.set("brain.development.critical_period", 100)

brain = pynlm.createBrain(config)
brain.initialize()

# Track development over time
development_stages = []
for step in range(500):
    brain.step(step)
    
    # Record developmental stage
    stage = brain.getDevelopmentalStage()
    development_stages.append(stage)
    
    if step % 100 == 0:
        print(f"Step {step}: {stage} - {brain.getFiringNeuronCount()} neurons active")

print(f"Development complete. Final stage: {development_stages[-1]}")
```

### Project 2: Learning Environment

```python
import pynlm

# Setup learning-capable agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)  # Reinforcement learning
agent.enableCuriosity(True)         # Intrinsic motivation
agent.enableStructuralPlasticity(True)  # Network growth
agent.enableDevelopment(True)       # Maturation

# Create learning environment with rewards
for episode in range(10):
    world.reset()
    rewards = []
    
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world (simple example)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        rewards.append(reward)
    
    avg_reward = sum(rewards) / len(rewards)
    print(f"Episode {episode}: Average reward = {avg_reward:.3f}")

print("Learning complete!")
```

### Project 3: Memory Investigation

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
config.set("memory.working.capacity", 100)  # Working memory size
config.set("memory.episodic.enable", True)   # Enable episodic memory
config.set("memory.associative.enable", True)  # Enable associative memory

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()

agent.initialize(world)

# Run memory test
print("Testing memory systems...")
memory_log = []

for step in range(300):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log memory statistics every 50 steps
    if step % 50 == 0:
        working_mem = brain.getWorkingMemoryCount()
        episodic_mem = brain.getEpisodicMemoryCount()
        memory_log.append((step, working_mem, episodic_mem))
        print(f"Step {step}: Working={working_mem}, Episodic={episodic_mem}")

print("\nMemory statistics:")
for step, working, episodic in memory_log:
    print(f"  Step {step}: {working} working, {episodic} episodic memories")
```

---

## Common Patterns

### Pattern 1: Standard Simulation Loop

```python
def run_standard_simulation(brain, world, agent, num_steps):
    """Generic simulation pattern for most NLM use cases."""
    for step in range(num_steps):
        # 1. Update environment
        world.update(0.1)
        
        # 2. Perceive environment
        percept = world.getSensoryPercept()
        
        # 3. Process perception
        agent.processSensoryInput(percept)
        
        # 4. Neural processing
        brain.step(step)
        
        # 5. Generate action
        action = agent.decodeMotorCommand()
        
        # 6. Act in world
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain.getTotalSpikeCount()

# Usage:
# brain = pynlm.createBrain(pynlm.createDefaultConfig())
# brain.initialize()
# world = pynlm.createSimpleWorld()
# agent = pynlm.createAgentBrain(brain)
# run_standard_simulation(brain, world, agent, 1000)
```

### Pattern 2: Monitoring and Logging

```python
def run_simulation_with_monitoring(brain, world, agent, num_steps, log_interval=50):
    """Pattern for long-running simulations with logging."""
    stats = []
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Log statistics
        if step % log_interval == 0:
            stat = {
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel()
            }
            stats.append(stat)
            print(f"Step {step}: {stat['firing_neurons']} neurons firing")
    
    return stats
```

### Pattern 3: Learning with Rewards

```python
def run_learning_simulation(brain, world, agent, num_steps):
    """Pattern for training agents with reward-based learning."""
    learning_stats = []
    
    for step in range(num_steps):
        world.update(0.1)
        
        # Get current state
        percept = world.getSensoryPercept()
        prev_action = agent.decodeMotorCommand()
        prev_reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        
        # Process and act
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(prev_reward, 0.0)
        
        # Log learning progress
        if step % 100 == 0:
            prediction_error = agent.getPredictionError()
            neuromodulation = agent.getNeuromodulationLevel()
            learning_stats.append({
                'step': step,
                'prediction_error': prediction_error,
                'neuromodulation': neuromodulation
            })
            print(f"Step {step}: PE={prediction_error:.3f}, NM={neuromodulation:.3f}")
    
    return learning_stats
```

---

## Advanced Configuration

### Custom Brain Parameters

```python
import pynlm

# Create custom configuration
custom_config = pynlm.createDefaultConfig()

# Modify brain parameters
custom_config.set("brain.neuron_count", 2000)  # Double neurons
custom_config.set("brain.synapse_density", 0.15)  # More connections
custom_config.set("brain.v_thresh", -45.0)  # Lower threshold
custom_config.set("brain.tau_mem", 15.0)  # Slower decay

# Enable specific plasticity mechanisms
custom_config.set("plasticity.stdp.enable", True)
custom_config.set("plasticity.hebbian.enable", True)
custom_config.set("plasticity.structural.enable", False)  # Turn off structural

# Create brain with custom config
brain = pynlm.createBrain(custom_config)
brain.initialize()
```

### World Configuration Options

```python
world = pynlm.createSimpleWorld()

# Set different world parameters
world.configure(
    width=30,                    # Map size (width)
    height=30,                   # Map size (height)
    visionWidth=12,              # Agent's vision range (width)
    visionHeight=12,             # Agent's vision range (height)
    maxEnergy=200.0,             # Maximum agent energy
    energyDecayRate=0.01,        # Energy loss per step
    timeStep=0.05,               # Simulation timestep
    randomSeed=42                # Reproducible randomness
)

world.reset()  # Initialize with current settings
world.setAgentStart(15.0, 15.0)  # Start position
```

---

## Quick Reference

### Creation and Initialization

| Action | Code Example | Purpose |
|--------|--------------|---------|
| Create brain | `pynlm.createBrain(config)` | Make neural network |
| Initialize brain | `brain.initialize()` | Set up initial state |
| Create world | `pynlm.createSimpleWorld()` | Make environment |
| Configure world | `world.configure(...)` | Set world parameters |
| Create agent | `pynlm.createAgentBrain(brain)` | Connect brain to world |
| Initialize agent | `agent.initialize(world)` | Setup agent interface |

### Processing Loop

| Action | Code Example | Purpose |
|--------|--------------|---------|
| Update world | `world.update(0.1)` | Advance simulation time |
| Get perception | `world.getSensoryPercept()` | See what's around |
| Process input | `agent.processSensoryInput(percept)` | Send to brain |
| Brain step | `brain.step(step)` | Neural computation |
| Get action | `agent.decodeMotorCommand()` | Read brain's decision |
| Apply action | `world.applyMotorCommand(action, time)` | Do something in world |

### Getting Statistics

| Metric | Code Example | What it Shows |
|--------|--------------|---------------|
| Total neurons | `brain.getTotalNeuronCount()` | Network size |
| Firing neurons | `brain.getFiringNeuronCount()` | Active neurons |
| Total spikes | `brain.getTotalSpikeCount()` | Neural activity |
| Firing rate | `brain.getAverageFiringRate()` | Average activity |
| Development | `brain.getDevelopmentalStage()` | Brain maturity |
| Curiosity | `agent.getCuriosityLevel()` | Exploration drive |
| Novelty | `agent.getNoveltyLevel()` | Surprise detection |

---

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
```
# Problem: Brain appears inactive
# Solution: Check initialization and enable features
brain.initialize()  # Always call this first!
agent.enableRewardModulation(True)  # Enable learning
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

**"The agent isn't moving"**
```
# Problem: Agent stuck in place
# Solution: Check action decoding and world interaction
print(f"Action: {agent.decodeMotorCommand()}")
print(f"World time: {world.getSimulationTime()}")
world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

**"Everything is 0"**
```
# Problem: No neural activity
# Solution: Warm up the brain and check connections
for i in range(100):  # Warm up period
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} firing")
```

**"High CPU usage"**
```
# Problem: Simulation too slow
# Solution: Optimize configuration
config.set("brain.neuron_count", 500)  # Reduce neurons
config.set("brain.synapse_density", 0.05)  # Reduce connections
world.configure(width=10, height=10)  # Smaller world
```

---

## Next Steps

When you're comfortable:

1. **Read HOW_TO_USE.md** - For detailed API reference and advanced topics
2. **Explore docs/ARCHITECTURE.md** - To understand system design
3. **Try examples in nlm_demo.py** - See working demonstrations
4. **Experiment with configs/** - Different brain configurations
5. **Check tests/ directory** - More examples and testing patterns

The NLM system is powerful and flexible. Start simple, then gradually add complexity as you understand how the brain, world, and agent components interact.