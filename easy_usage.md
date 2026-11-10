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

### Project 4: Advanced Brain Experiment

```python
import pynlm

# Set up advanced experiment with learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with full capabilities enabled
agent = pynlm.createAgentBrain(brain)

# Configure experiment world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setRandomSeed(42)  # reproducible experiments
agent.initialize(world)

# Enable all learning capabilities
agent.enableRewardModulation(True)      # learn from rewards
agent.enableStructuralPlasticity(True)  # grow new connections
agent.enableDevelopment(True)           # brain matures over time
agent.enableCuriosity(True)              # explore new things

# Run advanced experiment
for step in range(500):
    world.update(0.1)
    
    # Process what the agent sees
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks about what it perceives
    brain.step(step)
    
    # Agent decides what to do
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward-based learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update developmental stage
    agent.updateDevelopment(0.1)
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")

print("Advanced experiment complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Usage Examples

#### 1. Brain Copying and State Restoration

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some steps to create brain state
for i in range(100):
    brain.step(i)

# Create a copy of the brain for later use
saved_brain = brain.copy()

# Use the copy as a fresh start with same configuration
fresh_brain = saved_brain.copy()
fresh_brain.initialize()

# Run the fresh brain fresh_brain.step(0) for i in range(100):
print("Copy test complete!")
```

#### 2. Experiment Management

```python
import pynlm

# Create experiment-optimized configuration
config = pynlm.createExperimentConfig()

# Setup multiple brains for comparison
brains = []
for i in range(3):
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run same experiment on all brains
for step in range(200):
    for brain in brains:
        brain.step(step)

# Compare results
print("Experiment comparison:")
for i, brain in enumerate(brains):
    print(f"  Brain {i}: {brain.getTotalSpikeCount()} spikes")
```

#### 3. Context Manager Usage

```python
import pynlm

# Create a brain with context manager
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Use the brain in a controlled environment
    for step in range(50):
        brain.step(step)
    
    # Automatic cleanup when exiting the 'with' block

print("Context manager test complete!")
```

#### 4. Configuration Validation

```python
import pynlm

# Create a configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Validate configuration before use
keys = pynlm.validate_config(config)
print(f"Configuration keys: {keys}")

# Create configuration from Python dict
config_dict = {
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
}

experiment_config = pynlm.create_config_from_dict(config_dict)
```

#### 5. Simulation Control and Monitoring

```python
import pynlm

# Create monitoring utilities
config = pynlm.getLibraryInfo()
print(f"NLM Python bindings version: {config}")

# Setup comprehensive monitoring
config = pynlm.createExperimentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create monitoring environment
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent.initialize(world)

# Run with monitoring
for step in range(100):
    world.update(0.1)
    
    # Get comprehensive state information
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Monitor all aspects
    print(f"Step {step}:")
    print(f"  Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
```

## Common Patterns for Advanced Users

### Pattern 1: Run Multiple Experiments

```python
def run_multiple_experiments(num_experiments=5, steps_per_experiment=200):
    """Run the same experiment multiple times with different random seeds."""
    
    results = []
    
    for experiment in range(num_experiments):
        config = pynlm.createExperimentConfig()
        config.set("random_seed", 42 + experiment)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Run experiment
        for step in range(steps_per_experiment):
            agent.updateDevelopment(0.1)
            brain.step(step)
        
        results.append({
            'experiment': experiment,
            'total_spikes': brain.getTotalSpikeCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage()
        })
    
    return results

# Run multiple experiments
experiment_results = run_multiple_experiments()
print("Experiment results:", experiment_results)
```

### Pattern 2: Experiment Comparison

```python
def compare_experiments(configurations, steps=300):
    """Compare different configurations side by side."""
    
    experiment_results = {}
    
    for config_name, config in configurations.items():
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Run experiment
        for step in range(steps):
            agent.updateDevelopment(0.1)
            brain.step(step)
        
        experiment_results[config_name] = {
            'brain': brain,
            'agent': agent,
            'total_spikes': brain.getTotalSpikeCount(),
            'learning_efficiency': brain.getTotalSpikeCount() / steps,
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel()
        }
    
    return experiment_results

# Compare different configurations
configs = {
    'small': pynlm.createConfigFromDict({"brain.neuron_count": 500}),
    'medium': pynlm.createConfigFromDict({"brain.neuron_count": 1000}),
    'large': pynlm.createConfigFromDict({"brain.neuron_count": 2000}),
}

comparison = compare_experiments(configs)
print("Configuration comparison:", comparison)
```

### Pattern 3: Real-time Monitoring

```python
def monitor_brain(brain, world, agent, duration=60, sampling_rate=10):
    """Monitor brain activity in real-time."""
    
    monitoring_data = {
        'steps': [],
        'spike_counts': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'reward_values': []
    }
    
    for step in range(duration):
        world.update(0.1)
        
        # Collect data at sampling rate
        if step % sampling_rate == 0:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Store monitoring data
            monitoring_data['steps'].append(step)
            monitoring_data['spike_counts'].append(brain.getTotalSpikeCount())
            monitoring_data['firing_rates'].append(brain.getAverageFiringRate())
            monitoring_data['curiosity_levels'].append(agent.getCuriosityLevel())
            monitoring_data['novelty_levels'].append(agent.getNoveltyLevel())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            monitoring_data['reward_values'].append(reward)
    
    return monitoring_data

# Start monitoring
monitoring_results = monitor_brain(brain, world, agent, duration=100)
print("Monitoring complete. Collected data for", len(monitoring_results['steps']), "steps")
```

---

## Advanced Configuration Options

### 1. Custom Configuration Creation

```python
import pynlm

# Create custom configuration for specific research

# For cognitive experiments
config = pynlm.createConfigFromDict({
    "brain.neuron_count": 2000,
   Cas: "brain.synapse_density": 0.12,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.5,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "memory.working.capacity": 100,
    "memory.episodic.max_episodes": 50,
    "prediction.enabled": True,
    "cognition.planning.enabled": True
})

# For developmental studies
config = pynlm.createConfigFromDict({
    "brain.neuron_count": 800,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.001,
    "development.critical_period": 200,
    "development.maturation_rate": 0.1,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True
})
```

### 2. Configuration Validation

```python
import pynlm

# Validate your configuration
def validate_research_config(config):
    """Validate configuration for research purposes."""
    
    validation_errors = []
    validation_warnings = []
    
    # Check required settings
    if not config.has("brain.neuron_count"):
        validation_errors.append("Missing brain.neuron_count")
    
    if config.getOr<int64_t>("brain.neuron_count", 0) < 100:
        validation_warnings.append("Very small brain (fewer than 100 neurons) may not show interesting dynamics")
    
    # Check for reasonable values
    if config.has("brain.synapse_density"):
        density = config.getOr<float>("brain.synapse_density", 0.0)
        if density > 0.5:
            validation_warnings.append("High synapse density (>0.5) may cause network saturation")
    
    # Check plasticity settings
    if config.has("plasticity.stdp.enable") and config.getOr<bool>("plasticity.stdp.enable", false):
        if not config.has("plasticity.stdp.learning_rate") or config.getOr<float>("plasticity.stdp.learning_rate", 0.0) <= 0:
            validation_errors.append("STDP enabled but no positive learning rate specified")
    
    return {
        'valid': len(validation_errors) == 0,
        'errors': validation_errors,
        'warnings': validation_warnings
    }

# Validate configuration
validation_result = validate_research_config(config)
if not validation_result['valid']:
    print("Configuration errors:", validation_result['errors'])
if validation_result['warnings']:
    print("Configuration warnings:", validation_result['warnings'])
```

---

## Performance Optimization Tips

### 1. Batch Processing

```python
import pynlm

# For processing multiple steps efficiently
config = pynlm.createExperimentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Process many steps at once (batching improves performance)
for step in range(0, 1000, 100):
    for i in range(100):  # Process 100 steps at once
        brain.step(step + i)
    
    # Log progress periodically
    if step % 500 == 0:
        print(f"Processed {step + 100} steps")
        print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### 2. Memory Management

```python
import pynlm

# Use context managers for automatic resource cleanup
with pynlm.createBrain(pynlm.createExperimentConfig()) as brain:
    brain.initialize()
    
    # Run simulation
    for step in range(500):
        brain.step(step)
    
    # Brain automatically cleaned up when exiting context
```

### 3. Checkpointing

```python
import pynlm

# Save and restore brain states for long experiments

# Save brain state
config = pynlm.createExperimentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for a while
for step in range(300):
    brain.step(step)

# Save checkpoint
brain.save("experiment_checkpoint.brain")
print("Checkpoint saved!")

# Later, resume experiment
resume_brain = pynlm.createBrain(config)
resume_brain.load("experiment_checkpoint.brain")
print("Brain restored from checkpoint")

# Continue from where you left off
for step in range(301, 600):
    resume_brain.step(step)
```

---

## Troubleshooting Advanced Issues

### 1. Learning Saturation

**Problem:** Neural network saturates quickly and stops learning.

**Solutions:**
```python
import pynlm

# Reduce learning rates
config = pynlm.createExperimentConfig()
config.set("plasticity.stdp.learning_rate", 0.0001)
config.set("plasticity.hebbian.learning_rate", 0.0001)

# Add more exploration
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.5)
```

### 2. Slow Performance

**Problem:** Experiments are taking too long.

**Solutions:**
```python
import pynlm

# Optimize for performance
config = pynlm.createConfigFromDict({
    "brain.neuron_count": 500,  # Smaller network
    "plasticity.stdp.enable": False,  # Disable plasticity
    "plasticity.hebbian.enable": False,  # Disable Hebbian
    "neuromod.curiosity.enable": False,  # Disable curiosity
})

# Use batch processing
for step in range(0, 1000, 100):
    for i in range(100):
        brain.step(step + i)
```

### 3. Memory Issues

**Problem:** Memory usage is too high.

**Solutions:**
```python
import pynlm

# Reduce memory usage
config = pynlm.createConfigFromDict({
    "brain.neuron_count": 300,
    "memory.working.capacity": 50,
    "memory.episodic.max_episodes": 20,
})

# Use context managers for automatic cleanup
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Brain automatically cleaned up
```

---

## Next Steps for Advanced Users

1. **Read `HOW_TO_USE.md`** for detailed C++ API documentation
2. **Read `docs/ARCHITECTURE.md`** to understand system internals
3. **Read `docs/SCIENCE.md`** for scientific background
4. **Read `docs/EXPERIMENTS.md`** for experiment descriptions
5. **Experiment with different configurations** to understand parameter effects
6. **Join the NLM research community** for collaboration and support
7. **Contribute to the project** by adding new features or fixing bugs

---

## Getting Help

For advanced support:

1. **Check the documentation**: All NLM documentation is available in the `docs/` directory
2. **Ask questions on GitHub**: Report issues and request features
3. **Join the research community**: Collaborate with other NLM users
4. **Contact the authors**: For commercial support or consulting

That's it! You're now fully equipped to use NLM at an advanced level. The Python bindings now provide everything you need for sophisticated brain simulation research and experimentation.
