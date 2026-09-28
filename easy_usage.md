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

## Enhanced Patterns with New Convenience Functions

### Pattern 1: Run a Complete Agent Simulation (NEW!)

```python
def run_complete_agent_simulation(brain, world, agent, num_steps):
    """Run a complete simulation using the enhanced convenience function."""
    total_spikes = pynlm.run_agent_simulation(brain, world, agent, num_steps, verbose=True)
    return total_spikes

# Create and run a complete agent simulation in just 2 lines:
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run the simulation using the convenience function
total_spikes = run_complete_agent_simulation(brain, world, agent, 1000)
print(f"Simulation complete! Total spikes: {total_spikes}")
```

### Pattern 2: Learning Experiment (NEW!)

```python
def run_learning_experiment():
    """Run a learning experiment using the convenience function."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run learning experiment
    initial, final, spikes, change = pynlm.create_learning_experiment(brain, agent, world, 500)
    
    print(f"Learning Experiment Results:")
    print(f"  Initial mean weight: {initial:.4f}")
    print(f"  Final mean weight: {final:.4f}")
    print(f"  Weight change: {change:.4f}")
    print(f"  Total spikes: {spikes}")
    print(f"  Learning occurred: {abs(change) > 0.01}")

run_learning_experiment()
```

### Pattern 3: Brain Statistics and Monitoring (ENHANCED)

```python
def monitor_brain_health(brain, duration_steps=1000):
    """Monitor brain health and collect statistics."""
    # Get comprehensive stats
    stats = pynlm.get_brain_stats(brain)
    
    print("Brain Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Monitor over time
    history = pynlm.monitor_brain_health(brain, duration_steps, update_interval=100.0)
    
    print(f"\nCollected {len(history)} monitoring snapshots")
    return history

# Usage:
stats_history = monitor_brain_health(brain, duration_steps=1000)
```

### Pattern 4: Safe Save/Load with Validation (NEW!)

```python
def save_and_load_brain(brain, filepath):
    """Demonstrate safe save/load with validation."""
    # Save with backup and validation
    print("Saving brain with backup...")
    pynlm.save_brain_with_validation(brain, filepath, create_backup=True)
    print(f"Brain saved to {filepath}")
    
    # Load with validation
    print("Loading brain...")
    loaded_successfully = pynlm.load_brain_with_validation(brain, filepath)
    if loaded_successfully:
        print("Brain loaded successfully!")
    else:
        print("Failed to load brain")
        return False
    
    return True
```

### Pattern 5: Configuration Management (ENHANCED)

```python
def demonstrate_configurations():
    """Demonstrate different configuration options."""
    # Create different types of configurations
    default_config = pynlm.createDefaultConfig()
    
    brain_only_config = pynlm.create_advanced_config("brain_only")
    high_perf_config = pynlm.create_advanced_config("high_performance")
    dev_config = pynlm.create_advanced_config("developmental")
    
    print("Configuration validation:")
    for name, config in [("default", default_config), 
                         ("brain_only", brain_only_config),
                         ("high_performance", high_perf_config),
                         ("developmental", dev_config)]:
        warnings = pynlm.validate_config(config)
        print(f"  {name}: {len(warnings)} warnings")
        for warning in warnings:
            print(f"    - {warning}")
```

### Pattern 6: Quick Testing Setup (NEW!)

```python
def quick_test_setup():
    """Quick setup for testing and development."""
    config, brain = pynlm.quick_test_setup()
    
    print("Quick test setup complete:")
    print(f"  Config neurons: {config.getOr('brain.neuron_count', 1000)}")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  Brain synapses: {brain.getTotalSynapseCount()}")
    
    # Run a quick simulation
    for i in range(100):
        brain.step(i)
    
    print(f"  Total spikes after 100 steps: {brain.getTotalSpikeCount()}")
    return config, brain

# Usage:
config, brain = quick_test_setup()
```

### Pattern 7: Learning Demonstration (NEW!)

```python
def learning_demo():
    """Run a complete learning demonstration."""
    brain, final_weight, synapse_count = pynlm.run_learning_demo()
    
    print("Learning Demo Results:")
    print(f"  Final mean synaptic weight: {final_weight:.4f}")
    print(f"  Number of synapses: {synapse_count}")
    print(f"  Learning {'detected' if final_weight != 0.0 else 'not detected'}")
    print(f"  Brain spike count: {brain.getTotalSpikeCount()}")
    print(f"  Brain firing neurons: {brain.getFiringNeuronCount()}")

learning_demo()
```

### Pattern 8: Enhanced Basic Usage

```python
# Enhanced version of the basic examples with validation

# Example 1: Just neurons with error handling
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
if brain.getTotalNeuronCount() > 0:
    brain.initialize()
    for i in range(100):
        brain.step(i)
    print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
else:
    print("Error: Failed to create brain")

# Example 2: World interaction with monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run with monitoring
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation finished! Firing neurons: {brain.getFiringNeuronCount()}")
```

### Pattern 9: Enable All Advanced Features

```python
def advanced_brain_simulation():
    """Create a brain with all advanced features enabled."""
    config = pynlm.create_advanced_config("high_performance")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run with advanced monitoring
    print("Running advanced simulation with all features enabled...")
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            stats = pynlm.get_brain_stats(brain)
            print(f"  Step {step}: {stats['firing_neurons']} firing neurons, "
                  f"{stats['total_spikes']} total spikes, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("Advanced simulation complete!")

advanced_brain_simulation()
```

### Pattern 10: Error Handling and Robust Usage

```python
def robust_simulation():
    """Robust simulation with comprehensive error handling."""
    try:
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(500):
            try:
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Continue simulation despite errors
                continue
        
        print(f"Robust simulation completed successfully!")
        return brain.getTotalSpikeCount()
        
    except Exception as e:
        print(f"Fatal error in robust simulation: {e}")
        return None

# Usage:
spike_count = robust_simulation()
if spike_count is not None:
    print(f"Simulation generated {spike_count} total spikes")
```

### Pattern 11: Data Collection and Analysis

```python
def collect_and_analyze_data():
    """Collect and analyze brain data for research."""
    import json
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Collect data over time
    data_points = []
    for step in range(1000):
        brain.step(step)
        
        if step % 10 == 0:
            stats = pynlm.get_brain_stats(brain)
            stats["step"] = step
            data_points.append(stats)
    
    # Analyze collected data
    spike_analysis = {
        "max_spikes": max(dp["total_spikes"] for dp in data_points),
        "avg_firing_rate": sum(dp["avg_firing_rate"] for dp in data_points) / len(data_points),
        "stability": "stable" if len(set(dp["firing_neurons"] for dp in data_points)) < 100 else "unstable"
    }
    
    # Save data for later analysis
    with open("brain_data.json", "w") as f:
        json.dump({
            "data_points": data_points,
            "analysis": spike_analysis,
            "config": "default_1000"
        }, f, indent=2)
    
    print(f"Data collected and saved. Analysis: {spike_analysis}")
    return data_points

# Usage:
collected_data = collect_and_analyze_data()
```

### Pattern 12: Comparative Analysis

```python
def compare_configurations():
    """Compare different configurations."""
    configs_to_test = ["default", "brain_only", "high_performance", "developmental"]
    
    results = {}
    
    for config_name in configs_to_test:
        print(f"\nTesting configuration: {config_name}")
        config = pynlm.create_advanced_config(config_name)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run quick simulation
        for step in range(100):
            brain.step(step)
        
        final_stats = pynlm.get_brain_stats(brain)
        results[config_name] = final_stats
        
        print(f"  Neurons: {final_stats['total_neurons']}")
        print(f"  Synapses: {final_stats['total_synapses']}")
        print(f"  Total spikes: {final_stats['total_spikes']}")
        print(f"  Development stage: {final_stats['development_stage']}")
    
    # Find best performer
    best_config = max(results.keys(), 
                     key=lambda x: results[x]['total_spikes'] / results[x]['total_neurons'])
    
    print(f"\nBest performing configuration: {best_config}")
    return results

# Usage:
comparison_results = compare_configurations()
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
