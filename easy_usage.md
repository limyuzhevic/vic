# NLM - Easy Usage Guide for Beginners

**Version 2.0 - Enhanced with JSON/YAML support, better error handling, and advanced features**

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**New in Version 2.0:**
- JSON and YAML configuration file support
- Better error handling with custom exceptions
- Enhanced Python API with convenience functions
- Advanced simulation features
- Performance monitoring tools

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain with Configuration

```python
import pynlm

# Step 1: Create configuration (NEW: supports JSON/YAML)
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Custom neuron count

# Step 2: Make a brain
brain = pynlm.createBrain(config)

# Step 3: Turn it on
brain.initialize()

# Step 4: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World with Learning (ENHANCED)

```python
import pynlm

# Create everything with enhanced setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features (NEW: more powerful learning)
agent.enableRewardModulation(True)    # Learn from rewards
agent.enableCuriosity(True)           # Explore new things
agent.enableDevelopment(True)         # Brain matures over time
agent.enableStructuralPlasticity(True) # Grow new connections

# Run for 50 steps with learning
for step in range(50):
    world.update(0.1)  # Update world
    
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    
    brain.step(step)  # Brain thinks
    
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Apply reward for learning (NEW: automatic reward modulation)
    if step % 10 == 0:
        agent.applyRewardModulation(1.0, 0.5)

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Curiosity level:", agent.getCuriosityLevel())
print("Learning achieved:", agent.getNeuromodulationLevel() > 0.1)
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

## The Simplest Possible Example (WITH NEW CONFIG SUPPORT)

```python
import pynlm

# One line to create a virtual brain with custom config
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Complete Simulation with Learning (ENHANCED)

```python
def run_advanced_simulation(brain, world, agent, num_steps):
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
        
        # 7. Apply reward for learning (NEW)
        agent.applyRewardModulation(reward=1.0, predicted=0.5)
        
        # 8. Update development (NEW)
        agent.updateDevelopment(0.1)

# Use it like this:
run_advanced_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Comprehensive Brain Stats (IMPROVED)

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())

# NEW: Get additional stats
print("Working memory traces:", brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0)
print("Episodic memory episodes:", brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0)
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Neuromodulation level:", agent.getNeuromodulationLevel())
```

### Pattern 3: Enable Advanced Brain Features (EXPANDED)

```python
# These are all optional and now include new features:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enablePrediction(True)           # Enable prediction system
agent.enableAttention(True)             # Enable attention system

# NEW: Configuration factory functions
research_brain = pynlm.createResearchBrain()  # Advanced research setup
quickstart_brain = pynlm.createQuickStartBrain()  # Simple for beginners
curiosity_agent = pynlm.createCuriosityAgent(brain, world)  # Exploration-focused
learning_agent = pynlm.createLearningAgent(brain, world)    # Reward-based learning
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses. NEW: Supports JSON/YAML configuration files!

### `config.loadFromFile(filepath)` (NEW)
Load configuration from JSON or YAML file:
```python
config.loadFromFile("configs/default.json")    # JSON format
config.loadFromFile("configs/my_config.yaml")  # YAML format
config.loadFromFile("configs/simple.cfg")     # Simple key=value format (backward compatible)
```

### `config.loadFromDict(dict)` (NEW)
Load configuration from Python dictionary:
```python
config_dict = {"brain.neuron_count": 2000, "plasticity.stdp.enable": True}
config.loadFromDict(config_dict)
```

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

### NEW: Convenience Functions

**Configuration Factories:**
- `pynlm.createDefaultConfig()` - Basic configuration
- `pynlm.createResearchBrain()` - Advanced research setup
- `pynlm.createQuickStartBrain()` - Simple for beginners

**Agent Types:**
- `pynlm.createCuriosityAgent(brain, world)` - Exploration-focused
- `pynlm.createLearningAgent(brain, world)` - Reward-based learning

**Utilities:**
- `pynlm.runSimulation(brain, agent, world, steps)` - Run complete simulation
- `pynlm.getSimulationStats(brain)` - Get comprehensive statistics

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

### Project 2: Brain Watching a World (ENHANCED)

```python
import pynlm

# Setup with enhanced configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create learning agent (NEW)
agent = pynlm.createLearningAgent(brain, world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("Learning achieved:", agent.getNeuromodulationLevel() > 0.1)
```

### Project 3: Complete Agent with Learning (COMPLETE)

```python
import pynlm

# Setup with research-grade configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Create learning agent with curiosity
agent = pynlm.createLearningAgent(brain, world)

# Add some world objects
pynlm.addResource(world, 5.0, 5.0, 10.0)  # Resource at position (5,5) with value 10
pynlm.addHazard(world, 10.0, 10.0, 5.0)   # Hazard at position (10,10) with severity 5

# Run agent with learning
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on action outcome
    if step % 20 == 0:
        reward = world.getSimulationTime() * 0.1  # Simulate reward
        agent.applyRewardModulation(reward, 0.5)
        agent.updateDevelopment(0.1)
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Learning: {agent.getNeuromodulationLevel():.3f}")

print("Agent simulation complete!")
print("Final stats:")
print(f"  Total reward: {agent.getNeuromodulationLevel():.3f}")
print(f"  Memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create default config | `pynlm.createDefaultConfig()` |
| Create research brain | `pynlm.createResearchBrain()` |
| Create quickstart brain | `pynlm.createQuickStartBrain()` |
| Create curiosity agent | `pynlm.createCuriosityAgent(brain, world)` |
| Create learning agent | `pynlm.createLearningAgent(brain, world)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Run simulation | `pynlm.runSimulation(brain, agent, world, steps)` |
| Get stats | `pynlm.getSimulationStats(brain)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if learning features are enabled

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure motor neurons are being activated

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Enable learning features for better activation

**"Configuration error"**
- Check if JSON/YAML files are properly formatted
- Verify configuration keys are correct
- Use try-catch for configuration loading

**"Python import error"**
- Make sure pynlm is installed: `pip install pynlm`
- Check that build dependencies are available
- Try running the verification script: `python verify_installation.py`

---

## Next Steps

When you're comfortable:

1. **Advanced Configuration:** Explore `configs/` directory for JSON/YAML templates
2. **Deep Documentation:** Read `HOW_TO_USE.md` for comprehensive details
3. **Architecture:** Read `docs/ARCHITECTURE.md` to understand system design
4. **Scientific Background:** Check `docs/SCIENCE.md` for research context
5. **Experimentation:** Try different configurations and agent types
6. **Research Mode:** Use `createResearchBrain()` for advanced experiments
7. **Performance:** Use `getSimulationStats()` for monitoring
8. **Error Handling:** Implement try-catch for robust simulations

**New in Version 2.0:**
- JSON and YAML configuration file support
- Enhanced error handling with custom exceptions
- Advanced convenience functions for different use cases
- Performance monitoring and statistics
- Configuration factory functions for common setups

That's it! You're now ready to use the enhanced NLM with all its new features and capabilities.

---

**New Features in Version 2.0:**
✅ JSON/YAML configuration support
✅ Custom Python exceptions for better error handling
✅ Advanced convenience functions and factory methods
✅ Enhanced learning and development systems
✅ Performance monitoring tools
✅ Better documentation and examples
✅ Configuration templates and utilities