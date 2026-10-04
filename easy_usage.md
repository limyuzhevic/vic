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

### Example 2: Enhanced Brain with Advanced Features

```python
import pynlm

# Create configuration with advanced settings
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)

# Create and initialize the enhanced brain
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced features
brain.enableAdvancedFeatures()

# Run batch processing for efficiency
brain.batchSteps(1000, 0.001)

# Get comprehensive statistics
stats = brain.getStatistics()
print("Brain Statistics:", stats)

print("Enhanced simulation complete!")
```

### Example 3: Complete Enhanced Agent with World

```python
import pynlm

# Create configuration with world settings
config = pynlm.createDefaultConfig()
config.set("world.width", 30)
config.set("world.height", 30)
config.set("world.vision_width", 10)
config.set("world.vision_height", 10)

# Create enhanced brain
brain = pynlm.createBrain(config)
brain.initialize()
brain.enableAdvancedFeatures()

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
world.setMaxEnergy(200.0)

# Create enhanced agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all subsystems for realistic simulation
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run enhanced simulation with performance monitoring
for step in range(2000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input with advanced features
    agent.processSensoryInput(percept)
    
    # Run brain step with batch processing
    brain.step(step)
    
    # Get action from enhanced agent
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get performance metrics periodically
    if step % 100 == 0:
        perf = agent.getPerformanceMetrics()
        print(f"Step {step}: Reward={perf['reward']:.2f}, "
              f"Curiosity={perf['curiosity']:.3f}, "
              f"Novelty={perf['novelty']:.3f}")

# Get final statistics
final_stats = brain.getStatistics()
print(f"\nSimulation Results:")
print(f"  Total Neurons: {final_stats['total_neurons']}")
print(f"  Total Synapses: {final_stats['total_synapses']}")
print(f"  Learning Efficiency: {final_stats['learning_efficiency']:.3f}")
print(f"  Memory Capacity: {final_stats['memory_capacity']:.1f}")

print(f"\nEnhanced agent simulation complete!")
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

### Pattern 1: Run a Simulation with Performance Monitoring

```python
def run_monitored_simulation(brain, world, agent, num_steps):
    import time
    start_time = time.time()
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print performance every 100 steps
        if step % 100 == 0:
            perf = agent.getPerformanceMetrics()
            print(f"Step {step}: "
                  f"Perf={perf['performance']:.2f}, "
                  f"Learning={perf['learning_rate']:.3f}")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

### Pattern 2: Get Comprehensive Brain Statistics

```python
def print_brain_detailed_stats(brain):
    stats = brain.getStatistics()
    print("=== Detailed Brain Statistics ===")
    print(f"Neural Network:")
    print(f"  Total Neurons: {stats['total_neurons']}")
    print(f"  Total Synapses: {stats['total_synapses']}")
    print(f"  Active Neurons: {stats['active_neurons']}")
    print(f"  Firing Rate: {stats['firing_rate']:.3f}")
    print(f"  Excitation/Inhibition Ratio: {stats['e_i_ratio']:.3f}")
    
    print(f"\nLearning Metrics:")
    print(f"  Learning Efficiency: {stats['learning_efficiency']:.3f}")
    print(f"  Plasticity Rate: {stats['plasticity_rate']:.3f}")
    print(f"  Memory Capacity: {stats['memory_capacity']:.1f}")
    print(f"  Prediction Accuracy: {stats['prediction_accuracy']:.3f}")
    print(f"  Generalization Ability: {stats['generalization_ability']:.3f}")
    
    print(f"\nDevelopment:")
    print(f"  Developmental Stage: {stats['developmental_stage']}")
    print(f"  Neural Maturation: {stats['neural_maturation']:.3f}")
    print(f"  Synaptic Pruning: {stats['synaptic_pruning']:.3f}")
```

### Pattern 3: Export/Import Results

```python
def save_experiment_results(brain, world, agent, experiment_name):
    # Export brain state
    brain.exportToCSV(f"{experiment_name}_brain.csv")
    brain.exportToJSON(f"{experiment_name}_brain.json")
    
    # Export agent performance
    perf = agent.getPerformanceMetrics()
    with open(f"{experiment_name}_performance.json", 'w') as f:
        json.dump(perf, f, indent=2)
    
    # Export world state
    world.exportState(f"{experiment_name}_world.json")
    
    print(f"Experiment results saved as {experiment_name}_*")

def load_experiment_results(brain, world, agent, experiment_name):
    # Load brain state
    brain.importFromCSV(f"{experiment_name}_brain.csv")
    
    # Load agent performance
    with open(f"{experiment_name}_performance.json", 'r') as f:
        perf = json.load(f)
    
    agent.updatePerformanceMetrics(perf)
    
    # Load world state
    world.importState(f"{experiment_name}_world.json")
    
    print(f"Experiment results loaded from {experiment_name}_*")
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

### `brain.batchSteps(num_steps, timestep)`
Run multiple steps efficiently with batch processing

### `brain.getStatistics()`
Get comprehensive brain statistics including learning metrics, memory capacity, and performance

### `brain.enableAdvancedFeatures()`
Enable advanced features like performance monitoring, analysis tools, and enhanced logging

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.getPerformanceMetrics()`
Get comprehensive performance metrics for the agent

### `world.getSensoryPercept()`
See what's in the environment

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `brain.exportToCSV(filepath)`
Export brain results as CSV file

### `brain.exportToJSON(filepath)`
Export brain results as JSON file

### `pynlm.measurePerformance(brain, steps)`
Measure simulation performance with detailed metrics

### `pynlm.analyzePlasticity(brain)`
Analyze learning dynamics and plasticity

### `pynlm.getDevelopmentalStageName(stage)`
Get human-readable developmental stage name

---

## Mini Projects

### Project 1: Learning Brain (Just Neurons)

```python
import pynlm

# Create learning brain with plasticity
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
brain.enableAdvancedFeatures()

# Run learning simulation
print("Running learning simulation...")
for i in range(500):
    brain.step(i)

# Analyze learning results
stats = brain.getStatistics()
learning_efficiency = stats['learning_efficiency']
print(f"Learning efficiency: {learning_efficiency:.3f}")
print(f"Plasticity rate: {stats['plasticity_rate']:.3f}")

print("Learning simulation complete!")
```

### Project 2: Agent Watching a World

```python
import pynlm

# Setup enhanced brain with world understanding
config = pynlm.createDefaultConfig()
config.set("world.width", 20)
config.set("world.height", 20)
config.set("world.vision_width", 8)
config.set("world.vision_height", 8)

brain = pynlm.createBrain(config)
brain.initialize()
brain.enableAdvancedFeatures()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 100 steps with performance monitoring
print("Watching world...")
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

# Analyze world observation
stats = brain.getStatistics()
print(f"World observation analysis:")
print(f"  Network activity: {stats['network_activity']:.3f}")
print(f"  Learning progress: {stats['learning_progress']:.3f}")

print("World watching complete!")
```

### Project 3: Complete Agent with Advanced Learning

```python
import pynlm

# Setup complete advanced agent system
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.12)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
brain.enableAdvancedFeatures()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
world.reset()
world.setMaxEnergy(300.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complex task with monitoring
print("Running complex task simulation...")
performance_history = []

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        perf = agent.getPerformanceMetrics()
        performance_history.append(perf)
        print(f"Step {step}: "
              f"Reward={perf['reward']:.2f}, "
              f"Curiosity={perf['curiosity']:.3f}, "
              f"Novelty={perf['novelty']:.3f}")

# Analyze final performance
final_perf = performance_history[-1]
print(f"\nFinal Performance:")
print(f"  Overall Performance: {final_perf['performance']:.2f}")
print(f"  Learning Rate: {final_perf['learning_rate']:.3f}")
print(f"  Memory Utilization: {final_perf['memory_utilization']:.1f}")
print(f"  Prediction Error: {final_perf['prediction_error']:.3f}")

# Export results
brain.exportToCSV("advanced_agent_results.csv")
brain.exportToJSON("advanced_agent_results.json")

print(f"\nAdvanced agent simulation complete! Results exported.")
```

---

## Quick Reference

### Brain Operations

| Operation | Code | Description |
|-----------|------|-------------|
| Create brain | `pynlm.createBrain(config)` | Create a new brain |
| Initialize | `brain.initialize()` | Start the brain |
| Make brain think | `brain.step(step)` | Run one simulation step |
| Batch process | `brain.batchSteps(steps, timestep)` | Run multiple steps efficiently |
| Get stats | `brain.getStatistics()` | Get comprehensive statistics |
| Save state | `brain.save(filepath)` | Save brain state |
| Load state | `brain.load(filepath)` | Load brain state |
| Enable advanced | `brain.enableAdvancedFeatures()` | Enable advanced features |
| Export CSV | `brain.exportToCSV(file)` | Export results as CSV |
| Export JSON | `brain.exportToJSON(file)` | Export results as JSON |

### Agent Operations

| Operation | Code | Description |
|-----------|------|-------------|
| Create agent | `pynlm.createAgentBrain(brain)` | Create agent interface |
| Initialize | `agent.initialize(world)` | Connect to world |
| Process input | `agent.processSensoryInput(percept)` | Handle sensory data |
| Get action | `agent.decodeMotorCommand()` | Decide what to do |
| Get performance | `agent.getPerformanceMetrics()` | Monitor agent performance |

### World Operations

| Operation | Code | Description |
|-----------|------|-------------|
| Create world | `pynlm.createSimpleWorld()` | Create environment |
| Configure | `world.configure(w, h, vw, vh)` | Set world dimensions |
| Reset | `world.reset()` | Start fresh |
| Update | `world.update(timestep)` | Advance time |
| Get percept | `world.getSensoryPercept()` | See what's in environment |

### Advanced Operations

| Operation | Code | Description |
|-----------|------|-------------|
| Export CSV | `brain.exportToCSV(file)` | Export results as CSV |
| Export JSON | `brain.exportToJSON(file)` | Export results as JSON |
| Import from CSV | `brain.importFromCSV(file)` | Load results from CSV |
| Measure performance | `pynlm.measurePerformance(brain, steps)` | Measure simulation performance |
| Analyze plasticity | `pynlm.analyzePlasticity(brain)` | Analyze learning dynamics |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if advanced features are enabled

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Enable structural plasticity for more dynamic behavior

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Enable curiosity and neuromodulation for more activity
- Check configuration settings

**"Performance is slow"**
- Use `brain.batchSteps()` for faster processing
- Reduce the number of neurons for testing
- Disable visualization for better performance

**"Memory usage is high"**
- Use `brain.exportToCSV()` to save memory
- Reduce the number of neurons
- Monitor performance with `pynlm.measurePerformance()`

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - Includes API reference and examples
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into system architecture
3. **Experiment with different configurations** - Try different parameters for interesting results
4. **Use the new export/import features** - Save and load your experiments
5. **Monitor performance with advanced tools** - Use `pynlm.measurePerformance()` for optimization

**Important Note:** The enhanced bindings now include advanced features like batch processing, performance monitoring, and comprehensive statistics that make the NLM system much more powerful and user-friendly!

That's it! You're now ready to use NLM with the enhanced, feature-rich API!
