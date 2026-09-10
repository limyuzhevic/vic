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

## What Can You Do with NLM?

### Core Capabilities (Phase 2 - Real Neural Computation)

NLM implements **real spiking neural computation** with:

- **LIF Neurons**: Leaky Integrate-and-Fire dynamics with biological realism
- **STDP**: Spike-Timing-Dependent Plasticity for learning
- **Hebbian Learning**: Correlated firing strengthens connections
- **Structural Plasticity**: Creates and prunes synapses automatically

### Advanced Features (Phase 4 - Emerging Cognition)

- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Prediction System**: Learns temporal sequences and predicts action consequences
- **Cognitive Functions**: Attention, planning, concept formation, self-model
- **Neuromodulation**: Dopamine (reward), curiosity (exploration), novelty detection

### Development (Phase 6 - Final Integration)

- **Developmental Stages**: Brain matures from infantile to adult state
- **Experience-Dependent Learning**: All learning comes from interaction with environment
- **Integration Testing**: Phase 6 demo verifies all systems work together

## Example: Complete Agent with Modern Features

```python
import pynlm

# Create brain with modern configuration
config = pynlm.createDefaultConfig()
# Set advanced parameters
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create sophisticated agent interface
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run sophisticated simulation
print("Starting NLM Phase 6 integration demo...")
print("Brain initialized with:", brain.getTotalNeuronCount(), "neurons")
print("Total synapses:", brain.getTotalSynapseCount())
print("Available learning subsystems: Reward, Structural Plasticity, Development, Curiosity")

for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation (learning from consequences)
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update developmental stage
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}: "
              f"Neurons firing: {brain.getFiringNeuronCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}, "
              f"Dev Stage: {brain.getDevelopmentalStage()}")

print("Simulation complete!")
print("Final metrics:")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
```

## Example: Configuration with JSON/YAML

```python
import pynlm
import json

# Create configuration with nested structure
config = pynlm.createDefaultConfig()

# Modern configuration approach (JSON-like structure)
# Brain configuration
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 4)
config.set("brain.connection_probability", 0.1)

# Plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Development configuration
config.set("development.critical_period_length", 300.0)
config.set("development.maturation_rate", 0.1)

# Save to JSON for documentation and sharing
config.saveToFile("brain_config.json")
print("Configuration saved to JSON file")

# Or load from JSON
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("brain_config.json")
print("Configuration loaded successfully")
```

## Example: Advanced Visualization

```python
import pynlm
import matplotlib.pyplot as plt

# Create brain and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation while collecting data
spike_counts = []
firing_rates = []
development_stages = []

for step in range(1000):
    brain.step(step)
    spike_counts.append(brain.getFiringNeuronCount())
    firing_rates.append(brain.getAverageFiringRate())
    development_stages.append(brain.getDevelopmentalStage())

# Generate visualization using NLM's built-in visualization
from pynlm import VisualizationInterface

visualizer = VisualizationInterface(brain)
visualizer.setup({
    "animations": ["network", "activity", "spikes", "weights"],
    "save_to_file": True,
    "output_prefix": "nlm_simulation"
})

# Start real-time visualization
visualizer.start(brain, 1000)

print("Visualization complete! Check output directory for HTML files.")
```

## Example: Learning Experiment

```python
import pynlm
import time

# Setup learning experiment
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

print("Starting NLM Learning Experiment")
print("===========================")
print("Testing plasticity mechanisms:")
print("  - STDP: Spike-timing dependent plasticity")
print("  - Hebbian: Correlated firing strengthens synapses")
print("  - Reward-modulated: Learning from consequences")
print("  - Structural: Synaptogenesis and pruning")
print("  - Curiosity: Exploration of novel stimuli")
print()

# Record initial state
initial_spikes = brain.getTotalSpikeCount()
initial_neurons = brain.getTotalNeuronCount()

# Run learning experiment
learning_steps = 2000
print(f"Running learning experiment for {learning_steps} steps...")

for step in range(learning_steps):
    # Inject varied sensory input to stimulate learning
    for i in range(10):
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f + (i * 5))
    
    brain.step(step)
    
    if step % 200 == 0:
        progress = (step / learning_steps) * 100
        print(f"  Progress: {progress:.0f}% | "
              f"Spikes: {brain.getTotalSpikeCount() - initial_spikes} | "
              f"Firing: {brain.getFiringNeuronCount()} neurons")

# Analyze learning results
final_spikes = brain.getTotalSpikeCount()
total_spike_increase = final_spikes - initial_spikes
learning_efficiency = total_spike_increase / learning_steps

print()
print("Learning Experiment Results:")
print("============================")
print(f"  Initial spikes: {initial_spikes}")
print(f"  Final spikes: {final_spikes}")
print(f"  Total spike increase: {total_spike_increase}")
print(f"  Learning efficiency: {learning_efficiency:.3f} spikes/step")
print(f"  Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Network complexity: {brain.getTotalSynapseCount()} synapses")
print(f"  E/I balance: {brain.getExcitationInhibitionRatio():.2f}")

if total_spike_increase > 100:
    print()
    print("✅ SUCCESS: Significant learning detected!")
    print("   The brain is effectively forming new connections")
    print("   through plasticity mechanisms.")
else:
    print()
    print("⚠️  WARNING: Limited learning observed.")
    print("   Check configuration or input patterns.")

print()
print("Experiment complete. The NLM brain has demonstrated")
print("experience-dependent learning capabilities.")
```

## Quick Reference: Modern API Features

### Configuration
- `config.loadFromFile()` - Load from JSON, YAML, or key=value files
- `config.loadFromArgs()` - Command-line configuration
- `config.saveToFile()` - Save configuration
- Nested configuration support for complex settings

### Brain Operations
- `brain.initialize()` - Set up neural architecture
- `brain.step()` - Single simulation step
- `brain.getTotalNeuronCount()` - Query brain properties
- `brain.getTotalSynapseCount()` - Query network size

### Agent Interface
- `agent.processSensoryInput()` - Receive sensory data
- `agent.decodeMotorCommand()` - Produce actions
- `agent.applyRewardModulation()` - Learn from outcomes
- `agent.updateDevelopment()` - Track developmental stage

### Visualization
- `VisualizationInterface()` - Real-time neural visualization
- HTML output with interactive graphs
- Multiple view modes (network, activity, spikes, weights)

## Next Steps

1. **Explore Documentation**: Read `HOW_TO_USE.md` for detailed command-line usage
2. **Architecture Deep Dive**: Review `docs/ARCHITECTURE.md` for component details
3. **Experiment Examples**: Check `src/experiments/` for complete use cases
4. **Scientific Background**: Read `docs/SCIENCE.md` for theoretical foundations

NLM provides a complete, research-grade artificial brain system that:
- Simulates real neural dynamics
- Learns from experience
- Develops over time
- Integrates multiple cognitive functions
- Can be extended with new capabilities

The system is designed for both practical applications and scientific research into artificial developmental systems.

---

## Quick Start (One-Liner)

```python
# The simplest way to get started
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
print("Brain thought", brain.getTotalSpikeCount(), "times!")
```

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

That's it! You're now ready to use NLM.
