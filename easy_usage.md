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

print("Agent simulation complete!")
```

---

## Advanced Topics

### Working with Checkpoints

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps
for i in range(100):
    brain.step(i)

# Save checkpoint
brain.save("my_checkpoint.bin")
print("Checkpoint saved!")

# Reset and reload
brain.reset()
brain.initialize()
brain.load("my_checkpoint.bin")
print("Checkpoint loaded!")

# Verify state
print(f"Neurons after reload: {brain.getTotalNeuronCount()}")
```

### Performance Monitoring

```python
import pynlm

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation and monitor
print("Starting performance monitoring...")
for step in range(500):
    brain.step(step)
    
    if step % 100 == 0:
        # Get performance stats
        stats = brain.get_performance_stats()
        metrics = brain.get_metrics()
        
        print(f"Step {step}:")
        print(f"  Performance: {stats['neuron_count']} neurons, "
              f"{stats['connection_probability']:.2f} conn_prob")
        print(f"  Metrics: {metrics['total_spikes']} spikes, "
              f"EI ratio: {metrics['e_i_ratio']:.3f}")
```

### Memory Analysis

```python
import pynlm

# Create larger brain for memory analysis
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # More neurons = more memory usage
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Run simulation
brain.run_batch_simulation(300, 0.001)

# Get comprehensive metrics
metrics = brain.get_metrics()
performance = brain.get_performance_stats()

print("Memory analysis:")
print(f"  Total neurons: {metrics['total_neurons']}")
print(f"  Total synapses: {metrics['total_synapses']}")
print(f"  Active neurons: {metrics['firing_neurons']}")
print(f"  Total spikes: {metrics['total_spikes']}")
print(f"  Average firing rate: {metrics['avg_firing_rate']:.3f}")
print(f"  E/I ratio: {metrics['e_i_ratio']:.3f}")
print(f"  Development stage: {metrics['development_stage']}")
```

---

## Advanced Configuration Examples

### Custom Configuration Setup

```python
import pynlm

def create_experimental_brain(neuron_count=1000, plasticity_enabled=True):
    """Create a brain with experimental settings"""
    
    config = pynlm.createDefaultConfig()
    
    # Basic configuration
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.connection_probability", 0.1)
    
    # Plasticity settings
    config.set("plasticity.stdp.enable", plasticity_enabled)
    config.set("plasticity.hebbian.enable", plasticity_enabled)
    config.set("plasticity.structural.enable", plasticity_enabled)
    
    # Neuromodulation
    config.set("neuromod.dopamine_baseline", 0.1)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Reward settings
    config.set("reward.discount_factor", 0.95)
    
    return config

# Create experimental brains
experimental_config_1 = create_experimental_brain(1000, True)
experimental_config_2 = create_experimental_brain(2000, False)

brain1 = pynlm.createBrain(experimental_config_1)
brain1.initialize()

brain2 = pynlm.createBrain(experimental_config_2)
brain2.initialize()

print("Created two experimental brains with different configurations")
```

### Configuration Comparison

```python
import pynlm

def compare_configurations(configurations, steps=200):
    """Compare different brain configurations"""
    
    results = {}
    
    for name, config in configurations.items():
        print(f"Testing configuration: {name}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        brain.run_batch_simulation(steps, 0.001)
        
        # Collect metrics
        metrics = brain.get_metrics()
        performance = brain.get_performance_stats()
        
        results[name] = {
            'metrics': metrics,
            'performance': performance,
            'brain': brain
        }
        
        print(f"  Result: {metrics['total_spikes']} spikes, "
              f"EI ratio: {metrics['e_i_ratio']:.3f}")
    
    return results

# Define different configurations
configs = {
    "standard": pynlm.createDefaultConfig(),
    "high_plasticity": lambda: {
        config = pynlm.createDefaultConfig()
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        return config
    },
    "low_plasticity": lambda: {
        config = pynlm.createDefaultConfig()
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", False)
        config.set("plasticity.structural.enable", False)
        return config
    }
}

# Run comparison
comparison_results = compare_configurations(configs, 300)
```

### Developmental Stages Analysis

```python
import pynlm

def simulate_development_stages(duration_steps=500):
    """Simulate brain development across different stages"""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for world interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("Starting developmental simulation...")
    
    developmental_data = []
    
    for step in range(duration_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Agent decisions
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect developmental data
        if step % 50 == 0:
            stage = agent.getDevelopmentalStage()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            neuromod = agent.getNeuromodulationLevel()
            
            data_point = {
                'step': step,
                'developmental_stage': stage,
                'curiosity_level': curiosity,
                'novelty_level': novelty,
                'neuromodulation_level': neuromod,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate()
            }
            
            developmental_data.append(data_point)
            
            print(f"Step {step}: Stage {stage}, "
                  f"Curiosity: {curiosity:.3f}, "
                  f"Novelty: {novelty:.3f}, "
                  f"Spikes: {brain.getTotalSpikeCount()}")
    
    return developmental_data

# Run developmental simulation
development_results = simulate_development_stages(500)

# Analyze development patterns
print("\nDevelopment Analysis:")
for data in development_results:
    print(f"Step {data['step']}: "
          f"Stage {data['developmental_stage']}, "
          f"Curiosity: {data['curiosity_level']:.3f}, "
          f"Novelty: {data['novelty_level']:.3f}")
```

---

## Performance Optimization Examples

### Efficient Batch Processing

```python
import pynlm

def optimize_batch_processing(steps_per_batch=1000, num_batches=10):
    """Demonstrate efficient batch processing techniques"""
    
    all_results = []
    total_time = 0
    
    print(f"Running {num_batches} batches of {steps_per_batch} steps each...")
    
    for batch in range(num_batches):
        batch_start = time.time()
        
        # Create fresh brain for each batch
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)  # Keep size constant
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Use efficient batch processing
        brain.run_batch_simulation(steps_per_batch, 0.001)
        
        # Collect metrics
        metrics = brain.get_metrics()
        batch_time = time.time() - batch_start
        
        all_results.append({
            'batch': batch + 1,
            'metrics': metrics,
            'batch_time': batch_time,
            'steps_per_second': steps_per_batch / batch_time
        })
        
        total_time += batch_time
        
        print(f"  Batch {batch + 1}: {metrics['total_spikes']} spikes, "
              f"{batch_time:.2f}s ({steps_per_batch/batch_time:.0f} steps/sec)")
    
    # Summary statistics
    avg_steps_per_second = sum(r['steps_per_second'] for r in all_results) / len(all_results)
    
    print(f"\nBatch Processing Summary:")
    print(f"  Total time: {total_time:.2f}s")
    print(f"  Average speed: {avg_steps_per_second:.0f} steps/second")
    print(f"  Total spikes: {sum(r['metrics']['total_spikes'] for r in all_results)}")
    
    return all_results

# Run optimized batch processing
performance_results = optimize_batch_processing(1000, 5)
```

### Memory-Efficient Processing

```python
import pynlm

def memory_efficient_simulation(num_neurons=500, steps_per_simulation=200, 
                                num_simulations=8):
    """Run multiple simulations with memory-efficient approach"""
    
    simulation_results = []
    
    print(f"Running {num_simulations} memory-efficient simulations...")
    print(f"Each simulation: {num_neurons} neurons, {steps_per_simulation} steps")
    
    for sim in range(num_simulations):
        print(f"\nSimulation {sim + 1}/{num_simulations}")
        
        # Create configuration with memory limits
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", num_neurons)
        config.set("brain.connection_probability", 0.08)  # Lower density for memory
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        initial_memory = get_memory_usage() if 'get_memory_usage' in globals() else 0
        brain.run_batch_simulation(steps_per_simulation, 0.001)
        final_memory = get_memory_usage() if 'get_memory_usage' else 0
        
        # Collect metrics
        metrics = brain.get_metrics()
        memory_increase = final_memory - initial_memory
        
        sim_result = {
            'simulation_id': sim + 1,
            'neuron_count': num_neurons,
            'metrics': metrics,
            'memory_increase_kb': memory_increase,
            'efficiency_score': metrics['total_spikes'] / max(memory_increase, 1)
        }
        
        simulation_results.append(sim_result)
        
        print(f"  Spikes: {metrics['total_spikes']}")
        print(f"  Memory increase: {memory_increase:.1f} KB")
        print(f"  Efficiency: {sim_result['efficiency_score']:.2f} spikes/KB")
    
    # Compare simulation efficiency
    best_efficiency = max(r['efficiency_score'] for r in simulation_results)
    worst_efficiency = min(r['efficiency_score'] for r in simulation_results)
    
    print(f"\nMemory Efficiency Comparison:")
    print(f"  Best efficiency: {best_efficiency:.2f} spikes/KB")
    print(f"  Worst efficiency: {worst_efficiency:.2f} spikes/KB")
    print(f"  Efficiency ratio: {best_efficiency/worst_efficiency:.1f}x")
    
    return simulation_results

# Run memory-efficient simulations
memory_results = memory_efficient_simulation(500, 200, 6)
```

### Real-time Monitoring

```python
import pynlm
import time

def real_time_monitoring(duration_seconds=30, monitoring_interval=5):
    """Monitor simulation in real-time with periodic status updates"""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Starting real-time monitoring for {duration_seconds} seconds...")
    print("Monitoring interval:", monitoring_interval, "seconds")
    print("\nReal-time status:")
    print("Step | Spikes | Firing | Stage | Curiosity | Time (s)")
    print("-----|--------|--------|-------|----------|----------")
    
    start_time = time.time()
    step = 0
    
    while time.time() - start_time < duration_seconds:
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Agent decisions
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        step += 1
        
        # Log status periodically
        if step % monitoring_interval == 0:
            current_time = time.time() - start_time
            stage = agent.getDevelopmentalStage()
            curiosity = agent.getCuriosityLevel()
            
            print(f"{step:4d} | {brain.getTotalSpikeCount():6d} | "
                  f"{brain.getFiringNeuronCount():6d} | "
                  f"{stage:5d} | {curiosity:8.3f} | {current_time:7.1f}")
    
    # Final status
    final_time = time.time() - start_time
    print(f"\nSimulation completed in {final_time:.1f} seconds")
    print(f"Total steps: {step}")
    print(f"Steps per second: {step/final_time:.1f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final stage: {agent.getDevelopmentalStage()}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")

# Run real-time monitoring
real_time_monitoring(30, 5)
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Run batch efficiently | `brain.run_batch_simulation(steps, timestep)` |
| Get metrics | `brain.get_metrics()` |
| Get performance stats | `brain.get_performance_stats()` |
| Save checkpoint | `brain.save("filename.bin")` |
| Load checkpoint | `brain.load("filename.bin")` |
| Create config | `pynlm.createDefaultConfig()` |
| Set config | `config.set("key", value)` |
| Set multiple configs | `config.set_multiple({"key1": val1, "key2": val2})` |
| Enable features | `agent.enableRewardModulation(True)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that world is being updated

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Enable neuromodulation for better activity

**"Batch processing not working"**
- Make sure you're using `brain.run_batch_simulation()` for efficiency
- Check that timestep is appropriate for your use case
- Verify that brain is properly initialized

**"Memory usage too high"**
- Reduce number of neurons
- Lower connection probability
- Use checkpointing for long simulations

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM with advanced capabilities!
