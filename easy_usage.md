# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**NEW in Enhanced Version:** This guide now includes both the simple API for beginners and advanced features for more complex simulations!

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
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
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
print("Total spikes:", brain.getTotalSpikeCount())
print("Neuromodulation level:", agent.getNeuromodulationLevel())
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
print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
```

## Enhanced Project 4: Learning with Advanced Features

```python
import pynlm
from pynlm import ConfigBuilder, create_educational_simulation, run_episode

# Create a sophisticated simulation with advanced features
config = (ConfigBuilder()
         .set_neuron_count(1000)
         .set_region_count(3)
         .set_connection_probability(0.15)
         .enable_stdp(True, 0.01, 0.012, 20.0)  # Enable STDP with custom parameters
         .enable_hebbian(True)
         .enable_structural_plasticity(True)
         .enable_curiosity(True)
         .enable_novelty(True)
         .enable_reward_modulation(True)
         .set_replay_interval(50)
         .set_development_interval(500)
         .set_simulation_timestep(0.001)
         .set_random_seed(42)
         .build())

# Create components
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)

# Run comprehensive learning simulation
stats = run_episode(brain, world, agent, 1000, verbose=True)

# Analyze results
print("\n=== Learning Analysis ===")
print(f"Episode completed: {stats['steps_completed']} steps")
print(f"Total spikes generated: {stats['total_spikes']}")
print(f"Average firing rate: {stats['final_spikes_per_step']:.1f} spikes/step")
print(f"Final neuromodulation level: {stats['final_neuromodulation']:.3f}")
print(f"Final curiosity level: {stats['final_curiosity']:.3f}")
print(f"Final novelty level: {stats['final_novelty']:.3f}")
print(f"Final prediction error: {stats['final_prediction_error']:.3f}")
print(f"Simulation efficiency: {stats['simulation_efficiency']:.4f}")

# Show action statistics
print("\n=== Action Statistics ===")
for action, count in stats['action_counts'].items():
    print(f"  {action}: {count} times")
```

## Enhanced Project 5: High-Level NLM API

```python
import pynlm

# Use the high-level NLM interface
nlm = pynlm.NLM()

# Setup with configuration
nlm.setup(world_width=25.0, world_height=25.0, enable_learning=True)

# Run simulation with progress reporting
stats = nlm.run(1000, verbose=True)

print(f"\n=== Simulation Results ===")
print(f"Simulation completed successfully!")
print(f"Efficiency score: {stats['efficiency_score']:.4f}")
print(f"Performance: {stats['spikes_per_second']:.1f} spikes/second")
print(f"Stability: {stats['avg_step_time_ms']:.1f} ms/step (avg)")

# Access detailed metrics
print(f"\n=== Detailed Metrics ===")
print(f"Total neurons in simulation: {nlm.brain.getTotalNeuronCount()}")
print(f"Total synapses: {nlm.brain.getTotalSynapseCount()}")
print(f"Active neurons at peak: {nlm.brain.getActiveNeuronCount()}")
print(f"Total spikes: {nlm.brain.getTotalSpikeCount()}")
print(f"Developmental stage: {nlm.agent.getDevelopmentalStage().name}")
```

## Enhanced Project 6: Performance Monitoring

```python
import pynlm
from pynlm import create_performance_monitor

# Create simulation with performance monitoring
brain, world, agent = create_educational_simulation()

# Setup performance monitoring
monitor = create_performance_monitor()

# Run simulation with performance tracking
for step in range(100):
    monitor.start_step()
    
    # Single simulation step
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # End step and record metrics
    monitor.end_step(brain, world)

# Get performance summary
perf_stats = monitor.get_summary()

print("\n=== Performance Analysis ===")
print(f"Steps completed: {perf_stats['steps_completed']}")
print(f"Average step time: {perf_stats['avg_step_time_ms']:.2f} ms")
print(f"Step time consistency (std): {perf_stats['std_step_time_ms']:.2f} ms")
print(f"Neurons per step: {perf_stats['avg_neurons']:.0f}")
print(f"Spike generation rate: {perf_stats['spikes_per_second']:.1f} spikes/sec")
print(f"Overall efficiency score: {perf_stats['efficiency_score']:.4f}")

if perf_stats['efficiency_score'] > 0.1:
    print("✅ Simulation shows good efficiency and stability")
elif perf_stats['efficiency_score'] > 0.05:
    print("⚠️  Simulation is functional but could be optimized")
else:
    print("❌ Simulation may need configuration adjustments")
```

## Enhanced Project 7: Configuration Builder

```python
import pynlm
from pynlm import ConfigBuilder

# Create custom configuration using builder pattern
config = (ConfigBuilder()
         .set_neuron_count(1500)  # Moderate complexity
         .set_region_count(4)     # Multiple regions for diversity
         .set_connection_probability(0.12)
         .enable_stdp(True, ltp_weight=0.015, ltd_weight=0.013, tau=25.0)
         .enable_hebbian(True)
         .enable_structural_plasticity(True)
         .enable_curiosity(True)
         .enable_novelty(True)
         .enable_reward_modulation(True)
         .set_replay_interval(30)
         .set_development_interval(300)
         .set_simulation_timestep(0.0005)  # Faster simulation
         .set_random_seed(12345)            # Reproducible results
         .build())

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Expected developmental time: ~{config.get('development_interval', 300) / 10:.0f} simulation steps")
```

## Pattern Library

### Pattern 1: Run a Simulation (Enhanced)

```python
def run_learning_episode(brain, world, agent, num_steps=500, learning_enabled=True):
    """Run a learning episode with optional learning subsystems."""
    
    if learning_enabled:
        # Enable all learning features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
    
    for step in range(num_steps):
        # Update environment
        world.update(0.1)
        
        # Sense environment
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Act in environment
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        if learning_enabled:
            agent.applyRewardModulation(result.reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}, "
                  f"Spikes={brain.getFiringNeuronCount()}")
    
    return brain, world, agent
```

### Pattern 2: Get Brain Stats (Enhanced)

```python
def analyze_brain_performance(brain, agent, world, steps=100):
    """Comprehensive brain performance analysis."""
    
    stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'development_stage': brain.getDevelopmentalStage(),
        'neuromodulation_level': agent.getNeuromodulationLevel(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError(),
        'spike_counts': [],
        'firing_rates': [],
        'action_preferences': {}
    }
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Record statistics
        stats['spike_counts'].append(brain.getFiringNeuronCount())
        stats['firing_rates'].append(brain.getAverageFiringRate())
        action_name = action.name if hasattr(action, 'name') else str(action)
        stats['action_preferences'][action_name] = stats['action_preferences'].get(action_name, 0) + 1
    
    # Calculate averages
    stats['avg_spikes'] = sum(stats['spike_counts']) / len(stats['spike_counts'])
    stats['avg_firing_rate'] = sum(stats['firing_rates']) / len(stats['firing_rates'])
    stats['most_common_action'] = max(stats['action_preferences'].items(), 
                                     key=lambda x: x[1])[0] if stats['action_preferences'] else None
    
    return stats
```

### Pattern 3: Enable Brain Features (Flexible)

```python
def configure_brain_for_task(brain, agent, task_type='exploration'):
    """
    Configure brain subsystems based on task requirements.
    
    Args:
        brain: Neural brain
        agent: Agent interface
        task_type: 'exploration', 'exploitation', 'navigation', 'learning'
    """
    # Reset all features
    agent.enable_reward_modulation(False)
    agent.enable_curiosity(False)
    agent.enable_structural_plasticity(False)
    agent.enable_development(False)
    
    # Enable based on task
    if task_type == 'exploration':
        agent.enable_curiosity(True)  # Drive exploration
        agent.enable_reward_modulation(True)
    elif task_type == 'exploitation':
        agent.enable_reward_modulation(True)  # Learn optimal behavior
        agent.enable_development(True)  # Refine capabilities
    elif task_type == 'navigation':
        agent.enable_curiosity(True)  # Discover environment
        agent.enable_structural_plasticity(True)  # Adapt to terrain
    elif task_type == 'learning':
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
```

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create brain | `brain = pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getTotalNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `action = agent.decodeMotorCommand()` |
| Run complete simulation | `run_episode(brain, world, agent, 1000)` |
| High-level control | `nlm = pynlm.NLM().setup().run(1000)` |
| Custom config | `config = ConfigBuilder().set_neuron_count(1000).build()` |

## Troubleshooting (Enhanced)

### "My brain isn't learning"**
- Did you call `agent.enableRewardModulation(True)`?
- Try increasing the number of steps
- Check `agent.getCuriosityLevel()` and `agent.getNoveltyLevel()`

### "The agent isn't exploring"**
- Did you call `agent.enableCuriosity(True)`?
- Check `agent.getNeuromodulationLevel()` for motivation
- Ensure `brain.step()` is being called

### "Everything is zero"**
- Brains need time to "warm up" - try more steps
- Check if sensory input is connected: `world.getSensoryPercept()`
- Ensure `agent.initialize(world)` was called

### "Performance is slow"**
- Consider disabling learning features with `enable_reward_modulation(False)`
- Use `create_simple_config()` for faster simulations
- Monitor with `PerformanceMonitor()`

### "Memory issues"**
- Use `create_research_config()` for balanced performance
- Enable checkpointing: `brain.save("checkpoint.bin")`
- Monitor neuron count and spike rate

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more detailed technical information**
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations using `ConfigBuilder()`**
4. **Try the high-level `NLM` class for simplified simulation control**
5. **Monitor performance with `PerformanceMonitor()`**
6. **Create your own simulation patterns and share them!**

That's it! You're now ready to use NLM's enhanced Python interface. Enjoy building intelligent neural systems! 🚀

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
