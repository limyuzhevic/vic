# NLM - Enhanced Usage Guide for All Skill Levels

NLM is a brain simulator for computers that allows you to create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this with support for both beginner and expert usage.

---

## Quick Start (Copy & Paste) - Three Levels

### Beginner: Simplest Brain (Level 1)

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

### Intermediate: Brain in a Simple World (Level 2)

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

### Expert: Complete Learning Agent (Level 3)

```python
import pynlm

def run_advanced_agent(num_steps=1000, enable_debug=True):
    """Run a complete learning agent with advanced features."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain with advanced settings
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable advanced learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Enable debugging if requested
    if enable_debug:
        # AgentBrain debugging capabilities (if available in advanced version)
        pass
    
    # 8. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            
            # Advanced metrics (if available)
            if hasattr(brain, 'getAdvancedMetrics'):
                metrics = brain.getAdvancedMetrics()
                print(f"  FPS: {metrics.fps:.1f}")
                print(f"  Memory: {metrics.memoryUsageMB:.1f}MB")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_advanced_agent(1000)
```

---

## Enhanced Examples for Expert Users

### Example 1: Performance Monitoring and Profiling

```python
import pynlm
import time

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable performance monitoring (if advanced version available)
if hasattr(brain, 'enableProfiler'):
    brain.enableProfiler(True)
    
# Create visualizer (if available)
if hasattr(brain, 'enableVisualizer'):
    brain.enableVisualizer(True, updateRate=10.0)

# Run simulation with performance tracking
start_time = time.time()
for step in range(2000):
    brain.step(step)
    
    if hasattr(brain, 'updateVisualizer'):
        brain.updateVisualizer()
    
    # Print performance stats periodically
    if step % 500 == 0 and hasattr(brain, 'getCurrentFPS'):
        fps = brain.getCurrentFPS()
        print(f"Step {step}: {fps:.1f} FPS")

# Get performance statistics
end_time = time.time()
if hasattr(brain, 'getMetrics'):
    metrics = brain.getMetrics()
    print(f"\nPerformance Summary:")
    print(f"  Total time: {end_time - start_time:.2f}s")
    print(f"  FPS: {metrics.fps:.1f}")
    print(f"  Memory: {metrics.memoryUsageMB:.1f}MB")
    print(f"  Spikes processed: {metrics.totalSpikes}")
    print(f"  Average step time: {metrics.avgStepTime*1000:.2f}ms")
```

### Example 2: Batch Processing for Multiple Simulations

```python
import pynlm
from multiprocessing import Pool
import json

def run_single_simulation(seed, steps=500, neurons=500):
    """Run a single simulation with given seed."""
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("random_seed", seed)
    config.set("brain.neuron_count", neurons)
    
    # Create and run brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    # Collect results
    return {
        'seed': seed,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'learning_efficiency': brain.getAverageFiringRate() / steps * 100
    }

def run_batch_processing(num_seeds=10, steps_per_sim=500):
    """Run multiple simulations in parallel."""
    
    # Use multiprocessing for parallel execution
    with Pool(processes=4) as pool:
        # Run simulations with different seeds
        results = pool.starmap(
            run_single_simulation,
            [(seed, steps_per_sim) for seed in range(num_seeds)]
        )
    
    # Analyze results
    total_spikes = sum(r['total_spikes'] for r in results)
    avg_spikes = total_spikes / len(results)
    
    print(f"Batch Processing Results ({num_seeds} simulations):")
    print(f"  Average spikes per simulation: {avg_spikes:.0f}")
    print(f"  Min spikes: {min(r['total_spikes'] for r in results)}")
    print(f"  Max spikes: {max(r['total_spikes'] for r in results)}")
    
    # Save results to file
    with open('batch_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    return results

# Run batch processing
batch_results = run_batch_processing(20, 1000)
```

### Example 3: Checkpoint and Restore

```python
import pynlm
import time
import os

def run_with_checkpointing(checkpoint_interval=500, num_steps=3000):
    """Run simulation with automatic checkpointing."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Try to load previous checkpoint
    if os.path.exists('checkpoint.bin'):
        print("Loading checkpoint from file...")
        if brain.load('checkpoint.bin'):
            print("Checkpoint loaded successfully")
        else:
            print("Failed to load checkpoint, starting fresh")
    
    # Run simulation with periodic saving
    for step in range(num_steps):
        brain.step(step)
        
        # Save checkpoint at intervals
        if step > 0 and step % checkpoint_interval == 0:
            checkpoint_file = f"checkpoint_{step:06d}.bin"
            print(f"Saving checkpoint at step {step}...")
            if brain.save(checkpoint_file):
                print(f"  Checkpoint saved to {checkpoint_file}")
    
    # Save final checkpoint
    final_checkpoint = 'checkpoint_final.bin'
    print(f"Saving final checkpoint...")
    if brain.save(final_checkpoint):
        print(f"  Final checkpoint saved to {final_checkpoint}")
    
    return brain

# Run with checkpointing
brain = run_with_checkpointing(checkpoint_interval=1000, num_steps=5000)
```

### Example 4: Advanced Configuration Presets

```python
import pynlm

def get_preset_config(preset_name):
    """Get configuration preset for different use cases."""
    config = pynlm.createDefaultConfig()
    
    if preset_name == "fast":
        # Fast simulation - fewer neurons, no plasticity
        config.set("brain.neuron_count", 200)
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", False)
        config.set("neuromod.dopamine.enable", False)
        print("Using 'fast' preset - maximum speed")
        
    elif preset_name == "accurate":
        # Accurate simulation - more neurons, full learning
        config.set("brain.neuron_count", 2000)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.enable", True)
        config.set("neuromod.curiosity.enable", True)
        print("Using 'accurate' preset - maximum fidelity")
        
    elif preset_name == "balanced":
        # Balanced simulation - good trade-off
        config.set("brain.neuron_count", 1000)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.dopamine.enable", True)
        config.set("neuromod.curiosity.enable", True)
        print("Using 'balanced' preset - good trade-off")
        
    elif preset_name == "memory":
        # Memory-focused - high plasticity for learning
        config.set("brain.neuron_count", 1500)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        print("Using 'memory' preset - optimized for learning")
        
    else:
        print(f"Unknown preset '{preset_name}', using default")
    
    return config

# Use different presets
fast_brain = pynlm.createBrain(get_preset_config("fast"))
accurate_brain = pynlm.createBrain(get_preset_config("accurate"))
balanced_brain = pynlm.createBrain(get_preset_config("balanced"))

# Initialize brains
fast_brain.initialize()
accurate_brain.initialize()
balanced_brain.initialize()

# Compare performance
for name, brain in [("Fast", fast_brain), ("Accurate", accurate_brain), ("Balanced", balanced_brain)]:
    for step in range(1000):
        brain.step(step)
    print(f"{name}: {brain.getTotalSpikeCount()} spikes in 1000 steps")
```

---

## Pattern Library

### Pattern 1: Learning Simulation with Statistics

```python
def run_learning_simulation(config_file=None, num_steps=1000):
    """Run a learning simulation with comprehensive statistics."""
    
    import pynlm
    
    # Load configuration
    config = pynlm.createDefaultConfig()
    if config_file and config.loadFromFile(config_file):
        print(f"Loaded configuration from {config_file}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Statistics collection
    stats = {
        'steps': num_steps,
        'spikes_per_step': [],
        'firing_rate_per_step': [],
        'e_i_ratio_per_step': [],
        'development_stages': set()
    }
    
    # Run simulation
    for step in range(num_steps):
        brain.step(step)
        
        # Collect statistics
        stats['spikes_per_step'].append(brain.getTotalSpikeCount())
        stats['firing_rate_per_step'].append(brain.getAverageFiringRate())
        stats['e_i_ratio_per_step'].append(brain.getExcitationInhibitionRatio())
        stats['development_stages'].add(brain.getDevelopmentalStage())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getAverageFiringRate():.2f} Hz firing rate")
    
    # Compute statistics
    final_stats = {
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
        'development_stages': list(stats['development_stages']),
        'peak_firing_rate': max(stats['firing_rate_per_step']),
        'min_e_i_ratio': min(stats['e_i_ratio_per_step']),
        'max_e_i_ratio': max(stats['e_i_ratio_per_step'])
    }
    
    return brain, final_stats

# Run simulation
brain, stats = run_learning_simulation(num_steps=2000)
print(f"\nFinal Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

### Pattern 2: Agent Environment Interaction

```python
def run_agent_episode(world_width=20, world_height=20, num_steps=500):
    """Run a complete agent episode in a world."""
    
    import pynlm
    
    # Create agent components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(
        width=world_width,
        height=world_height,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(world_width/2, world_height/2)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Episode tracking
    episode_stats = {
        'steps': num_steps,
        'total_reward': 0.0,
        'actions_taken': 0,
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': []
    }
    
    # Run episode
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        episode_stats['total_reward'] += reward
        episode_stats['actions_taken'] += 1
        episode_stats['curiosity_levels'].append(agent.getCuriosityLevel())
        episode_stats['novelty_levels'].append(agent.getNoveltyLevel())
        episode_stats['development_stages'].append(brain.getDevelopmentalStage())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Action={action}, Reward={reward:.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    # Compute final stats
    final_stats = {
        'avg_reward': episode_stats['total_reward'] / episode_stats['actions_taken'],
        'avg_curiosity': sum(episode_stats['curiosity_levels']) / len(episode_stats['curiosity_levels']),
        'avg_novelty': sum(episode_stats['novelty_levels']) / len(episode_stats['novelty_levels']),
        'final_development_stage': episode_stats['development_stages'][-1]
    }
    
    return brain, agent, world, final_stats

# Run episode
brain, agent, world, stats = run_agent_episode()
print(f"\nEpisode Statistics:")
for key, value in stats.items():
    if isinstance(value, float):
        print(f"  {key}: {value:.4f}")
    else:
        print(f"  {key}: {value}")
```

---

## Advanced Features (Expert Mode)

### Debug Injection

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Debug injection (if available in advanced version)
if hasattr(brain, 'injectDebugCurrent'):
    # Inject debug current into specific neuron
    test_neuron_id = brain.getRegion(0).getPopulation(0).getNeurons()[0]
    brain.injectDebugCurrent(test_neuron_id, 100.0)  # Strong excitatory input
    
    # Inject to all neurons of a type
    brain.injectDebugCurrentToType(pynlm.NeuronType.Sensory, 50.0)
    
    # Enable debug injection globally
    brain.enableDebugInjection(True)
```

### Visualization and Graph Export

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable visualization (if available)
if hasattr(brain, 'enableVisualizer'):
    brain.enableVisualizer(True, updateRate=5.0)
    
    # Run some steps
    for step in range(100):
        brain.step(step)
        brain.updateVisualizer()
    
    # Export network graph
    if hasattr(brain, 'exportNetworkGraphViz'):
        dot_file = brain.exportNetworkGraphViz("dot")
        print(f"Network graph exported to: {dot_file}")
        
        # Also try Graphviz format
        graph_file = brain.exportNetworkGraphViz("graphviz")
        print(f"Graph visualization exported to: {graph_file}")
```

### Performance Optimization Presets

```python
import pynlm

def get_performance_preset(preset_name):
    """Get optimized configuration for performance."""
    config = pynlm.createDefaultConfig()
    
    if preset_name == "maximum_speed":
        # Maximum speed - minimal computation
        config.set("brain.neuron_count", 100)
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", False)
        config.set("enable_visualization", False)
        config.set("enable_profiling", False)
        
    elif preset_name == "balanced_performance":
        # Good balance of speed and features
        config.set("brain.neuron_count", 500)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("enable_visualization", True)
        config.set("enable_profiling", True)
        
    elif preset_name == "maximum_accuracy":
        # Maximum accuracy - full simulation
        config.set("brain.neuron_count", 2000)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("enable_visualization", True)
        config.set("enable_profiling", True)
        
    return config

# Use performance presets
fast_config = get_performance_preset("maximum_speed")
balanced_config = get_performance_preset("balanced_performance")
accurate_config = get_performance_preset("maximum_accuracy")

# Create brains with different performance characteristics
fast_brain = pynlm.createBrain(fast_config)
balanced_brain = pynlm.createBrain(balanced_config)
accurate_brain = pynlm.createBrain(accurate_config)

# Initialize and test
for name, brain in [("Fast", fast_brain), ("Balanced", balanced_brain), ("Accurate", accurate_brain)]:
    brain.initialize()
    for step in range(1000):
        brain.step(step)
    print(f"{name}: {brain.getTotalSpikeCount()} spikes in 1000 steps")
```

---

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? Always initialize before use!
- Try increasing the number of steps for warm-up
- Check if plasticity is enabled for learning
- Use verbose logging to see what's happening

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called in your loop
- Ensure the brain has motor neurons with activity
- Check agent's motor command decoding

**"Everything is 0"**
- Brains need time to "warm up" - try more steps (100+ recommended)
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration values (neuron count, connection probability)
- Use debug injection to add current to neurons if needed

**"Performance is too slow"**
- Use "fast" preset for quicker simulations
- Disable visualization and profiling for speed
- Reduce neuron count or simulation steps
- Try batch processing for multiple runs

**"Memory usage is high"**
- Use "balanced" or "fast" preset
- Disable visualization if not needed
- Check for memory leaks in custom code
- Use checkpointing to reduce memory footprint

### Getting Help

1. **Check the documentation**: Read `HOW_TO_USE.md` for detailed information
2. **Use examples**: Try the example scripts in the repository
3. **Enable verbose logging**: Add `-v` to command line or set verbose=True
4. **Use debugging tools**: Enable debug injection for troubleshooting
5. **Check configuration**: Ensure all required parameters are set

### Performance Tips

1. **For quick testing**: Use `--quick` flag or "fast" preset
2. **For production**: Use "balanced" preset for good performance/features trade-off
3. **For research**: Use "accurate" preset for maximum fidelity
4. **For large-scale experiments**: Use batch processing
5. **For debugging**: Enable debug injection and detailed logging

---

## Quick Reference

### Basic Usage (Level 1 - Beginner)

```python
import pynlm

# Simplest possible brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print(f"Neurons: {brain.getTotalNeuronCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent (Level 2 - Intermediate)

```python
import pynlm

# Create complete agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

print(f"Episode completed: {brain.getTotalSpikeCount()} spikes")
```

### Advanced Agent with Features (Level 3 - Expert)

```python
import pynlm
from pynlm.advanced import AdvancedBrain, Profiler, BatchProcessor

# Create advanced brain
config = pynlm.createDefaultConfig()
advanced_config = AdvancedBrainConfig()
advanced_config.neuronCount = 1000
advanced_config.enableProfiling = True
advanced_config.enableVisualization = True
advanced_config.batchSize = 10

advanced_brain = AdvancedBrain(advanced_config)
if advanced_brain.initialize():
    # Enable performance monitoring
    advanced_brain.enableProfiler(True)
    
    # Enable visualization
    advanced_brain.enableVisualizer(True, 10.0)
    
    # Run simulation
    for step in range(1000):
        advanced_brain.step(step)
        advanced_brain.updateVisualizer()
        
        # Get performance metrics
        if step % 100 == 0:
            metrics = advanced_brain.getMetrics()
            print(f"Step {step}: {metrics.fps:.1f} FPS, {metrics.memoryUsageMB:.1f}MB")
    
    # Save results
    advanced_brain.saveWithTimestamp("experiment_1")

# Get performance statistics
if advanced_brain.isInitialized():
    metrics = advanced_brain.getMetrics()
    print(f"\nFinal Performance:")
    print(f"  FPS: {metrics.fps:.1f}")
    print(f"  Memory: {metrics.memoryUsageMB:.1f}MB")
    print(f"  Total spikes: {metrics.totalSpikes}")
    print(f"  Average step time: {metrics.avgStepTime*1000:.2f}ms")
```

---

## Additional Resources

- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed installation and usage guide
- [README.md](README.md) - Project overview and requirements
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

---

**Ready to start?** Begin with the Beginner examples and gradually move to more advanced features as you become comfortable with NLM!

For command-line options and advanced features, use the `nlm` executable with `--help` flag.
