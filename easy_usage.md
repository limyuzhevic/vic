# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers with **Phase 6: Full Integration**. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns
- **Cognition** = The brain thinking, planning, and forming concepts
- **Development** = The brain growing and changing over time
- **Motivation** = The brain wanting to explore and achieve goals

That's it! NLM simulates all of this in an integrated artificial brain.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (with memory, cognition, motivation)
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

### Example 2: Brain in a Simple World (Enhanced)

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPerGroupPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

### Example 3: Complete Brain with All Integrated Systems

```python
import pynlm

def run_integrated_brain_simulation():
    """Run a complete simulation using ALL integrated systems"""
    
    # Create brain with integrated systems
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with all integrated systems
    agent.initialize(world)
    
    # Enable ALL integrated capabilities
    agent.enableRewardModulation(True)      # Memory: learn from rewards
    agent.enableStructuralPlasticity(True)  # Growth: new connections
    agent.enableDevelopment(True)           # Development: change over time
    agent.enableCuriosity(True)             # Motivation: explore
    
    # Run simulation with full integration
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward and development
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    print("✓ Integrated brain simulation complete!")
    print(f"✓ Working memory activity: {brain.getWorkingMemory().getMemoryActivity():.3f}")
    print(f"✓ Episodic memory stored: {brain.getEpisodicMemory().getEpisodes().size()} episodes")
    print(f"✓ Development stage: {brain.getDevelopmentalStage()}")\n    print(f"✓ Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"✓ Planning confidence: {brain.getPlanner().getPlanningConfidence():.3f}")

# Run the integrated simulation
run_integrated_b

# Run the integrated simulation
run_integrated_brain_simulation()
```

### Example 4: Access Advanced Brain Functions

```python
import pynlm

# Create integrated brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access and use integrated systems

# Working Memory - stores active thoughts
wm = brain.getWorkingMemory()
wm.store([0.1, 0.9, 0.5], 1.0)  # Store pattern
activity = wm.retrieve()      # Get current activity
print(f"Working memory activity: {activity}")

# Episodic Memory - stores experiences
em = brain.getEpisodicMemory()
print(f"Episodic memory episodes: {em.getEpisodes().size()}")

# Neural Planner - for planning actions
planner = brain.getPlanner()
planner.setPlanningDepth(5)    # Look 5 steps ahead
action = planner.planAction([0.1, 0.5, 0.9])  # Plan action
print(f"Planned action: {action}")

# Concept Formation - for learning patterns
concept = brain.getConceptFormation()
concept_id = concept.presentExperience([0.2, 0.8, 0.3], [0.1, 0.9], anga 0.5, 100)
print(f"Discovered concept: {concept_id}")

# Neuromodulation systems
novelty = brain.getNovelty()     # Novelty detection
print(f"Novelty level: {novelty.getLevel():.3f}")

curiosity = brain.getCuriosity()  # Curiosity/exploration
print(f"Curiosity level: {curiosity.getLevel():.3f}")

dopamine = brain.getDopamine()    # Reward prediction
print(f"Dopamine level: {dopamine.getLevel():.3f}")
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
| Your brain remembers experiences | `brain.getEpisodicMemory()` |
| Your brain plans ahead | `brain.getPlanner()` |
| Your brain stays curious | `brain.getCuriosity()` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain with ALL integrated systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active with integrated systems
brain.step(0)

print("Your integrated brain has", brain.getTotalNeuronCount(), "neurons!")
print("✓ Working memory:", brain.getWorkingMemory().getCapacity(), "traces")
print("✓ Episodic memory: unlimited episodes")
print("✓ Planning depth:", brain.getPlanner().getPlanningDepth())
print("✓ Curiosity enabled:", brain.getCuriosity().isExplorationEnabled())
```

---

## Common Patterns

### Pattern 1: Run a Complete Integrated Simulation

```python
def run_complete_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain (with prediction system)
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks (with all integrated systems)
        brain.step(step)
        
        # 5. Get action from neural planner
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_complete_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Advanced Brain Statistics

```python
# Just add these inside your loop:
print("Working memory:", brain.getWorkingMemory().getMemoryActivity())
print("Episodic memory:", brain.getEpisodicMemory().getEpisodes().size())
print("Neural planning:", brain.getPlanner().getPlanningConfidence())
print("Prediction confidence:", brain.getPredictionSystem().getConfidence())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable All Integrated Features

```python
# These are all optional - enable what you need:
agent.enableRewardModulation(True)      # Learn from rewards (neuromodulation)
agent.enableStructuralPlasticity(True)  # Grow new connections (memory)
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things (motivation)
```

---

## Mini Projects

### Project 1: Silent Integrated Brain (With All Systems)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Integrated brain test done!")
print("✓ Working memory:", brain.getWorkingMemory().getMemoryActivity())
print("✓ Episodic memory:", brain.getEpisodicMemory().getEpisodes().size())
print("✓ Planning confidence:", brain.getPlanner().getPlanningConfidence())
```

### Project 2: Brain Watching a World (With All Systems)

```python
import pynlm

# Setup with integrated systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps with all systems
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Working memory activity:", brain.getWorkingMemory().getMemoryActivity())
print("Planning confidence:", brain.getPlanner().getPlanningConfidence())
```

### Project 3: Complete Integrated Agent

```python
import pynlm

# Setup with ALL integrated systems
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 800)
config.set("memory.working_memory.capacity", 150)
config.set("cognition.planning_depth", 3)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL integrated features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent with full integration
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Working memory: {brain.getWorkingMemory().getMemoryActivity():.3f}")
        print(f"  Episodic memory: {brain.getEpisodicMemory().getEpisodes().size()} episodes")
        print(f"  Planning confidence: {brain.getPlanner().getPlanningConfidence():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")

print("Integrated agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create integrated brain with all systems | `pynlm.createBrain(config)` |
| Start brain with all subsystems | `brain.initialize()` |
| Make brain think with all systems | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent interface | `pynlm.createAgentBrain(brain)` |
| Get working memory | `brain.getWorkingMemory()` |
| Get episodic memory | `brain.getEpisodicMemory()` |
| Get prediction system | `brain.getPredictionSystem()` |
| Get neural planner | `brain.getPlanner()` |
| Get development system | `brain.getDevelopmentSystem()` |
| Get curiosity system | `brain.getCuriosity()` |

---

## Troubleshooting

**"My integrated brain isn't doing anything"**
- Did you call `brain.initialize()`? (needed for all subsystems)
- Try increasing the number of steps
- Check if your world is providing sensory input

**"The integrated agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Ensure `world.update()` is being called
- Check if your agent is properly initialized

**"Everything is 0"**
- Integrated brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if all integrated systems are properly initialized

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for advanced details** - Full documentation of all integrated systems
2. **Read `docs/ARCHITECTURE.md`** - Deep dive into system architecture
3. **Experiment with different configurations** - Try different settings for integrated systems
4. **Create your own projects** - Combine systems in new ways

That's it! You're now ready to use the **enhanced NLM** with **complete system integration**.

---

## New in Phase 6 (Enhanced)

This version includes **complete system integration**:

- ✅ **Working Memory** - Connected to sensory processing
- ✅ **Episodic Memory** - Stores experiences  
- ✅ **Concept Formation** - Discovers patterns
- ✅ **Neural Planning** - Plans multi-step actions
- ✅ **Prediction System** - Predicts future states
- ✅ **Development System** - Affects all systems over time
- ✅ **Neuromodulation** - Reward, curiosity, novelty
- ✅ **Attention System** - Selects information to process
- ✅ **Memory Replay** - Consolidates during rest

The NLM brain now functions as a **truly integrated artificial brain** where all systems work together as a coherent whole.

## Quick Version Comparison

| Feature | Old Version | New Version (Phase 6) |
|---------|-------------|----------------------|
| Working Memory | ❌ Disconnected | ✅ Connected to sensory |
| Episodic Memory | ❌ Empty placeholder | ✅ Stores experiences |
| Neural Planner | ❌ Not used | ✅ Used for action selection |
| Prediction System | ❌ Disconnected | ✅ Predicts future |
| Development | ⚠️ Minimal effect | ✅ Affects all systems |
| Neuromodulation | ⚠️ Basic dopamine | ✅ Full reward, curiosity, novelty |
| Integration | ❌ Disconnected systems | ✅ Complete brain loop |

This enhanced documentation shows you how to use the **complete integrated NLM brain** with all systems working together.

## Further Reading

- [HOW_TO_USE.md](HOW_TO_USE.md) - **Advanced usage** with all integrated systems
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture details
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Integration experiment descriptions