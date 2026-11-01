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

### Pattern 4: Batch Operations

```python
def run_batch_operations(brain, agent, world, steps_per_batch, num_batches):
    """Execute simulation with batch operations for better performance"""
    
    # Prepare batch inputs
    percepts_batch = []
    for _ in range(steps_per_batch):
        percept = world.getSensoryPercept()
        percepts_batch.append(percept)
    
    # Process all percepts in batch
    if len(percepts_batch) > 1:
        agent.processSensoryBatch(percepts_batch)
    else:
        agent.processSensoryInput(percepts_batch[0])
    
    # Inject batch currents
    neuron_ids = []
    for region in brain.getRegions():
        for neuron in region.getAllNeurons():
            if neuron.getType() == pynlm.NeuronType.Sensory:
                neuron_ids.append(neuron.getId())
    
    if len(neuron_ids) > 0:
        currents = [10.0] * len(neuron_ids)
        pynlm.injectCurrentBatch(brain, neuron_ids, currents)
    
    # Select multiple actions in parallel
    actions = pynlm.selectActionsParallel(brain, 3)
    
    return actions

# Use it like this:
actions_batch = []
for batch in range(num_batches):
    actions = run_batch_operations(brain, agent, world, 5, num_batches)
    actions_batch.extend(actions)
```

### Pattern 5: Advanced Statistics and Analysis

```python
def analyze_brain_network(brain, time_window=100):
    """Analyze brain network using advanced statistical methods"""
    
    # Neural connectivity analysis
    region_ids = brain.getRegionIds()
    connectivity = pynlm.analyzeConnectivity(brain, region_ids)
    print(f"Network connectivity: {connectivity.totalConnections} connections")
    print(f"Connection density: {connectivity.connectionDensity:.3f}")
    
    # Population statistics for each neuron type
    for neuron_type in [pynlm.NeuronType.Excitatory, pynlm.NeuronType.Inhibitory, 
                        pynlm.NeuronType.Internal, pynlm.NeuronType.Sensory,
                        pynlm.NeuronType.Motor]:
        stats = pynlm.getPopulationStats(brain, neuron_type)
        print(f"{neuron_type} population: {stats.neuronCount} neurons, "
              f"avg firing rate: {stats.averageFiringRate:.2f} Hz")
    
    # Advanced time-series analysis
    time_series = pynlm.analyzeAdvancedTimeSeries(brain, time_window)
    print(f"Time-series entropy: {time_series.overall.entropy:.3f}")
    print(f"Regularity: {time_series.overall.isRegular}")
    print(f"Bursting: {time_series.overall.isBursting}")
    
    return {
        'connectivity': connectivity,
        'population_stats': stats,
        'time_series': time_series
    }

# Use it like this:
analysis = analyze_brain_network(brain)
```

### Pattern 6: Configuration Management with Templates

```python
def create_and_validate_config():
    """Create and validate configuration using templates"""
    
    # Create configuration template
    template = pynlm.createConfigTemplate(
        name="HighPerformance",
        description="Optimized configuration for high-performance simulations",
        parameters={
            "brain.neuron_count": 1000,
            "brain.region_count": 5,
            "brain.connection_probability": 0.1,
            "brain.random_seed": 42,
            "world.width": 50.0,
            "world.height": 50.0,
            "agent.simulation_speed": 1.0,
            "debug.enable_logging": True,
            "debug.verbose_output": False
        }
    )
    
    # Create configuration using template
    config = pynlm.createDefaultConfig()
    for key, value in template.defaultValues.items():
        config.set(key, value)
    
    # Validate configuration
    validation = pynlm.validateConfig(config)
    
    if validation.isValid:
        print("✓ Configuration is valid")
        return config
    else:
        print("✗ Configuration has errors:")
        for error in validation.errors:
            print(f"  - {error}")
        return None

def compare_configurations(config1, config2):
    """Compare two configurations"""
    differences = []
    
    # Get all keys from both configs
    all_keys = set(config1.getKeys()) | set(config2.getKeys())
    
    for key in all_keys:
        value1 = config1.get(key) if config1.has(key) else None
        value2 = config2.get(key) if config2.has(key) else None
        
        if value1 != value2:
            differences.append({
                'key': key,
                'value1': value1,
                'value2': value2
            })
    
    return differences

# Use it like this:
config = create_and_validate_config()
```

### Pattern 7: Performance Monitoring and Benchmarking

```python
def run_performance_benchmark(brain, world, agent, duration_seconds):
    """Run performance monitoring during simulation"""
    
    # Create performance monitor
    monitor = pynlm.PerformanceMonitor(brain, 0.5)  # Sample every 0.5 seconds
    monitor.start()
    
    start_time = time.time()
    steps_completed = 0
    
    while time.time() - start_time < duration_seconds:
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(steps_completed)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        steps_completed += 1
        
        # Print performance every few steps
        if steps_completed % 50 == 0:
            metrics = monitor.getMetrics()
            print(f"Step {steps_completed}: "
                  f"Active neurons: {metrics.activeNeurons}, "
                  f"Firing rate: {metrics.averageFiringRate:.2f} Hz, "
                  f"Real-time factor: {metrics.realTimeFactor:.2f}x")
    
    # Stop monitoring and get final metrics
    monitor.stop()
    final_metrics = monitor.getMetrics()
    
    # Get performance history
    history = monitor.getHistory(100)
    
    # Calculate performance statistics
    cpu_usages = [m.cpuUsage for m in history]
    memory_usages = [m.memoryUsageMB for m in history]
    
    performance_report = {
        'total_steps': steps_completed,
        'duration_seconds': duration_seconds,
        'steps_per_second': steps_completed / duration_seconds,
        'average_cpu_usage': sum(cpu_usages) / len(cpu_usages),
        'max_cpu_usage': max(cpu_usages),
        'average_memory_usage': sum(memory_usages) / len(memory_usages),
        'max_memory_usage': max(memory_usages),
        'final_metrics': final_metrics,
        'performance_trend': 'increasing' if steps_completed > 100 else 'stable'
    }
    
    return performance_report

# Use it like this:
report = run_performance_benchmark(brain, world, agent, 60.0)  # 60 second benchmark
print(f"Benchmark completed: {report['total_steps']} steps in {report['duration_seconds']} seconds")
```

### Pattern 8: Advanced Debugging and Visualization

```python
def advanced_debug_session(brain, max_steps=1000):
    """Advanced debugging session with step-by-step control and visualization"""
    
    # Create debug inspector
    inspector = pynlm.DebugStateInspector(brain)
    
    # Create step controller for controlled execution
    controller = pynlm.AdvancedStepController(brain)
    controller.setMaxSteps(max_steps)
    controller.setAutoContinue(True)
    
    # Set up step callback for monitoring
    def step_callback(step, duration, metrics):
        if step % 100 == 0:
            print(f"Step {step}: Duration {duration:.2f}ms, "
                  f"Neurons processed: {metrics.neurons_processed}, "
                  f"Spikes generated: {metrics.spikes_generated}")
    
    controller.setStepCallback(step_callback)
    
    print("Starting advanced debug session...")
    print("Controls:")
    print("  - 'p': Pause step execution")
    print("  - 'r': Resume step execution")
    print("  - 's': Step forward one step")
    print("  - 'q': Quit session")
    
    # Interactive debug loop
    while controller.getCurrentStep() < max_steps:
        if controller.isPaused():
            print(f"\nPaused at step {controller.getCurrentStep()}")
            print("Brain state:", inspector.getBrainState())
            
            action = input("Enter command (p/r/s/q): ").strip().lower()
            
            if action == 'p':
                controller.pause()
            elif action == 'r':
                controller.resume()
            elif action == 's':
                controller.step()
            elif action == 'q':
                print("Quitting debug session...")
                break
        else:
            controller.step()
            
            # Show network visualization data
            if controller.getCurrentStep() % 200 == 0:
                viz_data = pynlm.getNetworkDebugData(brain)
                print(f"Network visualization: {viz_data.toString()}")
    
    print(f"\nDebug session completed: {controller.getCurrentStep()} steps")
    return controller.getLastStepMetrics()

# Use it like this:
metrics = advanced_debug_session(brain, 500)
```

### Pattern 9: Real-time Network Analysis

```python
def real_time_network_analysis(brain, update_interval=10):
    """Perform real-time network analysis during simulation"""
    
    print("Starting real-time network analysis...")
    print(f"Analysis updates every {update_interval} steps")
    print("Metrics:")
    print("  - Connectivity strength")
    print("  - Population statistics")
    print("  - Time-series entropy")
    print("  - Network efficiency")
    
    step_count = 0
    
    while step_count < 1000:  # Example limit
        brain.step(step_count)
        step_count += 1
        
        if step_count % update_interval == 0:
            # Get current network data
            viz_data = pynlm.getNetworkDebugData(brain)
            
            # Calculate network metrics
            connectivity = pynlm.analyzeConnectivity(brain, brain.getRegionIds())
            time_series = pynlm.analyzeAdvancedTimeSeries(brain, 50)
            
            print(f"\nStep {step_count}:")
            print(f"  Connectivity: {connectivity.totalConnections} connections")
            print(f"  Time-series entropy: {time_series.overall.entropy:.3f}")
            print(f"  Regularity: {time_series.overall.isRegular}")
            print(f"  Bursting: {time_series.overall.isBursting}")
            
            # Check for network patterns
            if time_series.overall.isRegular:
                print("  Pattern detected: Regular neural activity")
            elif time_series.overall.isBursting:
                print("  Pattern detected: Bursting neural activity")
            elif time_series.overall.isChaotic:
                print("  Pattern detected: Chaotic neural dynamics")
    
    return {
        'final_connectivity': connectivity,
        'final_time_series': time_series,
        'total_steps': step_count
    }

# Use it like this:
network_analysis = real_time_network_analysis(brain)
```

### Pattern 10: Parallel Processing Optimization

```python
def optimize_with_parallel_processing(brain, world, agent, num_repeats=5):
    """Run simulation multiple times with parallel processing optimization"""
    
    from concurrent.futures import ThreadPoolExecutor
    import threading
    
    results = []
    lock = threading.Lock()
    
    def run_single_simulation(sim_id):
        print(f"Starting simulation {sim_id + 1}/{num_repeats}")
        
        # Create fresh brain for this simulation
        config = pynlm.createDefaultConfig()
        brain_local = pynlm.createBrain(config)
        brain_local.initialize()
        
        agent_local = pynlm.createAgentBrain(brain_local)
        agent_local.initialize(world)
        
        # Enable parallel processing features
        if hasattr(brain_local, 'enableMultithreading'):
            brain_local.enableMultithreading(True)
        
        # Run simulation steps
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent_local.processSensoryInput(percept)
            brain_local.step(step)
            action = agent_local.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        metrics = {
            'simulation_id': sim_id,
            'total_spikes': brain_local.getTotalSpikeCount(),
            'firing_rate': brain_local.getAverageFiringRate(),
            'active_neurons': brain_local.getActiveNeuronCount(),
            'final_step': step
        }
        
        with lock:
            results.append(metrics)
        
        print(f"Completed simulation {sim_id + 1}/{num_repeats}: "
              f"{metrics['total_spikes']} spikes")
        
        return metrics
    
    # Run simulations in parallel
    with ThreadPoolExecutor(max_workers=num_repeats) as executor:
        futures = [executor.submit(run_single_simulation, i) for i in range(num_repeats)]
        
        for future in futures:
            future.result()  # Wait for completion and get result
    
    # Analyze results
    print(f"\nParallel processing results:")
    print(f"Total simulations: {len(results)}")
    print(f"Average spikes per simulation: {sum(r['total_spikes'] for r in results) / len(results):.1f}")
    print(f"Average firing rate: {sum(r['firing_rate'] for r in results) / len(results):.2f} Hz")
    print(f"Average active neurons: {sum(r['active_neurons'] for r in results) / len(results):.1f}")
    
    return results

# Use it like this:
parallel_results = optimize_with_parallel_processing(brain, world, agent, 5)
```

---

## Quick Reference

### New Batch Operations

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `injectCurrentBatch(brain, neuron_ids, currents)` | Inject current into multiple neurons simultaneously | brain, list of neuron IDs, list of currents | None |
| `processSensoryBatch(agent, percepts)` | Process multiple sensory inputs | agent, list of percepts | None |
| `selectActionsParallel(brain, num_actions)` | Select actions from multiple neurons in parallel | brain, number of actions | list of action types |

### Advanced Statistics

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `analyzeConnectivity(brain, region_ids)` | Analyze connection patterns between regions | brain, list of region IDs | ConnectivityAnalysis |
| `getPopulationStats(brain, neuron_type)` | Get comprehensive statistics for neuron population | brain, neuron type | PopulationStats |
| `analyzeSpikePatterns(brain, time_window)` | Analyze spike train patterns over time | brain, time window size | SpikePatternAnalysis |

### Configuration Enhancements

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `validateConfig(config, schema)` | Validate configuration against schema | config, optional schema | ConfigValidationResult |
| `createConfigTemplate(name, parameters)` | Create reusable configuration template | name, parameter dictionary | ConfigTemplate |

### Performance Monitoring

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `PerformanceMonitor(brain, interval)` | Initialize performance monitor | brain, sampling interval | PerformanceMonitor |
| `getMetrics()` | Get current performance metrics | monitor | PerformanceMetrics |
| `getHistory(limit)` | Get performance metrics history | monitor, limit | list of PerformanceMetrics |

### Advanced Debugging

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `DebugStateInspector(brain)` | Initialize debug inspector | brain | DebugStateInspector |
| `getBrainState()` | Get comprehensive brain state | inspector | string |
| `getNetworkDebugData(brain)` | Get network visualization data | brain | NetworkDebugData |

### Extended Enumerations

| Enum | Extended Values |
|------|----------------|
| `ActionType` | Sprint, Jump, Crouch, Attack, Defend |
| `WorldObjectType` | Portal, Checkpoint, Treasure, Trap |
| `NeuromodulatorType` | Adrenaline, Cortisol, Melatonin, SubstanceP |

### Performance Tips

1. **Use batch operations** for processing multiple neurons/percepts at once
2. **Enable parallel processing** for large-scale simulations
3. **Use performance monitoring** to identify bottlenecks
4. **Leverage configuration templates** for consistent experiments
5. **Utilize advanced statistics** for deeper insights into network dynamics
6. **Implement step-by-step debugging** for careful analysis
7. **Monitor network metrics** in real-time during simulation

---

## Next Steps

When you're comfortable with these advanced features:

1. **Read `HOW_TO_USE.md`** for more detailed information about advanced topics
2. **Read `docs/ARCHITECTURE.md`** to understand the technical architecture
3. **Explore the existing demos** in the repository to see advanced features in action
4. **Experiment with different configuration templates** to optimize performance
5. **Implement custom batch operations** for your specific use case
6. **Create custom statistical analyses** tailored to your research questions
7. **Set up automated performance benchmarking** for your applications

These advanced features make NLM a powerful tool for research, development, and experimentation with neural systems.

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
