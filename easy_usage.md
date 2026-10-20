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

## Advanced Patterns for Expert Users

### Advanced Pattern 1: Performance-Monitored Simulation

```python
import pynlm
import time

def run_monitored_simulation(brain, world, agent, num_steps, profiling_interval=100):
    """Run simulation with performance monitoring and metrics collection."""
    start_time = time.time()
    metrics = []
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step with performance tracking
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics periodically
        if step % profiling_interval == 0:
            step_metrics = {
                'step': step,
                'spike_count': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'neurons_active': brain.getActiveNeuronCount(),
                'simulation_time': time.time() - start_time,
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel()
            }
            metrics.append(step_metrics)
            print(f"Step {step}: {step_metrics}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    return metrics
```

### Advanced Pattern 2: Neural Activity Analysis

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def analyze_neural_activity(brain, duration_steps=1000):
    """Analyze neural activity patterns and generate insights."""
    
    # Collect neural activity data
    firing_rates = []
    spike_counts = []
    
    for step in range(duration_steps):
        brain.step(step)
        firing_rates.append(brain.getAverageFiringRate())
        spike_counts.append(brain.getTotalSpikeCount())
    
    # Calculate statistics
    mean_firing = np.mean(firing_rates)
    std_firing = np.std(firing_rates)
    mean_spikes = np.mean(spike_counts)
    
    # Advanced neural analysis
    activity_matrix = brain.advanced_get_neural_activity_matrix()
    network_metrics = brain.advanced_compute_network_metrics()
    
    print(f"Neural Activity Analysis:")
    print(f"  Mean firing rate: {mean_firing:.3f} Hz")
    print(f"  Firing rate std: {std_firing:.3f} Hz")
    print(f"  Mean spike count: {mean_spikes:.1f}")
    print(f"  Network efficiency: {network_metrics.get('efficiency', 0):.3f}")
    print(f"  Connection density: {network_metrics.get('density', 0):.3f}")
    
    # Generate visualization data
    visualization_data = {
        'firing_rates': firing_rates,
        'spike_counts': spike_counts,
        'activity_matrix': activity_matrix,
        'network_metrics': network_metrics,
        'timestamp': time.time()
    }
    
    return visualization_data
```

### Advanced Pattern 3: Expert Configuration Management

```python
import pynlm
import json
import yaml

def create_advanced_configuration():
    """Create an advanced configuration with expert features."""
    
    # Method 1: Using Python object
    config = pynlm.createDefaultConfig()
    
    # Advanced neural parameters
    config.set("brain.neuron_count", 5000, pynlm.ConfigSource.Runtime)
    config.set("brain.synapse_density", 0.15, pynlm.ConfigSource.Runtime)
    config.set("region_count", 3, pynlm.ConfigSource.Runtime)
    
    # Advanced plasticity settings
    config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.Runtime)
    config.set("plasticity.stdp.learning_rate", 0.002, pynlm.ConfigSource.Runtime)
    config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.Runtime)
    config.set("plasticity.hebbian.threshold", 0.5, pynlm.ConfigSource.Runtime)
    config.set("plasticity.structural.enable", True, pynlm.ConfigSource.Runtime)
    config.set("plasticity.structural.synaptogenesis_rate", 0.00005, pynlm.ConfigSource.Runtime)
    
    # Neuromodulation settings
    config.set("neuromod.dopamine.scale", 1.5, pynlm.ConfigSource.Runtime)
    config.set("neuromod.curiosity.enable", True, pynlm.ConfigSource.Runtime)
    config.set("neuromod.curiosity.exploration_bonus", 0.1, pynlm.ConfigSource.Runtime)
    config.set("neuromod.novelty.enable", True, pynlm.ConfigSource.Runtime)
    config.set("neuromod.novelty.sensitivity", 0.8, pynlm.ConfigSource.Runtime)
    
    # Development settings
    config.set("development.enable", True, pynlm.ConfigSource.Runtime)
    config.set("development.initial_plasticity", 1.0, pynlm.ConfigSource.Runtime)
    config.set("development.plasticity_decay_rate", 0.95, pynlm.ConfigSource.Runtime)
    
    return config

def save_configuration_with_validation(config, filepath):
    """Save configuration with validation and schema checking."""
    
    # Validate configuration
    is_valid = pynlm.validate_config_schema(config)
    if not is_valid:
        raise ValueError("Configuration validation failed")
    
    # Save in both JSON and YAML formats for different use cases
    json_path = filepath.replace('.', '_validated.json')
    yaml_path = filepath.replace('.', '_validated.yaml')
    
    config.saveToFile(json_path)  # This will save as JSON
    config.saveToFile(yaml_path)  # This will save as YAML
    
    print(f"Configuration saved as:")
    print(f"  JSON: {json_path}")
    print(f"  YAML: {yaml_path}")
    
    return json_path, yaml_path
```

### Advanced Pattern 4: Machine Learning Integration

```python
import pynlm
import numpy as np
from sklearn.cluster import KMeans

def integrate_with_ml_frameworks(brain, world, agent, training_steps=5000):
    """Integrate NLM with machine learning frameworks."""
    
    # Collect training data
    training_data = []
    rewards = []
    
    for step in range(training_steps):
        # Environment interaction
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Execute action and collect data
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Extract features for ML
        features = extract_neural_features(brain)
        reward = get_environment_reward(world)
        
        training_data.append(features)
        rewards.append(reward)
        
        # Apply neuromodulation
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Convert to numpy arrays for ML processing
    X = np.array(training_data)
    y = np.array(rewards)
    
    # Machine learning analysis
    print("Performing machine learning analysis...")
    
    # Cluster neural activity patterns
    kmeans = KMeans(n_clusters=5, random_state=42)
    cluster_labels = kmeans.fit_predict(X)
    
    # Analyze reward patterns
    reward_trends = analyze_reward_patterns(y)
    
    # Neural activity clustering
    neural_clusters = brain.advanced_cluster_neurons()
    
    return {
        'features': X,
        'rewards': y,
        'clusters': cluster_labels,
        'reward_trends': reward_trends,
        'neural_clusters': neural_clusters
    }
```

### Pattern 4: Real-time Monitoring

```python
import pynlm
import time
import threading
from collections import deque

def setup_real_time_monitor(brain, world, agent, update_interval=0.1):
    """Set up real-time monitoring of neural activity."""
    
    # Data buffers for monitoring
    firing_rate_history = deque(maxlen=100)
    spike_count_history = deque(maxlen=100)
    curiosity_history = deque(maxlen=100)
    novelty_history = deque(maxlen=100)
    
    def monitor_loop():
        """Background monitoring thread."""
        while True:
            try:
                firing_rate_history.append(brain.getAverageFiringRate())
                spike_count_history.append(brain.getTotalSpikeCount())
                curiosity_history.append(agent.getCuriosityLevel())
                novelty_history.append(agent.getNoveltyLevel())
                time.sleep(update_interval)
            except:
                break
    
    # Start monitoring thread
    monitor_thread = threading.Thread(target=monitor_loop, daemon=True)
    monitor_thread.start()
    
    return {
        'firing_rates': firing_rate_history,
        'spike_counts': spike_count_history,
        'curiosity': curiosity_history,
        'novelty': novelty_history,
        'thread': monitor_thread
    }
```

### Pattern 5: Reproducible Research

```python
import pynlm
import hashlib
import json

def create_reproducible_experiment_config(experiment_name, seed=42):
    """Create a reproducible configuration for research."""
    
    # Generate a unique config ID
    config_id = hashlib.md5(experiment_name.encode()).hexdigest()[:8]
    
    # Create reproducible configuration
    config = pynlm.createDefaultConfig()
    
    # Fixed random seed for reproducibility
    config.set("random_seed", seed, pynlm.ConfigSource.Default)
    config.set("simulation_timestep", 0.001, pynlm.ConfigSource.Default)
    
    # Stable neural parameters
    config.set("neuron_count", 2000, pynlm.ConfigSource.Default)
    config.set("region_count", 2, pynlm.ConfigSource.Default)
    config.set("connection_probability", 0.1, pynlm.ConfigSource.Default)
    
    # Controlled learning parameters
    config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.Default)
    config.set("plasticity.stdp.learning_rate", 0.001, pynlm.ConfigSource.Default)
    config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.Default)
    config.set("plasticity.hebbian.threshold", 0.5, pynlm.ConfigSource.Default)
    
    # Neurodevelopmental stability
    config.set("development.enable", True, pynlm.ConfigSource.Default)
    config.set("development.initial_plasticity", 1.0, pynlm.ConfigSource.Default)
    
    # Metadata for reproducibility
    metadata = {
        "experiment_name": experiment_name,
        "config_id": config_id,
        "seed": seed,
        "description": "Reproducible neural learning machine configuration",
        "created": time.time(),
        "version": pynlm.VERSION
    }
    
    print(f"Created reproducible experiment config: {experiment_name} (ID: {config_id})")
    return config, metadata
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
