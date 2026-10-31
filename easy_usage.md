# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**Phase 6 Integration**: NLM is now a fully integrated artificial brain with working memory, episodic memory, neuromodulation, prediction, cognition, and development systems all connected together!

---

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

## Phase 6 Integration Features

NLM is now a fully integrated artificial brain! Here's what you can do:

### Example: Advanced Brain with Memory and Learning

```python
import pynlm

# Create a brain with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)                    # More neurons
config.set("working_memory_capacity", 200)           # Larger working memory
config.set("enable_reward_modulation", True)       # Enable reward learning
config.set("enable_curiosity", True)                # Enable exploration
config.set("enable_structural_plasticity", True)    # Brain grows new connections
config.set("enable_development", True)              # Brain matures over time

brain = pynlm.createBrain(config)
brain.initialize()

# Enable all advanced features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures
agent.enableCuriosity(True)              # Explore new things

print("Advanced brain initialized with integrated systems!")
print("Working memory:", brain.getWorkingMemory().getCapacity())
print("Memory traces:", brain.getWorkingMemory().getActiveTraces())
```

### Example: Memory Formation and Replay

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate learning over time
for step in range(1000):
    brain.step(step)
    
    # Store current activity in working memory
    if step % 10 == 0:
        # Working memory automatically stores recent activity
        print(f"Step {step}: Working memory traces = {brain.getWorkingMemory().getActiveTraces()}")
    
    # Episodic memory stores experiences periodically
    if step % 100 == 0:
        episodes = brain.getEpisodicMemory().getRecentEpisodes(5)
        print(f"Step {step}: Episodic memory episodes = {episodes.size()}")
    
    # Check for learning through neural changes
    if step % 500 == 0:
        avg_weight = brain.getAverageFiringRate()
        print(f"Step {step}: Average firing rate = {avg_weight:.2f}")
```

### Example: Neuromodulation and Learning

```python
import pynlm

# Create agent with neuromodulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all neuromodulation systems
agent.enableRewardModulation(True)  # Dopamine - reward learning
agent.enableCuriosity(True)          # Curiosity - exploration
agent.enableNovelty(True)            # Novelty - change detection

# Run learning simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate and apply reward
    reward = world.computeReward(brain)
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 100 == 0:
        print(f"Step {step}: Dopamine = {agent.getNeuromodulationLevel():.2f}, "
              f"Curiosity = {agent.getCuriosityLevel():.2f}, "
              f"Novelty = {agent.getNoveltyLevel():.2f}")
```

---

## New Python API Functions

### Memory System Access
```python
# Working Memory
brain.getWorkingMemory().store(pattern, strength)  # Store information
brain.getWorkingMemory().retrieve()                # Get current content
brain.getWorkingMemory().clear()                   # Clear memory
brain.getWorkingMemory().getActiveTraces()          # Number of active traces
brain.getWorkingMemory().getCapacity()              # Memory capacity

# Episodic Memory
brain.getEpisodicMemory().storeEpisode(episode)    # Store experience
brain.getEpisodicMemory().retrieveSimilar(pattern) # Find similar experiences
brain.getEpisodicMemory().getEpisodeCount()         # Number of stored episodes
brain.getEpisodicMemory().getRecentEpisodes(10)     # Get recent experiences

# Associative Memory
brain.getAssociativeMemory().associate(patternA, patternB)  # Form association
brain.getAssociativeMemory().retrieve(pattern)                 # Get related patterns
```

### Cognitive Systems
```python
# Neural Planner
brain.getPlanner().planAction(current_state, goal)  # Plan next action
brain.getPlanner().evaluateSequence(actions, state) # Evaluate action sequence
brain.getPlanner().setPlanningDepth(depth)          # How far ahead to plan

# Attention System
brain.getAttention().processCompetition(competitors)  # Select focus
brain.getAttention().isAttended(neuron_id)              # Check if neuron is attended
brain.getAttention().focusOnRegion(region_id)           # Focus on area

# Concept Formation
brain.getConceptFormation().processPattern(pattern)   # Form concepts
brain.getConceptFormation().getConcepts()             # Get formed concepts
brain.getConceptFormation().updateConcepts()          # Update concepts
```

### Neuromodulation
```python
agent.getCuriosityLevel()         # How curious the agent is
agent.getNoveltyLevel()           # How novel the world is
agent.getPredictionError()        # How wrong predictions are
agent.getNeuromodulationLevel()   # Overall neuromodulation
```

---

## Configuration Examples

### Small, Fast Brain (For Testing)
```python
import pynlm
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)           # Only 100 neurons
config.set("region_count", 1)             # Single region
config.set("connection_probability", 0.05) # Sparse connections
config.set("max_simulation_steps", 1000)  # Short simulation

brain = pynlm.createBrain(config)
brain.initialize()
```

### Large, Complex Brain (For Research)
```python
import pynlm
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)           # 5000 neurons
config.set("working_memory_capacity", 1000) # Large working memory
config.set("episodic_memory_max_episodes", 10000) # Store many experiences
config.set("enable_structural_plasticity", True)   # Growing brain
config.set("enable_development", True)             # Maturation
config.set("enable_memory_pools", True)           # Performance optimization

brain = pynlm.createBrain(config)
brain.initialize()
```

### Specialized Brain for Different Tasks
```python
import pynlm

# Memory-focused brain
config = pynlm.createDefaultConfig()
config.set("working_memory_capacity", 500)
config.set("episodic_memory_max_episodes", 5000)
config.set("replay_interval", 50)
config.set("consolidation_interval", 500)

# Learning-focused brain  
config.set("enable_reward_modulation", True)
config.set("dopamine_baseline", 0.5)
config.set("curiosity_baseline", 0.8)
config.set("reward_discount_factor", 0.95)

# Development-focused brain
config.set("enable_development", True)
config.set("development_synaptogenesis_rate", 0.002)
config.set("development_pruning_rate", 0.0002)
```

---

## Mini Projects

### Project 1: Simple Memory Agent
```python
import pynlm

def memory_agent_simulation(steps=500):
    """Create an agent that learns and remembers experiences."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Memory={brain.getWorkingMemory().getActiveTraces()}, "
                  f"Episodes={brain.getEpisodicMemory().getEpisodeCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}")
    
    return brain, agent
```

### Project 2: Learning with Replay
```python
import pynlm

def replay_learner_simulation(steps=2000):
    """Create an agent that uses memory replay for learning."""
    config = pynlm.createDefaultConfig()
    config.set("replay_interval", 100)      # Replay every 100 steps
    config.set("consolidation_interval", 1000)  # Consolidate every 1000 steps
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Starting replay-learner simulation...")
    print("(Agent will replay important memories to consolidate learning)")
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Periodically check memory state
        if step % 500 == 0:
            working_mem = brain.getWorkingMemory().getActiveTraces()
            episodic_mem = brain.getEpisodicMemory().getEpisodeCount()
            replay_enabled = brain.getEpisodicMemory().isReplayEnabled()
            print(f"Step {step}: Working={working_mem}, "
                  f"Episodic={episodic_mem}, "
                  f"Replay={'ON' if replay_enabled else 'OFF'}")
    
    return brain, agent
```

### Project 3: Advanced Cognitive Agent
```python
import pynlm

def cognitive_agent_simulation(steps=1000):
    """Create an agent with planning, concept formation, and attention."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Set cognitive parameters
    brain.getPlanner().setPlanningDepth(3)        # Plan 3 steps ahead
    brain.getAttention().setInhibitionStrength(0.6)   # Competition
    brain.getAttention().setExcitationStrength(1.8)   # Selection bias
    brain.getConceptFormation().setLearningRate(0.02) # Concept learning
    
    print("Starting cognitive agent simulation...")
    print("(Agent will plan, form concepts, and focus attention)")
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 200 == 0:
            # Check cognitive system status
            planner_confidence = brain.getPlanner().getPlanningConfidence()
            attention_winners = len(brain.getAttention().getWinners())
            goal = brain.getPlanner().getCurrentGoal()
            print(f"Step {step}: "
                  f"Planning={planner_confidence:.2f}, "
                  f"Attention={attention_winners}, "
                  f"Goal size={len(goal)}")
    
    return brain, agent
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
| Store in memory | `brain.getWorkingMemory().store(pattern)` |
| Plan action | `brain.getPlanner().planAction(state)` |
| Focus attention | `brain.getAttention().focusOnRegion(id)` |
| Learn concepts | `brain.getConceptFormation().processPattern(pattern)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Enable curiosity or neuromodulation

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that you're using the agent properly

**"Memory isn't working"**
- Check that memory systems are enabled in config
- Working memory needs neural activity to maintain information
- Episodic memory stores experiences at intervals

**"Planning isn't happening"**
- Make sure cognitive systems are enabled
- Set a goal for the planner: `brain.getPlanner().setCurrentGoal(state)`
- Increase planning depth: `brain.getPlanner().setPlanningDepth(5)`

---

## Next Steps

When you're comfortable with Phase 6:

1. **Read `HOW_TO_USE.md` for more details** - The full C++ documentation
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Technical deep dive
3. **Read `docs/PHASE6_FINAL_AUDIT.md` to see all improvements made** - Complete documentation of Phase 6 integration
4. **Experiment with different configurations!** - Find what works best for your use case
5. **Try creating your own experiment configurations** - Customize settings for your specific needs

That's it! You're now ready to use the fully integrated NLM Phase 6 brain system. The brain now has working memory, episodic memory, neuromodulation, prediction, cognition, development, and persistence systems all working together as a coherent artificial brain!

Happy experimenting! 🚀
