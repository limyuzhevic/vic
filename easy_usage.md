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

### Example 3: Advanced Agent with Learning

```python
import pynlm

# Setup with custom configuration
config = pynlm.createDefaultConfig()
# Configure some parameters
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.learning_rate", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent for 500 steps
for step in range(500):
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Progress display every 50 steps
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Reward received: {world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0}")

print("Advanced agent simulation complete!")
print(f"Final concept count: {agent.getConceptCount()}")
print(f"Final self-model confidence: {agent.getSelfModelConfidence():.3f}")
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
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Concept count:", agent.getConceptCount())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

### Pattern 4: Advanced Learning Configuration

```python
# Configure advanced learning parameters
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.threshold", 0.3)
```

### Pattern 5: Memory and Cognition Features

```python
# Check what memory systems are available
if brain.getWorkingMemory():
    print("Working memory has:", brain.getWorkingMemory().getActiveTraces(), "active traces")
    
if brain.getEpisodicMemory():
    print("Episodic memory has:", brain.getEpisodicMemory().getEpisodeCount(), "episodes")
    
if brain.getConceptFormation():
    print("Concept formation has:", agent.getConceptCount(), "concepts")
    
if brain.getPlanner():
    print("Planner suggests:", brain.getPlanner().planAction(last_vision))
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

### `agent.applyRewardModulation(reward, predicted_reward)`
Lets the brain learn from rewards and predictions.

### `agent.updateDevelopment(timestep)`
Updates the brain's developmental stage over time.

### `agent.enableSubsystem(True/False)`
Enables or disables specific learning and cognitive features.

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

### Project 4: Advanced Brain with All Systems

```python
import pynlm

# Create sophisticated brain with all features enabled
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.threshold", 0.3)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run extended simulation
for step in range(1000):
    world.update(0.1)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks and learns
    brain.step(step)
    
    # Agent decides action
    action = agent.decodeMotorCommand()
    
    # Execute action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Periodic comprehensive status
    if step % 100 == 0:
        print(f"=== Step {step} ===")
        print(f"Brain State: {brain.getFiringNeuronCount()} firing neurons")
        print(f"Learning Systems:")
        print(f"  Reward Modulation: {agent.isRewardModulationEnabled()}")
        print(f"  Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
        print(f"  Development: {agent.isDevelopmentEnabled()}")
        print(f"  Curiosity: {agent.isCuriosityEnabled()}")
        print(f"Agent Cognition:")
        print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
        print(f"  Concept Count: {agent.getConceptCount()}")
        print(f"  Self-Confidence: {agent.getSelfModelConfidence():.3f}")

print("=== Advanced Brain Simulation Complete ===")
print(f"Total simulation steps: 1000")
print(f"Final concept count: {agent.getConceptCount()}")
print(f"Final self-model confidence: {agent.getSelfModelConfidence():.3f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
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
| Learn from rewards | `agent.applyRewardModulation(reward, predicted)` |
| Update development | `agent.updateDevelopment(timestep)` |

---

## Advanced Features

### Brain Configuration Options

#### Neuron Parameters
- `brain.neuron_count`: Total number of neurons (default: 1000)
- `brain.v_thresh`: Threshold potential (default: -50.0)
- `brain.v_rest`: Resting potential (default: -70.0)
- `brain.v_reset`: Reset potential (default: -75.0)
- `brain.tau_mem`: Membrane time constant (default: 20.0)

#### Plasticity Parameters
- `plasticity.stdp.learning_rate`: STDP learning rate (default: 0.001)
- `plasticity.stdp.tau_plus`: STDP LTP time constant (default: 20.0)
- `plasticity.stdp.tau_minus`: STDP LTD time constant (default: 20.0)
- `plasticity.hebbian.learning_rate`: Hebbian learning rate (default: 0.002)

#### Neuromodulation Parameters
- `neuromod.dopamine.scale`: Dopamine modulation scale (default: 1.0)
- `neuromod.curiosity.threshold`: Curiosity threshold (default: 0.3)

### Agent Capabilities

#### Cognitive Systems
- **Working Memory**: Stores active traces for current processing
- **Episodic Memory**: Stores experience episodes for replay
- **Concept Formation**: Discovers and categorizes patterns
- **Self-Model**: Tracks own capabilities and confidence
- **Social Learning**: Observes and imitates other agents

#### Learning Mechanisms
- **Reward Modulation**: Learns from prediction errors
- **Structural Plasticity**: Creates and prunes connections
- **Development**: Ages the brain, changing its learning dynamics
- **Curiosity**: Drives exploration of novel stimuli

### World Features

#### Sensory Systems
- **Vision**: 16x16 grid of light intensity values (0-1)
- **Touch**: 8 proximity sensors (0-1)
- **Internal**: Internal state signals (-1 to 1)
- **Proprioception**: Body position sensors (0-1)

#### Objects
- **Resource**: Positive reward items
- **Hazard**: Negative reward items  
- **Wall**: Boundaries
- **Marker**: Navigation aids

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that world is being updated

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that the agent has a brain

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that motor groups have activity

**"Learning isn't happening"**
- Enable reward modulation: `agent.enableRewardModulation(True)`
- Enable structural plasticity: `agent.enableStructuralPlasticity(True)`
- Check reward values from world

**"No concepts formed"**
- Run longer simulations (500+ steps)
- Enable concept formation through full agent integration
- Ensure there's meaningful sensory input

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Full documentation of all parameters and features
   - Examples of all demo executables

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - Deep dive into the neural architecture
   - Explanation of all brain subsystems

3. **Experiment with different configurations!**
   - Try different neuron counts
   - Adjust learning rates
   - Test different world layouts
   - Build your own simulations

4. **Advanced usage (once you're confident)**:
   - Create custom worlds with multiple agents
   - Run complex experiments with different behaviors
   - Use checkpoints to save and resume simulations
   - Build your own demo executables using the brain library

---

## Additional Resources

- **Phase 6 Integration Demo**: `nlm_phase6_demo` - Comprehensive demonstration of all brain systems
- **Phase 5 Experiment**: `nlm_phase5_demo` - Performance optimizations and advanced features
- **Phase 4 Demo**: `nlm_phase4_demo` - Full agent with cognition and learning
- **Phase 3 Demo**: `nlm_phase3_demo` - Basic agent with world interaction
- **Phase 2 Demo**: `nlm` - Core neural computation with spiking neurons

That's it! You're now ready to use NLM at beginner and advanced levels.
NLM combines neuroscience research with practical AI development - have fun exploring the simulated brain!

