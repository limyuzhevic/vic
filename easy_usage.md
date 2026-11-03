# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

***NLM PHASE 2: ENHANCED LEARNING CAPABILITIES***

Phase 2 introduces major improvements to the learning system:

### Learning Improvements

#### Before Phase 2: Network Saturation
The previous version stimulated ALL sensory neurons repeatedly:
- 30.0f current × 20 times per step = 600.0f total per step
- Entire network fired synchronously
- Learning was measurable but not selective
- Weights tended to saturate at 1.0

#### Phase 2: Selective Pattern Learning
Phase 2 now uses intelligent, selective pattern learning:
- Stimulates only specific neurons (pattern learning)
- Smaller current strength (15.0f vs 30.0f)
- Allows for more realistic learning patterns
- Better selective pattern association

### The 3 Things You Need to Know

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

## Advanced Features (New in Phase 2)

### Enhanced Command-Line Interface

The NLM Phase 2 demo now supports advanced command-line options:

```bash
# Help
./nlm --help

# Verbose logging
./nlm --verbose

# Custom plasticity experiment settings
./nlm --plasticity-episodes 3 --plasticity-steps 500
./nlm --plasticity-episodes 10 --plasticity-current 20.0
./nlm --plasticity-no-selective
```

**Available Options:**
- `--plasticity-episodes N`: Number of learning episodes (default: 5)
- `--plasticity-steps N`: Steps per episode (default: 1000)
- `--plasticity-current F`: Current strength per neuron (default: 15.0)
- `--plasticity-no-selective`: Disable selective learning pattern

### New Learning Pattern (FIXED Saturation)

**Phase 2 Learning Improvements:**
- **OLD**: All sensory neurons stimulated (network saturation)
- **NEW**: Selective pattern learning for better results

**Benefits:**
- More realistic neural learning
- Avoids saturation effects
- Better selective pattern association
- More stable weight changes

### Enhanced Output

The demo now provides:
- **Detailed logging** of learning progress
- **Average synaptic weights** during learning
- **Current strength information**
- **Learning analysis reports**
- **Phase 2 improvement summary**

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

## Advanced Usage (New)

### Command Line Options

The NLM Phase 2 demo includes these advanced command-line options:

**Basic:**
- `--help, -h`: Show help message
- `--verbose, -v`: Enable verbose logging

**Phase 2 Advanced (Plasticity Experiment):**
- `--plasticity-episodes N`: Number of learning episodes (default: 5)
- `--plasticity-steps N`: Steps per episode (default: 1000)
- `--plasticity-current F`: Current strength per neuron (default: 15.0)
- `--plasticity-no-selective`: Disable selective learning pattern

**Example Usage:**
```bash
./nlm --plasticity-episodes 3 --plasticity-steps 500 --plasticity-current 20.0
./nlm --plasticity-no-selective
./nlm --verbose
```

### Learning Analysis (New)

Phase 2 provides detailed learning analysis:

```
=== Phase 2 Learning Improvements ===
OLD: All sensory neurons stimulated (saturation)
NEW: Selective pattern learning for better results
BENEFIT: More realistic, selective pattern learning

=== Advanced Configuration (Phase 3)
Plasticity episodes: 5
Steps per episode: 1000
Current strength: 15.0
Selective learning: enabled
```

### What Changed in Phase 2

1. **Learning Pattern**: Selective instead of network-wide stimulation
2. **Command Line**: Advanced options for customizing experiments
3. **Logging**: Enhanced output with learning analysis
4. **Output**: Summary of improvements and benefits

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

***Phase 2 Key Improvements:***
- ✓ Fixed network saturation issue
- ✓ Implemented selective pattern learning  
- ✓ Added advanced command-line options
- ✓ Enhanced logging and analysis
- ✓ Better user experience

***The Phase 2 NLM now demonstrates real neural computation with improved learning selectivity!***

That's it! You're now ready to use NLM with enhanced learning capabilities!
