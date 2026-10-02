# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an **experimental artificial developmental brain** that simulates a neural system beginning in a primitive state and acquiring increasingly complex abilities through interaction with an environment.

Think of it like this:
- **Neurons** = Brain cells that send signals to each other using electrical impulses
- **Synapses** = Chemical/electrical connections between brain cells that transmit signals
- **Learning** = When synaptic connections strengthen or weaken based on neural activity (STDP, Hebbian learning)
- **Memory** = The brain storing and retrieving patterns of neural activity
- **Development** = The brain maturing and changing its structure over time
- **Neuromodulation** = Chemical signals (dopamine, curiosity, novelty) that regulate brain activity
- **Prediction** = The brain anticipating future events based on past experience
- **Cognition** = The brain planning, reasoning, and making decisions

NLM simulates all of this using real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons and event-driven dynamics.

NLM is NOT:
- A traditional deep learning model (transformer, CNN, RNN)
- A chatbot or large language model
- A pretrained AI system
- Claiming consciousness or human-like intelligence

What NLM IS:
- A neural system that learns from direct experience through interaction
- A brain-inspired architecture with biologically realistic neurons and synapses
- A system that develops and adapts over simulated "time"
- A closed-loop system where cognition emerges from neural dynamics

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks, learns, and develops
2. **World** - The environment the brain lives in, providing sensory input and feedback
3. **Agent** - The bridge connecting brain to world, handling perception and action

This architecture follows the brain loop:
SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE → REWARD/SURPRISE/ERROR → PLASTICITY → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (Silent Brain)

```python
import pynlm

# Step 1: Make a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on (initialize)
brain.initialize()

# Step 3: Make it think for 100 steps (basic neural computation)
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Average firing rate:", brain.getAverageFiringRate())
```

### Example 2: Brain in a Simple World (Interactive Agent)

```python
import pynlm
import time

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps of interaction
for step in range(50):
    world.update(0.1)  # Update world by 0.1 seconds
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
```

### Example 3: Advanced Agent with Learning (Complete Learning System)

```python
import pynlm
import time

def run_advanced_agent_simulation(num_steps=500):
    """Run a complete agent simulation with all learning systems enabled."""
    
    # 1. Create configuration with enhanced parameters
    config = pynlm.createDefaultConfig()
    # Enable advanced learning features
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.setAgentStart(10.0, 10.0)  # Start in center
    world.reset()
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all advanced learning subsystems
    agent.enableRewardModulation(True)      # Learn from rewards (dopamine)
    agent.enableStructuralPlasticity(True)  # Grow new connections (synaptogenesis)
    agent.enableDevelopment(True)           # Brain matures over time (development stages)
    agent.enableCuriosity(True)             # Explore new things (curiosity-driven)
    
    print(f"Starting advanced agent simulation for {num_steps} steps...")
    print("Enabled subsystems: Reward modulation, Structural plasticity, Development, Curiosity")
    
    # 7. Run simulation loop with learning
    for step in range(num_steps):
        # Update world
        world.update(0.05)  # Smaller timestep for smoother dynamics
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step (includes plasticity, neuromodulation, development)
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation based on internal state
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, agent.getExpectedReward())
        
        # Update development system
        agent.updateDevelopment(0.05)
        
        # Print progress and brain statistics every 50 steps
        if step % 50 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    print("\nAdvanced agent simulation complete!")
    print("Key achievements:")
    print(f"  - {brain.getTotalSpikeCount()} total spikes generated")
    print(f"  - {brain.getAverageFiringRate():.3f} average firing rate")
    print(f"  - Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  - Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_advanced_agent_simulation(500)
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
        
        # 4. Brain thinks (includes learning)
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Print progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop or after simulation:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Total neurons:", brain.getTotalNeuronCount())
print("Total synapses:", brain.getTotalSynapseCount())
print("E/I balance ratio:", brain.getExcitationInhibitionRatio())
print("Working memory traces:", brain.getWorkingMemory().getActiveTraces())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional and can be enabled at any time:
agent.enableRewardModulation(True)      # Learn from rewards (dopamine-based)
agent.enableStructuralPlasticity(True)  # Grow new connections (synaptogenesis)
agent.enableDevelopment(True)           # Brain matures over time (development stages)
agent.enableCuriosity(True)             # Explore new things (curiosity-driven exploration)

# You can also disable them later:
agent.enableRewardModulation(False)
```

### Pattern 4: Save and Load Brain State

```python
# Save brain state (checkpointing)
brain.save("my_brain_checkpoint.bin")

# Later, load it back
brain.load("my_brain_checkpoint.bin")

# Or save configuration separately
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")
```

### Pattern 5: Create Custom Configuration

```python
import pynlm

# Create a custom configuration
config = pynlm.createDefaultConfig()

# Modify specific parameters
config.set("brain.neuron_count", 2000)           # More neurons
config.set("brain.synapse_density", 0.15)        # More connections
config.set("neuromod.dopamine.scale", 1.5)       # Stronger reward signals
config.set("plasticity.stdp.enable", True)      # Enable spike-timing dependent plasticity
config.set("plasticity.hebbian.enable", True)   # Enable Hebbian learning
config.set("plasticity.structural.enable", True) # Enable structural plasticity

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

---

## What Each Part Does

### Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Config` | Brain configuration settings | `set()`, `getOr()`, `loadFromFile()` |
| `Brain` | Main neural simulation engine | `initialize()`, `step()`, `save()`, `load()` |
| `AgentBrain` | Bridge between brain and world | `processSensoryInput()`, `decodeMotorCommand()` |
| `SimpleWorld` | 2D simulation environment | `update()`, `applyMotorCommand()`, `getSensoryPercept()` |

### Key Methods for Beginners

- **`pynlm.createBrain(config)`**: Creates a virtual brain with neurons and synapses
- **`brain.initialize()`**: Starts up the brain. Always call this before using the brain!
- **`brain.step(step_number)`**: Makes the brain process one moment in time. The brain:
  - Checks each neuron
  - Sends signals between connected neurons  
  - Updates connections based on learning rules
- **`world.update(time)`**: Updates the virtual world by `time` seconds
- **`agent.processSensoryInput(percept)`**: Gives sensory information (vision, touch, etc.) to the brain
- **`agent.decodeMotorCommand()`**: Reads the brain's motor neurons to decide what action to take

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
print(f"Result: {brain.getTotalSpikeCount()} spikes generated")
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
print("Total activity:", brain.getTotalSpikeCount(), "spikes")
```

### Project 3: Complete Agent with Learning

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
print(f"Final stats: {brain.getTotalSpikeCount()} spikes, {brain.getAverageFiringRate():.3f} firing rate")
```

### Project 4: Advanced Learning Agent

```python
import pynlm

# Create a brain with advanced learning capabilities
config = pynlm.createDefaultConfig()
config.set("neuromod.dopamine.scale", 2.0)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("brain.neuron_count", 500)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Running advanced learning agent...")
print("This will demonstrate:")
print("  - Dopamine-based reward learning")
print("  - Spike-timing dependent plasticity (STDP)")
print("  - Hebbian learning")
print("  - Structural plasticity (growing new connections)")
print("  - Developmental stages")
print("  - Curiosity-driven exploration")
print()

for step in range(200):
    world.update(0.05)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Activity: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Learning: dopamine={agent.getNeuromodulationLevel():.3f}, curiosity={agent.getCuriosityLevel():.3f}")
        print(f"  Development: stage={brain.getDevelopmentalStage()}")

print("\nAdvanced learning complete!")
print(f"Final state: {brain.getTotalSpikeCount()} spikes, {brain.getAverageFiringRate():.3f} firing rate")
print(f"Final development: {brain.getDevelopmentalStage()}")
```

---

## Quick Reference

### Core Operations

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create default config | `pynlm.createDefaultConfig()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

### Common Statistics

| Statistic | Meaning | Method |
|-----------|---------|--------|
| Neurons firing | Currently active neurons | `brain.getFiringNeuronCount()` |
| Total spikes | All spikes generated | `brain.getTotalSpikeCount()` |
| Average firing rate | Spikes per neuron per second | `brain.getAverageFiringRate()` |
| Development stage | Current brain maturity | `brain.getDevelopmentalStage()` |
| Curiosity level | Exploration motivation | `agent.getCuriosityLevel()` |
| Novelty level | Surprise detection | `agent.getNoveltyLevel()` |
| Neuromodulation | Reward/prediction error | `agent.getNeuromodulationLevel()` |

### Configuration Options

```python
# Control brain size
config.set("brain.neuron_count", 1000)

# Control learning strength  
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.0)

# Control developmental timing
config.set("development.initial_plasticity", 1.0)
config.set("development.adult_plasticity", 0.2)

# Control memory capacity
config.set("memory.working.capacity", 100)
config.set("memory.episodic.max_episodes", 1000)
```

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that neurons have enough input (sensory input, current injection)
- Verify configuration has sufficient neurons and synapses

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are receiving sufficient activity
- Look at agent logs for motor command decoding issues

**"Everything is 0"**
- Brains need time to "warm up" - try more steps (50-100 minimum)
- Some neurons need input to fire - make sure sensory input is connected
- Check that the world has objects for the agent to interact with
- Verify configuration parameters are reasonable

**"Brain saves but loads wrong"**
- Save and load at the same step number
- Ensure world state is compatible between save/load
- Check that configuration matches saved brain state

**"Too much memory usage"**
- Reduce number of neurons in configuration
- Disable unnecessary subsystems (working memory, episodic memory)
- Use smaller simulation timesteps
- Save checkpoint periodically to free memory

---

## Next Steps

When you're comfortable with the basics:

1. **Read `HOW_TO_USE.md` for more details** - Complete technical reference
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into NLM design
3. **Read `docs/PHASE6_FINAL_AUDIT.md`** - Learn about Phase 6 final integration
4. **Experiment with different configurations!** - Try different parameters and see what happens
5. **Build your own projects** - Use NLM as a foundation for your research
6. **Contribute to NLM** - Help improve the system and documentation

---

## Getting Help

If you have questions or run into issues:

1. Check the troubleshooting section above
2. Read through `HOW_TO_USE.md` for complete API documentation
3. Look at the example projects and adapt them to your needs
4. The NLM repository contains additional documentation in the `docs/` folder

That's it! You're now ready to use NLM and create your own neural learning machines. Remember: the key is to start simple, then gradually add complexity as you understand how the system works.

Good luck with your neural simulation journey!