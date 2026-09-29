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

## Advanced Examples for Expert Users

### Expert Example 1: Using Advanced Configuration

```python
import pynlm

# Create a scientific-grade configuration with advanced features
scientific_config = pynlm.createScientificConfig()

# Create brain with advanced settings
brain = pynlm.createBrain(scientific_config)
brain.initialize()

# Create world with specific dimensions
world = pynlm.createWorldWithSize(width=30, height=30, visionWidth=12, visionHeight=12)

# Create learning agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a more complex simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Reward: {world.computeReward():.2f}")

print("Advanced simulation complete!")
```

### Expert Example 2: Advanced Experiment with Detailed Reporting

```python
import pynlm
import time

# Create components
brain = pynlm.createBrain(pynlm.createScientificConfig())
brain.initialize()

world = pynlm.createWorldWithSize(20, 20, 8, 8)

# Run experiment with detailed reporting
start_time = time.time()
metrics, rewards, firing_rates = pynlm.runExperimentWithReport(
    brain, world, numSteps=500, detailedLogging=True
)

end_time = time.time()

print("=== EXPERIMENT RESULTS ===")
print(f"Average Reward: {metrics['average_reward']:.2f}")
print(f"Average Firing Rate: {metrics['average_firing_rate']:.2f}")
print(f"Learning Efficiency: {metrics['learning_efficiency']:.2f}")
print(f"Total steps: {len(rewards)}")
print(f"Total time: {end_time - start_time:.2f} seconds")
```

### Expert Example 3: Batch Processing Multiple Configurations

```python
import pynlm

# Create multiple configurations for comparison
configs = []
for neurons in [100, 500, 1000, 2000]:
    cfg = pynlm.createDefaultConfig()
    cfg.set("neuron_count", neurons)
    configs.append(cfg)

# Run batch simulation
print("Running batch simulation...")
results = pynlm.runBatchSimulation(configs, stepsPerConfig=200)

print("=== BATCH RESULTS ===")
for i, (reward, firing_rate, time_taken) in enumerate(results):
    print(f"Config {i+1}: Neurons={configs[i].get('neuron_count')}, "
          f"AvgReward={reward:.2f}, AvgFiring={firing_rate:.2f}, Time={time_taken:.2f}s")
```

### Expert Example 4: Extracting and Analyzing Brain State

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createScientificConfig())
brain.initialize()

# Run a simulation
world = pynlm.createWorldWithSize(15, 15, 8, 8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Extract brain state for analysis
brain_state = pynlm.extractBrainState(brain)

print("=== BRAIN STATE ANALYSIS ===")
print(f"Total neurons: {brain_state['total_neurons']}")
print(f"Total synapses: {brain_state['total_synapses']}")
print(f"Active neurons: {brain_state['active_neurons']}")
print(f"Firing neurons: {brain_state['firing_neurons']}")
print(f"Average firing rate: {brain_state['average_firing_rate']:.2f}")
print(f"E/I ratio: {brain_state['excitatory_inhibition_ratio']:.2f}")
print(f"Developmental stage: {brain_state['developmental_stage']}")

if 'working_memory_traces' in brain_state:
    print(f"Working memory traces: {brain_state['working_memory_traces']}")
if 'episodic_memory_episodes' in brain_state:
    print(f"Episodic memory episodes: {brain_state['episodic_memory_episodes']}")
```

### Expert Example 5: Visualization Data Extraction

```python
import pynlm

# Create brain and run simulation
brain = pynlm.createBrain(pynlm.createScientificConfig())
brain.initialize()

world = pynlm.createWorldWithSize(25, 25, 10, 10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Extract visualization data
neuron_activities, neuron_positions = pynlm.extractVisualizationData(brain, maxNeurons=100)

print("=== VISUALIZATION DATA ===")
print(f"Extracted {len(neuron_activities)} neuron activity vectors")
print(f"Extracted {len(neuron_positions)} neuron position vectors")
print(f"Sample activity: {neuron_activities[0] if neuron_activities else 'N/A'}")
print(f"Sample position: {neuron_positions[0] if neuron_positions else 'N/A'}")
```

### Expert Example 6: Learning Curve Analysis

```python
import pynlm

# Create learning agent
brain, world, agent = pynlm.createLearningAgent(pynlm.createScientificConfig())

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Analyze learning curve
firing_rates, spike_counts = pynlm.analyzeLearningCurve(brain, windowSize=50)

print("=== LEARNING CURVE ANALYSIS ===")
print(f"Firing rate range: {min(firing_rates):.2f} - {max(firing_rates):.2f}")
print(f"Spike count range: {min(spike_counts):.0f} - {max(spike_counts):.0f}")
print(f"Final firing rate: {firing_rates[-1]:.2f}")
print(f"Final spike count rate: {spike_counts[-1]:.0f}")
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

## Advanced Convenience Functions

### `pynlm.createScientificConfig()`
Create a scientific-grade configuration with all learning features enabled.

### `pynlm.createMinimalConfig()`
Create a minimal configuration for basic testing.

### `pynlm.createWorldWithSize(width, height, visionWidth, visionHeight)`
Create a world with specified dimensions and vision capabilities.

### `pynlm.createLearningAgent(config)`
Create a fully configured learning agent with world and brain.

### `pynlm.runSimpleExperiment(brain, world, numSteps)`
Run a simple experiment and return average reward and firing rate.

### `pynlm.runExperimentWithReport(brain, world, numSteps, detailedLogging)`
Run experiment with detailed reporting of metrics.

### `pynlm.runBatchSimulation(configs, stepsPerConfig)`
Run batch simulation with multiple configurations.

### `pynlm.extractBrainState(brain)`
Extract current brain state as a dictionary for analysis.

### `pynlm.extractVisualizationData(brain, maxNeurons)`
Extract neuron activity data for visualization.

### `pynlm.analyzeLearningCurve(brain, windowSize)`
Analyze learning curve (placeholder for advanced analysis).

### `pynlm.getDetailedStatistics(brain)`
Get detailed brain statistics for analysis.

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

| Expert Features | Code |
|----------------|------|
| Scientific config | `pynlm.createScientificConfig()` |
| Minimal config | `pynlm.createMinimalConfig()` |
| Sized world | `pynlm.createWorldWithSize(width, height, visionW, visionH)` |
| Learning agent | `pynlm.createLearningAgent(config)` |
| Simple experiment | `pynlm.runSimpleExperiment(brain, world, steps)` |
| Detailed experiment | `pynlm.runExperimentWithReport(brain, world, steps, logging)` |
| Batch simulation | `pynlm.runBatchSimulation(configs, steps)` |
| Brain analysis | `pynlm.extractBrainState(brain)` |
| Visualization data | `pynlm.extractVisualizationData(brain, max)` |
| Learning curve | `pynlm.analyzeLearningCurve(brain, window)` |
| Statistics | `pynlm.getDetailedStatistics(brain)` |

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
1. Read `HOW_TO_USE.md` for more details (if available)
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the advanced functions for expert use
5. Explore the batch processing and analysis tools

That's it! You're now ready to use NLM with both basic and advanced functionality.