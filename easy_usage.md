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

## Advanced Patterns and Examples

### Pattern 1: Factory-Based Configuration

This pattern uses the new factory functions to create specialized brains for different use cases.

```python
import pynlm

# Get configuration presets
presets = pynlm.createConfigPresets()

# Create different types of brains based on presets
simple_brain = pynlm.createBrain(presets["simple"])
simple_brain.initialize()

advanced_brain = pynlm.createBrain(presets["advanced"])
advanced_brain.initialize()

# Both brains work with the same interface, just with different capabilities
```

### Pattern 2: Complete Learning Environment with Data Collection

This pattern demonstrates how to use the advanced learning environment factory with built-in data collection.

```python
import pynlm

# Create a complete learning environment
brain, world, agent = pynlm.createLearningEnvironment()

# Set up data collection
collected_data = {}

def record_step(step):
    if step % 20 == 0:
        step_data = {
            "step": step,
            "spike_count": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "development_stage": brain.getDevelopmentalStage(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel()
        }
        collected_data[step] = step_data

# Run simulation with data collection
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    record_step(step)
    
    # Optional: decode motor command if agent is initialized
    if agent.getBrain():
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Analyze results
if collected_data:
    final_step = max(collected_data.keys())
    print(f"Simulation completed with {final_step} steps recorded")
    print(f"Final spike count: {collected_data[final_step]['spike_count']}")
    print(f"Learning efficiency: {collected_data[final_step]['firing_rate']:.2f} Hz")
```

### Pattern 3: Performance Monitoring and Benchmarking

This pattern shows how to use the built-in performance monitoring utilities.

```python
import pynlm
import time

# Create a brain for benchmarking
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Benchmark performance
steps, duration, steps_per_second = pynlm.benchmarkBrain(brain, 1000)
print(f"Performance: {steps_per_second:.0f} steps/second")

# Profile activity for analysis
profile_stats = pynlm.profileBrainActivity(brain, 200)
print(f"Profile - Max firing: {profile_stats['max_firing_neurons']} neurons")
print(f"Profile - Total spikes: {profile_stats['total_spikes']}")
print(f"Profile - Average rate: {profile_stats['average_firing_rate']:.2f} Hz")
```

### Pattern 4: Research-Grade Brain Setup

This pattern demonstrates the advanced research-grade brain creation.

```python
import pynlm

# Create a research-grade brain with all systems enabled
brain, config = pynlm.createResearchGradeBrain()

# Verify all systems are enabled
print("Research-grade brain configuration:")
print(f"- Total neurons: {brain.getTotalNeuronCount()}")
print(f"- STDP enabled: {config.getOr<bool>('plasticity.stdp.enable', false)}")
print(f"- Hebbian enabled: {config.getOr<bool>('plasticity.hebbian.enable', false)}")
print(f"- Structural plasticity enabled: {config.getOr<bool>('plasticity.structural.enable', false)}")
print(f"- Neuromodulation enabled: {config.getOr<bool>('neuromod.dopamine.scale', 0.0f) > 0}")

# Run a research experiment
brain.initialize()
for step in range(100):
    brain.step(step, step * 0.001)

print("Research experiment completed")
```

### Pattern 5: Production-Optimized Brain

This pattern shows how to create a production-ready brain with optimized settings.

```python
import pynlm

# Create a production brain with optimized settings
production_brain = pynlm.createProductionBrain(5000)

# Verify production settings
config = production_brain.getConfig()
print("Production brain configuration:")
print(f"- Neurons: {config.getOr<int64_t>('brain.neuron_count', 0)}")
print(f"- STDP learning rate: {config.getOr<float>('plasticity.stdp.learning_rate', 0.0f)}")
print(f"- Simulation timestep: {config.getOr<double>('simulation_timestep', 0.0)}")

# Run production simulation
production_brain.initialize()
steps_completed = production_brain.runSteps(100)
print(f"Production simulation completed {steps_completed} steps")
```

### Pattern 6: Advanced Experiment Setup

This pattern demonstrates how to create complete experiment setups for different research scenarios.

```python
import pynlm

# Create a plasticity experiment
plasticity_setup = pynlm.createExperimentSetup("plasticity")
brain = plasticity_setup["brain"]
world = plasticity_setup["world"]
agent = plasticity_setup["agent"]

# Run plasticity experiment
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Plasticity experiment completed")

# Create a development experiment
development_setup = pynlm.createExperimentSetup("development")
brain2 = development_setup["brain"]
world2 = development_setup["world"]
agent2 = development_setup["agent"]

# Run development experiment
for step in range(200):
    world2.update(0.1)
    agent2.processSensoryInput(world2.getSensoryPercept())
    brain2.step(step)
    action2 = agent2.decodeMotorCommand()
    world2.applyMotorCommand(action2, world2.getSimulationTime())

print("Development experiment completed")
```

### Pattern 7: Controlled Simulation with Callbacks

This pattern shows how to use the controlled simulation feature with custom callbacks.

```python
import pynlm

# Create a brain for controlled simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Define pre and post step callbacks
def pre_step_callback(step):
    if step % 50 == 0:
        print(f"Pre-step {step}: current spike count = {brain.getTotalSpikeCount()}")

def post_step_callback(step):
    if step % 50 == 0:
        print(f"Post-step {step}: firing neurons = {brain.getFiringNeuronCount()}")

# Run controlled simulation
pynlm.createControlledSimulation(brain, pre_step_callback, post_step_callback, 200)

print("Controlled simulation completed")
```

### Pattern 8: Batch Processing and Data Collection

This pattern demonstrates batch processing with efficient data collection.

```python
import pynlm

# Create multiple brains for batch processing
brains = []
for i in range(3):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brains.append(brain)

# Batch process data collection
def batch_data_collector(step, brain_list):
    if step % 25 == 0:
        for i, brain in enumerate(brain_list):
            print(f"Brain {i} - Step {step}: {brain.getTotalSpikeCount()} spikes")

# Process all brains in batch
for step in range(100):
    batch_data_collector(step, brains)
    for brain in brains:
        brain.step(step, step * 0.001)

print("Batch processing completed")
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

### Project 4: Advanced Brain Configuration with Convenience Methods

```python
import pynlm

# Use advanced factory methods for different scenarios
print("=== Project 4: Advanced Configuration Setup ===")

# 1. Research-grade brain with all systems enabled
print("\n1. Creating research-grade brain...")
research_brain, config = pynlm.createResearchGradeBrain()
print(f"   - Neurons: {research_brain.getTotalNeuronCount()}")
print(f"   - Synapse count: {research_brain.getTotalSynapseCount()}")

# 2. Production-ready brain with optimized settings
print("\n2. Creating production brain...")
production_brain = pynlm.createProductionBrain(3000)
print(f"   - Neurons: {production_brain.getTotalNeuronCount()}")
print(f"   - Firing neurons: {production_brain.getFiringNeuronCount()}")

# 3. Advanced experiment setup
print("\n3. Creating advanced experiment...")
experiment_setup = pynlm.createExperimentSetup("plasticity")
brain = experiment_setup["brain"]
world = experiment_setup["world"]
agent = experiment_setup["agent"]

# Run the experiment
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"   - Experiment completed with {brain.getTotalSpikeCount()} total spikes")

# 4. Using convenience methods for simulation control
print("\n4. Running controlled simulation...")
def logProgress(step):
    if step % 50 == 0:
        print(f"   - Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Simulate with pre/post step hooks
for step in range(100):
    brain.step(step, step * 0.001)
    logProgress(step)

print("   - Advanced setup complete!")
```

### Project 5: Learning Environment with Data Collection

```python
import pynlm

print("=== Project 5: Learning Environment with Data Collection ===")

# Create a complete learning environment with built-in data collection
brain, world, agent = pynlm.createLearningEnvironment("configs/default.cfg")

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Create a data collection system
collected_data = {}

def data_collector(step):
    if step % 10 == 0:
        step_data = {
            "step": step,
            "brain_spikes": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "development_stage": brain.getDevelopmentalStage(),
            "curiosity_level": agent.getCuriosityLevel(),
            "reward_modulation": agent.isRewardModulationEnabled()
        }
        collected_data[step] = step_data

# Run episode with data collection
steps_taken = agent.runEpisode(world, collected_data, max_steps=500)

print(f"Episode completed in {steps_taken} steps")
print(f"Collected {len(collected_data)} data points")

# Analyze collected data
if collected_data:
    final_step = max(collected_data.keys())
    final_stats = collected_data[final_step]
    print(f"Final spike count: {final_stats['brain_spikes']}")
    print(f"Final firing rate: {final_stats['firing_rate']:.2f} Hz")
    print(f"Development stage: {final_stats['development_stage']}")
    print(f"Curiosity level: {final_stats['curiosity_level']:.3f}")

print("Data collection complete!")
```

### Project 6: Performance Benchmarking

```python
import pynlm
import time

print("=== Project 6: Performance Benchmarking ===")

# Create configuration for benchmarking
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Benchmark different scenarios
print("\n1. Benchmarking basic brain steps...")
steps, duration, steps_per_second = pynlm.benchmarkBrain(brain, 1000)
print(f"   - {steps} steps in {duration} ms")
print(f"   - {steps_per_second:.0f} steps per second")

print("\n2. Benchmarking plasticity learning...")
config2 = pynlm.createDefaultConfig()
config2.set("plasticity.stdp.enable", True)
config2.set("plasticity.hebbian.enable", True)
config2.set("plasticity.structural.enable", True)

brain2 = pynlm.createBrain(config2)
brain2.initialize()
steps2, duration2, steps_per_second2 = pynlm.benchmarkBrain(brain2, 500)
print(f"   - {steps2} steps in {duration2} ms")
print(f"   - {steps_per_second2:.0f} steps per second")

print("\n3. Profiling brain activity...")
profile_stats = pynlm.profileBrainActivity(brain, 200)
print(f"   - Max firing neurons: {profile_stats['max_firing_neurons']}")
print(f"   - Total spikes: {profile_stats['total_spikes']}")
print(f"   - Average firing rate: {profile_stats['average_firing_rate']:.2f} Hz")

print("Benchmarking complete!")
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
