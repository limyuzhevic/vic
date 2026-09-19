# NLM - Easy Usage Guide for Beginners

**Version 2.0 - Complete Learning Experience**

## Table of Contents
1. [Quick Start for Absolute Beginners](#quick-start-for-absolute-beginners)
2. [The 3 Things You Need to Know](#the-3-things-you-need-to-know)
3. [Learning Path & Progression](#learning-path--progression)
4. [Complete Examples](#complete-examples)
5. [Understanding the Components](#understanding-the-components)
6. [Common Patterns](#common-patterns)
7. [Advanced Usage](#advanced-usage)
8. [Troubleshooting & Error Handling](#troubleshooting--error-handling)
9. [Best Practices](#best-practices)

---

## Quick Start for Absolute Beginners

**Welcome!** This guide will take you from "I've never used NLM before" to running a complete simulation in under 10 minutes.

### What You're About to Build
You're going to create a virtual brain that can learn, remember things, and make decisions. Think of it as teaching a computer to think and learn like a brain!

### The Three Essentials

1. **Brain** - The virtual brain that thinks and learns
2. **World** - The environment the brain interacts with
3. **Agent** - The bridge that connects the brain to the world

### Installation (if needed)

```bash
# Install the Python library
pip install pynlm
```

### Your First Brain (30 seconds)

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Turn it on
brain.initialize()

# Make it think
for i in range(10):
    brain.step(i)

print("Success! Your brain is alive.")
print(f"It has thought {brain.getTotalSpikeCount()} times")
print(f"It contains {brain.getTotalNeuronCount()} neurons")
```

**What just happened?** You created a brain with thousands of virtual neurons, started it up, and watched it think for 10 time steps.

---

## The 3 Things You Need to Know

### 1. Brain - The Thinking Engine
The brain is your virtual artificial brain. It contains:
- **Neurons** - Like brain cells that process information
- **Synapses** - Connections between neurons that transmit signals
- **Learning rules** - How neurons adjust their connections based on experience
- **Memory systems** - Ways the brain stores experiences

**Key Methods:**
- `initialize()` - Start up the brain (call this FIRST!)
- `step()` - Make the brain process one moment in time
- `getTotalSpikeCount()` - How active the brain is
- `getTotalNeuronCount()` - How many neurons the brain has

### 2. World - The Environment
The world is where your brain lives and interacts with things. It contains:
- **Visual input** - What the brain can see (like a camera)
- **Touch/collision data** - Physical interactions
- **Internal signals** - The brain's own state
- **Objects and agents** - Things that exist in the environment

**Key Methods:**
- `configure()` - Set up the world size and vision
- `reset()` - Start the world fresh
- `update()` - Advance the world in time
- `getSensoryPercept()` - Get what the brain experiences

### 3. Agent - The Bridge
The agent connects your brain to the world, handling:
- **Sensory input** - Taking in information from the world
- **Motor output** - Turning brain activity into actions
- **Learning** - Helping the brain learn from experience
- **Development** - Making the brain mature over time

**Key Methods:**
- `initialize()` - Connect the agent to the world
- `processSensoryInput()` - Feed sensory data to the brain
- `decodeMotorCommand()` - Get actions from the brain
- `applyRewardModulation()` - Help the brain learn

---

## Learning Path & Progression

### Step 1: Silent Brain (5 minutes)
Just neurons thinking without any input. Great for understanding the basics.

### Step 2: Brain Watching a World (10 minutes)
Your brain observes the world but doesn't act yet. Perfect for sensory processing.

### Step 3: Complete Agent (20 minutes)
Your brain can sense, think, and ACT on the world. The full experience!

### Step 4: Learning Agent (30 minutes)
Enable learning features and watch your brain actually improve through experience.

### Step 5: Development (45 minutes)
Watch the brain mature and change over time. Truly fascinating!

---

## Complete Examples

### Example 1: Just a Brain (Silent)
Perfect for when you want to study neuron dynamics without external influences.

```python
import pynlm

# Create and start brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Let it think for a while
for step in range(100):
    brain.step(step)
    
    # Optional: Print progress
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Learning rate: {brain.getAverageFiringRate():.3f}")

print("✓ Silent brain simulation complete!")
```

### Example 2: Brain Observing the World
Great for understanding how brains process sensory information.

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

# Watch the world
print("Starting observation...")
for step in range(50):
    world.update(0.1)  # Advance world
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Let the brain think
    brain.step(step)
    
    # Optional: Track learning
    if step % 10 == 0:
        print(f"Step {step}:")
        print(f"  Vision size: {len(percept.getVision())}")
        print(f"  Brain activity: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Learning: {brain.getAverageFiringRate():.3f} Hz")

print("✓ Observation complete!")
print(f"Final brain state: {brain.getTotalSpikeCount()} total spikes")
```

### Example 3: Complete Agent with Learning (The Full Experience)
This is where your brain actually interacts with and learns from the world!

```python
import pynlm
import random

def run_agent_simulation():
    print("=== Starting Complete Agent Simulation ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)  # Start in center
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Features enabled: Learning, Curiosity, Development, Growth")
    
    # Run simulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process what the brain sees
        agent.processSensoryInput(percept)
        
        # Let the brain think
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do the action in the world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from the result
        reward = 0.0
        if action == pynlm.MotorCommand.MoveForward:
            reward = 0.5  # Moving forward is good
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Optional: Track progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Action: {action}")
            print(f"  Reward: {reward}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("✓ Agent simulation complete!")
    print(f"Final stats:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation()
```

### Example 4: Learning Experiment
Demonstrates how the brain actually learns through experience.

```python
import pynlm

print("=== Brain Learning Experiment ===")

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable learning
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Record initial state
print("Initial state:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Synapses: {brain.getTotalSynapseCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")

# Run learning experiment
print("\nRunning learning experiment (200 steps)...")
for step in range(200):
    world.update(0.1)
    
    # Add some interesting sensory input
    if step % 20 == 0:
        # Create a visual pattern to learn
        percept = world.getSensoryPercept()
    
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Small reward for exploration
    reward = 0.1 if step % 30 == 0 else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")

# Record final state
print("\nFinal state:")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"  Development stage: {brain.getDevelopmentalStage()}")
print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")

print("\n✓ Learning experiment complete!")
print("The brain has learned through experience!")
```

---

## Understanding the Components

### Brain Class
Think of the Brain class as your virtual artificial brain. It contains:

- **Neurons** - Individual processing units (thousands of them!)
- **Synapses** - Connections between neurons that carry signals
- **Memory systems** - Working memory, episodic memory, associative memory
- **Cognition** - Planning, attention, concept formation
- **Development** - How the brain matures over time

**Main Responsibilities:**
- Process sensory input
- Make decisions
- Store experiences
- Learn from rewards
- Plan actions

### SimpleWorld Class
The world is where your brain lives. Think of it as a 2D environment with:

- **Visual field** - What the brain can see (like eyes)
- **Physical objects** - Things that exist in the world
- **Agent position** - Where the brain is located
- **Time** - The simulation advances in discrete steps

**Key Concepts:**
- The world updates in small time steps
- Your brain gets sensory input from the world
- Actions affect the world
- Rewards come from world interactions

### AgentBrain Class
The agent is like the brain's interface with the world - think of it as the:

- **Sensory bridge** - Takes world information and gives it to the brain
- **Motor translator** - Takes brain activity and turns it into actions
- **Learning coordinator** - Helps the brain learn from experience
- **Developmental guide** - Helps the brain mature

**What It Does:**
1. Receives sensory input from the world
2. Injects it into the brain's neurons
3. Reads motor activity from the brain
4. Converts it to meaningful actions
5. Applies rewards and learning signals
6. Updates development state

---

## Common Patterns

### Pattern 1: Run a Simulation
Use this pattern when you want to run a complete brain-world interaction cycle.

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # 4. Get action
        action = agent.decodeMotorCommand()
        
        # 5. Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Learn
        reward = 0.0  # Or compute your own
        agent.applyRewardModulation(reward, 0.0)
    
    return brain, world, agent
```

### Pattern 2: Get Brain Statistics
Add these lines to your simulation loop to monitor brain state.

```python
# Inside your simulation loop:
print("=== Brain Statistics ===")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
```

### Pattern 3: Enable Advanced Features
Enable all the cool learning features your brain has!

```python
# After creating agent
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Drive exploration
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

print("Advanced features enabled!")
print("The brain will:")
print("  - Learn from reward prediction errors")
print("  - Explore unknown/surprising things")
print("  - Mature and change over time")
print("  - Create new synaptic connections")
```

---

## Advanced Usage

### Custom Configuration
Create custom brain configurations for specific experiments.

```python
import pynlm
import json

# Create default config
config = pynlm.createDefaultConfig()

# Modify specific parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 2.0)

# Save to file
config.saveToFile("my_brain_config.json")

# Or load from file
config.loadFromFile("my_brain_config.json")

# Create brain with custom config
brain = pynlm.createBrain(config)
```

### Batch Processing
Run multiple brains in parallel for experiments.

```python
def run_batch_experiment(num_brains, steps_per_brain):
    brains = []
    worlds = []
    agents = []
    
    # Create batch of brains
    for i in range(num_brains):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning for all
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        brains.append(brain)
        worlds.append(world)
        agents.append(agent)
    
    # Run all brains
    for brain_idx, (brain, world, agent) in enumerate(zip(brains, worlds, agents)):
        print(f"Running brain {brain_idx + 1}/{num_brains}")
        
        for step in range(steps_per_brain):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = 0.1 if step % 20 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
    
    return brains, worlds, agents

# Run batch experiment
brains, worlds, agents = run_batch_experiment(5, 100)
```

---

## Troubleshooting & Error Handling

### Common Issues and Solutions

**"My brain isn't doing anything!"**
```python
# PROBLEM: Brain is silent
# SOLUTION: Check initialization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # ← Always call this!
```

**"The agent isn't moving!"**
```python
# PROBLEM: Agent takes no actions
# SOLUTION: Check world updates and sensory input
world.update(0.1)  # ← Update world first
percept = world.getSensoryPercept()  # ← Get what's seen
agent.processSensoryInput(percept)  # ← Tell brain
brain.step(step)  # ← Brain thinks
action = agent.decodeMotorCommand()  # ← Get action
world.applyMotorCommand(action, world.getSimulationTime())  # ← Do action
```

**"Everything is 0!"**
```python
# PROBLEM: Nothing happening
# SOLUTIONS:
# 1. Give brain more time to warm up
for step in range(1000):  # More steps
    brain.step(step)

# 2. Add sensory input
agent.processSensoryInput(world.getSensoryPercept())

# 3. Enable learning
agent.enableRewardModulation(True)
```

### Error Handling in Python

```python
import pynlm

# Always wrap your code in try-catch blocks
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    print("Brain created successfully")
    
    for i in range(100):
        brain.step(i)
        
except RuntimeError as e:
    print(f"Runtime error: {e}")
    print("Check your configuration or initialization")
    
except Exception as e:
    print(f"Unexpected error: {e}")
    import traceback
    traceback.print_exc()

print("Program ended gracefully")
```

---

## Best Practices

### 1. Always Initialize Before Use
```python
# WRONG - Crashes
brain = pynlm.createBrain(config)
brain.step(0)  # Boom! Not initialized

# RIGHT - Works
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)  # Good!
```

### 2. Reset Before Reusing
```python
# Good practice: Reset before reusing
brain.initialize()  # First time
# ... run simulation ...
brain.reset()      # Clean up
brain.initialize()  # Fresh start
```

### 3. Check Agent Initialization
```python
# Always initialize agent after world
agent.initialize(world)  # ← This is crucial!

# Then you can use it
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
```

### 4. Use Descriptive Variable Names
```python
# Good - Clear and descriptive
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Great - Add context
 exploration_brain = pynlm.createBrain(config)
 random_seed = 42
 simulation_steps = 1000
```

### 5. Add Comments for Clarity
```python
# Explain what each section does
import pynlm

# Create and initialize the brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Setup the world environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

# Create the agent that connects brain to world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run the simulation
for step in range(100):
    # Update world state
    world.update(0.1)
    
    # Process sensory information
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Let brain think
    brain.step(step)
    
    # Get and apply actions
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 6. Document Your Experiments
```python
# Keep track of your experiments
experiment_config = {
    "brain_neurons": 1000,
    "world_size": "20x20",
    "learning_enabled": True,
    "steps": 500,
    "description": "Exploration and learning in simple world"
}

# Save your results
results = {
    "total_spikes": brain.getTotalSpikeCount(),
    "final_curiosity": agent.getCuriosityLevel(),
    "development_stage": brain.getDevelopmentalStage(),
    "avg_firing_rate": brain.getAverageFiringRate()
}
```

---

## Next Steps

When you're ready to dive deeper:

### 1. Read HOW_TO_USE.md
For comprehensive technical documentation and all available methods.

### 2. Explore the Documentation
- `docs/ARCHITECTURE.md` - Deep dive into system design
- `docs/SCIENCE.md` - The science behind NLM
- `docs/EXPERIMENTS.md` - Real experiment examples

### 3. Experiment with Different Configurations
Try different brain sizes, world settings, and learning parameters.

### 4. Create Your Own Experiments
Use NLM as a research platform for your own projects!

### 5. Join the Community
Connect with other NLM users and researchers.

---

## Welcome to NLM!

You've completed the beginner's guide! You're now ready to:

- ✅ Create and initialize virtual brains
- ✅ Run simulations with agents in worlds
- ✅ Enable learning and development
- ✅ Debug common issues
- ✅ Write clean, well-documented code

**Remember:** The most important rule is `initialize()` your brain BEFORE using it!

Your brain is now ready to think, learn, and experience the world! 🎯

---

*<footer>Version 2.0 - Created with ❤️ for the NLM community</footer>*