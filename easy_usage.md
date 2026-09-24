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

### Pattern 4: Advanced Agent with Full Integration

```python
import pynlm

# Setup complete agent with all systems enabled
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)  # Larger brain for complex tasks
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features for power users
agent.enableRewardModulation(True)      # Learn from rewards (dopamine system)
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time  
agent.enableCuriosity(True)             # Explore novel situations
agent.enablePredictionSystem(True)      # Predict future states
agent.enableMemorySystems(True)         # Use working and episodic memory
agent.enableCognitiveSystems(True)      # Enable neural planner and concepts

# Advanced configuration
agent.setDevelopmentStage(DevelopmentalStage::CriticalPeriod)
agent.setPlasticityModifier(1.5f)
agent.setNeuromodulationLevel(0.8f)
agent.setCuriosityLevel(0.6f)

# Run simulation with full integration
print("Starting advanced integration test...")
print("Enabled features:")
print("  ✓ Reward modulation (dopamine learning)")
print("  ✓ Structural plasticity (synaptogenesis/pruning)")  
print("  ✓ Development (brain maturation)")
print("  ✓ Curiosity (exploration behavior)")
print("  ✓ Prediction (future state prediction)")
print("  ✓ Memory systems (working and episodic memory)")
print("  ✓ Cognitive systems (neural planning and concepts)")

for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide reward feedback
    if step % 25 == 0:
        reward = 1.0f if world.getRobotAt(10, 10) == 'treasure' else 0.0f
        agent.applyRewardModulation(reward, agent.getExpectedReward())
        
        if step % 50 == 0:
            print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, "
                  f"Spikes={brain.getTotalSpikeCount()}, "
                  f"Reward={reward:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}")

print("Advanced simulation completed!")
print("Agent developed and learned from experience.")
```

### Pattern 5: Debugging and Inspection

```python
import pynlm

# Create simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable features for debugging
agent.enableRewardModulation(True)
agent.enableMemorySystems(True)

# Run a few steps to gather data
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 10 == 0:
        # Inspect brain state
        print(f"\n=== State Dump Step {step} ===")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
        
        # Dump detailed memory state
        print("  Memory state:")
        if hasattr(agent, 'dumpMemoryState'):
            agent.dumpMemoryState()
        
        # Dump network state  
        print("  Network state:")
        if hasattr(agent, 'dumpNetworkState'):
            agent.dumpNetworkState()

# Save/load functionality
print("\n=== Brain State Persistence ===")
agent.exportBrainState("my_brain_checkpoint.json")

# Later...
agent.loadBrainState("my_brain_checkpoint.json")  # Note: This method might need to be created
```

### Pattern 6: Custom Developmental Trajectory

```python
import pynlm

# Create agent with custom development profile
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)

print("Custom developmental trajectory:")
print("  Stage 1: Initial (high plasticity, rapid learning)")
print("  Stage 2: Critical Period (optimal learning)")
print("  Stage 3: Maturation (slower, more stable)")
print("  Stage 4: Adult (conserved knowledge)")

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryInput())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Manual development control
    if step == 100:
        agent.setDevelopmentStage(DevelopmentalStage::CriticalPeriod)
        agent.setPlasticityModifier(1.2f)
    elif step == 300:
        agent.setDevelopmentStage(DevelopmentalStage::Maturation)
        agent.setPlasticityModifier(0.7f)
    elif step == 450:
        agent.setDevelopmentStage(DevelopmentalStage::Adult)
        agent.setPlasticityModifier(0.3f)
    
    if step % 100 == 0:
        stage = agent.getDevelopmentalStage()
        modifier = agent.getNeuromodulationLevel()
        print(f"  Step {step:3d}: Stage {stage}, Plasticity {modifier:.2f}")

print("Custom development completed!")
```

### Pattern 7: Advanced Learning with Memory

```python
import pynlm

# Create agent specialized for memory-based learning
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced memory features
agent.enableRewardModulation(True)
agent.enableMemorySystems(True)
agent.enableCuriosity(True)
agent.enablePredictionSystem(True)

print("Memory-based learning system:")
print("  Learning from rewards and experiences")
print("  Storing episodes in episodic memory")
print("  Using working memory for active information")
print("  Predicting future states and outcomes")

# Create learning environment with patterns
learning_patterns = [
    ([0, 1, 0, 0, 1, 0, 0, 0], 1.0f),  # Pattern A -> Reward
    ([1, 0, 0, 0, 0, 1, 0, 0], 0.0f),  # Pattern B -> No Reward
    ([0, 0, 1, 0, 0, 0, 1, 0], 0.5f),  # Pattern C -> Partial Reward
]

# Train agent on patterns
for episode, (pattern, reward) in enumerate(learning_patterns):
    print(f"\nTraining episode {episode + 1}:")
    
    # Reset environment
    world.reset()
    
    # Create pattern in world (simplified representation)
    for step in range(50):
        world.update(0.1)
        
        # Present pattern to agent
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Provide reward based on pattern
        agent.applyRewardModulation(reward, agent.getExpectedReward())
        
        if step % 10 == 0:
            print(f"    Step {step:2d}: Reward={reward:.1f}, "
                  f"PredictionError={agent.getPredictionError():.3f}")

print("\nLearning completed! Agent has formed associations between patterns and rewards.")
```

### Pattern 8: Competition and Attention

```python
import pynlm

# Create agent to demonstrate attention and competition
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable attention systems
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("Attention and competition system:")
print("  Selective attention to salient stimuli")
print("  Competition between neural populations")
print("  Salience-based processing")
print("  Top-down and bottom-up attention")

# Simulate attentional processing
for step in range(100):
    # Create multiple sensory inputs (simulated)
    vision_input = world.getSensoryPercept().getVision()
    touch_input = world.getSensoryPercept().getTouch()
    
    # Process inputs through agent's attentional system
    agent.processSensoryInput(world.getSensoryPercept())
    
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation based on attention
    attention_weight = agent.getCuriosityLevel() * 0.5f
    reward = touch_input[0] * attention_weight  # Higher reward if attended to touch
    agent.applyRewardModulation(reward, agent.getExpectedReward())
    
    if step % 20 == 0:
        curiosity = agent.getCuriosityLevel()
        attention = agent.getNeuromodulationLevel()
        print(f"  Step {step:3d}: Curiosity={curiosity:.2f}, "
              f"Attention={attention:.2f}, Action={action}")

print("Attention simulation completed!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
