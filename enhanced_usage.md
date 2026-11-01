# NLM - Enhanced Usage Guide

## Quick Start

This guide provides comprehensive examples for using NLM with the enhanced features. All examples assume NLM is properly installed.

## Core Usage Patterns

### Pattern 1: Basic Brain Setup

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Pattern 2: Simple Simulation

```python
import pynlm
import time

# Setup simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getActiveNeuronCount()} active neurons, "
              f"{brain.getTotalSpikeCount()} total spikes")

print("✓ Simulation completed successfully")
```

### Pattern 3: Batch Operations

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Example batch current injection
# (Neurons need to be made accessible through brain API)
print("✓ Batch operations available:")
print("  - injectCurrentBatch(neuron_ids, currents)")
print("  - processSensoryBatch(agent, percepts)")
print("  - selectActionsParallel(brain, num_actions)")

# Example batch sensory processing
percepts = []
for i in range(5):
    percept = pynlm.SensoryPercept()
    percept.setVision([0.1, 0.2, 0.3, 0.4, 0.5])
    percepts.append(percept)
print("✓ Batch sensory processing available")
```

### Pattern 4: Advanced Statistics

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Connectivity analysis
region_ids = brain.getRegionIds()
if region_ids:
    connectivity = pynlm.analyzeConnectivity(brain, region_ids)
    print(f"✓ Connectivity analysis: {connectivity.totalConnections} connections")
    print(f"  - Clustering coefficient: {connectivity.clusteringCoefficient:.3f}")
    print(f"  - Path length: {connectivity.characteristicPathLength:.3f}")

# Population statistics
print("✓ Population statistics available for all neuron types")
print("  - Excitatory neurons")
print("  - Inhibitory neurons") 
print("  - Internal neurons")
print("  - Sensory neurons")
print("  - Motor neurons")

# Spike pattern analysis
print("✓ Spike pattern analysis available")
print("  - Pattern detection (regular, bursting, chaotic)")
print("  - Statistical properties (mean, variance, entropy)")
```

### Pattern 5: Configuration Management

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 5)
config.set("world.width", 50.0)
config.set("world.height", 50.0)
config.set("simulation.speed", 2.0)

# Check configuration
print("✓ Configuration management available:")
print(f"  - Neuron count: {config.get('brain.neuron_count')}")
print(f"  - Region count: {config.get('brain.region_count')}")
print(f"  - World dimensions: {config.get('world.width')} x {config.get('world.height')}")

# Configuration validation
validation = pynlm.validateConfig(config)
if validation.isValid:
    print("✓ Configuration validation passed")
else:
    print(f"⚠ Validation errors: {len(validation.errors)}")

# Configuration templates
print("✓ Configuration template creation available")
print("  - Template with validation rules")
print("  - Template with constraints and dependencies")
```

### Pattern 6: Performance Monitoring

```python
import pynlm
import time

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create and start performance monitor
monitor = pynlm.PerformanceMonitor(brain, 0.1)
monitor.start()

# Run simulation
for step in range(50):
    brain.step(step)
    time.sleep(0.05)  # Real-time factor

# Get performance metrics
metrics = monitor.getMetrics()
print("✓ Performance monitoring available:")
print(f"  - CPU Usage: {metrics.cpuUsage:.1f}%")
print(f"  - Memory Usage: {metrics.memoryUsageMB:.1f} MB")
print(f"  - Active Neurons: {metrics.activeNeurons}")
print(f"  - Firing Neurons: {metrics.firingNeurons}")
print(f"  - Total Spikes: {metrics.totalSpikes}")
print(f"  - Real-time Factor: {metrics.realTimeFactor:.2f}x")
print(f"  - Network Utilization: {metrics.networkUtilization:.1f}%")

# Get performance history
history = monitor.getHistory(10)
print(f"✓ Performance history: {len(history)} samples collected")

monitor.stop()
```

### Pattern 7: Advanced Debugging

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create debug tools
inspector = pynlm.DebugStateInspector(brain)
controller = pynlm.AdvancedStepController(brain)

print("✓ Advanced debugging tools available:")
print("  - DebugStateInspector: Comprehensive state inspection")
print("  - AdvancedStepController: Step-by-step execution control")
print("  - Network visualization tools")
print("  - Performance metrics per step")
print("  - Real-time monitoring capabilities")

# Example usage
print("✓ Debugging features:")
print("  - Neuron state inspection")
print("  - Region state analysis")
print("  - Brain state monitoring")
print("  - Step pause/resume control")
print("  - Performance tracking")
```

### Pattern 8: Extended Enumerations

```python
import pynlm

print("✓ Extended enumerations available:")
print("\n  Extended Action Types:")
for action in [pynlm.ActionType.MoveForward, pynlm.ActionType.Sprint,
               pynlm.ActionType.Jump, pynlm.ActionType.Crouch,
               pynlm.ActionType.Attack, pynlm.ActionType.Defend]:
    print(f"    - {action}")

print("\n  Extended World Object Types:")
for obj in [pynlm.WorldObjectType.Resource, pynlm.WorldObjectType.Portal,
            pynlm.WorldObjectType.Checkpoint, pynlm.WorldObjectType.Treasure]:
    print(f"    - {obj}")

print("\n  Extended Neuromodulator Types:")
for mod in [pynlm.NeuromodulatorType.Dopamine, pynlm.NeuromodulatorType.Serotonin,
            pynlm.NeuromodulatorType.Acetylcholine, pynlm.NeuromodulatorType.Noradrenaline]:
    print(f"    - {mod}")
```

## Advanced Usage Examples

### Real-time Network Analysis

```python
import pynlm

def real_time_network_analysis(brain, update_interval=10):
    """Perform real-time network analysis during simulation"""
    print("Starting real-time network analysis...")
    print(f"Analysis updates every {update_interval} steps")
    
    step_count = 0
    
    while step_count < 200:
        brain.step(step_count)
        step_count += 1
        
        if step_count % update_interval == 0:
            viz_data = pynlm.getNetworkDebugData(brain)
            connectivity = pynlm.analyzeConnectivity(brain, brain.getRegionIds())
            
            print(f"\nStep {step_count}:")
            print(f"  Network: {viz_data.neurons} neurons, {viz_data.connections} connections")
            print(f"  Connectivity: {connectivity.totalConnections} total connections")
            print(f"  Clustering: {connectivity.clusteringCoefficient:.3f}")
            print(f"  Path length: {connectivity.characteristicPathLength:.3f}")
    
    return {
        'final_connectivity': connectivity,
        'final_network_data': viz_data,
        'total_steps': step_count
    }
```

### Performance Benchmarking

```python
import pynlm
import time
import numpy as np

def run_performance_benchmark(brain, duration_seconds=60):
    """Run comprehensive performance benchmark"""
    from concurrent.futures import ThreadPoolExecutor
    
    start_time = time.time()
    
    # Create performance monitor
    monitor = pynlm.PerformanceMonitor(brain, 0.5)
    monitor.start()
    
    # Run simulation
    for step in range(int(duration_seconds * 10)):  # 0.1s per step
        brain.step(step)
        if step % 10 == 0:
            time.sleep(0.1)  # Real-time factor
    
    # Stop monitoring
    monitor.stop()
    
    # Analyze results
    final_metrics = monitor.getMetrics()
    history = monitor.getHistory(100)
    
    # Calculate performance statistics
    cpu_usages = [m.cpuUsage for m in history]
    memory_usages = [m.memoryUsageMB for m in history]
    
    performance_report = {
        'total_steps': int(duration_seconds * 10),
        'duration_seconds': duration_seconds,
        'steps_per_second': (duration_seconds * 10) / duration_seconds,
        'average_cpu_usage': np.mean(cpu_usages),
        'max_cpu_usage': np.max(cpu_usages),
        'average_memory_usage': np.mean(memory_usages),
        'max_memory_usage': np.max(memory_usages),
        'final_metrics': final_metrics,
        'performance_trend': 'stable'
    }
    
    return performance_report
```

### Parallel Processing Optimization

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor
import threading

def optimize_with_parallel_processing(brain, world, agent, num_repeats=5):
    """Run simulation multiple times with parallel processing"""
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
    print(f"Average spikes per simulation: {np.mean([r['total_spikes'] for r in results]):.1f}")
    print(f"Average firing rate: {np.mean([r['firing_rate'] for r in results]):.2f} Hz")
    print(f"Average active neurons: {np.mean([r['active_neurons'] for r in results]):.1f}")
    
    return results
```

## Quick Reference

### Core Functions

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `createBrain(config)` | Create neural brain | config | Brain object |
| `createSimpleWorld()` | Create simulation world | None | World object |
| `createAgentBrain(brain)` | Create agent interface | brain | AgentBrain object |
| `createDefaultConfig()` | Create default configuration | None | Config object |

### Batch Operations

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `injectCurrentBatch(brain, neuron_ids, currents)` | Inject current to multiple neurons | brain, neuron_ids, currents | None |
| `processSensoryBatch(agent, percepts)` | Process multiple sensory inputs | agent, percepts | None |
| `selectActionsParallel(brain, num_actions)` | Parallel action selection | brain, num_actions | list of ActionType |

### Statistics

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `analyzeConnectivity(brain, region_ids)` | Neural connectivity analysis | brain, region_ids | ConnectivityAnalysis |
| `getPopulationStats(brain, neuron_type)` | Population statistics | brain, neuron_type | PopulationStats |
| `analyzeSpikePatterns(brain, time_window)` | Spike pattern analysis | brain, time_window | SpikePatternAnalysis |

### Configuration

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `validateConfig(config, schema)` | Configuration validation | config, optional schema | ConfigValidationResult |
| `createConfigTemplate(name, parameters)` | Create configuration template | name, parameters | ConfigTemplate |

### Monitoring

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `PerformanceMonitor(brain, interval)` | Initialize performance monitor | brain, interval | PerformanceMonitor |
| `monitor.getMetrics()` | Get current metrics | monitor | PerformanceMetrics |
| `monitor.getHistory(limit)` | Get metrics history | monitor, limit | list of PerformanceMetrics |

### Debugging

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `DebugStateInspector(brain)` | Initialize debug inspector | brain | DebugStateInspector |
| `getNetworkDebugData(brain)` | Get network visualization data | brain | NetworkDebugData |

### Extended Enumerations

| Enum | New Values |
|------|------------|
| `ActionType` | Sprint, Jump, Crouch, Attack, Defend, EmoteHappy, EmoteSad, EmoteAngy, EmoteSurprised |
| `WorldObjectType` | Portal, Checkpoint, Treasure, Trap, NPC, Vehicle |
| `NeuromodulatorType` | Adrenaline, Cortisol, Melatonin, SubstanceP, Enkephalin, Dynorphin, Bombesin, Rimonabant |

## Performance Tips

1. **Use batch operations** for processing multiple neurons/percepts at once
2. **Enable parallel processing** for large-scale simulations
3. **Use performance monitoring** to identify bottlenecks
4. **Leverage configuration templates** for consistent experiments
5. **Utilize advanced statistics** for deeper insights into network dynamics
6. **Implement step-by-step debugging** for careful analysis
7. **Monitor network metrics** in real-time during simulation

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**Performance issues**
- Use batch operations for efficiency
- Enable parallel processing for large simulations
- Monitor performance with `PerformanceMonitor`

## Next Steps

When you're comfortable with these advanced features:

1. **Read `demo_advanced_features.py`** for comprehensive demonstrations
2. **Read `HOW_TO_USE.md`** for detailed technical information
3. **Explore the existing demos** in the repository to see features in action
4. **Experiment with different configuration templates** to optimize performance
5. **Implement custom batch operations** for your specific use case
6. **Create custom statistical analyses** tailored to your research questions
7. **Set up automated performance benchmarking** for your applications
8. **Contribute to the NLM project** by adding new features or improving existing ones

These advanced features transform the NLM Python bindings from a basic neural simulation tool into a powerful platform for research, development, and experimentation with neural systems.

---

*Enhanced Usage Guide Version 2.0*
*Created for NLM Advanced Python Bindings*
*Last updated: $(date)*