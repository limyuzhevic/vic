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

### Project 4: Advanced Agent with Learning

```python
import pynlm

# Setup with research brain for reinforcement learning
brain = pynlm.createResearchBrain("reinforcement_learning")

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run enhanced simulation with learning
rewards = []
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward from world
    percept = world.getSensoryPercept()
    reward = sum(percept.getInternal() + percept.getTouch())
    agent.applyRewardModulation(reward, 0.0)
    
    rewards.append(reward)
    
    if step % 100 == 0:
        print(f"Step {step}: Reward={reward:.3f}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}, "
              f"Firing={brain.getFiringNeuronCount()}")

print(f"\nSimulation complete!")
print(f"Total reward: {sum(rewards):.2f}")
print(f"Average curiosity: {sum(agent.getCuriosityLevel() for _ in rewards)/len(rewards):.3f}")
```

### Project 5: Batch Simulation for Experimentation

```python
import pynlm
import time

# Create batch processor for running multiple experiments
batch_processor = pynlm.createBatchProcessor(maxParallel=4)

def run_single_experiment(experiment_id, config_overrides=None):
    """Run a single experiment with optional configuration overrides."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Apply experiment-specific configuration
    if config_overrides:
        for key, value in config_overrides.items():
            if isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run experiment
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    return {
        'experiment_id': experiment_id,
        'final_reward': agent.getNeuromodulationLevel(),
        'curiosity': agent.getCuriosityLevel(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount()
    }

# Define multiple experiments
experiments = [
    {'id': 'exp1', 'brain.neuron_count': 500, 'plasticity.stdp.learning_rate': 0.001},
    {'id': 'exp2', 'brain.neuron_count': 1000, 'plasticity.stdp.learning_rate': 0.01},
    {'id': 'exp3', 'brain.neuron_count': 2000, 'plasticity.stdp.learning_rate': 0.005},
    {'id': 'exp4', 'brain.neuron_count': 500, 'neuromod.dopamine.scale': 2.0},
    {'id': 'exp5', 'brain.neuron_count': 1500, 'neuromod.dopamine.scale': 0.5},
]

# Run experiments in batch
print("Running batch experiments...")
start_time = time.time()

results = []
for exp_config in experiments:
    result = run_single_experiment(
        exp_config['id'],
        {k: v for k, v in exp_config.items() if k != 'id'}
    )
    results.append(result)
    print(f"Completed {exp_config['id']}: "
          f"Reward={result['final_reward']:.3f}, "
          f"Curiosity={result['curiosity']:.3f}")

end_time = time.time()

print(f"\nBatch completed in {end_time - start_time:.2f} seconds!")
print("\nSummary:")
for result in results:
    print(f"  {result['experiment_id']}: "
          f"Reward={result['final_reward']:.3f}, "
          f"Firing={result['firing_neurons']}, "
          f"Spikes={result['total_spikes']}")
```

### Project 6: Configuration Management

```python
import pynlm
import json

# Create and manage complex configuration
config = pynlm.createDefaultConfig()

# Get all available configuration options
all_options = pynlm.getAllConfigOptions()

# Modify configuration programmatically
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.initial_plasticity", 1.2)

# Create brain from config
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Synapses: {brain.getTotalSynapseCount()}")

# Save configuration
config.saveToFile("my_brain_config.txt")

# Create configuration from Python dictionary (for JSON integration)
brain_config = {
    "brain.neuron_count": 1500,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.learning_rate": 0.002,
    "neuromod.curiosity.enable": True,
    "development.initial_plasticity": 1.0
}

research_brain = pynlm.createBrainFromDict(brain_config)
research_brain.initialize()
print(f"Created research brain with {research_brain.getTotalNeuronCount()} neurons")
```

### Project 7: System Information and Advanced Features

```python
import pynlm

# Get comprehensive system information
system_info = pynlm.getSystemInfo()
print("=== System Information ===")
for key, value in system_info.items():
    print(f"{key}: {value}")

# Get full version information with build details
version_info = pynlm.getFullVersionInfo()
print(f"\n=== Version Information ===")
print(f"Version: {version_info['version']}")
print(f"Build Date: {version_info['build_date']}")
print(f"Build Type: {version_info['build_type']}")
print(f"Compiler: {version_info['compiler']}")
print(f"Project: {version_info['project']}")
print(f"Phase: {version_info['phase']}")

# Get available examples
examples = pynlm.getExamples()
print(f"\n=== Available Examples ===")
print(f"Number of examples: {len(examples)}")
for i, example in enumerate(examples, 1):
    print(f"\nExample {i}:")
    print(example[:200] + "..." if len(example) > 200 else example)

# Show all configuration options
config_options = pynlm.getAllConfigOptions()
print(f"\n=== Configuration Options ===")
print(f"Total options: {len(config_options)}")
for i, (key, description) in enumerate(config_options.items(), 1):
    print(f"{i}. {key}: {description}")
    if i >= 10:  # Show first 10 options
        print("   ... (more options available)")
        break
```

---

## Advanced Features for Power Users

### 1. Research Brain Configurations

The NLM library includes specialized brain configurations for different research domains:

```python
# Reinforcement Learning Brain
rl_brain = pynlm.createResearchBrain("reinforcement_learning")

# Pattern Recognition Brain
pr_brain = pynlm.createResearchBrain("pattern_recognition")

# Sequential Learning Brain
seq_brain = pynlm.createResearchBrain("sequential_learning")

# Custom domain brain
custom_brain = pynlm.createResearchBrain("custom", "path/to/config.cfg")
```

### 2. Episode Management

```python
# Use episode manager for controlled simulation episodes
episode_manager = pynlm.createEpisodeManager()
rewards = episode_manager.runEpisode(steps=1000, config_file="research_config.cfg")
```

### 3. Batch Processing

```python
# Process multiple experiments in parallel
batch_processor = pynlm.createBatchProcessor(maxParallel=8)
results = batch_processor.runExperiments(experiment_configs)
```

### 4. Configuration Dictionary API

```python
# Create complex configurations using Python dictionaries
complex_config = {
    "brain.neuron_count": 3000,
    "brain.synapse_density": 0.25,
    "plasticity.stdp.learning_rate": 0.01,
    "plasticity.hebbian.enable": True,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "development.initial_plasticity": 1.5,
    "memory.working_capacity": 200,
    "memory.episodic_duration": 3000,
    "prediction.enabled": True,
    "prediction.horizon": 20
}

brain = pynlm.createBrainFromDict(complex_config)
```

---

## Quick Reference for Advanced Users

### Core Functions

| Function | Description | Example |
|----------|-------------|---------|
| `createResearchBrain(domain)` | Create specialized brain for research domain | `createResearchBrain("reinforcement_learning")` |
| `createBrainFromDict(dict)` | Create brain from Python dictionary | `createBrainFromDict(config_dict)` |
| `saveBrainState(brain)` | Save brain state to file | `saveBrainState(brain)` |
| `runEpisode(steps, config_file)` | Run complete simulation episode | `runEpisode(1000)` |
| `getSystemInfo()` | Get system capabilities | `getSystemInfo()` |
| `getFullVersionInfo()` | Get detailed version info | `getFullVersionInfo()` |
| `getAllConfigOptions()` | Get all configuration options | `getAllConfigOptions()` |

### Advanced Configuration Keys

| Category | Key | Description |
|----------|-----|-------------|
| Brain | `brain.neuron_count` | Total number of neurons |
| Brain | `brain.synapse_density` | Synapse connectivity density |
| Plasticity | `plasticity.stdp.learning_rate` | STDP learning rate |
| Neuromodulation | `neuromod.dopamine.scale` | Dopamine modulation scale |
| Development | `development.initial_plasticity` | Initial plasticity level |
| Memory | `memory.working_capacity` | Working memory capacity |

### Error Handling

```python
import pynlm
import traceback

try:
    brain = pynlm.createResearchBrain("reinforcement_learning")
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Complex simulation
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
except RuntimeError as e:
    print(f"Runtime error: {e}")
    # Handle configuration or initialization errors
    
except Exception as e:
    print(f"Unexpected error: {e}")
    traceback.print_exc()
```

---

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
```python
# Make sure to initialize the brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # CRITICAL!
for i in range(100):
    brain.step(i)
```

**"The agent isn't moving"**
```python
# Check that all components are properly connected
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Make sure world.update() is called
for step in range(100):
    world.update(0.1)  # CRITICAL!
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())  # CRITICAL!
```

**"Everything is 0"**
```python
# Brains need time to "warm up" - try more steps and provide input
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Give the brain some input
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)

for i in range(500):  # More steps for warmup
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: Firing neurons = {brain.getFiringNeuronCount()}")
```

### Advanced Debugging

```python
import pynlm

# Create a debug-enabled brain with detailed logging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with detailed output
for step in range(50):
    print(f"\n--- Step {step} ---")
    print(f"Brain status: {brain.getDevelopmentalStage()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    if step % 10 == 0:
        # Log detailed statistics
        brain.logStatus()
    
    brain.step(step)
```

---

## Final Tips

1. **Start Simple**: Begin with the minimal examples to understand the basic workflow
2. **Incremental Development**: Build up complexity gradually
3. **Use Logging**: Take advantage of the logging system for debugging
4. **Experiment**: Try different configuration parameters
5. **Batch Processing**: Use batch processing for systematic experiments
6. **Documentation**: Use the configuration API for complex setups
7. **Error Handling**: Implement robust error handling for production use

---

That's it! You're now ready to use NLM at any level from beginner to advanced researcher. The library provides everything you need for simple brain simulations up to complex research experiments.


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
