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

### Advanced Tools (CLI)

| Tool | Description | Key Command |
|------|-------------|-------------|
| `nlm-config` | Configuration management | `nlm-config create --brain.name=MyBrain` |
| `nlm-batch` | Batch simulation runs | `nlm-batch --config scenarios.txt` |
| `nlm-analysis` | Results analysis | `nlm-analysis --input results/` |
| `nlm-visualize` | Activity visualization | `nlm-visualize --input checkpoint.bin` |
| `nlm-checkpoint` | State management | `nlm-checkpoint save checkpoint.bin` |
| `nlm-profile` | Performance profiling | `nlm-profile --config config.txt` |

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

## Advanced Tools

For more advanced usage, NLM provides several command-line tools to help with:

### Brain Configuration
```bash
# Create a new brain configuration
nlm-config create --brain.name=MyBrain --brain.region_count=2

# Load configuration from file
nlm-config load config.txt

# List configuration keys
nlm-config list
```

### Batch Simulations
```bash
# Run multiple simulations with different parameters
nlm-batch --config scenarios.txt --output results/

# Run a single simulation with specific settings
nlm-batch --config single_scenario.nml
```

### Analysis
```bash
# Analyze simulation results
nlm-analysis --input results/ --report analysis.txt

# Compare multiple simulation runs
nlm-analysis --compare run1/ run2/ --output comparison.txt
```

### Checkpoint Management
```bash
# Save brain state
nlm-checkpoint save checkpoint.bin --input brain_state

# Load brain state
nlm-checkpoint load checkpoint.bin --output brain_state

# List saved checkpoints
nlm-checkpoint list
```

### Performance Profiling
```bash
# Profile simulation performance
nlm-profile --config config.txt --output profile.json

# Compare performance across runs
nlm-profile --compare run1/ run2/ --output comparison.txt
```

### Example Advanced Usage

Here's a complete example showing how to use the advanced tools together:

```bash
# 1. Create and save a configuration for batch processing
cat > experiment_config.nml << EOF
brain:
  name: "LearningExperiment"
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.15
  simulation:
    timestep: 0.001
    max_steps: 5000
  learning:
    enable_stdp: true
    enable_hebbian: true
    enable_reward: true
EOF

# 2. Create multiple configurations for batch processing
nlm-config create --brain.name="Exp1_1000" --brain.neuron_count=1000 \
    --brain.region_count=1 --simulation.timestep=0.001 \
    --simulation.max_steps=5000 --learning.enable_stdp=true \
    --learning.enable_hebbian=true --output exp1_config.nml

nlm-config create --brain.name="Exp2_2000" --brain.neuron_count=2000 \
    --brain.region_count=2 --simulation.timestep=0.002 \
    --simulation.max_steps=10000 --learning.enable_stdp=true \
    --learning.enable_hebbian=true --output exp2_config.nml

# 3. Run batch simulations
nlm-batch --config batch_scenarios.txt --output results/experiment1/ \
    --parallel 4 --dry-run

nlm-batch --config batch_scenarios.txt --output results/experiment2/ \
    --parallel 4

# 4. Save checkpoints during simulation
nlm-checkpoint save results/experiment1/checkpoint_1000.bin \
    --input results/experiment1/sim_state_1000

nlm-checkpoint save results/experiment1/checkpoint_5000.bin \
    --input results/experiment1/sim_state_5000

# 5. Analyze results
nlm-analysis --input results/experiment1/ --report analysis1.txt \
    --metrics:learning_rate --threshold:0.01

nlm-analysis --compare results/experiment1/ results/experiment2/ \
    --output comparison.txt

# 6. Profile performance
nlm-profile --config results/experiment1/config.nml \
    --output performance_report.json --detailed

# 7. View visualizations
nlm-visualize --input results/experiment1/checkpoint_1000.bin \
    --output visualization1.html --type:timeline

nlm-visualize --input results/experiment1/checkpoint_5000.bin \
    --output visualization2.html --type:network
```

### Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the advanced CLI tools for more powerful control
5. Read the advanced tool documentation in the `docs/` directory

That's it! You're now ready to use NLM at both beginner and advanced levels.
