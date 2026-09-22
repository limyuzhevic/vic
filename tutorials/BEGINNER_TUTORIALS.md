# NLM Beginner Tutorials

## Introduction

Welcome to NLM! This guide is designed for people who are new to NLM (Neural Learning Machine). We'll start with the very basics and work our way up through practical examples that you can run immediately.

## Tutorial 1: Your First Brain (0-5 minutes)

### What You're Going to Learn
- How to create a virtual brain
- How neurons work in NLM
- How to run a simple simulation

### Step 1: Set Up Your Environment

First, make sure you have NLM installed:

```bash
# If you haven't installed it yet:
pip install nlm-neural-learning-machine

# Or if you're working with source code:
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
pip install .
```

### Step 2: Create Your First Python Script

Create a file called `first_brain.py` with this code:

```python
import pynlm

print("=== NLM First Brain Tutorial ===")

# 1. Create a brain
print("\n1. Creating a brain...")
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# 2. Turn it on
print("2. Initializing the brain...")
brain.initialize()

# 3. Make it think
print("3. Running simulation for 5 steps...")
for step in range(5):
    brain.step(step)
    print(f"   Step {step}: Brain is thinking...")

# 4. Check the results
print("\n4. Checking results:")
print(f"   Total neurons: {brain.getTotalNeuronCount()}")
print(f"   Total spikes: {brain.getTotalSpikeCount()}")
print(f"   Firing neurons: {brain.getFiringNeuronCount()}")

print("\n=== Success! Your brain ran a simulation! ===")
```

### Step 3: Run the Tutorial

```bash
python first_brain.py
```

### What Happened?

1. **Brain Creation**: `pynlm.createBrain(pynlm.createDefaultConfig())` creates a virtual brain with 1000 neurons by default
2. **Initialization**: `brain.initialize()` sets up the brain for operation
3. **Simulation**: Each call to `brain.step(step)` processes one moment in time:
   - Neurons check their inputs
   - Synaptic connections send signals
   - Learning happens based on activity
4. **Results**: The brain is now active and has recorded spikes (firing events)

## Tutorial 2: Brain in a World (5-10 minutes)

### What You're Going to Learn
- How to create a virtual world
- How an agent connects brain to world
- How perception and action work together

### Step 1: Create the Complete Setup

Create a file called `world_tutorial.py`:

```python
import pynlm

print("=== NLM World Tutorial ===")

# 1. Create the brain
print("\n1. Creating brain...")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# 2. Create the world (environment)
print("2. Creating world...")
world = pynlm.createSimpleWorld()
world.configure(
    width=10,    # World is 10x10 units
    height=10,
    visionWidth=8,   # Agent sees 8x8 pixels
    visionHeight=8
)

# Reset world to start
world.reset()
print("   World reset - agent starts at center")

# 3. Create agent (brain-to-world interface)
print("3. Creating agent...")
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable learning
print("4. Enabling learning features...")
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)        # Explore new things

# 5. Run the simulation loop
print("5. Running simulation for 10 steps...")
for step in range(10):
    # Update world (time passes)
    world.update(0.1)  # Each update is 0.1 seconds
    
    # Get what the agent perceives
    percept = world.getSensoryPercept()
    print(f"   Step {step}: Agent sees world")
    
    # Tell brain what it perceives
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # Do the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Show what's happening
    position = world.getAgentBody().getPosition()
    firing = brain.getFiringNeuronCount()
    curiosity = agent.getCuriosityLevel()
    print(f"     Position: ({position.x:.1f}, {position.y:.1f})")
    print(f"     Neurons firing: {firing}")
    print(f"     Curiosity: {curiosity:.2f}")

# 6. Final results
print("\n6. Final results:")
print(f"   Total spikes: {brain.getTotalSpikeCount()}")
print(f"   Average firing rate: {brain.getAverageFiringRate():.1f}")
print(f"   Final curiosity: {agent.getCuriosityLevel():.2f}")

print("\n=== Success! Your brain explored the world! ===")
```

### Step 2: Run the Tutorial

```bash
python world_tutorial.py
```

### Understanding the Agent Loop

This is the core NLM loop that happens every simulation step:

1. **Perception**: `world.getSensoryPercept()` → What the agent sees
2. **Transduction**: `agent.processSensoryInput(percept)` → Brain gets the input
3. **Processing**: `brain.step(step)` → Brain processes information and learns
4. **Action**: `agent.decodeMotorCommand()` → Brain decides what to do
5. **Execution**: `world.applyMotorCommand(action, time)` → Action happens in world
6. **Feedback**: Agent receives reward based on action outcome

## Tutorial 3: Building a Simple Task (10-15 minutes)

### What You're Going to Learn
- How to design a simple task for your brain
- How to give feedback for learning
- How to measure performance

### Step 1: Create a Target Task

Create a file called `target_tutorial.py`:

```python
import pynlm
import math

print("=== NLM Target Finding Task ===")

# Create brain with learning enabled
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller brain for faster learning
brain = pynlm.createBrain(config)
brain.initialize()

# Create world with a target
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Place a target in the world
target_x, target_y = 15.0, 15.0  # Target location
world.addObject(pynlm.WorldObjectType.Resource, target_x, target_y, value=100.0)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print(f"Target placed at ({target_x}, {target_y})")
print("Agent must learn to find the target...")

# Run the task
steps_to_target = []
episode = 0

for episode in range(20):  # 20 episodes of learning
    # Reset for new episode
    world.reset()
    brain.reset()
    agent.reset()
    brain.initialize()
    agent.initialize(world)
    
    steps_for_episode = 0
    target_found = False
    
    # Run episode until target found or timeout
    for step in range(200):
        # Agent's step in world
        world.update(0.1)
        
        # Sense the world
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks and learns
        brain.step(step)
        
        # Decide action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        position = world.getAgentBody().getPosition()
        distance_to_target = math.sqrt((position.x - target_x)**2 + (position.y - target_y)**2)
        
        # Reward for approaching target
        if distance_to_target < 2.0:
            reward = 100.0  # Big reward when close
            if not target_found:
                target_found = True
                steps_to_target.append(steps_for_episode + 1)
                print(f"   Episode {episode}: Target found in {steps_for_episode + 1} steps!")
        elif distance_to_target < 5.0:
            reward = 10.0  # Small reward for being near
        else:
            reward = -0.1  # Small penalty for being far
        
        # Apply reward to learning
        agent.applyRewardModulation(reward, 0.0)
        
        steps_for_episode = step + 1
        
        if target_found or step >= 199:  # Timeout
            break
    
    # Show episode summary
    curiosity = agent.getCuriosityLevel()
    development_stage = brain.getDevelopmentalStage()
    print(f"   Episode {episode} completed: Curiosity={curiosity:.2f}, Stage={development_stage}")

# Show learning results
print("\n=== Task Results ===")
if steps_to_target:
    print(f"Target found in {len(steps_to_target)} episodes")
    print(f"First success: {steps_to_target[0]} steps")
    print(f"Average steps to target: {sum(steps_to_target)/len(steps_to_target):.1f}")
    print(f"Best performance: {min(steps_to_target)} steps")
else:
    print("Target not found within 20 episodes")

print(f"Final development stage: {brain.getDevelopmentalStage()}")
print("=== Task Complete! ===")
```

### Step 2: Run the Tutorial

```bash
python target_tutorial.py
```

### What This Teaches You

1. **Task Design**: Creating a goal-directed behavior (finding a target)
2. **Reward Design**: Giving appropriate feedback based on performance
3. **Learning Curves**: Watching performance improve over episodes
4. **Development**: Observing how the brain matures through development stages

## Tutorial 4: Understanding Brain Statistics

### What You're Going to Learn
- How to read brain statistics
- What different metrics mean
- How to debug brain behavior

### Step 1: Create an Analysis Script

Create a file called `stats_tutorial.py`:

```python
import pynlm
import matplotlib.pyplot as plt

print("=== NLM Statistics Tutorial ===")

# Create and run a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent and world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Collect statistics for 50 steps
steps = list(range(50))
firing_rates = []
spike_counts = []
curiosity_levels = []
novelty_levels = []
development_stages = []

print("\nCollecting statistics...")
for step in range(50):
    # Run simulation
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Collect data
    firing_rates.append(brain.getFiringNeuronCount())
    spike_counts.append(brain.getTotalSpikeCount())
    curiosity_levels.append(agent.getCuriosityLevel())
    novelty_levels.append(agent.getNoveltyLevel())
    development_stages.append(brain.getDevelopmentalStage().name)

print(f"Collected {len(steps)} data points")

# Print summary statistics
print("\n=== Statistics Summary ===")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.1f}")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Show development stage progression
print(f"\nDevelopment stages encountered: {set(development_stages)}")

# Create simple plots (if matplotlib available)
try:
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
    
    # Firing rate over time
    ax1.plot(steps, firing_rates)
    ax1.set_title('Firing Neuron Count Over Time')
    ax1.set_xlabel('Simulation Step')
    ax1.set_ylabel('Number of Firing Neurons')
    ax1.grid(True, alpha=0.3)
    
    # Spike count over time
    ax2.plot(steps, spike_counts)
    ax2.set_title('Total Spikes Accumulated')
    ax2.set_xlabel('Simulation Step')
    ax2.set_ylabel('Total Spikes')
    ax2.grid(True, alpha=0.3)
    
    # Curiosity over time
    ax3.plot(steps, curiosity_levels)
    ax3.set_title('Curiosity Level Over Time')
    ax3.set_xlabel('Simulation Step')
    ax3.set_ylabel('Curiosity Level (0-1)')
    ax3.grid(True, alpha=0.3)
    
    # Novelty over time
    ax4.plot(steps, novelty_levels)
    ax4.set_title('Novelty Level Over Time')
    ax4.set_xlabel('Simulation Step')
    ax4.set_ylabel('Novelty Level (0-1)')
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('brain_statistics.png', dpi=150, bbox_inches='tight')
    print("\nStatistics plot saved as 'brain_statistics.png'")
    
except ImportError:
    print("\nMatplotlib not available - skipping plot generation")
    print("Install with: pip install matplotlib")

print("\n=== Statistics Tutorial Complete ===")
```

### Step 2: Run the Tutorial

```bash
python stats_tutorial.py
```

## Common Issues and Solutions

### Issue 1: "Brain isn't doing anything"

**Problem**: Your brain seems inactive

**Solution**:
1. Make sure you call `brain.initialize()` before `brain.step()`
2. Try running more simulation steps (increase loop range)
3. Check if your brain has enough neurons for activity

### Issue 2: Agent isn't moving

**Problem**: The agent stays in one place

**Solution**:
1. Verify you're calling `world.applyMotorCommand()`
2. Check that `world.update()` is being called
3. Make sure the agent has motor neurons enabled

### Issue 3: Everything shows 0

**Problem**: All statistics are zero

**Solution**:
1. Brains need time to "warm up" - run more steps
2. Ensure sensory input is connected to brain
3. Check that your world has objects the agent can sense

## Practice Exercises

### Exercise 1: Modify the Tutorial Brain Size

Change the brain configuration to create a brain with:
- 500 neurons (half of default)
- Fewer synapses
- See how this affects performance

### Exercise 2: Create a Different World

Design a world with:
- Obstacles the agent must avoid
- Multiple goals to reach
- Changing conditions over time

### Exercise 3: Experiment with Learning Parameters

Try different combinations:
- More or less curiosity
- Different reward values
- Varying development stages

## Next Steps

After completing these tutorials:

1. **Read the full HOW-TO_USE.md** for detailed information
2. **Check out ARCHITECTURE.md** to understand how it all works
3. **Explore the experiment framework** for advanced testing
4. **Try Phase 6 demos** to see all systems integrated

## Quick Reference

| Command | What it does |
|---------|-------------|
| `pynlm.createBrain(config)` | Creates a new brain |
| `brain.initialize()` | Starts up the brain |
| `brain.step(step)` | Runs one simulation step |
| `pynlm.createSimpleWorld()` | Creates a simple world |
| `world.update(time)` | Updates world by time |
| `agent.processSensoryInput(percept)` | Feeds input to brain |
| `agent.decodeMotorCommand()` | Gets action from brain |

You now have everything you need to start creating your own NLM brains! Happy experimenting!
