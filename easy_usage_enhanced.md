# NLM - Enhanced Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project with Python bindings for neural simulation. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other  
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

**That's it! NLM simulates all of this.**

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in  
3. **Agent** - The bridge connecting brain to world

> **NEW in Enhanced Version:** Use factory functions for common scenarios, context managers for safe resource management, and fluent APIs for cleaner code!

---

## Quick Start (Copy & Paste) - NEW ENHANCED VERSION

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain (ENHANCED: Use factory functions)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World - ENHANCED

```python
import pynlm

# Create everything (ENHANCED: Use factory functions)
brain = pynlm.createBrain(pynlm.createDevelopmentConfig())
brain.initialize()

world = pynlm.createDefaultWorld()  # NEW: Factory function
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning (NEW: Convenience methods)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

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

### Example 3: Using Context Managers - NEW!

```python
import pynlm

# Create components
brain = pynlm.createBrain(pynlm.createResearchConfig(200))
world = pynlm.createDefaultWorld()
agent = pynlm.createAgentBrain(brain)

# Use context managers for safe operations (NEW: Context managers)
with pynlm.brain_context(brain) as brain_ctx:
    with pynlm.world_context(world) as world_ctx:
        with pynlm.agent_context(agent) as agent_ctx:
            world_ctx.configure(20, 20, 8, 8)
            world_ctx.reset()
            agent_ctx.initialize()
            
            # Run simulation
            for i in range(100):
                world_ctx.update(0.1)
                agent_ctx.processSensoryInput(world_ctx.get_sensory_percept())
                brain_ctx.step(i)
                action = agent_ctx.decode_motor_command()
                world_ctx.apply_motor_command(action, world_ctx.get_simulation_time())
                
                if i % 20 == 0:
                    print(f"Step {i}: Firing rate: {brain_ctx.get_average_firing_rate():.2f}")
```

### Example 4: Complete Agent Pattern - ENHANCED

```python
def run_complete_agent_simulation(brain_size=500, steps=1000):
    """Enhanced complete agent simulation with learning"""
    
    # Create components using factory functions
    config = pynlm.createResearchConfig(brain_size, 0.01)
    brain = pynlm.createBrain(config)
    
    world = pynlm.createDefaultWorld(30, 30, 12, 12)
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize everything
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enable_all_systems()  # NEW: Convenience method
    
    print(f"Starting simulation with {brain_size} neurons for {steps} steps...")
    
    # Run simulation
    for step in range(steps):
        # Update environment
        world.update(0.1)
        
        # Get perception and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress periodically
        if step % 200 == 0:
            curiosity = agent.getCuriosityLevel()
            firing_rate = brain.getAverageFiringRate()
            print(f"Step {step}: Curiosity={curiosity:.3f}, "
                  f"Firing Rate={firing_rate:.2f} Hz")
    
    # Print final statistics
    print(f"\n=== Simulation Complete ===")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'total_spikes': brain.getTotalSpikeCount(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_firing_rate': brain.getAverageFiringRate()
    }

# Run the simulation
simulation_results = run_complete_agent_simulation(300, 800)
```

### Example 5: Fluent API - NEW!

```python
# Use fluent API for cleaner configuration (NEW: Fluent APIs)
config = (pynlm.fluent_config()
         .with_neuron_count(500)
         .with_synapse_density(0.08)
         .with_learning(True)
         .with_dev_stages(True)
         .build())

brain = pynlm.createBrain(config)
brain.initialize()

# Chain brain operations
(brain = pynlm.createBrain(pynlm.createDevelopmentConfig())
     .initialize()
     .step(0)
     .step(1)
     .step(2))
```

### Example 6: Performance Monitoring - NEW!

```python
import pynlm

# Create brain and monitor performance
brain = pynlm.createBrain(pynlm.createProductionConfig(1000))

# Monitor performance
monitor = pynlm.performance_monitor(brain)

for i in range(500):
    monitor.step()
    
    if i % 100 == 0:
        stats = monitor.get_stats()
        print(f"Progress: {i} steps, "
              f"Throughput: {stats['steps_per_second']:.2f} steps/sec, "
              f"Firing rate: {stats['average_firing_rate']:.2f} Hz")
```

### Example 7: Learning Progress - NEW!

```python
# Create complete setup for learning analysis
brain = pynlm.createBrain(pynlm.createResearchConfig(400))
world = pynlm.createDefaultWorld(25, 25, 10, 10)
agent = pynlm.createAgentBrain(brain)

agent.initialize(world)
agent.enable_all_systems()

# Run learning simulation
for episode in range(5):
    print(f"\n=== Learning Episode {episode + 1} ===")
    
    world.reset()
    agent.reset()
    
    # Track learning metrics
    rewards = []
    firing_rates = []
    curiosity_levels = []
    
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward calculation
        reward = (agent.getCuriosityLevel() * 0.3 + 
                 brain.getAverageFiringRate() / 100.0 * 0.7)
        agent.applyRewardModulation(reward, 0.0)
        
        rewards.append(reward)
        firing_rates.append(brain.getAverageFiringRate())
        curiosity_levels.append(agent.getCuriosityLevel())
    
    # Episode summary
    avg_reward = sum(rewards) / len(rewards)
    avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
    
    print(f"  Average reward: {avg_reward:.3f}")
    print(f"  Average curiosity: {avg_curiosity:.3f}")
    print(f"  Final firing rate: {firing_rates[-1]:.2f} Hz")
```

### Example 8: Advanced Configuration Templates - NEW!

```python
# Use different configuration templates for different applications

# 1. Reinforcement Learning Template
rl_config = pynlm.create_reinforcement_learning_config(500)
rl_brain = pynlm.createBrain(rl_config)

# 2. Supervised Learning Template  
supervised_config = pynlm.create_supervised_learning_config(200)
supervised_brain = pynlm.createBrain(supervised_config)

# 3. Production Template
production_config = pynlm.create_production_config(2000, False)
production_brain = pynlm.createBrain(production_config)

# 4. Emergency Response Template
emergency_config = pynlm.create_emergency_response_config()
emergency_brain = pynlm.createBrain(emergency_config)
```

### Example 9: Experiment Management - NEW!

```python
# Create and run experiments
experiment_setup = pynlm.create_standard_experiment_setup("my_experiment")

brain = experiment_setup["brain"]
world = experiment_setup["world"] 
agent = experiment_setup["agent"]

# Run experiment
results = pynlm.run_experiment(experiment_setup, steps=1000)

# Analyze results
print(f"Experiment completed!")
print(f"Average reward: {results['average_reward']:.3f}")
print(f"Final reward: {results['final_reward']:.3f}")
print(f"Total steps: {len(results['rewards'])}")

# Save results
pynlm.save_experiment_results(results, "/tmp/experiment_results.csv")
```

### Example 10: Neural Network Integration - NEW!

```python
# Create neural network architectures
# Feed-forward network
ff_brain = pynlm.create_neural_network_architecture(
    input_size=784,      # MNIST image size
    hidden_size=256,     # Hidden layer
    output_size=10,      # 10 classes
    topology="dense"     # Fully connected
)

# Convolutional network simulation
conv_brain = pynlm.create_neural_network_architecture(
    input_size=32*32*3,  # RGB image
    hidden_size=128,    # Convolutional layers
    output_size=5,      # 5 classes
    topology="local"    # Local connectivity
)

# Autoencoder
autoencoder_brain = pynlm.create_neural_network_architecture(
    input_size=64,      # Input dimension
    hidden_size=32,     # Bottleneck
    output_size=64,     # Reconstruction
    topology="sparse"   # Sparse connectivity
)
```

---

## The Simplest Possible Example (UNCHANGED)

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

## Common Patterns (ENHANCED)

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    # Use context managers for safe operations
    with pynlm.brain_context(brain) as brain_ctx:
        with pynlm.world_context(world) as world_ctx:
            with pynlm.agent_context(agent) as agent_ctx:
                world_ctx.reset()
                agent_ctx.initialize()
                
                for step in range(num_steps):
                    world_ctx.update(0.1)
                    percept = world_ctx.getSensoryPercept()
                    agent_ctx.processSensoryInput(percept)
                    brain_ctx.step(step)
                    action = agent_ctx.decodeMotorCommand()
                    world_ctx.applyMotorCommand(action, world_ctx.getSimulationTime())

# Use it like this:
# run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats (ENHANCED)

```python
def get_brain_stats(brain):
    """Get comprehensive brain statistics"""
    stats = {}
    
    # Basic stats
    stats['total_neurons'] = brain.getTotalNeuronCount()
    stats['total_synapses'] = brain.getTotalSynapseCount()
    stats['firing_rate'] = brain.getAverageFiringRate()
    stats['total_spikes'] = brain.getTotalSpikeCount()
    
    # System status
    stats['working_memory'] = brain.getWorkingMemory() is not None
    stats['episodic_memory'] = brain.getEpisodicMemory() is not None
    stats['neuromodulation'] = brain.getDopamine() is not None
    
    return stats

# Just add these inside your loop:
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        stats = get_brain_stats(brain)
        print("Neurons firing:", stats['firing_rate'])
        print("Total spikes:", stats['total_spikes'])
```

### Pattern 3: Enable Brain Features (ENHANCED)

```python
# These are all optional (PLUS NEW: enable_all_systems method)
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections  
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enable_all_systems()              # Enable all at once
```

---

## What Each Part Does

### Enhanced `pynlm.createBrain(config)`
- Creates a virtual brain with neurons and synapses
- **NEW:** Automatically initializes with configuration
- **NEW:** Validates configuration before creation

### Enhanced `brain.initialize()`
- Starts up the brain. Always call this before using the brain!
- **NEW:** Creates performance monitor
- **NEW:** Enables all default systems

### Enhanced `brain.step(step_number)`
- Makes the brain process one moment in time
- **NEW:** Can use fluent API: `brain.step(0).step(1).step(2)`
- **NEW:** Context manager support

### Enhanced `world.update(time)`
- Updates the virtual world by `time` seconds
- **NEW:** Factory functions for different world types
- **NEW:** Context manager support

### Enhanced `agent.processSensoryInput(percept)`
- Gives sensory information (vision, touch, etc.) to the brain
- **NEW:** Convenience methods for common processing
- **NEW:** Performance monitoring

### Enhanced `agent.decodeMotorCommand()`
- Reads the brain's motor neurons to decide what action to take
- **NEW:** Context manager support
- **NEW:** Action validation

---

## Mini Projects (ENHANCED)

### Project 1: Silent Brain (UNCHANGED)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World (UNCHANGED)

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

### Project 3: Complete Agent (ENHANCED)

```python
import pynlm

def run_complete_agent(brain_size=500, world_size=20, steps=1000):
    """Enhanced complete agent simulation"""
    
    # Setup with enhanced configuration
    config = pynlm.create_research_config(brain_size, 0.01)
    brain = pynlm.createBrain(config)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, 
                    visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enable_all_systems()
    
    # Run agent
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 200 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")

    print("Agent simulation complete!")
    return brain, world, agent

# Run the simulation
brain, world, agent = run_complete_agent(300, 15, 800)
```

### Project 4: Performance Benchmark (NEW!)

```python
import pynlm

def benchmark_brain_performance(brain_configs, steps_per_config=500):
    """Benchmark multiple brain configurations"""
    
    results = []
    
    for name, config in brain_configs.items():
        print(f"Benchmarking {name}...")
        
        brain = pynlm.createBrain(config)
        benchmark = pynlm.benchmark_brain_performance(brain, steps_per_config)
        
        benchmark['config_name'] = name
        results.append(benchmark)
        
        print(f"  Throughput: {benchmark['steps_per_second']:.2f} steps/sec")
        print(f"  Average firing rate: {benchmark['average_firing_rate']:.2f} Hz")
    
    # Create comparison
    print("\n=== Performance Comparison ===")
    for result in results:
        print(f"{result['config_name']}: {result['steps_per_second']:.2f} steps/sec, "
              f"{result['average_firing_rate']:.2f} Hz")
    
    return results

# Define configurations to benchmark
brain_configs = {
    "small": pynlm.create_small_config(100),
    "standard": pynlm.create_research_config(500),
    "large": pynlm.create_production_config(1000, False)
}

benchmark_results = benchmark_brain_performance(brain_configs, 300)
```

### Project 5: Experiment Suite (NEW!)

```python
import pynlm

def run_experiment_suite(base_config, param_ranges, num_trials=10):
    """Run experiment suite with parameter variations"""
    
    experiments = []
    
    # Generate parameter combinations
    param_combinations = []
    for param_name in param_ranges:
        if len(param_combinations) == 0:
            for value in param_ranges[param_name]:
                param_combinations.append({param_name: value})
        else:
            new_combinations = []
            for combination in param_combinations:
                for value in param_ranges[param_name]:
                    new_combo = combination.copy()
                    new_combo[param_name] = value
                    new_combinations.append(new_combo)
            param_combinations = new_combinations
    
    # Run experiments
    for i, params in enumerate(param_combinations):
        if i >= num_trials:
            break
            
        # Create experiment configuration
        experiment_config = base_config.copy()
        experiment_config.update(params)
        
        # Run experiment
        brain = pynlm.createBrain(experiment_config)
        world = pynlm.createDefaultWorld()
        agent = pynlm.createAgentBrain(brain)
        
        agent.initialize(world)
        agent.enable_all_systems()
        
        # Collect data
        results = pynlm.run_experiment(brain, world, agent, steps=500)
        
        experiment = {
            'params': params,
            'results': results,
            'experiment_id': i
        }
        
        experiments.append(experiment)
        
        print(f"Completed experiment {i + 1}: {params}")
    
    return experiments

# Define parameter ranges for exploration
param_ranges = {
    "brain.neuron_count": [100, 500, 1000],
    "plasticity.stdp.learning_rate": [0.001, 0.01, 0.1],
    "neuromod.dopamine.scale": [0.5, 1.0, 2.0]
}

# Create base configuration
base_config = {
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.curiosity.enable": True
}

# Run experiment suite
experiment_results = run_experiment_suite(base_config, param_ranges, 15)
```

---

## Enhanced Quick Reference

| What you want | Enhanced Code |
|--------------|---------------|
| Create brain | `pynlm.createBrain(pynlm.createDevelopmentConfig())` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createDefaultWorld(width, height, visionWidth, visionHeight)` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getAverageFiringRate()`, `brain.getTotalSpikeCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Enable all systems | `agent.enable_all_systems()` |
| Context manager | `with pynlm.brain_context(brain):` |
| Fluent config | `pynlm.fluent_config().with_neuron_count(1000).build()` |

---

## Enhanced Troubleshooting

### "My brain isn't doing anything"**

- **Did you call `brain.initialize()`?** ✅ **NEW:** Use context manager for automatic initialization
- **Try increasing the number of steps** ✅ **NEW:** Use performance monitor to check progress
- **Try using factory functions**: `pynlm.createDevelopmentConfig()` for active learning

### "The agent isn't moving"**

- **Did you call `world.applyMotorCommand()`?** ✅ **NEW:** Context manager ensures proper setup
- **Check that `world.update()` is being called** ✅ **NEW:** Use `pynlm.createDefaultWorld()` for easier setup
- **Enable curiosity for exploration**: `agent.enableCuriosity(True)`

### "Everything is 0"**

- **Brains need time to "warm up" - try more steps** ✅ **NEW:** Use `pynlm.createSmallBrain()` for testing
- **Some neurons need input to fire - make sure sensory input is connected** ✅ **NEW:** Use `pynlm.createResearchConfig()` with higher learning rates

### NEW: Performance Issues

- **Low throughput:** Use `pynlm.performance_monitor()` to identify bottlenecks
- **Memory usage:** Use `pynlm.create_memory_pool()` for large operations
- **Slow initialization:** Use context managers for automatic setup

### NEW: Learning Issues

- **No learning:** Check `agent.enable_all_systems()` and ensure reward modulation is working
- **Poor exploration:** Verify `agent.enableCuriosity(True)` is set
- **Overfitting:** Use checkpointing and different configurations

---

## Enhanced Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** (UNCHANGED)
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** (UNCHANGED)
3. **Experiment with different configurations!** (ENHANCED: Try different factory functions)
4. **Use performance monitoring for optimization** (NEW)
5. **Create comprehensive experiments with parameter sweeps** (NEW)
6. **Benchmark different brain architectures** (NEW)
7. **Use context managers for reliable resource management** (NEW)
8. **Implement fluent APIs for cleaner code** (NEW)

---

## 🚀 What's New in Enhanced Version

1. **Factory Functions**: `createDevelopmentConfig()`, `createResearchConfig()`, etc.
2. **Context Managers**: Safe resource management with automatic cleanup
3. **Fluent APIs**: Method chaining for cleaner configuration
4. **Performance Monitoring**: Track simulation speed and efficiency
5. **Learning Progress Tracking**: Monitor agent development over time
6. **Experiment Management**: Create and run systematic experiments
7. **Advanced Configurations**: Templates for different use cases
8. **Neural Network Integration**: Create brain architectures for ML
9. **Comprehensive Testing**: Built-in validation and benchmarking
10. **Enhanced Error Handling**: Custom exception types and validation
11. **Resource Management**: Memory pools and garbage collection hints
12. **Data Export/Import**: CSV, JSON, and binary formats
13. **Configuration Templates**: Standard, learning, production, emergency
14. **Documentation**: Comprehensive examples and best practices
15. **Best Practices**: Guidelines for optimal usage

---

**That's it! You're now ready to use the enhanced NLM with powerful new features and capabilities!** 🎉