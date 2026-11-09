# NLM Advanced Usage Guide

## Overview

This guide covers advanced usage patterns, simulation management, and comprehensive analysis tools for the NLM (Neural Learning Machine) framework. It builds upon the basics from `easy_usage.md` and introduces sophisticated techniques for research, experimentation, and production use.

## Table of Contents

1. [Simulation Runner](#simulation-runner)
2. [Metrics Collection and Analysis](#metrics-collection-and-analysis)
3. [Advanced Configuration](#advanced-configuration)
4. [Experimentation and Testing](#experimentation-and-testing)
5. [Performance Optimization](#performance-optimization)
6. [Export and Import](#export-and-import)
7. [Production Deployment](#production-deployment)

## 1. Simulation Runner

### Introduction

The `SimulationRunner` class provides a comprehensive interface for managing complex brain-world simulations. It encapsulates the entire simulation loop and provides utilities for recording, analysis, and control.

### Creating a Simulation Runner

```python
import pynlm

# Create all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Create simulation runner
runner = pynlm.createSimulationRunner(brain, world, agent)
```

### Running Episodes

```python
# Run a short episode for testing
print("Running test episode...")
runner.runEpisode(500, headless=False)

# Run a longer episode for experimentation
print("Running full experiment...")
runner.runEpisode(5000, headless=True)
```

### Accessing Results

```python
# Get metrics from the simulation
metrics = runner.getMetrics()

# Access simulation components
brain = runner.getBrain()
world = runner.getWorld()
agent = runner.getAgent()

# Print summary statistics
print(f"Episode completed: {metrics.getStepCount()} steps")
print(f"Mean firing rate: {metrics.getMeanFiringRate():.2f} Hz")
print(f"Max firing rate: {metrics.getMaxFiringRate():.2f} Hz")
print(f"Total spikes: {metrics.getTotalSpikes()}")
```

## 2. Metrics Collection and Analysis

### Introduction

The `MetricsCollector` class provides comprehensive tracking and analysis of neural simulation metrics. It collects data on neural activity, behavior, and system performance.

### Creating Metrics Collector

```python
import pynlm

# Create metrics collector
metrics = pynlm.createMetricsCollector()

# Record metrics for each simulation step
# (typically called from within a simulation loop)
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record metrics
    metrics.recordStep(step, brain, agent)
```

### Available Metrics

#### Neural Activity Metrics

```python
# Get neural activity statistics
stats = metrics.getNeuralActivityStats()
print(f"Mean firing rate: {stats.meanFiringRate:.2f} Hz")
print(f"Mean E/I ratio: {stats.meanExcitatoryInhibitionRatio:.2f}")
print(f"Mean curiosity: {stats.meanCuriosity:.3f}")
print(f"Mean novelty: {stats.meanNovelty:.3f}")
```

#### Performance Metrics

```python
print(f"Mean firing rate: {metrics.getMeanFiringRate():.2f} Hz")
print(f"Max firing rate: {metrics.getMaxFiringRate():.2f} Hz")
print(f"Total spikes: {metrics.getTotalSpikes()}")
print(f"Average action count: {metrics.getAverageActionCount():.2f}")
print(f"Step count: {metrics.getStepCount()}")
```

### Exporting Metrics

```python
# Export metrics to CSV for analysis
metrics.exportCSV("simulation_results.csv")
print("Metrics exported to simulation_results.csv")

# Import metrics for analysis
import pandas as pd
df = pd.read_csv("simulation_results.csv")
print(df.head())
```

### Advanced Analysis

```python
# Calculate derived metrics
mean_firing = metrics.getMeanFiringRate()
max_firing = metrics.getMaxFiringRate()
spike_count = metrics.getTotalSpikes()

# Determine if simulation was stable
firing_stability = 1.0 - (max_firing - mean_firing) / max_firing if max_firing > 0 else 1.0
print(f"Neural stability: {firing_stability:.3f}")

# Check for learning
if spike_count > 100:
    print("✓ Learning detected: High spike count indicates neural activity")
else:
    print("⚠ Low activity: Consider increasing simulation time or adjusting parameters")
```

## 3. Advanced Configuration

### Training Configurations

The `createTrainingConfig` function provides pre-configured settings for different types of neural network training scenarios.

```python
import pynlm

# Create a standard training configuration
training_config = pynlm.createTrainingConfig(
    numNeurons=1000,
    numSteps=1000,
    learningRate=0.001
)

# Create a high-performance training configuration
performance_config = pynlm.createTrainingConfig(
    numNeurons=5000,
    numSteps=5000,
    learningRate=0.0001
)

# Modify configuration parameters
if training_config.has("brain.neuron_count"):
    training_config.set("brain.neuron_count", 2000, ConfigSource.Runtime)
```

### World Configurations

The `createWorldConfig` function provides pre-configured world settings for different environments.

```python
# Create a standard world configuration
world_config = pynlm.createWorldConfig(
    width=20,
    height=20,
    timestep=0.1
)

# Create a detailed simulation world
detailed_config = pynlm.createWorldConfig(
    width=50,
    height=50,
    timestep=0.05
)
detailed_config.set("world.max_energy", 100.0, ConfigSource.Runtime)
detailed_config.set("world.energy_decay_rate", 0.5, ConfigSource.Runtime)
```

### Configuration Templates

```python
# Template for goal-directed behavior
template_config = pynlm.createTrainingConfig(numNeurons=2000, numSteps=2000, learningRate=0.001)
template_config.set("plasticity.stdp.enable", True, ConfigSource.Default)
template_config.set("plasticity.stdp.learning_rate", 0.01, ConfigSource.Default)
template_config.set("neuromod.curiosity.enable", True, ConfigSource.Default)
template_config.set("neuromod.novelty.enable", True, ConfigSource.Default)

# Template for social learning
social_config = pynlm.createTrainingConfig(numNeurons=3000, numSteps=3000, learningRate=0.001)
social_config.set("plasticity.hebbian.enable", True, ConfigSource.Default)
social_config.set("neuromod.dopamine.scale", 0.5, ConfigSource.Default)

# Template for development
dev_config = pynlm.createTrainingConfig(numNeurons=1500, numSteps=1500, learningRate=0.002)
dev_config.set("neuromod.curiosity.enable", True, ConfigSource.Default)
dev_config.set("neuromod.novelty.enable", True, ConfigSource.Default)
dev_config.set("development.enable", True, ConfigSource.Default)
```

## 4. Experimentation and Testing

### Parameterized Experimentation

```python
import pynlm

# Run a parameter sweep
experiments = []

# Different neuron counts
for neuron_count in [500, 1000, 2000, 3000, 5000]:
    config = pynlm.createTrainingConfig(numNeurons=neuron_count, numSteps=500, learningRate=0.001)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Run experiment
    runner = pynlm.createSimulationRunner(brain, world, agent)
    runner.runEpisode(500, headless=True)
    
    metrics = runner.getMetrics()
    experiments.append({
        'neurons': neuron_count,
        'mean_firing': metrics.getMeanFiringRate(),
        'max_firing': metrics.getMaxFiringRate(),
        'total_spikes': metrics.getTotalSpikes()
    })

# Analyze results
print("Neuron Count | Mean Firing | Max Firing | Total Spikes")
print("-" * 60)
for exp in experiments:
    print(f"{exp['neurons']:11d} | {exp['mean_firing']:11.2f} | {exp['max_firing']:11.2f} | {exp['total_spikes']:13d}")
```

### Multi-Episode Experiments

```python
import pynlm

# Run multiple episodes with different conditions
results = []

# Experiment 1: High learning rate
config1 = pynlm.createTrainingConfig(numNeurons=1000, numSteps=100, learningRate=0.01)
brain1 = pynlm.createBrain(config1)
brain1.initialize()
agent1 = pynlm.createAgentBrain(brain1)
world1 = pynlm.createSimpleWorld()
world1.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world1.reset()
agent1.initialize(world1)
runner1 = pynlm.createSimulationRunner(brain1, world1, agent1)
runner1.runEpisode(100, headless=True)
metrics1 = runner1.getMetrics()
results.append({
    'learning_rate': 0.01,
    'mean_firing': metrics1.getMeanFiringRate(),
    'stability': metrics1.getMeanFiringRate() / metrics1.getMaxFiringRate() if metrics1.getMaxFiringRate() > 0 else 1.0
})

# Experiment 2: Low learning rate
config2 = pynlm.createTrainingConfig(numNeurons=1000, numSteps=100, learningRate=0.0001)
brain2 = pynlm.createBrain(config2)
brain2.initialize()
agent2 = pynlm.createAgentBrain(brain2)
world2 = pynlm.createSimpleWorld()
world2.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world2.reset()
agent2.initialize(world2)
runner2 = pynlm.createSimulationRunner(brain2, world2, agent2)
runner2.runEpisode(100, headless=True)
metrics2 = runner2.getMetrics()
results.append({
    'learning_rate': 0.0001,
    'mean_firing': metrics2.getMeanFiringRate(),
    'stability': metrics2.getMeanFiringRate() / metrics2.getMaxFiringRate() if metrics2.getMaxFiringRate() > 0 else 1.0
})

# Analyze comparative results
print("Learning Rate | Mean Firing | Stability")
print("-" * 45)
for res in results:
    print(f"{res['learning_rate']:13.4f} | {res['mean_firing']:11.2f} | {res['stability']:9.3f}")
```

## 5. Performance Optimization

### Batch Processing

```python
import pynlm

# Process multiple configurations efficiently
import time

start_time = time.time()

# Batch process multiple neuron counts
configs = []
for neurons in [500, 1000, 2000]:
    config = pynlm.createTrainingConfig(numNeurons=neurons, numSteps=100, learningRate=0.001)
    configs.append(config)

print(f"Created {len(configs)} configurations in {time.time() - start_time:.2f} seconds")
```

### Memory Management

```python
import pynlm

# Use context managers for automatic cleanup
class NLPSimulation:
    def __init__(self, config):
        self.config = config
        self.brain = None
        self.agent = None
        self.world = None
        self.runner = None
    
    def __enter__(self):
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        self.world.reset()
        self.agent.initialize(self.world)
        self.runner = pynlm.createSimulationRunner(self.brain, self.world, self.agent)
        return self
    
    def run(self, num_steps=100):
        if self.runner:
            self.runner.runEpisode(num_steps, headless=True)
        return self.runner.getMetrics() if self.runner else None
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup resources
        if self.runner:
            metrics = self.runner.getMetrics()
            if metrics:
                metrics.reset()
        self.brain = None
        self.agent = None
        self.world = None
        self.runner = None

# Use the simulation manager
with NLPSimulation(pynlm.createTrainingConfig(numNeurons=1000, numSteps=100, learningRate=0.001)) as sim:
    metrics = sim.run(100)
    if metrics:
        print(f"Simulation completed with {metrics.getStepCount()} steps")
```

## 6. Export and Import

### Checkpoint Management

```python
import pynlm

# Create and save a brain checkpoint
config = pynlm.createTrainingConfig(numNeurons=1000, numSteps=100, learningRate=0.001)
brain = pynlm.createBrain(config)
brain.initialize()

# Save brain state
brain.save("my_brain_checkpoint.bin")
print("Brain checkpoint saved")

# Load brain state into a new brain
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("my_brain_checkpoint.bin")
print("Brain state loaded from checkpoint")
```

### Metrics Export

```python
import pynlm

# Collect metrics over time
metrics_collector = pynlm.createMetricsCollector()

for episode in range(10):
    # Run simulation (simplified)
    config = pynlm.createTrainingConfig(numNeurons=500, numSteps=50, learningRate=0.001)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # ... setup world and agent ...
    
    # Record metrics for each step
    for step in range(50):
        metrics_collector.recordStep(step, brain, None)  # agent would be needed for real metrics
    
    # Save episode metrics
    metrics_collector.exportCSV(f"episode_{episode}_metrics.csv")
    metrics_collector.reset()

print("All episode metrics saved to CSV files")
```

## 7. Production Deployment

### Automated Testing Framework

```python
import pynlm

class NLMTestFramework:
    def __init__(self):
        self.results = []
    
    def run_test(self, test_name, config, num_steps=100):
        print(f"Running test: {test_name}")
        
        start_time = time.time()
        
        # Create simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        agent.initialize(world)
        
        runner = pynlm.createSimulationRunner(brain, world, agent)
        runner.runEpisode(num_steps, headless=True)
        
        metrics = runner.getMetrics()
        
        # Calculate test results
        result = {
            'test_name': test_name,
            'mean_firing': metrics.getMeanFiringRate(),
            'max_firing': metrics.getMaxFiringRate(),
            'total_spikes': metrics.getTotalSpikes(),
            'stability': metrics.getMeanFiringRate() / metrics.getMaxFiringRate() if metrics.getMaxFiringRate() > 0 else 1.0,
            'duration': time.time() - start_time,
            'step_count': metrics.getStepCount()
        }
        
        self.results.append(result)
        return result
    
    def run_regression_tests(self):
        print("Running regression tests...")
        
        # Test 1: Default configuration
        config1 = pynlm.createDefaultConfig()
        self.run_test("DefaultConfig", config1, 50)
        
        # Test 2: Training configuration
        config2 = pynlm.createTrainingConfig(numNeurons=500, numSteps=50, learningRate=0.001)
        self.run_test("TrainingConfig", config2, 50)
        
        # Test 3: World configuration
        config3 = pynlm.createWorldConfig(width=10, height=10, timestep=0.1)
        self.run_test("WorldConfig", config3, 50)
        
        # Analyze results
        self.analyze_results()
    
    def analyze_results(self):
        print("\nTest Results Summary:")
        print("Test Name      | Mean Firing | Stability | Duration (s)")
        print("-" * 70)
        for result in self.results:
            print(f"{result['test_name']:15s} | {result['mean_firing']:11.2f} | {result['stability']:9.3f} | {result['duration']:11.2f}")
        
        # Check for regressions
        avg_firing = sum(r['mean_firing'] for r in self.results) / len(self.results)
        avg_stability = sum(r['stability'] for r in self.results) / len(self.results)
        
        print(f"\nAverage firing rate: {avg_firing:.2f} Hz")
        print(f"Average stability: {avg_stability:.3f}")
        
        # Determine if all tests passed
        all_passed = all(r['stability'] > 0.1 and r['step_count'] == 50 for r in self.results)
        if all_passed:
            print("✓ All regression tests passed")
        else:
            print("⚠ Some regression tests failed")

# Run the test framework
framework = NLMTestFramework()
framework.run_regression_tests()
```

### Continuous Integration

```python
# For CI/CD pipelines, you might want to run quick validation tests
def quick_validation():
    """Quick validation that the basic functionality works"""
    import pynlm
    
    # Test basic creation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic simulation
    for i in range(10):
        brain.step(i)
    
    # Verify brain is functional
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    
    print("✓ Quick validation passed")
    return True

if __name__ == "__main__":
    quick_validation()
```

## Conclusion

This advanced usage guide provides comprehensive examples and patterns for working with the NLM framework at scale. The helper functions (`SimulationRunner`, `MetricsCollector`, and configuration builders) significantly simplify complex simulation tasks and enable researchers and developers to focus on experimentation rather than boilerplate code.

The examples demonstrate:

- **Comprehensive simulation management** through `SimulationRunner`
- **Detailed metrics collection and analysis** through `MetricsCollector`
- **Advanced configuration management** through specialized configuration builders
- **Robust testing and validation** frameworks
- **Production-ready deployment patterns**

These improvements make the NLM framework more accessible to users with varying levels of expertise while maintaining the power and flexibility needed for advanced research applications.