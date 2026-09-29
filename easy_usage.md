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

---

## Performance Monitoring and Debugging Tools

### Monitoring System Status

The NLM Python bindings include enhanced monitoring capabilities to help you track simulation performance and debug issues:

```python
import pynlm
import time

def monitorSimulation(brain, world, agent, num_steps=500):
    """Monitor simulation with real-time statistics and alerts"""
    print("Starting simulation monitoring...")
    
    # Initialize monitoring data
    monitoring_data = {
        'steps': [],
        'neuron_counts': [],
        'firing_rates': [],
        'spike_counts': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'memory_usage': [],
        'warnings': []
    }
    
    # Pre-simulation validation
    if not brain.isHealthy():
        monitoring_data['warnings'].append("Brain health check failed")
        print("⚠ Warning: Brain may not be in healthy state")
    
    start_time = time.time()
    
    for step in range(num_steps):
        # Simulate one step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning if enabled
        agent.enableRewardModulation(True)
        
        # Collect monitoring data
        monitoring_data['steps'].append(step)
        monitoring_data['neuron_counts'].append(brain.getTotalNeuronCount())
        monitoring_data['firing_rates'].append(brain.getAverageFiringRate())
        monitoring_data['spike_counts'].append(brain.getTotalSpikeCount())
        monitoring_data['curiosity_levels'].append(agent.getCuriosityLevel())
        monitoring_data['novelty_levels'].append(agent.getNoveltyLevel())
        
        # Performance alerts
        firing_rate = brain.getAverageFiringRate()
        if firing_rate > 100.0:  # Alert if firing rate is too high
            monitoring_data['warnings'].append(f"Step {step}: High firing rate ({firing_rate:.2f} Hz)")
        
        if step % 100 == 0:
            elapsed = time.time() - start_time
            steps_per_sec = step / elapsed if elapsed > 0 else 0
            print(f"Progress: Step {step}/{num_steps} "
                  f"({steps_per_sec:.1f} steps/sec)")
    
    total_time = time.time() - start_time
    
    # Generate performance report
    report = {
        'total_steps': num_steps,
        'total_time': total_time,
        'steps_per_second': num_steps / total_time,
        'final_neuron_count': brain.getTotalNeuronCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'max_firing_rate': max(monitoring_data['firing_rates']),
        'avg_firing_rate': sum(monitoring_data['firing_rates']) / len(monitoring_data['firing_rates']),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'warning_count': len(monitoring_data['warnings']),
        'warnings': monitoring_data['warnings']
    }
    
    print("\n=== Performance Report ===")
    print(f"Total simulation time: {report['total_time']:.2f} seconds")
    print(f"Performance: {report['steps_per_second']:.1f} steps/second")
    print(f"Total spikes generated: {report['total_spikes']}")
    print(f"Average firing rate: {report['avg_firing_rate']:.2f} Hz")
    print(f"Peak firing rate: {report['max_firing_rate']:.2f} Hz")
    print(f"Final curiosity level: {report['final_curiosity']:.3f}")
    print(f"Final novelty level: {report['final_novelty']:.3f}")
    print(f"Warnings generated: {report['warning_count']}")
    
    if report['warning_count'] > 0:
        print("\nWarnings:")
        for warning in report['warnings'][:5]:  # Show first 5 warnings
            print(f"  - {warning}")
    
    return report, monitoring_data

# Example usage
def exampleMonitoring():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 6, 6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    report, data = monitorSimulation(brain, world, agent, 1000)
    return report
```

### Debugging and Validation Tools

```python
import pynlm

def debugBrainConfiguration(config):
    """Validate and debug brain configuration"""
    print("=== Brain Configuration Debug ===")
    
    # Check critical parameters
    issues = []
    
    # Neuron count validation
    neuron_count = config.getOr<int64_t>("neuron_count", 1000)
    if neuron_count <= 0:
        issues.append("Invalid neuron count: must be positive")
    elif neuron_count > 10000:
        issues.append("Large neuron count may cause performance issues")
    
    # Region validation
    region_count = config.getOr<int64_t>("region_count", 1)
    if region_count <= 0:
        issues.append("Invalid region count: must be positive")
    
    # Connection probability validation
    conn_prob = config.getOr<double>("connection_probability", 0.05)
    if conn_prob < 0.0 or conn_prob > 1.0:
        issues.append("Invalid connection probability: must be between 0 and 1")
    
    # Simulation timestep validation
    timestep = config.getOr<double>("simulation_timestep", 0.001)
    if timestep <= 0:
        issues.append("Invalid timestep: must be positive")
    elif timestep > 1.0:
        issues.append("Large timestep may cause simulation instability")
    
    # Display configuration
    print(f"Neuron count: {neuron_count}")
    print(f"Region count: {region_count}")
    print(f"Connection probability: {conn_prob}")
    print(f"Simulation timestep: {timestep}")
    
    if issues:
        print("\nConfiguration Issues Found:")
        for issue in issues:
            print(f"  - {issue}")
        return False
    else:
        print("\n✓ Configuration is valid")
        return True

def validateSimulationState(brain, world, agent):
    """Validate the current state of the simulation"""
    print("=== Simulation State Validation ===")
    
    checks = []
    
    # Brain health check
    if brain.isHealthy():
        checks.append("✓ Brain is in healthy state")
    else:
        checks.append("✗ Brain health check failed")
    
    # Memory usage check
    memory_stats = pynlm.getSystemMemoryUsage()
    print(f"Memory usage: {memory_stats['approx_memory_mb']:.1f} MB")
    
    # Activity level check
    firing_rate = brain.getAverageFiringRate()
    if firing_rate > 0.1:
        checks.append(f"✓ Brain is active (firing rate: {firing_rate:.2f} Hz)")
    else:
        checks.append(f"⚠ Low brain activity (firing rate: {firing_rate:.2f} Hz)")
    
    # World state check
    body = world.getAgentBody()
    if body.energy > 0:
        checks.append(f"✓ Agent has energy ({body.energy:.1f})")
    else:
        checks.append("⚠ Agent energy is low")
    
    # Agent systems check
    agent_status = agent.getSystemStatus()
    enabled_systems = sum(agent_status.values())
    checks.append(f"✓ Agent systems: {enabled_systems}/4 enabled")
    
    # Print all checks
    for check in checks:
        print(f"  {check}")
    
    return all("✓" in check for check in checks)

def benchmarkMemoryUsage():
    """Benchmark memory usage for different brain configurations"""
    print("=== Memory Usage Benchmark ===")
    
    configurations = [
        {"neurons": 100, "regions": 1, "name": "Small"},
        {"neurons": 1000, "regions": 1, "name": "Medium"},
        {"neurons": 5000, "regions": 2, "name": "Large"},
        {"neurons": 10000, "regions": 3, "name": "Extra Large"}
    ]
    
    results = []
    
    for config_info in configurations:
        print(f"\nTesting {config_info['name']} brain ({config_info['neurons']} neurons)...")
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", config_info['neurons'])
        config.set("region_count", config_info['regions'])
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        # Get statistics
        stats = {
            'name': config_info['name'],
            'neurons': config_info['neurons'],
            'time': end_time - start_time,
            'steps_per_second': 100 / (end_time - start_time),
            'final_neurons': brain.getTotalNeuronCount(),
            'final_spikes': brain.getTotalSpikeCount()
        }
        
        results.append(stats)
        print(f"  Time: {stats['time']:.3f}s, "
              f"Speed: {stats['steps_per_second']:.0f} steps/s, "
              f"Spikes: {stats['final_spikes']}")
    
    print("\n=== Benchmark Summary ===")
    for result in results:
        print(f"{result['name']:12s}: {result['steps_per_second']:8.0f} steps/s, "
              f"{result['time']*1000:6.1f}ms per 100 steps")
    
    return results

# Environment validation
print("=== Environment Validation ===")
env_checks = pynlm.checkEnvironment()
for check, status in env_checks.items():
    print(f"  {check}: {'✓' if status else '✗'}")
```

---

## System Validation and Health Checks

These tools help ensure your simulation runs smoothly and identify potential issues before they become problems.

---
