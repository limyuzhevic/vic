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

```python
import py_nlm_enhanced as pynlm

# Setup
brain = pynlm.create_advanced_brain()
brain.initialize(enable_learning=True, enable_curiosity=True, enable_development=True)

world = pynlm.create_advanced_world(width=30, height=30)
world.configure(
    width=30, height=30,
    vision_width=10, vision_height=10,
    dynamic_objects_enabled=True,
    resource_map_enabled=True,
    weather_enabled=True
)
world.reset()
agent = pynlm.create_advanced_agent(brain, world)

# Run advanced simulation
results = pynlm.run_advanced_simulation(brain, world, agent, num_steps=500, mode='adaptive')

# Analyze results
print(f"Simulation completed with {results['final_step']} steps")
print(f"Total neurons: {results['total_neurons']}")
print(f"Total synapses: {results['total_synapses']}")
print(f"Total spikes: {results['total_spikes']}")
print(f"Performance metrics: {results['performance_metrics']}")

# Save results
pynlm.save_simulation_results(results, 'simulation_results.json')
print("Results saved to simulation_results.json")
```

### Advanced Simulation Features

The enhanced brain includes several advanced features you can enable:

```python
# Learning enabled brain
brain = pynlm.create_advanced_brain()
brain.initialize(enable_learning=True)

# Curiosity-driven exploration
brain = pynlm.create_advanced_brain()
brain.initialize(enable_curiosity=True)

# Full adaptive brain with learning, development, and curiosity
brain = pynlm.create_advanced_brain()
brain.initialize(
    enable_learning=True,
    enable_development=True,
    enable_curiosity=True,
    enable_reward=True
)
```

### Feature Management

Dynamically enable/disable features during simulation:

```python
# Enable curiosity after initialization
brain.enable_feature('curiosity')

# Disable learning later
brain.disable_feature('learning')
```

### Event Callbacks

Add callbacks to monitor brain activity:

```python
def on_step(brain, step):
    print(f"Step {step}: {brain.get_firing_neuron_count()} neurons firing")

brain.add_step_callback(on_step)
```

### Save and Load Brain State

Checkpoint your simulation:

```python
# Save brain state
brain.export_brain_state('checkpoint.json')

# Restore later
brain.import_brain_state('checkpoint.json')
```

---

## Advanced API Reference

### EnhancedBrain Class

The `AdvancedBrain` class provides enhanced control over neural simulation:

- **Configuration Options**: Extensive configuration through dictionary
- **Feature Enable/Disable**: Dynamically enable learning, development, curiosity, and reward features
- **Event System**: Add callbacks for step monitoring and learning events
- **State Management**: Save/load brain state for checkpointing
- **Performance Monitoring**: Track statistics like firing rates, learning events, exploration scores

### EnhancedWorld Class

The `AdvancedWorld` class provides enhanced world simulation:

- **Dynamic Objects**: Simulate moving entities in the environment
- **Resource Mapping**: Track and detect resources in the environment
- **Weather System**: Weather conditions affect agent behavior
- **Enhanced Perception**: Richer sensory input including world properties

### AdvancedAgent Class

The `AdvancedAgent` class provides sophisticated agent behavior:

- **Memory System**: Short-term, long-term, episodic, and semantic memory
- **Planning System**: Goal-oriented planning and decision making
- **Decision System**: Advanced decision making with learned rules
- **Motivation System**: Intrinsic motivation based on curiosity and competence

---

## Complete Advanced Example: Learning Agent in Dynamic World

This example demonstrates a complete advanced agent simulation:

```python
import py_nlm_enhanced as pynlm
import numpy as np

def run_advanced_agent_simulation():
    """Run a complete advanced agent simulation with learning and adaptation"""
    
    # 1. Create advanced brain with learning capabilities
    brain_config = {
        'brain': {
            'neuron_count': 1500,
            'region_count': 2,
            'connection_probability': 0.08,
            'v_thresh': -50.0,
            'v_rest': -70.0,
            'tau_mem': 20.0
        },
        'plasticity': {
            'stdp': {'enable': True, 'learning_rate': 0.001},
            'hebbian': {'enable': True, 'learning_rate': 0.001},
            'structural': {'enable': True, 'synaptogenesis_rate': 0.00005}
        }
    }
    
    brain = pynlm.create_advanced_brain(config=brain_config, name="LearningAgent")
    
    # 2. Create enhanced world with resources and dynamic objects
    world = pynlm.create_advanced_world(width=50, height=50)
    world.configure(
        width=50, height=50,
        vision_width=12, vision_height=12,
        dynamic_objects_enabled=True,
        resource_map_enabled=True,
        weather_enabled=True
    )
    
    # 3. Create advanced agent
    agent = pynlm.create_advanced_agent(brain, world)
    agent.setGoal("explore and learn environment")
    
    # 4. Initialize with advanced features
    brain.initialize(
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True,
        enable_reward=True
    )
    
    # 5. Simulation loop with advanced processing
    num_steps = 1000
    performance_data = []
    
    for step in range(num_steps):
        # Update world dynamics
        world.update(world.timestep)
        
        # Get enhanced sensory input
        percept = world.get_sensory_percept()
        
        # Process with advanced cognitive system
        agent.processSensoryInput(percept)
        
        # Run neural simulation
        brain.step(step)
        
        # Make decision using advanced agent
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward-based learning
        reward = percept.get('internal', [0])[0] if percept.get('internal') else 0.0
        prediction_error = np.random.normal(0, 0.1)  # Simulated prediction error
        agent.applyRewardModulation(reward, prediction_error)
        
        # Update developmental processes
        agent.updateDevelopment(world.timestep)
        
        # Record performance metrics
        performance_data.append({
            'step': step,
            'firing_neurons': brain.get_firing_neuron_count(),
            'total_spikes': brain.get_total_spike_count(),
            'curiosity': agent.get_curiosity_level(),
            'novelty': agent.get_novelty_level(),
            'prediction_error': agent.getPredictionError(),
            'reward': reward
        })
        
        # Print progress with advanced metrics
        if step % 100 == 0:
            curiosity = agent.get_curiosity_level()
            novelty = agent.get_novelty_level()
            print(f"Step {step}:")
            print(f"  - Neurons firing: {brain.get_firing_neuron_count()}")
            print(f"  - Total spikes: {brain.get_total_spike_count()}")
            print(f"  - Curiosity level: {curiosity:.3f}")
            print(f"  - Novelty level: {novelty:.3f}")
            print(f"  - Developmental stage: {agent.getDevelopmentalStage().name}")
            print(f"  - Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    
    # 6. Create final results
    results = {
        'final_step': num_steps,
        'performance_data': performance_data,
        'final_metrics': {
            'total_neurons': brain.get_total_neuron_count(),
            'total_synapses': brain.get_total_synapse_count(),
            'total_spikes': brain.get_total_spike_count(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'learning_events': brain.stats['learning_events'],
            'exploration_score': brain.stats['exploration_score'],
            'novelty_score': brain.stats['novelty_score']
        },
        'agent_performance': agent.getPerformanceMetrics(),
        'brain_state_summary': brain.get_state_summary()
    }
    
    return results

# Run the simulation
if __name__ == "__main__":
    print("Starting advanced agent simulation...")
    results = run_advanced_agent_simulation()
    
    # Save results
    pynlm.save_simulation_results(results, 'advanced_simulation_results.json')
    
    # Print summary
    print("\nSimulation Complete!")
    print(f"Total steps: {results['final_step']}")
    print(f"Final neurons firing: {results['final_metrics']['total_neurons']}")
    print(f"Final synapses: {results['final_metrics']['total_synapses']}")
    print(f"Total learning events: {results['final_metrics']['learning_events']}")
    print(f"Final curiosity level: {results['final_metrics']['exploration_score']:.3f}")
    print(f"Final novelty level: {results['final_metrics']['novelty_score']:.3f}")
    
    print("\nResults saved to: advanced_simulation_results.json")
    print("\nAdvanced simulation completed successfully!")
```

---

## Performance Monitoring

The enhanced brain provides detailed performance monitoring:

- **Learning Metrics**: Track learning events, weight changes, memory formation
- **Curiosity Metrics**: Measure exploration and novelty detection
- **Development Metrics**: Track developmental stage and maturation
- **Efficiency Metrics**: Monitor performance and resource usage

Example performance monitoring:

```python
brain = pynlm.create_advanced_brain()
brain.add_step_callback(lambda b, s: print(f"Step {s}: {b.get_firing_neuron_count()} neurons"))

# Get comprehensive performance summary
performance = brain.get_state_summary()
print(f"Learning events: {performance['learning_events']}")
print(f"Exploration score: {performance['exploration_score']}")
```

---

## Data Export and Analysis

Export simulation results for analysis:

```python
# Save raw performance data
pynlm.save_simulation_results(results, 'experiment_data.json')

# Load for analysis
loaded_results = pynlm.load_simulation_results('experiment_data.json')

# Analyze trends
steps = [d['step'] for d in loaded_results['performance_data']]
firing_rates = [d['firing_neurons'] / loaded_results['final_metrics']['total_neurons'] 
                for d in loaded_results['performance_data']]

import matplotlib.pyplot as plt
plt.plot(steps, firing_rates)
plt.title('Firing Rate Over Time')
plt.xlabel('Step')
plt.ylabel('Firing Rate')
plt.show()
```

---

## Next Steps

When you're comfortable with the basic features:

1. **Explore advanced configurations**: Use the configuration dictionary to fine-tune brain parameters
2. **Implement custom callbacks**: Create your own event handlers for specialized monitoring
3. **Add custom memory systems**: Extend the agent with specialized memory architectures
4. **Create world domains**: Implement custom world environments for specific applications
5. **Study performance patterns**: Analyze the rich performance data for insights
6. **Read architecture documentation**: Understand the underlying C++ implementation
7. **Experiment with different modes**: Try learning, exploration, and adaptive modes
8. **Combine features**: Mix and match different enabled features for unique behaviors

That's it! You're now ready to use the enhanced NLM with advanced features for sophisticated neural simulations. The enhanced API provides powerful tools for creating intelligent agents that learn, develop, and adapt to their environments.

To get started with advanced features, check out the `py_nlm_enhanced.py` module for the complete API documentation and examples.
