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

## Advanced Learning Examples

### Example 1: Working Memory for Sequential Tasks

```python
import pynlm

# Create brain with working memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable working memory system
working_memory = brain.getWorkingMemory()
working_memory.setCapacity(5)
working_memory.setDecayRate(0.05)

# Store a sequence in working memory
sequence_actions = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.Interact
]

for i, action in enumerate(sequence_actions):
    # Encode action with context
    pattern = [float(action.value), float(i) / len(sequence_actions)]
    working_memory.store(pattern, strength=1.0)
    print(f"Stored {action} in working memory")

# Retrieve and use working memory
retrieved = working_memory.retrieve()
print(f"Working memory content: {retrieved}")

# Plan next action based on memory
if working_memory.getMemoryActivity() > 0.3:
    print("Working memory active - planning based on stored sequence")
```

### Example 2: Self-Model Development

```python
import pynlm

# Create agent with self-model
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable self-model development
self_model = agent.getSelfModel()

# Develop self-model through experience
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record action-effect relationships
    before_state = agent.getCurrentSensorimotorState()
    after_state = world.getSensoryPercept()
    self_model.recordSelfAction(action, before_state, after_state)
    
    if step % 100 == 0:
        confidence = self_model.getSelfModelConfidence(action)
        print(f"Self-model confidence for {action}: {confidence:.3f}")
        print(f"Body awareness: {self_model.getBodyAwareness():.3f}")
```

### Example 3: Social Learning

```python
import pynlm

# Create two agents for social learning
agent1 = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
agent1.initialize(pynlm.createSimpleWorld())

agent2 = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
agent2.initialize(pynlm.createSimpleWorld())

# Enable social learning
social_learning1 = agent1.getSocialLearning()
social_learning2 = agent2.getSocialLearning()

# Agent 2 demonstrates an action
observed_action = pynlm.ActionType.Eat
observer_state = [0.1, 0.2, 0.3]
resulting_state = [0.1, 0.2, 0.4]  # Effect of action

# Agent 1 observes
agent1.observeAgentAction(observed_action, observer_state, resulting_state)

# Agent 2 learns from its own action
agent2.updateSocialKnowledge(1.0)

print(f"Agent 1 can imitate: {social_learning1.canImitate(observed_action)}")
print(f"Agent 2 observations: {social_learning2.getObservationCount()}")
```

### Example 4: Prediction-Based Planning

```python
import pynlm

# Create agent with prediction system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable planning with predictions
agent.setPlanningDepth(3)
agent.setCurrentGoal([0.8, 0.3, 0.5])  # Target location/state

print("Running prediction-based planning...")
print(f"Goal: {agent.getCurrentGoal()}")
print(f"Planning depth: {agent.getPlanningDepth()}")

for step in range(150):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Get planned action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        candidate = agent.evaluateSequence(
            [action, pynlm.ActionType.MoveForward], 
            agent.getCurrentState()
        )
        print(f"Step {step}: Action {action}")
        print(f"  Expected reward: {candidate.expectedReward:.3f}")
        print(f"  Confidence: {candidate.confidence:.3f}")
```

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
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
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

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("Curiosity level:", agent.getCuriosityLevel())
```

### Project 3: Complete Agent with All Features

```python
import pynlm

# Setup with advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Larger brain
config.set("plasticity.structural.enable", True)  # Allow growth

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Set up advanced capabilities
agent.setPlanningDepth(4)
agent.setCurrentGoal([0.5, 0.5, 0.0])

# Run complete agent simulation
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply all learning systems
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Action: {action}")
        print(f"  Reward: {reward:.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Planning Confidence: {agent.getPlanningConfidence():.3f}")

print("Agent simulation complete!")
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

**Advanced Features:**
| Advanced capability | Code |
|-------------------|------|
| Working memory | `brain.getWorkingMemory()` |
| Prediction system | `brain.getPredictionSystem()` |
| Planning | `brain.getPlanner()` |
| Self-model | `brain.getSelfModel()` |
| Social learning | `brain.getSocialLearning()` |
| Attention | `brain.getAttention()` |

---

## Troubleshooting

**"My brain isn't learning"**
- Did you call `agent.enableRewardModulation(True)`?
- Is the agent receiving sensory input?
- Try increasing the number of steps

**"The agent isn't planning"**
- Did you set up a goal? `agent.setCurrentGoal([0.5, 0.5, 0.0])`
- Is planning depth set? `agent.setPlanningDepth(3)`
- Is the planner initialized?

**"Working memory not working"**
- Did you access the working memory system? `brain.getWorkingMemory()`
- Is working memory capacity set?
- Check that working memory decay rate is appropriate

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that learning features are enabled

---

## Next Steps

When you're comfortable:
1. **Read `HOW_TO_USE.md`** for more detailed installation and configuration instructions
2. **Read `docs/ARCHITECTURE.md`** to understand how all systems integrate
3. **Experiment with different configurations!**
4. **Try creating your own examples** using the advanced features
5. **Read `EXAMPLES.md`** for more complex example programs
6. **Join the NLM research community** to share your findings!

## New Features Available

The current version includes **Phase 6 integration** with advanced cognitive capabilities:

- **Working Memory**: `brain.getWorkingMemory()` - Store and maintain information
- **Prediction System**: `brain.getPredictionSystem()` - Predict future states
- **Planning**: `brain.getPlanner()` - Plan multi-step actions
- **Self-Model**: `brain.getSelfModel()` - Learn about self and body
- **Social Learning**: `brain.getSocialLearning()` - Learn from others
- **Attention**: `brain.getAttention()` - Selectively focus on information
- **Concept Formation**: `brain.getConceptFormation()` - Discover patterns
- **Development**: `brain.getDevelopmentSystem()` - Age-based changes
- **Neuromodulation**: `brain.getDopamine()`, `brain.getCuriosity()` - Drive behavior

**All these features are available through the Python API!**

That's it! You're now ready to use NLM with both basic and advanced capabilities.
