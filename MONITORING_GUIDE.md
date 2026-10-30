# NLM - Advanced Monitoring and Debugging Tools

## Overview

This document provides comprehensive tools for monitoring, debugging, and analyzing NLM simulations. These tools enhance the standard Python API with advanced capabilities for tracking neural activity, performance profiling, system state management, and detailed error analysis.

## Advanced Monitoring Classes

### NLMMonitor - Central Monitoring System

```python
import pynlm

# Create advanced monitor
monitor = pynlm.NLMMonitor()

# Configure monitoring
monitor.set_log_level(pynlm.LogLevel.Debug)
monitor.enable_neural_activity_logging(True)
monitor.enable_performance_profiling(True)
monitor.set_max_history(10000)

# Add tracking targets
monitor.track_brain(brain, "main_brain")
monitor.track_agent(agent, "main_agent")
monitor.track_world(world, "simulation_world")

# Start monitoring
monitor.start()

# Run simulation with monitoring
for step in range(1000):
    # Your simulation code here
    pass

# Stop monitoring
monitor.stop()

# Generate analysis report
report = monitor.generate_report()
print(report)
```

### PerformanceProfiler - Advanced Performance Tracking

```python
import pynlm
import time

# Create specialized profilers
brain_profiler = pynlm.PerformanceProfiler("brain_operations")
world_profiler = pynlm.PerformanceProfiler("world_updates")
agent_profiler = pynlm.PerformanceProfiler("agent_processing")

# Profile specific operations
brain_profiler.start()
# Perform brain operations
brain_profiler.stop()

world_profiler.start()
# Update world
world_profiler.stop()

agent_profiler.start()
# Process agent input
agent_profiler.stop()

# Get detailed performance analysis
analysis = brain_profiler.get_detailed_analysis()
print(f"Average brain operation time: {analysis['average_ms']:.2f} ms")
print(f"Performance bottleneck: {analysis['bottleneck_operation']}")

# Compare with benchmarks
benchmarks = brain_profiler.compare_with_benchmarks()
print(f"Performance vs baseline: {benchmarks['improvement_percent']:.1f}%")
```

### NeuralActivityTracker - Detailed Neural Activity Analysis

```python
import pynlm
import numpy as np

# Create neural activity tracker
tracker = pynlm.NeuralActivityTracker()

# Configure tracking
aggregator = tracker.create_activity_aggregator(
    neuron_type=pynlm.NeuronType.Excitatory,
    min_firing_rate=0.1,
    max_history=1000
)

# Track different neural populations
tracker.track_region(brain, 0, "cortex_0")
tracker.track_population(brain, 0, 0, "excitatory_population")

# Start tracking
tracker.start()

# Run simulation
for step in range(500):
    brain.step(step)
    tracker.capture_snapshot(step)

# Stop tracking
tracker.stop()

# Analyze neural patterns
patterns = tracker.analyze_neural_patterns()
print(f"Detected patterns: {patterns['patterns']}")
print(f"Pattern stability: {patterns['stability']:.2f}")

# Generate neural signature
neural_signature = tracker.generate_neural_signature()
print(f"Neural signature complexity: {neural_signature['complexity']:.2f}")
```

### DebugConsole - Interactive Debugging Interface

```python
import pynlm

# Create debug console
debug_console = pynlm.DebugConsole()

# Enable debugging
debug_console.enable_real_time_logging(True)
debug_console.enable_step_by_step_debugging(True)
debug_console.set_breakpoints([100, 200, 300])  # Break at specific steps

# Connect to simulation
debug_console.attach_simulation(brain, world, agent)

# Start interactive debugging
debug_console.start()

# Run simulation with debugging
simulation = debug_console.run_simulation(1000)

# Analyze debugging session
debug_report = debug_console.generate_debug_report()
print(f"Debug analysis: {debug_report['summary']}")

# Get error trace if any
if debug_report['errors']:
    debug_console.print_error_trace(debug_report['errors'][0])
```

### SystemStateManager - State Management and Persistence

```python
import pynlm
import json

# Create state manager
state_manager = pynlm.SystemStateManager()

# Configure state management
state_manager.enable_checkpointing(True)
state_manager.set_checkpoint_interval(100)
state_manager.enable_compression(True)

# Track system state
state_manager.capture_initial_state(brain, world, agent)

# Run simulation with state tracking
for step in range(1000):
    # Your simulation code
    
    # Periodic checkpoint
    if step % 100 == 0:
        checkpoint = state_manager.create_checkpoint(step, brain, world, agent)
        state_manager.save_checkpoint(f"checkpoint_step_{step}.nlm", checkpoint)

# Restore from checkpoint if needed
if state_manager.checkpoint_exists("checkpoint_step_500.nlm"):
    saved_state = state_manager.load_checkpoint("checkpoint_step_500.nlm")
    state_manager.restore_state(saved_state, brain, world, agent)

# Generate state analysis
state_analysis = state_manager.analyze_state_transitions()
print(f"State transition patterns: {state_analysis['patterns']}")
```

## Advanced Monitoring Features

### 1. Multi-Level Logging

```python
# Set up comprehensive logging
logger = pynlm.create_logger("nlm_simulation")
logger.set_level(pynlm.LogLevel.Trace)
logger.enable_file_logging("simulation.log")
logger.enable_console_logging(True)
logger.set_log_format(
    "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
)

# Log different types of events
logger.log_neural_event("spike", neuron_id, timestamp)
logger.log_plasticity_event("weight_change", synapse_id, delta)
logger.log_world_event("action", action_type, parameters)
logger.log_agent_event("decision", action, reward)
```

### 2. Performance Benchmarking

```python
import pynlm

# Create benchmark suite
benchmark_suite = pynlm.PerformanceBenchmarkSuite()

# Define test scenarios
scenarios = [
    ("small_brain", {"neuron_count": 100, "region_count": 2}),
    ("medium_brain", {"neuron_count": 1000, "region_count": 4}),
    ("large_brain", {"neuron_count": 10000, "region_count": 8}),
]

# Run benchmarks
for name, config in scenarios:
    benchmark_suite.run_scenario(
        name,
        neuron_count=config["neuron_count"],
        region_count=config["region_count"],
        steps=500
    )

# Analyze benchmarks
results = benchmark_suite.get_benchmark_results()
for name, result in results.items():
    print(f"Scenario {name}:")
    print(f"  Average step time: {result['avg_step_time_ms']:.2f} ms")
    print(f"  Neurons per second: {result['neurons_per_second']:.0f}")
    print(f"  Memory efficiency: {result['memory_efficiency']:.2f}")
```

### 3. Neural Activity Analysis

```python
import pynlm
import numpy as np

# Create neural analyzer
analyzer = pynlm.NeuralActivityAnalyzer(brain)

# Analyze different aspects
firing_patterns = analyzer.analyze_firing_patterns()
spike_train_stats = analyzer.analyze_spike_trains()
connectivity_analysis = analyzer.analyze_connectivity()
plasticity_metrics = analyzer.analyze_plasticity()

# Get detailed statistics
stats = {
    "firing_rate_distribution": firing_patterns['firing_rate_distribution'],
    "spike_timing": spike_train_stats['interspike_intervals'],
    "connection_strength": connectivity_analysis['weight_distribution'],
    "learning_efficiency": plasticity_metrics['learning_rate']
}

print(f"Firing rate standard deviation: {np.std(stats['firing_rate_distribution']):.2f}")
print(f"Spike timing coefficient of variation: {np.std(stats['spike_timing']) / np.mean(stats['spike_timing']):.2f}")
```

### 4. Error Analysis and Debugging

```python
import pynlm

# Create error analyzer
error_analyzer = pynlm.ErrorAnalyzer()

# Configure error detection
error_analyzer.enable_pattern_detection(True)
error_analyzer.enable_temporal_analysis(True)
error_analyzer.set_error_threshold(0.01)

# Run simulation and collect errors
simulation = run_simulation_with_errors(brain, world, agent)
errors = error_analyzer.analyze_errors(simulation)

# Get error classification
error_classification = error_analyzer.classify_errors(errors)
print(f"Error types: {error_classification['types']}")
print(f"Most frequent error: {error_classification['most_frequent']}")

# Generate error report
error_report = error_analyzer.generate_error_report(errors)
print(error_report)
```

## Integration Examples

### Complete Monitoring Setup

```python
import pynlm
import time
import threading

def run_monitored_simulation():
    # Initialize monitoring systems
    monitor = pynlm.NLMMonitor()
    monitor.set_log_level(pynlm.LogLevel.Info)
    
    # Configure specialized trackers
    neural_tracker = pynlm.NeuralActivityTracker()
    perf_profiler = pynlm.PerformanceProfiler("simulation")
    
    # Create simulation
    config = pynlm.create_preset_config("advanced")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enable_all_systems()
    
    # Start monitoring
    monitor.start()
    neural_tracker.start()
    perf_profiler.start()
    
    # Run simulation with detailed monitoring
    start_time = time.time()
    
    for step in range(1000):
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step, step * 0.001)
        
        # Agent decision making
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record performance
        step_time = (time.time() - step_start) * 1000
        perf_profiler.record_operation("simulation_step", step_time)
        
        # Capture neural activity
        neural_tracker.capture_snapshot(step)
        
        # Apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.001)
        
        # Periodic checkpoints and analysis
        if step % 100 == 0:
            monitor.capture_checkpoint(step, brain, world, agent)
            neural_tracker.analyze_patterns(step)
            perf_profiler.analyze_bottlenecks()
    
    total_time = time.time() - start_time
    
    # Stop monitoring
    monitor.stop()
    neural_tracker.stop()
    perf_profiler.stop()
    
    # Generate comprehensive report
    comprehensive_report = {
        "simulation_summary": {
            "total_steps": 1000,
            "total_time_seconds": total_time,
            "steps_per_second": 1000 / total_time,
            "brain_neurons": brain.getTotalNeuronCount(),
            "world_width": world.getWidth(),
            "world_height": world.getHeight()
        },
        "performance_analysis": perf_profiler.get_performance_analysis(),
        "neural_analysis": neural_tracker.get_neural_analysis(),
        "monitoring_stats": monitor.get_system_stats(),
        "recommendations": generate_recommendations(perf_profiler, neural_tracker, monitor)
    }
    
    return comprehensive_report

# Run the monitored simulation
results = run_monitored_simulation()

# Save results to file
with open("simulation_results.json", "w") as f:
    json.dump(results, f, indent=2)

print("Simulation monitoring complete!")
print(f"Performance: {results['simulation_summary']['steps_per_second']:.2f} steps/second")
print(f"Neural activity: {results['neural_analysis']['activity_metrics']['avg_firing_rate']:.2f} Hz")
```

## Monitoring Configuration

### NLMMonitor Configuration

```python
monitor_config = {
    "log_level": pynlm.LogLevel.Debug,
    "neural_activity_logging": True,
    "performance_profiling": True,
    "max_history": 10000,
    "checkpoint_interval": 100,
    "compression_enabled": True,
    "real_time_reporting": True,
    "error_detection": True,
    "pattern_analysis": True,
    "baseline_establishment": True
}
```

### NeuralActivityTracker Configuration

```python
tracker_config = {
    "sampling_rate": 10,  # Hz
    "min_firing_rate": 0.01,
    "max_history": 5000,
    "analyze_patterns": True,
    "detect_transients": True,
    "calculate_statistics": True,
    "export_data": True
}
```

## API Reference

### NLMMonitor Methods

| Method | Description |
|--------|-------------|
| `start()` | Start monitoring |
| `stop()` | Stop monitoring |
| `capture_checkpoint(step, brain, world, agent)` | Capture system state |
| `generate_report()` | Generate comprehensive monitoring report |
| `get_system_stats()` | Get system statistics |

### NeuralActivityTracker Methods

| Method | Description |
|--------|-------------|
| `start()` | Start neural activity tracking |
| `stop()` | Stop neural activity tracking |
| `capture_snapshot(step)` | Capture neural activity snapshot |
| `analyze_patterns(step)` | Analyze neural patterns |
| `get_neural_analysis()` | Get neural activity analysis |

### PerformanceProfiler Methods

| Method | Description |
|--------|-------------|
| `start()` | Start performance profiling |
| `stop()` | Stop performance profiling |
| `record_operation(name, time_ms)` | Record operation timing |
| `analyze_bottlenecks()` | Analyze performance bottlenecks |
| `get_performance_analysis()` | Get performance analysis |

## Conclusion

These advanced monitoring and debugging tools provide comprehensive capabilities for tracking, analyzing, and optimizing NLM simulations. The integrated monitoring system allows users to:

1. Track neural activity at multiple scales
2. Profile performance and identify bottlenecks
3. Analyze patterns and detect anomalies
4. Manage system state and checkpoints
5. Generate detailed reports and recommendations

By leveraging these tools, users can gain deeper insights into their neural simulations, optimize performance, and ensure reliable operation in production environments.
