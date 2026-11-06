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

## Advanced Projects (Phase 6 Features)

### Project 4: Memory-Enhanced Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Create memory systems
working_memory = pynlm.WorkingMemory()
episodic_memory = pynlm.EpisodicMemory()
semantic_memory = pynlm.SemanticMemory()
associative_memory = pynlm.AssociativeMemory()

# Run memory-enhanced agent
for step in range(200):
    # Store experience in working memory
    working_memory.store(brain.getTotalNeuronCount(), step * 0.1f)
    
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Store episode in episodic memory
    if step % 20 == 0:
        episode = pynlm.EpisodicMemoryItem()
        episode.timestamp = step
        # Store neural activity pattern
        # (In real implementation, would capture actual neural data)
        episodic_memory.storeEpisode(episode)
        
        # Store semantic facts
        semantic_memory.storeFact("position", f"({agent.getBrain().getAgentBody().x}, {agent.getBrain().getAgentBody().y})")
    
    if step % 50 == 0:
        print(f"Step {step}: Working memory size: {working_memory.getCurrentSize()}")
        print(f"         Episodic memory: {episodic_memory.getEpisodeCount()} episodes")
        print(f"         Brain stats: {brain.getFiringNeuronCount()} firing neurons")

print("Memory-enhanced agent simulation complete!")
print(f"Final episodic memory: {episodic_memory.getEpisodeCount()} episodes")
```

### Project 5: Neuromodulation Control

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

# Create neuromodulators
dopamine = pynlm.Dopamine()
acetycholine = pynlm.Acetylcholine()
norepinephrine = pynlm.Norepinephrine()
serotonin = pynlm.Serotonin()

# Initialize neuromodulators
print("Neuromodulation system initialized:")
print(f"  Dopamine level: {dopamine.getLevel()}")
print(f"  Acetylcholine level: {acetycholine.getLevel()}")
print(f"  Norepinephrine level: {norepinephrine.getLevel()}")
print(f"  Serotonin level: {serotonin.getLevel()}")

# Enable neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run with neuromodulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    
    # Simulate reward
    if step % 25 == 0:
        dopamine.signalReward(1.0)
        dopamine.signalRewardPredictionError(0.1)
    
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 25 == 0:
        print(f"Step {step}: Neuromodulation levels updated")
        print(f"         Dopamine: {dopamine.getLevel()}, Curiosity: {agent.getCuriosityLevel()}")

print("Neuromodulation simulation complete!")
```

### Project 6: Prediction and Planning

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create cognitive systems
planner = pynlm.NeuralPlanner()
self_model = pynlm.SelfModel()
social_learning = pynlm.SocialLearning()

# Initialize cognitive systems
planner.initialize(brain.getBrain())
self_model.initialize(brain.getBrain())
social_learning.initialize(brain.getBrain())

# Create prediction system
prediction = pynlm.PredictionSystem()

# Run planning and prediction agent
for step in range(150):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with prediction system
    current_state = percept.getVision()  # Simplified for demo
    predicted = prediction.predictNextState(current_state)
    
    # Process sensory input to brain
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Use neural planner for action
    current_state = percept.getVision()  # Simplified
    action_type = planner.planAction(current_state)
    
    # Convert action type to motor command for demo
    action = pynlm.Action(action_type)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update prediction with actual outcome
    actual = percept.getVision()  # Simplified
    prediction.updatePredictions(predicted, actual)
    
    # Update planning based on outcome
    if step % 30 == 0:
        print(f"Step {step}: Planning confidence: {planner.getPlanningConfidence():.2f}")
        print(f"         Prediction error: {prediction.getPredictionError():.2f}")
        print(f"         Self-model confidence: {self_model.getSelfModelConfidence(action_type):.2f}")

print("Planning and prediction simulation complete!")
```

### Project 7: Text-As-A-Sense

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=18, height=18, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Phase 6: Text-as-a-sense processing
print("Text-as-a-sense demo:")

# Encode different words into neural patterns
words = ["apple", "banana", "tree", "house", "water"]
for word in words:
    pattern = pynlm.encodeText(word)
    print(f"  Word '{word}' encoded as neural pattern: {pattern}")

# Create text percept (simplified for demo)
class TextPercept:
    def __init__(self, text):
        self.text = text
        self.vision = [0.5] * 256  # Dummy vision data

# Simulate text learning
for step in range(100):
    if step % 20 == 0 and step > 0:
        # Inject text at regular intervals
        word = words[step // 20 % len(words)]
        text_percept = TextPercept(word)
        
        # Process text as sensory input
        # (In real implementation, would inject into agent's sensory system)
        print(f"  Step {step}: Processing text '{word}'")
        
        # Get brain statistics
        if step % 25 == 0:
            print(f"    Brain stats: {brain.getFiringNeuronCount()} neurons firing")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
            print(f"    Self-model ready: {agent.getBrain().getSelfModelReady()}")

print("Text-as-a-sense simulation complete!")
```

### Project 8: Development and Plasticity

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)

print("Development simulation:")
print(f"Initial stage: {brain.getDevelopmentalStage()}")
print(f"Initial age: {agent.getDevelopmentalAge():.2f}")

# Run developmental simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log development progress
    if step % 40 == 0:
        stage = brain.getDevelopmentalStage()
        age = agent.getDevelopmentalAge()
        plasticity = agent.getPlasticityModifier()
        curiosity = agent.getCuriosityLevel()
        
        print(f"  Step {step}: Stage={stage}, Age={age:.2f}, "
              f"Plasticity={plasticity:.2f}, Curiosity={curiosity:.2f}")
        
        # Check for critical period
        if agent.isCriticalPeriod():
            print(f"    CRITICAL PERIOD: Enhanced learning!")

print("Development simulation complete!")
```

### Project 9: Social Learning

```python
import pynlm

# Setup two agents for social learning
config = pynlm.createDefaultConfig()
brain1 = pynlm.createBrain(config)
brain1.initialize()
brain2 = pynlm.createBrain(config)
brain2.initialize()

world1 = pynlm.createSimpleWorld()
world1.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world1.reset()
world2 = pynlm.createSimpleWorld()
world2.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world2.reset()

agent1 = pynlm.createAgentBrain(brain1)
agent1.initialize(world1)
agent2 = pynlm.createAgentBrain(brain2)
agent2.initialize(world2)

# Enable social learning
social1 = pynlm.SocialLearning()
social2 = pynlm.SocialLearning()
social1.initialize(brain1)
social2.initialize(brain2)

print("Social learning simulation (Teacher-Agent):")
print("Agent 1 will observe Agent 2's actions")

# Run social learning simulation
for step in range(150):
    # Teacher (Agent 2) demonstrates actions
    action2 = agent2.decodeMotorCommand()
    percept2_before = world2.getSensoryPercept()
    
    # Teacher performs action in its world
    world2.update(0.1)
    world2.applyMotorCommand(action2, world2.getSimulationTime())
    percept2_after = world2.getSensoryPercept()
    
    # Observer (Agent 1) records observation
    social1.observeAgentAction(action2, percept2_before.getVision(), percept2_after.getVision())
    
    # Observer acts in its own world
    world1.update(0.1)
    action1 = agent1.decodeMotorCommand()
    world1.applyMotorCommand(action1, world1.getSimulationTime())
    
    # Observer processes sensory input
    agent1.processSensoryInput(world1.getSensoryPercept())
    brain1.step(step)
    
    # Check if observer can imitate
    can_immitate = social1.canImitate(action2)
    imitation_action = social1.getImitationAction(world1.getSensoryPercept().getVision())
    
    if step % 30 == 0:
        print(f"  Step {step}: Social knowledge = {social1.hasSocialKnowledge()}")
        print(f"           Can imitate Agent 2's action: {can_immitate}")
        print(f"           Best imitation: {imitation_action}")
        print(f"           Observation count: {social1.getObservationCount()}")

print("Social learning simulation complete!")
```

### Project 10: Advanced Brain Diagnostics

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Advanced brain diagnostics:")

# Run diagnostic simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 10 == 0:
        # Comprehensive brain statistics
        print(f"\n  Step {step} Brain Diagnostics:")
        print(f"    Neural Statistics:")
        print(f"      - Total neurons: {brain.getTotalNeuronCount()}")
        print(f"      - Total synapses: {brain.getTotalSynapseCount()}")
        print(f"      - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"      - Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"      - Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
        print(f"      - Total spikes: {brain.getTotalSpikeCount()}")
        
        print(f"    Agent Statistics:")
        print(f"      - Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
        print(f"      - Curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"      - Novelty level: {agent.getNoveltyLevel():.2f}")
        print(f"      - Prediction error: {agent.getPredictionError():.2f}")
        print(f"      - Developmental stage: {agent.getDevelopmentalStage()}")
        print(f"      - Developmental age: {agent.getDevelopmentalAge():.2f}")
        
        # Get thought trace (Phase 5)
        thought = agent.getThoughtTrace()
        if thought != "none":
            print(f"      - Thought trace: {thought}")
        
        # Get Phase 6 diagnostics
        print(f"    Phase 6 Diagnostics:")
        print(f"      - Concept count: {agent.getConceptCount()}")
        print(f"      - Self-model confidence: {agent.getSelfModelConfidence():.2f}")
        print(f"      - Social observations: {agent.getSocialObservationCount()}")
        print(f"      - Planner overrides: {agent.getPlannerOverrideCount()}")
        print(f"      - Planner agreements: {agent.getPlannerAgreementCount()}")

print("\nAdvanced diagnostics simulation complete!")
```

---

## Quick Reference: New Python Functions

### Memory Systems
- `pynlm.WorkingMemory()` - Create working memory
- `pynlm.EpisodicMemory()` - Create episodic memory
- `pynlm.SemanticMemory()` - Create semantic memory
- `pynlm.ProceduralMemory()` - Create procedural memory
- `pynlm.AssociativeMemory()` - Create associative memory

### Neuromodulation
- `pynlm.Neuromodulator` - Abstract neuromodulator base class
- `pynlm.Dopamine()` - Reward neuromodulator
- `pynlm.Acetylcholine()` - Attention neuromodulator
- `pynlm.Norepinephrine()` - Arousal neuromodulator
- `pynlm.Serotonin()` - Mood neuromodulator

### Prediction System
- `pynlm.PredictionSystem()` - Prediction system
- `pynlm.DevelopmentSystem()` - Development system

### Cognition
- `pynlm.NeuralPlanner()` - Neural planner for action sequences
- `pynlm.SelfModel()` - Self-model for sensorimotor awareness
- `pynlm.SocialLearning()` - Social learning from observation

### Text Processing
- `pynlm.encodeText(text)` - Encode text into neural pattern (Phase 6)
- `pynlm.getThoughtTrace()` - Get thought trace
- `pynlm.getInternalFiringRates()` - Get internal neuron firing rates

### Advanced Statistics
- Various Phase 6 diagnostic counters (planner, concept, self, social metrics)

All new bindings are documented with comprehensive docstrings and examples above.

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
