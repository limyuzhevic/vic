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

---

## Advanced Usage (Phase 6 Integration)

Once you understand the basics, you can use Phase 6's integrated features for a complete artificial brain experience:

### Enable Full Integration
```python
# Create integrated brain with all features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable all integration features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning and development
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore and discover
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

# Enable memory systems
agent.enableMemorySystems(True)         # Working and episodic memory
agent.enablePrediction(True)             # Predictive capabilities
agent.enableCognition(True)              # Planning and concept formation
```

### Brain State Analysis
```python
# Get comprehensive brain status
print("=== Phase 6 Integration Analysis ===")
print("Development Stage:", brain.getDevelopmentalStage())
print("Systems Integrated:", brain.getIntegrationScore())
print("Memory Activity:", brain.getWorkingMemory().getMemoryActivity())
print("Prediction Error:", brain.getPredictionSystem().getPredictionError())
print("Planning Confidence:", brain.getPlanner().getPlanningConfidence())

# Memory statistics
print("\n=== Memory Statistics ===")
print("Working Memory Traces:", brain.getWorkingMemory().getActiveTraces())
print("Episodic Episodes:", brain.getEpisodicMemory().getEpisodeCount())
print("Concepts Discovered:", brain.getConceptFormation().getConceptCount())
```

### Advanced Configuration
```python
# Configure integration features
brain.configure({
    "integration_level": "full",           # Basic, standard, full
    "enable_sleep_cycle": True,           # Memory consolidation during rest
    "enable_prediction": True,            # Predictive processing
    "enable_planning": True,              # Multi-step planning
    "enable_concept_formation": True,    # Pattern discovery
    "memory_capacity": "large",           # Memory limits
    "development_speed": "normal",        # How fast brain develops
    "novelty_threshold": 0.3,             # What counts as novel
    "exploration_rate": 0.2               # How much to explore vs exploit
})

# Configure memory systems
brain.getWorkingMemory().setCapacity(1000)
brain.getEpisodicMemory().setMaxEpisodes(5000)
brain.getEpisodicMemory().setConsolidationRate(0.3f)

# Configure prediction system
brain.getPredictionSystem().setPredictionHorizon(10)
brain.getPredictionSystem().setConfidenceThreshold(0.7f)

# Configure planning system
brain.getPlanner().setPlanningDepth(5)
brain.getPlanner().setPlanningConfidence(0.8f)

# Configure concept formation
brain.getConceptFormation().setFormationThreshold(0.5f)
brain.getConceptFormation().setStabilityWindow(100)
```

### Debug and Analysis Mode
```python
# Run with debugging for integration issues
brain.enableDebugMode(True)
brain.enableAnalysisMode(True)

# Get integration quality score
integrationScore = brain.getIntegrationScore()
print(f"Integration Quality: {integrationScore}%")

if integrationScore < 80:
    print("Warning: Integration quality below optimal")
    print("Issues found:")
    print(" - Memory system connectivity problems")
    print(" - Prediction system not working")
    print(" - Cognition system not integrated")
else:
    print("Excellent integration - brain is highly functional!")
```

### Sleep/Rest Cycle Control
```python
# Configure rest periods for memory consolidation
brain.configureRestCycle({
    "rest_interval": 100,        # Rest every 100 steps
    "consolidation_strength": 0.5,  # How much consolidation
    "development_during_rest": True, # Continue development in rest
    "replay_episodes": 5,         # Number of episodes to replay
    "sleep_duration": 50         # How long rest lasts
})

# Manually trigger rest and consolidation
brain.triggerRestPeriod()
brain.consolidateMemories()
brain.replayImportantEpisodes()
```

### Memory Management
```python
# Memory system controls
brain.getWorkingMemory().setDecayRate(0.01f)
brain.getWorkingMemory().setCapacity(2000)
brain.getWorkingMemory().setCompetitionThreshold(0.3f)

brain.getEpisodicMemory().setConsolidationRate(0.3f)
brain.getEpisodicMemory().setMaxEpisodes(10000)
brain.getEpisodicMemory().setReplayInterval(50)
brain.getEpisodicMemory().setConsolidationInterval(200)
```

### Command Line Interface
```bash
# Advanced CLI options
./nlm_phase6_demo --help

Available options:
  --debug           Enable detailed debug output
  --analysis        Run integration analysis
  --memory-stats    Show memory system statistics
  --prediction      Enable prediction debugging
  --cognition      Enable cognition system debugging
  --neuromodulation Enable neuromodulation debugging
  --development     Enable development system debugging
  --config FILE     Load specific configuration
  --log-level LEVEL Set logging level (debug, info, warning, error)

Example:
  ./nlm_phase6_demo --debug --analysis --memory-stats --log-level debug
```

### Troubleshooting Integration Issues
```python
# If integration quality is poor, check these:

print("=== Integration Check ===")
print("1. Brain Systems:")
print("   - Working Memory:", "✓" if brain.getWorkingMemory() else "✗")
print("   - Episodic Memory:", "✓" if brain.getEpisodicMemory() else "✗")
print("   - Dopamine:", "✓" if brain.getDopamine() else "✗")
print("   - Curiosity:", "✓" if brain.getCuriosity() else "✗")
print("   - Planner:", "✓" if brain.getPlanner() else "✗")
print("   - Concept Formation:", "✓" if brain.getConceptFormation() else "✗")

print("\n2. Integration Functionality:")
print("   - Prediction System:", "✓" if brain.getPredictionSystem() else "✗")
print("   - Attention System:", "✓" if brain.getAttention() else "✗")
print("   - Development System:", "✓" if brain.getDevelopmentSystem() else "✗")

print("\n3. Memory Status:")
if brain.getWorkingMemory():
    print("   - Working Memory Active:", "✓" if brain.getWorkingMemory().getMemoryActivity() > 0.1 else "✗")
    print("   - Memory Competition:", "✓" if brain.getWorkingMemory().hasWinners() else "✗")

if brain.getEpisodicMemory():
    print("   - Episodic Memory:", "✓" if brain.getEpisodicMemory().getEpisodeCount() > 0 else "✗")

print("\n4. Development Status:")
print("   - Current Stage:", brain.getDevelopmentalStage())
print("   - Development Active:", "✓" if brain.getDevelopmentSystem() else "✗")
```

---

## Summary

You now have a complete artificial brain with all Phase 6 integration features! Your brain can:

- **Learn from experience** using reward prediction error
- **Explore and discover** new patterns through curiosity
- **Plan multi-step actions** using predictive dynamics
- **Form concepts** from recurring patterns
- **Remember experiences** through working and episodic memory
- **Consolidate memories** during sleep/rest cycles
- **Develop and mature** over time
- **Analyze itself** through comprehensive debugging tools

You're now ready to use NLM as a fully integrated artificial brain system!

That's it! You're now ready to use NLM with Phase 6 integration!

---

**Authors:** NLM Research Team
**Phase:** 6 - Final Integration
**Updated:** 2026-10-05
