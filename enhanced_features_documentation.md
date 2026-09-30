# NLM - Enhanced Features Documentation

**Phase 6: Final Integration with Enhanced Features**

This document describes the enhanced features added to NLM (Neural Learning Machine) Phase 6 implementation.
These improvements provide better Python API, performance monitoring, data analysis tools,
and advanced simulation capabilities.

## Overview of Enhancements

### 1. Enhanced Python API (pynlm_enhanced)

The enhanced Python bindings provide:

#### New Classes

**EnhancedConfig**
- Validates configuration parameters
- Type-safe value setting
- Better error messages

**EnhancedBrain**
- Activity recording (firing history, activity history)
- Enhanced simulation steps with recording
- Batch processing for performance
- Statistics collection
- Auto-save functionality

**EnhancedAgentBrain**
- Learning statistics tracking
- Learning stagnation detection
- Enhanced reward processing with learning signals
- History recording for analysis

**EnhancedSimpleWorld**
- Time series metrics
- Profiling support
- Enhanced update methods

**BrainContext**
- Context manager for automatic resource cleanup
- Auto-save capabilities

#### Key Methods

```python
# Enhanced configuration
config = pynlm.createEnhancedConfig()
config.safeSet("brain.neuron_count", 1000)

# Enhanced brain
brain = pynlm.createEnhancedBrain(config)
brain.enhancedStep(step, time)
brain.batchSteps(start_step, end_step, timestep)
brain.enableAutoSave("checkpoint.bin")

# Enhanced agent
agent = pynlm.createEnhancedAgent(brain)
agent.enhancedApplyRewardModulation(reward, predicted_reward)
agent.getLearningStats()

# Enhanced world
world = pynlm.createEnhancedWorld()
world.enhancedUpdate(timestep)
world.enableProfiling()
world.recordMetric("metric_name", value)

# Context manager
with pynlm.createContext(brain, agent, world) as context:
    context.enableAutoSave("checkpoint.bin")
    # Simulation here
```

### 2. Performance Monitoring

#### PerformanceMonitor
Tracks simulation performance with:

- Step timing metrics
- Custom metrics collection
- Simulation duration tracking
- Performance statistics

```python
monitor = PerformanceMonitor()

for step in range(1000):
    step_start = time.time()
    # Run simulation step
    world.update(0.1)
    agent.processSensoryInput(percept)
    brain.step(step)
    
    step_end = time.time()
    monitor.recordStep(step_end - step_start)
    
    # Add custom metrics
    monitor.addCustomMetric("reward", reward)
    monitor.addCustomMetric("firing_rate", brain.getAverageFiringRate())

metrics = monitor.getMetrics()
print(f"Average step time: {metrics.wallTimePerStep*1000:.2f}ms")
print(f"Steps per second: {1.0/metrics.wallTimePerStep:.1f}")
```

#### Performance Metrics Structure

```python
{
    "cpuUsage": 0.0,                    # CPU usage percentage
    "memoryUsage": 0.0,                # Memory usage (MB)
    "simulationTime": 0.0,             # Total simulation time (s)
    "wallTimePerStep": 0.0,            # Average wall time per step (s)
    "stepsCompleted": 1000,             # Number of completed steps
    "customMetrics": {                 # Additional custom metrics
        "reward": 42.0,
        "firing_rate": 5.2,
        "curiosity": 0.8
    }
}
```

### 3. Data Analysis Tools

#### DataAnalyzer
Comprehensive data analysis utilities:

- Basic statistics (mean, median, mode, variance, std dev)
- Anomaly detection using Z-score
- Time series smoothing (moving average)
- Peak detection
- Integration with simulation data

```python
# Example usage with simulation data
firing_history = brain.getFiringHistory()  # numpy array

# Calculate statistics
stats = DataAnalyzer.calculateStats(firing_history)
print(f"Mean firing rate: {stats.mean:.2f}")
print(f"Standard deviation: {stats.stdDev:.2f}")

# Detect anomalies (spikes)
anomalies = DataAnalyzer.detectAnomalies(firing_history, zThreshold=2.0)
print(f"Found {len(anomalies)} anomalous steps")

# Smooth data
smoothed = DataAnalyzer.smoothTimeSeries(firing_history, windowSize=10)

# Find peaks
peaks = DataAnalyzer.findPeaks(firing_history, threshold=10.0)
print(f"Found {len(peaks)} significant peaks")
```

### 4. Progress Tracking

#### ProgressTracker
Visual progress tracking for long simulations:

```python
progress = ProgressTracker("Running simulation", total_steps=10000)

for step in range(total_steps):
    # Run simulation step
    world.update(0.1)
    agent.processSensoryInput(percept)
    brain.step(step)
    
    progress.increment()
    
progress.showFinalProgress()
```

Features:
- Configurable message
- ETA calculation
- Percentage display
- Optional disable for silent operation

### 5. Experiment Management

#### ExperimentRunner
Manages multiple simulation experiments:

```python
runner = ExperimentRunner()

# Define experiment
def curiosity_experiment(brain, agent, world):
    """Experiment testing curiosity-driven behavior"""
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Run simulation with curiosity enabled
    agent.enableCuriosity(True)
    
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        # Collect metrics
        if step % 100 == 0:
            print(f"Step {step}: {agent.getCuriosityLevel():.2f}")

# Run experiment
runner.runExperiment("Curiosity Experiment", curiosity_experiment)

# Get results
results = runner.getResults()
for result in results:
    print(f"\nExperiment: {result.name}")
    print(f"Success: {result.success}")
    print(f"Total Reward: {result.totalReward}")
    print(f"Final Curiosity: {result.finalCuriosity}")
    print(f"Simulation Time: {result.totalTime:.2f}s")

runner.printSummary()
```

### 6. Enhanced Simulation Runner

#### EnhancedSimulationRunner
Comprehensive simulation framework with advanced features:

```python
# Configuration
config = EnhancedSimulationRunner.SimulationConfig()
config.steps = 1000
config.timestep = 0.05
config.name = "My Simulation"
config.enableProgress = True
config.enableProfiling = True
config.recordData = True

# Simulation step function
def simulation_step(step, time):
    world.update(time)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step, time)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, time)
    
    reward = compute_reward(percept)
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(time)

# Initialization (optional)
def initialize():
    agent.initialize(world)
    agent.enableCuriosity(True)
    brain.logStatus()

# Reward function (optional)
def reward_function(step):
    return calculate_reward_from_environment(step)

# Run simulation
result = EnhancedSimulationRunner.runSimulation(
    config=config,
    simulation_step=simulation_step,
    initialization=initialize,
    reward_function=reward_function
)

# Check results
if result.success:
    print(f"Simulation completed successfully!")
    print(f"Total reward: {result.totalReward}")
    print(f"Average firing rate: {result.avgFiringRate}")
    print(f"Simulation time: {result.simulationTime:.2f}s")
else:
    print(f"Simulation failed: {result.errorMessage}")
```

### 7. Utility Functions (SimUtils)

Convenience functions for common simulation patterns:

```python
# Agent simulation
agent_result = SimUtils.runAgentSimulation(
    config, brain, agent, world
)

# World simulation
world_result = SimUtils.runWorldSimulation(
    config, world
)

# Memory profiling
memory_result = SimUtils.runMemoryProfilingSimulation(
    config, brain
)
```

### 8. Output and Visualization Utilities

#### OutputUtils
Functions for processing and displaying simulation results:

```python
# Print results to console
OutputUtils.printSimulationResults(result)

# Save results to file
OutputUtils.saveResults(result, "simulation_results.csv")

# Plot results (requires matplotlib)
OutputUtils.plotResults(result)
```

## Integration Examples

### Example 1: Complete Agent with All Features

```python
import pynlm
import time
from pynlm_enhanced import PerformanceMonitor, ExperimentRunner, EnhancedSimulationRunner

def run_comprehensive_agent_simulation():
    """Comprehensive agent simulation using all enhanced features"""
    
    # Performance monitoring
    monitor = PerformanceMonitor()
    
    # Experiment runner
    runner = ExperimentRunner()
    
    # Create components
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Enhanced simulation configuration
    sim_config = EnhancedSimulationRunner.SimulationConfig()
    sim_config.steps = 1000
    sim_config.timestep = 0.05
    sim_config.name = "Comprehensive Agent Simulation"
    sim_config.enableProgress = True
    sim_config.enableProfiling = True
    sim_config.recordData = True
    
    # Simulation step
    def simulation_step(step, time):
        step_start = time.time()
        
        # World update
        world.update(time)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step, time)
        
        # Action and world interaction
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, time)
        
        # Reward and learning
        reward = compute_environment_reward(world, agent)
        agent.applyRewardModulation(reward, 0.0)
        
        # Development
        agent.updateDevelopment(time)
        
        # Record performance
        step_end = time.time()
        monitor.recordStep(step_end - step_start)
        
        # Record metrics
        if step % 100 == 0:
            world.recordMetric("step", step)
            world.recordMetric("reward", reward)
            world.recordMetric("firing_rate", brain.getAverageFiringRate())
            world.recordMetric("curiosity", agent.getCuriosityLevel())
    
    # Run simulation
    result = EnhancedSimulationRunner.runSimulation(
        config=sim_config,
        simulation_step=simulation_step,
        initialization=lambda: agent.enableCuriosity(True)
    )
    
    # Add to experiment
    def experiment_wrapper(brain, agent, world):
        # This would be the simplified experiment for runner
        pass
    
    runner.runExperiment("Comprehensive Simulation", experiment_wrapper)
    
    # Get performance metrics
    metrics = monitor.getMetrics()
    
    # Analyze data
    firing_history = brain.getFiringHistory()
    if not firing_history.empty:
        stats = DataAnalyzer.calculateStats(firing_history)
        anomalies = DataAnalyzer.detectAnomalies(firing_history, zThreshold=2.0)
        
        print(f"Firing rate statistics:")
        print(f"  Mean: {stats.mean:.2f}")
        print(f"  Std Dev: {stats.stdDev:.2f}")
        print(f"  Min: {stats.min:.2f}")
        print(f"  Max: {stats.max:.2f}")
        print(f"  Anomalies detected: {len(anomalies)}")
    
    # Print results
    OutputUtils.printSimulationResults(result)
    print(f"Performance metrics: {metrics.wallTimePerStep*1000:.2f}ms per step")
    
    return result, metrics

# Run comprehensive simulation
results, performance = run_comprehensive_agent_simulation()
```

### Example 2: Experiment Comparison

```python
import pynlm
from pynlm_enhanced import ExperimentRunner

def run_experiments():
    """Run multiple experiments to compare different strategies"""
    
    runner = ExperimentRunner()
    
    # Experiment 1: High curiosity
    def high_curiosity_exp(brain, agent, world):
        world.configure(15, 15, 8, 8)
        world.reset()
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        for step in range(300):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
            
            if step % 50 == 0:
                print(f"Step {step}: Curiosity = {agent.getCuriosityLevel():.2f}")
    
    # Experiment 2: Low curiosity (baseline)
    def low_curiosity_exp(brain, agent, world):
        world.configure(15, 15, 8, 8)
        world.reset()
        agent.enableCuriosity(False)
        agent.enableDevelopment(True)
        
        for step in range(300):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
            
            if step % 50 == 0:
                print(f"Step {step}: Curiosity = {agent.getCuriosityLevel():.2f}")
    
    # Run experiments
    runner.runExperiment("High Curiosity Strategy", high_curiosity_exp)
    runner.runExperiment("Low Curiosity Strategy", low_curiosity_exp)
    
    # Compare results
    results = runner.getResults()
    print("\n=== Experiment Comparison ===")
    
    for result in results:
        print(f"\n{result.name}:")
        print(f"  Success: {result.success}")
        print(f"  Total Reward: {result.totalReward:.2f}")
        print(f"  Final Curiosity: {result.finalCuriosity:.3f}")
        print(f"  Simulation Time: {result.totalTime:.2f}s")
    
    runner.printSummary()
    
    # Find best experiment
    successful_results = [r for r in results if r.success]
    if successful_results:
        best = max(successful_results, key=lambda x: x.totalReward)
        print(f"\nBest performing experiment: {best.name}")
        print(f"Reward: {best.totalReward:.2f}")

# Run experiment comparison
run_experiments()
```

### Example 3: Performance Benchmarking

```python
import pynlm
from pynlm_enhanced import PerformanceMonitor
import time

def benchmark_performance():
    """Benchmark different simulation configurations"""
    
    configurations = [
        {"neurons": 100, "regions": 1, "steps": 500},
        {"neurons": 500, "regions": 2, "steps": 1000},
        {"neurons": 1000, "regions": 3, "steps": 2000}
    ]
    
    print("Performance Benchmarking")
    print("========================")
    
    for config in configurations:
        print(f"\nConfiguration: {config['neurons']} neurons, {config['regions']} regions")
        
        # Setup
        nlm_config = pynlm.createDefaultConfig()
        nlm_config.set("brain.neuron_count", config["neurons"])
        nlm_config.set("brain.region_count", config["regions"])
        
        brain = pynlm.createBrain(nlm_config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Performance monitoring
        monitor = PerformanceMonitor()
        
        # Run simulation
        start_time = time.time()
        
        for step in range(config["steps"]):
            step_start = time.time()
            
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
            
            step_end = time.time()
            monitor.recordStep(step_end - step_start)
        
        total_time = time.time() - start_time
        
        # Get metrics
        metrics = monitor.getMetrics()
        
        # Calculate derived metrics
        steps_per_second = config["steps"] / total_time
        avg_step_time_ms = metrics.wallTimePerStep * 1000
        
        print(f"  Total time: {total_time:.2f}s")
        print(f"  Steps per second: {steps_per_second:.1f}")
        print(f"  Average step time: {avg_step_time_ms:.2f}ms")
        print(f"  Total reward: {metrics.customMetrics.get('reward', 0):.2f}")
        print(f"  Average firing rate: {metrics.customMetrics.get('firing_rate', 0):.2f}")

# Run benchmark
benchmark_performance()
```

## Migration Guide

### From Basic to Enhanced API

#### Basic API
```python
# Old way
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    world.update(0.1)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.1)
```

#### Enhanced API
```python
# New way with enhanced features
from pynlm_enhanced import EnhancedBrain, EnhancedAgentBrain, EnhancedSimulationRunner

brain = pynlm.createBrain(config)
brain.initialize()

# Or use enhanced versions
brain = pynlm.createEnhancedBrain(config)

# Configuration
sim_config = EnhancedSimulationRunner.SimulationConfig()
sim_config.steps = 100

# Run with monitoring
result = EnhancedSimulationRunner.runSimulation(
    config=sim_config,
    simulation_step=lambda step, time: run_step(world, agent, brain, step, time)
)

print(f"Simulation completed with {result.stepsCompleted} steps")
print(f"Success: {result.success}")
```

### Adding New Features to Your Code

1. **Activity Recording**: Use `brain.recordActivity()` and `brain.getFiringHistory()`
2. **Learning Analysis**: Use `agent.getLearningStats()` for detailed analysis
3. **Performance Monitoring**: Use `PerformanceMonitor` for timing and resource tracking
4. **Experiment Management**: Use `ExperimentRunner` for systematic testing
5. **Data Analysis**: Use `DataAnalyzer` for statistical analysis of simulation data
6. **Progress Tracking**: Use `ProgressTracker` for long-running simulations

## Technical Details

### Memory Management

The enhanced features include:

- **Logger Pool**: Reuses logger instances to reduce memory allocation
- **String Interning**: Caches frequently used strings to reduce memory overhead
- **Buffered I/O**: Reduces file system calls for better performance
- **Thread-Local Storage**: Optimizes logging performance for multi-threaded applications

### Performance Optimizations

- **Efficient Timestamp Generation**: Pre-computed epoch and formatted timestamps
- **Optimized Message Formatting**: Uses `snprintf` for faster string operations
- **Batch Processing**: Efficiently handles multiple simulation steps
- **Minimal Locking**: Reduces contention in multi-threaded scenarios

### Error Handling

Enhanced error handling includes:

- **Custom Exception Types**: Specific exceptions for different error conditions
- **Graceful Degradation**: Continues simulation even if some features fail
- **Detailed Error Messages**: Provides context for debugging
- **Safe Defaults**: Uses sensible defaults when configuration is invalid

## Conclusion

The enhanced NLM features provide a comprehensive suite of tools for building, running, and analyzing neural simulations. These features enable:

1. **Better Performance**: Optimized code with monitoring and benchmarking
2. **Enhanced Analysis**: Statistical tools and data visualization
3. **Improved Usability**: High-level APIs and comprehensive documentation
4. **Better Debugging**: Detailed logging and error reporting
5. **Experiment Management**: Systematic testing and comparison

These enhancements make it easier to develop, test, and analyze advanced neural simulations while maintaining backward compatibility with the existing basic API.
