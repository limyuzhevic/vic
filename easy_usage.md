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

### Example 3: Complete Agent with Learning

```python
import pynlm

# Setup with custom configuration
config = pynlm.createDefaultConfig()
# Configure advanced options
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on performance
    reward = 1.0 if world.getAgentBody().energy > 0.8 else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}: Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development Stage: {agent.getDevelopmentalStage()}")

print("Advanced agent simulation complete!")
print(f"Final stats - Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

---

## Advanced Features for Experienced Users

### Batch Processing

```python
import pynlm

def run_batch(configs, world_config, steps=500):
    results = []
    
    for i, config in enumerate(configs):
        print(f"Running batch simulation {i+1}/{len(configs)}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableBatchMode(True)
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning if enabled
            if agent.isMetricsEnabled():
                reward = 1.0 if world.getAgentBody().energy > 0.5 else 0.0
                agent.applyRewardModulation(reward, 0.0)
        
        # Collect metrics
        result = {
            'config_id': i,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_reward': agent.getNeuromodulationLevel(),
            'development_stage': agent.getDevelopmentalStage(),
            'network_health': brain.isHealthy()
        }
        results.append(result)
        
        # Save checkpoint if needed
        if agent.isCheckpointEnabled():
            brain.save(f"checkpoint_batch_{i}.bin")
    
    return results

# Example batch configuration
configs = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500 + i * 200)
    config.set("brain.connection_probability", 0.1 + i * 0.02)
    configs.append(config)

world_config = {
    'width': 15,
    'height': 15,
    'visionWidth': 6,
    'visionHeight': 6
}

# Run batch with metrics
results = run_batch(configs, world_config, steps=200)

# Analyze results
for result in results:
    print(f"Batch {result['config_id']}: Firing rate {result['final_firing_rate']:.3f}, "
          f"Spikes {result['total_spikes']}, Curiosity {result['final_curiosity']:.3f}")
```

---

## Command-Line Interface

### Basic Usage

```python
#!/usr/bin/env python3
"""
NLM Command-Line Interface
Advanced usage with scripting capabilities
"""
import pynlm
import json
import sys
from pathlib import Path

class NLMAgentCLI:
    def __init__(self):
        self.agent = None
        self.world = None
        self.brain = None
        self.config = pynlm.createDefaultConfig()
        
    def run_episode(self, steps=500, enable_learning=True):
        """Run a single simulation episode"""
        if not self.brain:
            print("No brain initialized. Create brain first.")
            return None
            
        if not self.world:
            print("No world initialized. Create world first.")
            return None
            
        if not self.agent:
            print("No agent initialized. Create agent first.")
            return None
            
        print(f"Running episode with {steps} steps")
        
        episode_data = {
            'steps': steps,
            'rewards': [],
            'actions': [],
            'observations': [],
            'brain_stats': []
        }
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get observation
            percept = self.world.getSensoryPercept()
            episode_data['observations'].append(percept.getAllSignals())
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get motor command
            action = self.agent.decodeMotorCommand()
            episode_data['actions'].append(action)
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward if learning is enabled
            if enable_learning:
                # Simple reward based on agent performance
                body = self.world.getAgentBody()
                reward = 1.0 if body.energy > 0.8 else 0.0
                episode_data['rewards'].append(reward)
                self.agent.applyRewardModulation(reward, 0.0)
                self.agent.updateDevelopment(0.1)
            else:
                episode_data['rewards'].append(0.0)
            
            # Collect brain stats periodically
            if step % 100 == 0:
                episode_data['brain_stats'].append({
                    'step': step,
                    'firing_neurons': self.brain.getFiringNeuronCount(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'average_firing_rate': self.brain.getAverageFiringRate(),
                    'curiosity': self.agent.getCuriosityLevel(),
                    'novelty': self.agent.getNoveltyLevel()
                })
        
        return episode_data
    
    def execute_command(self, command_line):
        """Execute a command from command line"""
        parts = command_line.strip().split()
        if not parts:
            return False
            
        command = parts[0].lower()
        args = parts[1:]
        
        if command == "help":
            print("""
NLM Command-Line Interface
===========================

Available commands:
  create-brain            Create a new brain with current configuration
  create-world            Create a simple world
  create-agent            Create an agent brain interface
  configure               Configure brain parameters (key=value format)
  run-episode [steps]     Run simulation episode
  stats                   Show current system statistics
  checkpoint <file>       Save brain state to checkpoint
  load-checkpoint <file>   Load brain state from checkpoint
  exit                    Exit CLI
            """)
                
        elif command == "create-brain":
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            print(f"Created brain with {self.brain.getTotalNeuronCount()} neurons")
            
        elif command == "create-world":
            self.world = pynlm.createSimpleWorld()
            print("Created simple world")
            
        elif command == "create-agent":
            if self.brain and self.world:
                self.agent = pynlm.createAgentBrain(self.brain)
                self.agent.initialize(self.world)
                print("Created agent brain interface")
            else:
                print("Error: Brain and world must be created first")
                
        elif command == "configure":
            if args:
                key_value = "=".join(args)
                if "=" in key_value:
                    key, value = key_value.split("=", 1)
                    # Simple type conversion
                    if value.isdigit():
                        value = int(value)
                    elif "." in value and value.replace(".", "").isdigit():
                        value = float(value)
                    self.config.set(key.strip(), value)
                    print(f"Set {key.strip()} = {value}")
                else:
                    print("Usage: configure <key>=<value>")
            else:
                print("Current configuration:")
                keys = self.config.getKeys()
                for key in keys:
                    print(f"  {key}")
                    
        elif command == "run-episode":
            steps = int(args[0]) if args else 500
            enable_learning = not self.agent.isBatchMode()
            self.run_episode(steps, enable_learning)
                
        elif command == "stats":
            if self.brain and self.agent and self.world:
                body = self.world.getAgentBody()
                print("=== Current System Statistics ===")
                print(f"Brain:")
                print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
                print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
                print(f"  Active neurons: {self.brain.getActiveNeuronCount()}")
                print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
                print(f"  Average firing rate: {self.brain.getAverageFiringRate():.3f}")
                print(f"  Healthy: {self.brain.isHealthy()}")
                print(f"Agent:")
                print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
                print(f"  Development stage: {self.agent.getDevelopmentalStage()}")
                print(f"  Neuromodulation: {self.agent.getNeuromodulationLevel():.3f}")
                print(f"World:")
                print(f"  Dimensions: {self.world.getWidth()} x {self.world.getHeight()}")
                print(f"  Vision: {self.world.getVisionWidth()} x {self.world.getVisionHeight()}")
                print(f"  Simulation time: {self.world.getSimulationTime():.3f}")
                print(f"  Agent position: ({body.x:.1f}, {body.y:.1f})")
                print(f"  Agent energy: {body.energy:.3f}")
            else:
                print("Error: Brain, agent, and world must be created first")
                
        elif command == "checkpoint":
            if args and self.brain:
                filename = args[0]
                if self.brain.save(filename):
                    print(f"Checkpoint saved to {filename}")
                else:
                    print(f"Failed to save checkpoint to {filename}")
            else:
                print("Usage: checkpoint <file>")
                
        elif command == "load-checkpoint":
            if args and self.brain:
                filename = args[0]
                if self.brain.load(filename):
                    print(f"Checkpoint loaded from {filename}")
                else:
                    print(f"Failed to load checkpoint from {filename}")
            else:
                print("Usage: load-checkpoint <file>")
                
        elif command == "exit":
            return True
            
        else:
            print(f"Unknown command: {command}")
            print("Type 'help' for available commands")
            
        return False

def main():
    cli = NLMAgentCLI()
    
    print("NLM Command-Line Interface")
    print("Type 'help' for available commands")
    print("Type 'exit' to quit")
    
    for line in sys.stdin:
        if line.strip().lower() == "exit":
            break
        cli.execute_command(line.strip())

if __name__ == "__main__":
    main()
```

---

## Advanced Features for Research and Development

### Performance Monitoring and Profiling

```python
import pynlm
import time
import matplotlib.pyplot as plt
import numpy as np

def monitor_performance(brain, world, agent, steps=1000):
    """Monitor simulation performance over time"""
    start_time = time.time()
    
    performance_data = {
        'timestamps': [],
        'step_times': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'memory_usage': []
    }
    
    for step in range(steps):
        step_start = time.time()
        
        # Run simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        step_time = time.time() - step_start
        performance_data['timestamps'].append(step * 0.1)
        performance_data['step_times'].append(step_time)
        performance_data['firing_rates'].append(brain.getFiringNeuronCount())
        performance_data['curiosity_levels'].append(agent.getCuriosityLevel())
        performance_data['novelty_levels'].append(agent.getNoveltyLevel())
        
        # Performance analysis
        if step % 100 == 0:
            avg_step_time = np.mean(performance_data['step_times'])
            print(f"Step {step}: {avg_step_time:.4f}s per step, "
                  f"Speed: {1/avg_step_time:.2f} steps/s")
    
    end_time = time.time()
    performance_data['total_time'] = end_time - start_time
    performance_data['steps_per_second'] = steps / performance_data['total_time']
    
    return performance_data

# Example usage with performance monitoring
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableMetrics(True)

# Run with performance monitoring
print("Running simulation with performance monitoring...")
performance = monitor_performance(brain, world, agent, steps=500)

# Performance analysis
total_time = performance['total_time']
avg_step_time = np.mean(performance['step_times'])
steps_per_sec = performance['steps_per_second']

print(f"\n=== Performance Summary ===")
print(f"Total time: {total_time:.2f}s")
print(f"Average step time: {avg_step_time:.4f}s")
print(f"Steps per second: {steps_per_sec:.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Performance visualization
plt.figure(figsize=(15, 10))

plt.subplot(2, 3, 1)
plt.plot(performance['timestamps'], performance['firing_rates'])
plt.title('Firing Rate Over Time')
plt.xlabel('Time (s)')
plt.ylabel('Firing Rate')

plt.subplot(2, 3, 2)
plt.plot(performance['timestamps'], performance['curiosity_levels'])
plt.title('Curiosity Level Over Time')
plt.xlabel('Time (s)')
plt.ylabel('Curiosity')

plt.subplot(2, 3, 3)
plt.plot(performance['timestamps'], performance['novelty_levels'])
plt.title('Novelty Level Over Time')
plt.xlabel('Time (s)')
plt.ylabel('Novelty')

plt.subplot(2, 3, 4)
plt.hist(performance['step_times'], bins=50, alpha=0.7)
plt.title('Step Time Distribution')
plt.xlabel('Step Time (s)')
plt.ylabel('Frequency')

plt.subplot(2, 3, 5)
plt.scatter(performance['curiosity_levels'], performance['novelty_levels'], alpha=0.5, s=1)
plt.title('Curiosity vs Novelty')
plt.xlabel('Curiosity')
plt.ylabel('Novelty')

plt.subplot(2, 3, 6)
plt.plot(performance['timestamps'], np.cumsum(performance['step_times']))
plt.title('Cumulative Time')
plt.xlabel('Time (s)')
plt.ylabel('Cumulative Time (s)')

plt.tight_layout()
plt.show()
```

### Advanced Configuration Management

```python
import pynlm
import json
import os
from pathlib import Path

class AdvancedNLMConfigurator:
    """Advanced configuration management for NLM"""
    
    def __init__(self, config_dir="configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        
    def create_research_config(self, name, params):
        """Create a research-oriented configuration"""
        config = pynlm.createDefaultConfig()
        
        # Set parameters
        if "neuron_count" in params:
            config.set("brain.neuron_count", params["neuron_count"])
        if "connection_probability" in params:
            config.set("brain.connection_probability", params["connection_probability"])
        if "region_count" in params:
            config.set("brain.region_count", params["region_count"])
        
        # Plasticity settings
        if "plasticity" in params:
            plastic = params["plasticity"]
            if "stdp_enable" in plastic:
                config.set("plasticity.stdp.enable", plastic["stdp_enable"])
            if "stdp_learning_rate" in plastic:
                config.set("plasticity.stdp.learning_rate", plastic["stdp_learning_rate"])
            if "hebbian_enable" in plastic:
                config.set("plasticity.hebbian.enable", plastic["hebbian_enable"])
            if "structural_enable" in plastic:
                config.set("plasticity.structural.enable", plastic["structural_enable"])
        
        # Neuromodulation settings
        if "neuromodulation" in params:
            neuromod = params["neuromodulation"]
            if "dopamine_scale" in neuromod:
                config.set("neuromod.dopamine.scale", neuromod["dopamine_scale"])
            if "curiosity_enable" in neuromod:
                config.set("neuromod.curiosity.enable", neuromod["curiosity_enable"])
            if "novelty_enable" in neuromod:
                config.set("neuromod.novelty.enable", neuromod["novelty_enable"])
        
        # Development settings
        if "development" in params:
            dev = params["development"]
            if "synaptogenesis_rate" in dev:
                config.set("development.synaptogenesis_rate", dev["synaptogenesis_rate"])
            if "pruning_rate" in dev:
                config.set("development.pruning_rate", dev["pruning_rate"])
        
        # Save configuration
        config_path = self.config_dir / f"{name}.json"
        self.save_config(config, config_path)
        
        return config
    
    def save_config(self, config, filepath):
        """Save configuration to file"""
        import json
        
        # Extract key parameters for JSON
        config_data = {
            "name": config.getOr("name", "unknown"),
            "description": config.getOr("description", "NLM configuration"),
            "parameters": {
                "brain": {
                    "neuron_count": config.getOr("brain.neuron_count", 1000),
                    "connection_probability": config.getOr("brain.connection_probability", 0.1),
                    "region_count": config.getOr("brain.region_count", 1)
                },
                "plasticity": {
                    "stdp_enable": config.getOr("plasticity.stdp.enable", True),
                    "hebbian_enable": config.getOr("plasticity.hebbian.enable", True),
                    "structural_enable": config.getOr("plasticity.structural.enable", True),
                    "learning_rate": config.getOr("plasticity.learning_rate", 0.01)
                },
                "neuromodulation": {
                    "dopamine_scale": config.getOr("neuromod.dopamine.scale", 1.0),
                    "curiosity_enable": config.getOr("neuromod.curiosity.enable", True),
                    "novelty_enable": config.getOr("neuromod.novelty.enable", True)
                },
                "development": {
                    "synaptogenesis_rate": config.getOr("development.synaptogenesis_rate", 0.001),
                    "pruning_rate": config.getOr("development.pruning_rate", 0.0001)
                }
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        return True
    
    def load_config(self, filepath):
        """Load configuration from file"""
        import json
        
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        config = pynlm.createDefaultConfig()
        
        # Apply parameters
        if "parameters" in config_data:
            params = config_data["parameters"]
            
            # Brain parameters
            if "brain" in params:
                brain_params = params["brain"]
                if "neuron_count" in brain_params:
                    config.set("brain.neuron_count", brain_params["neuron_count"])
                if "connection_probability" in brain_params:
                    config.set("brain.connection_probability", brain_params["connection_probability"])
                if "region_count" in brain_params:
                    config.set("brain.region_count", brain_params["region_count"])
            
            # Plasticity parameters
            if "plasticity" in params:
                plastic_params = params["plasticity"]
                if "stdp_enable" in plastic_params:
                    config.set("plasticity.stdp.enable", plastic_params["stdp_enable"])
                if "hebbian_enable" in plastic_params:
                    config.set("plasticity.hebbian.enable", plastic_params["hebbian_enable"])
                if "structural_enable" in plastic_params:
                    config.set("plasticity.structural.enable", plastic_params["structural_enable"])
                if "learning_rate" in plastic_params:
                    config.set("plasticity.learning_rate", plastic_params["learning_rate"])
            
            # Neuromodulation parameters
            if "neuromodulation" in params:
                neuromod_params = params["neuromodulation"]
                if "dopamine_scale" in neuromod_params:
                    config.set("neuromod.dopamine.scale", neuromod_params["dopamine_scale"])
                if "curiosity_enable" in neuromod_params:
                    config.set("neuromod.curiosity.enable", neuromod_params["curiosity_enable"])
                if "novelty_enable" in neuromod_params:
                    config.set("neuromod.novelty.enable", neuromod_params["novelty_enable"])
            
            # Development parameters
            if "development" in params:
                dev_params = params["development"]
                if "synaptogenesis_rate" in dev_params:
                    config.set("development.synaptogenesis_rate", dev_params["synaptogenesis_rate"])
                if "pruning_rate" in dev_params:
                    config.set("development.pruning_rate", dev_params["pruning_rate"])
        
        return config
    
    def create_research_suite(self):
        """Create a suite of research configurations"""
        configs = {}
        
        # Minimal configuration for baseline testing
        configs['minimal'] = self.create_research_config('minimal', {
            'neuron_count': 100,
            'connection_probability': 0.05,
            'plasticity': {
                'stdp_enable': True,
                'hebbian_enable': False,
                'structural_enable': False
            },
            'neuromodulation': {
                'dopamine_scale': 1.0,
                'curiosity_enable': False,
                'novelty_enable': False
            }
        })
        
        # Standard configuration for general research
        configs['standard'] = self.create_research_config('standard', {
            'neuron_count': 1000,
            'connection_probability': 0.1,
            'region_count': 1,
            'plasticity': {
                'stdp_enable': True,
                'hebbian_enable': True,
                'structural_enable': True,
                'learning_rate': 0.01
            },
            'neuromodulation': {
                'dopamine_scale': 1.5,
                'curiosity_enable': True,
                'novelty_enable': True
            },
            'development': {
                'synaptogenesis_rate': 0.002,
                'pruning_rate': 0.0002
            }
        })
        
        # Large-scale configuration for high-performance computing
        configs['large_scale'] = self.create_research_config('large_scale', {
            'neuron_count': 5000,
            'connection_probability': 0.15,
            'region_count': 3,
            'plasticity': {
                'stdp_enable': True,
                'hebbian_enable': True,
                'structural_enable': True,
                'learning_rate': 0.005
            },
            'neuromodulation': {
                'dopamine_scale': 2.0,
                'curiosity_enable': True,
                'novelty_enable': True
            },
            'development': {
                'synaptogenesis_rate': 0.005,
                'pruning_rate': 0.0005
            }
        })
        
        # Developmental configuration for lifespan studies
        configs['developmental'] = self.create_research_config('developmental', {
            'neuron_count': 2000,
            'connection_probability': 0.12,
            'plasticity': {
                'stdp_enable': True,
                'hebbian_enable': True,
                'structural_enable': True
            },
            'neuromodulation': {
                'dopamine_scale': 1.8,
                'curiosity_enable': True,
                'novelty_enable': True
            },
            'development': {
                'synaptogenesis_rate': 0.003,
                'pruning_rate': 0.0003,
                'maturation_rate': 0.001
            }
        })
        
        return configs
    
    def run_research_suite(self, configs, world_config, steps=1000):
        """Run a suite of research simulations"""
        print("Running research suite...")
        print(f"Number of configurations: {len(configs)}")
        print(f"Simulation steps per configuration: {steps}")
        
        results = {}
        
        for name, config in configs.items():
            print(f"\n--- Running: {name} ---")
            
            # Create simulation components
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(**world_config)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableMetrics(True)
            
            # Run simulation
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply learning if enabled
                if agent.isRewardModulationEnabled():
                    reward = 1.0 if world.getAgentBody().energy > 0.5 else 0.0
                    agent.applyRewardModulation(reward, 0.0)
                
                agent.updateDevelopment(0.1)
                
                if step % 200 == 0:
                    print(f"  Step {step}: Firing rate {brain.getAverageFiringRate():.2f}, "
                          f"Curiosity {agent.getCuriosityLevel():.3f}")
            
            # Collect results
            results[name] = {
                'config_name': name,
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'final_curiosity': agent.getCuriosityLevel(),
                'final_novelty': agent.getNoveltyLevel(),
                'final_reward': agent.getNeuromodulationLevel(),
                'development_stage': agent.getDevelopmentalStage(),
                'network_health': brain.isHealthy(),
                'total_time': steps * 0.1  # Approximate
            }
            
            # Save results
            self.save_results(results[name], f"research_results_{name}.json")
        
        return results
    
    def save_results(self, result, filename):
        """Save research results"""
        import json
        
        with open(filename, 'w') as f:
            json.dump(result, f, indent=2)
        
        print(f"Results saved to {filename}")
        return True

# Example usage
if __name__ == "__main__":
    # Initialize configurator
    configurator = AdvancedNLMConfigurator()
    
    # Create research suite
    research_configs = configurator.create_research_suite()
    
    print(f"Created {len(research_configs)} research configurations:")
    for name in research_configs:
        print(f"  - {name}")
    
    # World configuration
    world_config = {
        'width': 25,
        'height': 25,
        'visionWidth': 10,
        'visionHeight': 10
    }
    
    # Run research suite
    results = configurator.run_research_suite(
        research_configs, world_config, steps=800
    )
    
    # Print summary
    print(f"\n{'='*60}")
    print("RESEARCH SUITE SUMMARY")
    print(f"{'='*60}")
    
    for name, result in results.items():
        print(f"\n{name.upper()}:")
        print(f"  Neurons: {result['final_firing_rate']:.1f}")
        print(f"  Total spikes: {result['total_spikes']}")
        print(f"  Final curiosity: {result['final_curiosity']:.3f}")
        print(f"  Network health: {'✓ Healthy' if result['network_health'] else '✗ Unhealthy'}")
```
