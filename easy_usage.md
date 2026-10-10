# NLM - Enhanced Usage Guide for Beginners and Power Users

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

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

## Quick Start (Copy & Paste) - Enhanced Version

### Example 1: Simplest Brain with Resource Management

```python
import pynlm

# Step 1: Make a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Use context manager for automatic resource management
with pynlm.simulation_guard(brain) as guard:
    # Step 3: Make it think for 100 steps
    for i in range(100):
        brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Enhanced Brain in a Simple World

```python
import pynlm

# Create everything with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # More neurons for interesting behavior
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning systems
agent.enableAllSystems(True)  # Enable reward, plasticity, development, curiosity

# Run enhanced simulation with monitoring
with pynlm.simulation_guard(brain, auto_save=True, backup_path="checkpoint") as guard:
    for step in range(200):
        world.update(0.1)  # Update world
        percept = world.getSensoryPercept()  # What does the agent see?
        agent.processSensoryInput(percept)  # Brain sees it
        brain.step(step)  # Brain thinks
        action = agent.decodeMotorCommand()  # Brain decides action
        world.applyMotorCommand(action, world.getSimulationTime())  # Do action
        
        # Apply reward modulation for learning
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)

print("Enhanced simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
```

### Example 3: Batch Processing for Performance

```python
import pynlm
import time

# Create multiple brains for batch processing
config = pynlm.createDefaultConfig()
num_brains = 10
brains = [pynlm.createBrain(config) for _ in range(num_brains)]

# Initialize all brains
for brain in brains:
    brain.initialize()

# Use batch processor for efficient processing
with pynlm.batch_processor(brains) as processor:
    # Process multiple steps for all brains
    steps = processor.batchStep(range(100))
    print(f"Completed {len(steps)} batch steps efficiently across {num_brains} brains")

# Get performance metrics for each brain
for i, brain in enumerate(brains):
    metrics = brain.getPerformanceMetrics()
    print(f"Brain {i}: {metrics.firing_neurons} firing neurons, {metrics.average_firing_rate:.2f} Hz")
```

### Example 4: Advanced Feature Extraction for ML

```python
import pynlm

# Setup complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation and collect features for machine learning
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Extract comprehensive features for ML applications
features = agent.extractFeatures()
print("Extracted ML features:")
for key, value in features.items():
    print(f"  {key}: {value}")

# Get neuromodulation state for advanced analysis
neuromod_state = agent.getFullNeuromodulationState()
print(f"Neuromodulation State: {neuromod_state}")
```

### Example 5: Performance Monitoring and Profiling

```python
import pynlm
import time

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Start performance profiler
profiler = pynlm.PerformanceProfiler()
profiler.start()

# Run simulation with profiling
for step in range(1000):
    start_time = time.time()
    brain.step(step)
    end_time = time.time()
    profiler.recordStep((end_time - start_time) * 1000)  # Convert to milliseconds

profiler.stop()

# Get detailed performance metrics
metrics = profiler.getMetrics()
print("Performance Metrics:")
for key, value in metrics.items():
    print(f"  {key}: {value}")

# Get brain performance metrics
brain_metrics = brain.getPerformanceMetrics()
print(f"Brain Performance: {brain_metrics.firing_neurons} firing neurons, {brain_metrics.average_firing_rate:.2f} Hz")
```

### Example 6: Advanced Checkpoint and Recovery

```python
import pynlm
import os

# Create brain with checkpoint support
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation with checkpoint saving
checkpoint_dir = "checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)

with pynlm.simulation_guard(brain, auto_save=True, backup_path=checkpoint_dir) as guard:
    # Run simulation
    for step in range(500):
        brain.step(step)
        
        # Save checkpoint every 100 steps
        if step % 100 == 0:
            brain.saveCheckpoint(checkpoint_dir, step)
            print(f"Checkpoint saved at step {step}")
    
    # Get simulation statistics
    spikes_generated = guard.getSpikeCountDelta()
    steps_processed = guard.getCurrentStep()
    print(f"Simulation completed: {steps_processed} steps, {spikes_generated} spikes")

# Load latest checkpoint
latest_checkpoint = f"{checkpoint_dir}/checkpoint_step_400.bin"
if os.path.exists(latest_checkpoint):
    brain.loadCheckpoint(latest_checkpoint)
    print(f"Loaded checkpoint from {latest_checkpoint}")
    print(f"Brain state restored: {brain.getTotalSpikeCount()} total spikes")
```

---

## Enhanced Common Patterns

### Pattern 1: Run a Simulation with Monitoring

```python
def run_monitored_simulation(brain, world, agent, num_steps, config_file=None):
    """Run simulation with comprehensive monitoring and error handling."""
    
    # Load configuration if specified
    if config_file:
        try:
            config = agent.getBrain().getConfig()
            config.loadFromFile(config_file)
        except Exception as e:
            print(f"Warning: Could not load config file: {e}")
    
    # Use context manager for automatic resource management
    with pynlm.simulation_guard(brain, auto_save=True, backup_path="auto_backup") as guard:
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
            
            # 7. Apply reward modulation
            reward = agent.getNeuromodulationLevel()
            agent.applyRewardModulation(reward, 0.0)
            
            # 8. Update development
            agent.updateDevelopment(0.1)
            
            # 9. Print progress and metrics every 50 steps
            if step % 50 == 0:
                metrics = brain.getPerformanceMetrics()
                neuromod_state = agent.getFullNeuromodulationState()
                print(f"Step {step}:")
                print(f"  Firing neurons: {metrics.firing_neurons}")
                print(f"  Curiosity: {neuromod_state.curiosity:.3f}")
                print(f"  Novelty: {neuromod_state.novelty:.3f}")
                print(f"  Dev Stage: {neuromod_state.developmental_stage}")
    
    return {
        "steps_completed": num_steps,
        "total_spikes": brain.getTotalSpikeCount(),
        "final_firing_rate": brain.getAverageFiringRate(),
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel()
    }
```

### Pattern 2: Advanced Configuration Management

```python
def create_advanced_config():
    """Create an advanced configuration with all systems enabled."""
    config = pynlm.createDefaultConfig()
    
    # Neural configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("brain.connection_probability", 0.1)
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.scale", 0.5)
    
    # Development configuration
    config.set("development.initial_plasticity", 1.0)
    config.set("development.critical_period_end", 100.0)
    
    # Performance optimization
    config.set("performance.memory_pool_size", 1000000)
    config.set("performance.event_queue_size", 100000)
    
    return config

# Use the advanced configuration
config = create_advanced_config()
brain = pynlm.createBrain(config)
brain.initialize()
```

### Pattern 3: Batch Processing for Research

```python
def run_research_experiment(config, world_template, num_replicates=10, steps=1000):
    """Run multiple replicates of an experiment for statistical analysis."""
    
    experiments = []
    results = []
    
    # Create batch processor for efficiency
    with pynlm.batch_processor([pynlm.createBrain(config) for _ in range(num_replicates)]) as processor:
        # Run simulation for all replicates
        for i in range(num_replicates):
            # Create fresh world for each replicate
            world = pynlm.createSimpleWorld()
            world.configure(**world_template)
            world.reset()
            
            # Create agent for this replicate
            brain = processor.brains[i]
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning for this experiment
            agent.enableAllSystems(True)
            
            # Run simulation
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply learning
                reward = agent.getNeuromodulationLevel()
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            # Collect results
            features = agent.extractFeatures()
            features["replicate"] = i
            features["final_firing_rate"] = brain.getAverageFiringRate()
            features["total_spikes"] = brain.getTotalSpikeCount()
            features["curiosity"] = agent.getCuriosityLevel()
            features["novelty"] = agent.getNoveltyLevel()
            
            results.append(features)
            experiments.append({
                "brain": brain,
                "agent": agent,
                "world": world,
                "features": features
            })
    
    return experiments, results
```

### Pattern 4: Visualization and Analysis

```python
def analyze_neural_dynamics(brain, steps=100):
    """Analyze and visualize neural dynamics."""
    
    # Get performance metrics
    metrics = brain.getPerformanceMetrics()
    
    # Visualize using NLM's built-in tools
    pynlm.VisualizationUtils.plot_neural_activity(brain, steps)
    pynlm.VisualizationUtils.plot_spike_raster(brain, steps)
    pynlm.VisualizationUtils.plot_connectivity(brain)
    
    # Create analysis report
    report = {
        "basic_stats": {
            "total_neurons": metrics.total_neuron_count,
            "total_synapses": metrics.total_synapse_count,
            "firing_neurons": metrics.firing_neuron_count,
            "active_neurons": metrics.active_neuron_count,
            "average_firing_rate": metrics.average_firing_rate,
            "ei_ratio": metrics.excitation_inhibition_ratio,
            "total_spikes": metrics.total_spike_count
        },
        "learning_indicators": {
            "excitation_inhibition_ratio": metrics.excitation_inhibition_ratio,
            "plasticity_enabled": True,
            "development_stage": "Analysis"
        }
    }
    
    return report
```

---

## Enhanced Quick Reference

| What you want | Code (Enhanced Version) |
|--------------|-------------------------|
| Create brain | `pynlm.createBrain(config)` |
| Create advanced brain | `with pynlm.simulation_guard(brain): ...` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step_number)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create advanced world | `world.configure(width, height, visionWidth, visionHeight)` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getPerformanceMetrics()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Process batch inputs | `agent.processSensoryBatch(percepts)` |
| Run batch steps | `brain.batchStep(steps)` |
| Get ML features | `agent.extractFeatures()` |
| Monitor performance | `profiler = pynlm.PerformanceProfiler()` |
| Visualize activity | `pynlm.VisualizationUtils.plot_neural_activity(brain, steps)` |
| Advanced checkpoint | `brain.saveCheckpoint(path, step)` |
| Get neuromod state | `agent.getFullNeuromodulationState()` |

---

## Enhanced Troubleshooting

### "My brain isn't doing anything" (Enhanced)

**Possible causes:**
- Did you call `brain.initialize()`?
- Is the simulation guard properly managing resources?
- Check performance metrics: `brain.getPerformanceMetrics()`
- Verify that learning systems are enabled: `agent.enableAllSystems(True)`
- Check if world is updating: `world.getSimulationTime()`

**Solutions:**
```python
# Debug solution
brain = pynlm.createBrain(config)
brain.initialize()

# Check performance
metrics = brain.getPerformanceMetrics()
print(f"Neurons: {metrics.total_neuron_count}")
print(f"Firing: {metrics.firing_neuron_count}")
print(f"Rate: {metrics.average_firing_rate} Hz")

# Enable all systems
agent.enableAllSystems(True)
```

### "The agent isn't moving" (Enhanced)

**Possible causes:**
- Did you call `world.applyMotorCommand()`?
- Is the agent properly initialized?
- Check agent's action selection: `agent.decodeMotorCommand()`
- Verify world is updating: `world.update(0.1)`

**Solutions:**
```python
# Debug solution
agent.initialize(world)  # Ensure proper initialization
world.update(0.1)  # Make sure world is updating
percept = world.getSensoryPercept()  # Get sensory input
agent.processSensoryInput(percept)  # Process it
brain.step(step)  # Let brain think
action = agent.decodeMotorCommand()  # Get action
world.applyMotorCommand(action, world.getSimulationTime())  # Apply it
```

### "Everything is 0" (Enhanced)

**Possible causes:**
- Brain needs time to "warm up" - try more steps
- Check if neurons need input - ensure sensory input is connected
- Verify learning systems are enabled
- Check performance metrics for actual activity

**Solutions:**
```python
# Debug solution with enhanced monitoring
with pynlm.simulation_guard(brain) as guard:
    for step in range(1000):
        brain.step(step)
        
        # Check for activity
        metrics = brain.getPerformanceMetrics()
        if metrics.firing_neuron_count == 0 and step > 100:
            print(f"No activity at step {step}")
            print(f"Neuromodulation: {agent.getNeuromodulationLevel()}")
            print(f"Curiosity: {agent.getCuriosityLevel()}")
            # Add sensory input to stimulate activity
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
```

### "Performance is too slow" (Enhanced)

**Possible causes:**
- Large number of neurons/synapses
- Inefficient batch processing
- Lack of performance optimization

**Solutions:**
```python
# Performance optimization solution
import time

profiler = pynlm.PerformanceProfiler()
profiler.start()

# Use batch processing for efficiency
steps = brain.batchStep(range(1000))  # Much faster than loop

profiler.stop()
metrics = profiler.getMetrics()
print(f"Batch processing: {metrics.steps_per_second:.0f} steps/second")

# Optimize configuration
config.set("brain.neuron_count", 1000)  # Adjust based on hardware
config.set("performance.memory_pool_size", 2000000)
```

---

## Enhanced Next Steps

When you're comfortable with the basics:

1. **Read the C++ documentation** for advanced configuration options
2. **Experiment with different configurations** using `pynlm.createAdvancedConfig()`
3. **Use batch processing** for research and statistical analysis
4. **Implement performance monitoring** with `pynlm.PerformanceProfiler()`
5. **Create custom visualizations** using `pynlm.VisualizationUtils()`
6. **Build complex experiments** using the enhanced simulation guards
7. **Export data for ML analysis** using `agent.extractFeatures()`

The enhanced NLM Python bindings now provide:
- **Comprehensive error handling** with Pythonic exceptions
- **NumPy integration** for efficient data operations
- **Context managers** for automatic resource management
- **Batch processing capabilities** for performance
- **Comprehensive monitoring** and profiling tools
- **Advanced simulation controls** for power users
- **Better API design** with Pythonic conventions
- **Enhanced documentation** and examples
- **Machine learning integration** with feature extraction
- **Advanced visualization tools** and analysis

That's it! You're now ready to use the enhanced NLM with professional-grade features for both beginners and advanced users.