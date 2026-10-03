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

## New Advanced Features Available

### 1. Error Handling

```python
import pynlm
from pynlm import NLMError

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except NLMError as e:
    print(f"Brain creation failed: {e}")
```

### 2. Configuration Management

```python
import pynlm
import json

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")
```

### 3. Memory System Access

```python
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory: {working_memory.getCapacity()} traces")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory: {episodic_memory.getEpisodeCount()} episodes")

# Access prediction system
prediction_system = brain.getPredictionSystem()
if prediction_system:
    print("Prediction system active")
```

### 4. Command Line Interface

```bash
# Install CLI
chmod +x nlm_cli.py

# Basic commands
nlm create --neurons 1000 --steps 100
nlm save brain_state.bin
nlm load brain_state.bin
nlm inspect
nlm compare
```

### 5. Benchmarking

```python
import pynlm
from nlm_benchmark import NLMBenchmark

benchmark = NLMBenchmark()

# Run quick benchmark
results = benchmark.compare_configurations([
    {'neurons': 1000, 'steps': 1000, 'description': 'Standard'},
    {'neurons': 5000, 'steps': 1000, 'description': 'Large'},
])

# Generate report
benchmark.generate_report("benchmark_report.txt")
```

### 6. Advanced Error Handling with Custom Exceptions

```python
import pynlm
from pynlm import (
    NLMError, 
    NLMInitializationError, 
    NLMInvalidArgumentError,
    NLMMemoryError,
    NLMFileError
)

# Try to create a brain with invalid configuration
try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -1)  # Invalid: negative neurons
    brain = pynlm.createBrain(config)
    brain.initialize()
except NLMInvalidArgumentError as e:
    print(f"Invalid configuration: {e}")
except NLMInitializationError as e:
    print(f"Brain initialization failed: {e}")
except NLMMemoryError as e:
    print(f"Memory error: {e}")
except NLMFileError as e:
    print(f"File error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

### 7. Enhanced Configuration Access

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 2.0)

# Get configuration values
neurons = config.get_int("brain.neuron_count", 1000)
synapse_density = config.get_float("brain.synapse_density", 0.1)
stdp_enabled = config.get_string("plasticity.stdp.enable", "true")

print(f"Config: {neurons} neurons, density {synapse_density}, STDP {stdp_enabled}")
```

### 8. Agent Configuration with Advanced Features

```python
import pynlm

# Create agent with custom configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Configure subsystem parameters
print(f"Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"Development: {agent.isDevelopmentEnabled()}")
print(f"Curiosity: {agent.isCuriosityEnabled()}")

# Monitor learning progress
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Development stage: {agent.getDevelopmentalStage()}")
```

### 9. World Configuration with Custom Parameters

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()
world.setRandomSeed(12345)  # Reproducible results
world.setAgentStart(15.0, 15.0)  # Starting position

# Configure world physics
world.setMaxEnergy(200.0)  # Maximum energy
world.setEnergyDecayRate(0.005)  # Energy decay per step

print(f"World: {world.getWidth()}x{world.getHeight()}")
print(f"Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
print(f"Max energy: {world.getMaxEnergy()}")
print(f"Energy decay: {world.getEnergyDecayRate()}")
```

### 10. Complex Brain Configuration

```python
import pynlm

# Create comprehensive configuration
config = pynlm.createDefaultConfig()

# Brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 4)
config.set("brain.synapse_density", 0.08)
config.set("brain.connection_probability", 0.04)

# Plasticity parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.001)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.00005)
config.set("plasticity.structural.pruning_rate", 0.000005)

# Neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Development parameters
config.set("development.initial_plasticity", 1.0)
config.set("development.critical_period_end", 100.0)
config.set("development.adult_plasticity", 0.1)

# Simulation parameters
config.set("simulation.timestep", 0.001)
config.set("simulation.max_steps", 1000)
config.set("random_seed", 42)

# Create brain with comprehensive configuration
brain = pynlm.createBrain(config)
brain.initialize()

print("Brain created with comprehensive configuration:")
print(f"  - {brain.getTotalNeuronCount()} neurons")
print(f"  - {brain.getRegionCount()} regions")
print(f"  - {brain.getTotalSynapseCount()} synapses")
```

### 11. Advanced Error Handling and Validation

```python
import pynlm
from pynlm import NLMError

def safe_simulation_run(neurons, steps, config_file=None):
    """Run a safe simulation with error handling."""
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        if config_file:
            # Load configuration from file
            if not config.loadFromFile(config_file):
                raise NLMError(f"Failed to load configuration from {config_file}")
        
        # Validate configuration
        if neurons <= 0:
            raise NLMError(f"Invalid neuron count: {neurons}")
        
        config.set("brain.neuron_count", neurons)
        
        # Create brain
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            raise NLMError("Failed to initialize brain")
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
            
            # Log progress
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        return brain
        
    except NLMError as e:
        print(f"Simulation error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

# Use the safe simulation
brain = safe_simulation_run(1000, 500, "config.json")
if brain:
    print("Simulation completed successfully!")
```

### 12. Performance Benchmarking

```python
import pynlm
from nlm_benchmark import NLMBenchmark

# Create benchmark
benchmark = NLMBenchmark()

# Test different configurations
configs = [
    {'neurons': 500, 'steps': 500, 'description': 'Small'},
    {'neurons': 1000, 'steps': 1000, 'description': 'Medium'},
    {'neurons': 5000, 'steps': 1000, 'description': 'Large'},
]

# Run comparisons
results = benchmark.compare_configurations(configs)

# Generate detailed report
report = benchmark.generate_report("performance_report.txt")

print("Benchmark complete!")
print("Performance report saved to: performance_report.txt")
```

### 13. Advanced World Simulation with Complex Interactions

```python
import pynlm

def complex_world_simulation(world_size, num_objects, simulation_steps):
    """Run a complex world simulation with many interactions."""
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", world_size * 10)  # Scale with world size
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=float(world_size),
        height=float(world_size),
        visionWidth=16,
        visionHeight=16
    )
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run complex simulation
    for step in range(simulation_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action with result
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward-based learning
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log significant events
        if result.success and result.reward != 0:
            print(f"Step {step}: {result.message} (reward: {result.reward})")
        
        if step % 100 == 0:
            print(f"Progress: Step {step}/{simulation_steps}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

# Run complex simulation
brain, agent, world = complex_world_simulation(30, 10, 1000)
print("Complex simulation completed!")
```

---

## Troubleshooting

### "My brain isn't doing anything"

- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that the brain has neurons configured

### "The agent isn't moving"

- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Make sure the agent has motor neurons

### "Everything is 0"

- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that your world has obstacles and resources

### "ImportError: No module named 'pynlm'"

```bash
pip install --force-reinstall .
```

### "Segmentation fault"

- Ensure `initialize()` is called before `step()`
- Make sure `reset()` is called before re-running simulation
- Check that all required libraries are properly linked

---

## Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the Command Line Interface with `nlm --help`
5. Use benchmarking with `python nlm_benchmark.py`
6. Create custom configurations
7. Implement advanced error handling and validation

That's it! You're now ready to use NLM with advanced features.

