# NLM Experiment Management Framework Documentation

## Overview

The NLM Experiment Management Framework provides a comprehensive system for running, managing, and analyzing neural learning machine experiments. It includes experiment configuration, metrics tracking, experiment running, and result comparison capabilities.

This framework is designed for:
- **Simulation Research** - Easy setup of different experimental conditions
- **Hyperparameter Tuning** - Support for parameter sweeps
- **Benchmarking** - Performance measurement
- **Reproducible Research** - Checkpoint saving/loading
- **Ablation Studies** - System component testing
- **Comparative Analysis** - Multiple experiment comparison

---

## Core Components

### 1. ExperimentConfig

**Purpose**: Configuration for neural learning machine experiments with validation and serialization.

**Key Features**:
- Comprehensive configuration options
- Validation and error reporting
- File I/O support
- Configuration merging
- Factory methods for common configurations

**Available Factory Methods**:
- `createQuickTestConfig()` - Minimal configuration for development/testing
- `createTrainingConfig()` - Training experiment configuration
- `createBenchmarkConfig()` - Performance benchmarking configuration
- `createAblationConfig()` - System component ablation studies

**Python API**:
```python
import pynlm

# Create quick test config
config = pynlm.ExperimentConfig.createQuickTestConfig()

# Create training config
config = pynlm.ExperimentConfig.createTrainingConfig()
config.setNeuronCount(500)
config.setMaxSteps(10000)

# Create benchmark config
config = pynlm.ExperimentConfig.createBenchmarkConfig()
config.setNeuronCount(1000)
config.setMaxSteps(5000)

# Create ablative config
config = pynlm.ExperimentConfig.createAblationConfig()
config.disableDevelopment()
config.disableCuriosity()

# Set configuration values
config.setName("my_experiment")
config.setNeuronCount(200)
config.setMaxSteps(2000)

# Validate configuration
if not config.validate():
    print(f"Config error: {config.validateAndGetError()}")

# Save configuration
config.saveToFile("experiment_config.json")

# Load configuration
config.loadFromFile("experiment_config.json")

# Merge configurations
config.mergeWith(other_config)
```

**Key Methods**:
- `getName()`, `setName()` - Experiment name
- `getMaxSteps()`, `setMaxSteps()` - Maximum simulation steps
- `getNeuronCount()`, `setNeuronCount()` - Number of neurons
- `getRegionCount()`, `setRegionCount()` - Number of brain regions
- `getConnectionProbability()`, `setConnectionProbability()` - Connection probability
- `getEnableCheckpointing()`, `setEnableCheckpointing()` - Checkpointing enable/disable
- `getEnableReplay()`, `setEnableReplay()` - Experience replay enable/disable
- `getEnableDevelopment()`, `setEnableDevelopment()` - Development enable/disable
- `getCheckpointPath()`, `setCheckpointPath()` - Checkpoint file path
- `getSeed()`, `setSeed()` - Random seed
- `validate()` - Configuration validation
- `validateAndGetError()` - Get validation error message

### 2. ExperimentMetrics

**Purpose**: Comprehensive metrics tracking for neural learning machine experiments.

**Key Features**:
- Real-time metrics collection
- Multiple metric types
- Time series analysis
- Statistical analysis
- Export/import support
- Comparison capabilities
- Snapshot functionality

**Python API**:
```python
import pynlm

# Create metrics tracker
metrics = pynlm.ExperimentMetrics()

# Record step data
metrics.recordStep(step, reward, firing_rate, active_neurons, firing_neurons, total_spikes)

# Record episode data
metrics.recordEpisode(total_reward, avg_firing_rate, total_active_neurons, total_firing_neurons, total_spikes)

# Record individual metrics
metrics.recordReward(reward)
metrics.recordFiringRate(firing_rate)
metrics.recordActiveNeurons(count)
metrics.recordFiringNeurons(count)
metrics.recordTotalSpikes(spikes)
metrics.recordNeuromodulation(curiosity, novelty, dopamine)
metrics.recordSteps(steps)

# Get metrics
print(f"Total reward: {metrics.getTotalReward()}")
print(f"Average firing rate: {metrics.getAverageFiringRate()}")
print(f"Average reward per step: {metrics.getAverageRewardPerStep()}")
print(f"Total steps: {metrics.getTotalSteps()}")
print(f"Curiosity level: {metrics.getCuriosityLevel()}")
print(f"Novelty level: {metrics.getNoveltyLevel()}")
print(f"Dopamine level: {metrics.getDopamineLevel()}")

# Get step-specific metrics
step_data = metrics.getStepMetrics(step)

# Get all step metrics
all_step_data = metrics.getAllStepMetrics()

# Statistical analysis
print(f"Reward mean: {metrics.getRewardMean()}")
print(f"Reward std dev: {metrics.getRewardStdDev()}")
print(f"Firing rate mean: {metrics.getFiringRateMean()}")
print(f"Firing rate std dev: {metrics.getFiringRateStdDev()}")

# Create snapshot
snapshot = metrics.createSnapshot()

# Get summary report
report = metrics.getSummaryReport()
print(report)

# Compare with other metrics
comparison = metrics.compareWith(other_metrics, "total_reward")

# Export/import
metrics.exportToFile("metrics.json")
metrics.importFromFile("metrics.json")

# Reset/clear
metrics.reset()
metrics.clear()
```

**Key Methods**:
- `recordStep()` - Record step-specific metrics
- `recordEpisode()` - Record episode summary metrics
- `recordReward()` - Record reward value
- `recordFiringRate()` - Record firing rate
- `recordActiveNeurons()` - Record active neuron count
- `recordFiringNeurons()` - Record firing neuron count
- `recordTotalSpikes()` - Record total spike count
- `recordNeuromodulation()` - Record neuromodulation levels
- `recordSteps()` - Record time steps
- `getTotalReward()` - Get total reward accumulated
- `getAverageFiringRate()` - Get average firing rate
- `getAverageRewardPerStep()` - Get average reward per step
- `getTotalSteps()` - Get total number of steps
- `getStepMetrics()` - Get metrics for specific step
- `getAllStepMetrics()` - Get all step metrics
- `getSummaryReport()` - Get formatted summary report
- `compareWith()` - Compare metrics with other metrics

### 3. ExperimentRunner

**Purpose**: Core experiment running framework with comprehensive experiment management capabilities.

**Key Features**:
- Automatic experiment setup and initialization
- Episode-based simulation execution
- Multi-episode experiment running
- Checkpoint saving and loading
- Thread-safe operations
- Comprehensive metrics collection
- Parameter validation
- Error handling
- Progress tracking
- Component management

**Python API**:
```python
import pynlm

# Create experiment runner
runner = pynlm.createExperimentRunner(config)

# Setup experiment
runner.setup(config, brain, agent, world, environment)

# Run single episode
reward = runner.runEpisode()

# Run multiple episodes
runner.runExperiment(maxEpisodes=100)

# Component management
runner.initializeComponents()
runner.resetComponents()
runner.cleanupComponents()

# Checkpoint management
runner.saveCheckpoint("checkpoint.json")
runner.loadCheckpoint("checkpoint.json")

# State management
runner.pause()
runner.resume()
runner.isRunning()

# Results and metrics
metrics = runner.getExperimentMetrics()
current_metrics = runner.getCurrentMetrics()
summary = runner.getResultsSummary()

# Configuration access
config = runner.getConfig()
runner.setConfig(config)

# Component access
brain = runner.getBrain()
agent = runner.getAgent()
world = runner.getWorld()
environment = runner.getEnvironment()

# Progress tracking
progress = runner.getProgress()
current_episode = runner.getCurrentEpisode()
total_episodes = runner.getTotalEpisodes()

# Validation and error handling
is_valid = runner.validateSetup()
error_message = runner.validateAndGetError()

# Thread safety
runner.lock()
runner.unlock()
runner.isThreadSafe()
```

**Key Methods**:
- `setup()` - Setup experiment with configuration and components
- `runEpisode()` - Run single episode
- `runExperiment()` - Run multiple episodes
- `initializeComponents()` - Initialize all components
- `resetComponents()` - Reset all components
- `cleanupComponents()` - Cleanup all components
- `saveCheckpoint()` - Save experiment state
- `loadCheckpoint()` - Load experiment state
- `pause()` - Pause experiment
- `resume()` - Resume experiment
- `isRunning()` - Check if experiment is running
- `getExperimentMetrics()` - Get experiment metrics
- `getCurrentMetrics()` - Get current metrics
- `getResultsSummary()` - Get formatted results summary
- `getConfig()` - Get experiment configuration
- `setConfig()` - Set experiment configuration
- `getBrain()` - Get brain component
- `getAgent()` - Get agent component
- `getWorld()` - Get world component
- `getEnvironment()` - Get environment component
- `getProgress()` - Get experiment progress (0.0 to 1.0)
- `getCurrentEpisode()` - Get current episode number
- `getTotalEpisodes()` - Get total number of episodes
- `validateSetup()` - Validate experiment setup
- `validateAndGetError()` - Get setup validation error

### 4. Phase6Experiment

**Purpose**: Phase 6 integration test class that validates all NLM system components and their integration.

**Key Features**:
- Comprehensive system integration testing
- Memory system testing
- Neuromodulation system testing
- Checkpoint testing
- Replay system testing
- Performance benchmarking
- Integration validation

**Python API**:
```python
import pynlm

# Create Phase 6 experiment
phase6 = pynlm.Phase6Experiment()

# Run Phase 6 integration test
config = pynlm.Phase6Config()
config.neuronCount = 500
config.maxSteps = 2000

success, result = pynlm.runPhase6Experiment(config)

print(f"Integration successful: {success}")
print(f"Integration summary: {result.getIntegrationSummary()}")
print(f"Total reward: {result.totalReward}")
print(f"Memory working memory integrated: {result.memoryWorkingMemoryIntegrated}")
print(f"Neuromodulation working: {result.neuromodulationWorking}")
print(f"Development system working: {result.developmentSystemWorking}")
print(f"Checkpointing working: {result.checkpointingWorking}")
print(f"Replay system working: {result.replaySystemWorking}")
```

**Key Methods**:
- `run()` - Run Phase 6 integration test
- `verifyIntegration()` - Verify system integration
- `testMemoryIntegration()` - Test memory system
- `testNeuromodulationIntegration()` - Test neuromodulation
- `testCheckpointing()` - Test checkpoint functionality
- `testReplay()` - Test replay system

### 5. Convenience Functions

**Purpose**: High-level convenience functions for common experiment management tasks.

**Available Functions**:
- `createExperimentRunner(config)` - Factory function to create ExperimentRunner
- `runPhase6Experiment(config)` - Run Phase 6 experiment
- `compareExperiments(experiments, metric_name)` - Compare multiple experiments
- `createQuickTestBrain()` - Create a quick test brain for development and testing
- `createTrainingEnvironment()` - Create a complete training environment
- `createEvaluationEnvironment()` - Create an evaluation environment with advanced features

**Python API**:
```python
import pynlm

# Create quick test brain
brain = pynlm.createQuickTestBrain()
brain.initialize()

# Create training environment
brain, world, agent = pynlm.createTrainingEnvironment()
brain.initialize()

# Create evaluation environment
brain, world, agent = pynlm.createEvaluationEnvironment()
brain.initialize()

# Create experiment runner
config = pynlm.ExperimentConfig.createTrainingConfig()
runner = pynlm.createExperimentRunner(config)

# Run Phase 6 experiment
config = pynlm.Phase6Config()
success, result = pynlm.runPhase6Experiment(config)

# Compare experiments
experiments = [runner1, runner2, runner3]
comparisons = pynlm.compareExperiments(experiments, "total_reward")
```

---

## Usage Examples

### Example 1: Simple Experiment Setup

```python
import pynlm

# Create configuration
config = pynlm.ExperimentConfig.createQuickTestConfig()
config.setName("simple_test")
config.setNeuronCount(100)
config.setMaxSteps(1000)

# Validate configuration
if not config.validate():
    print(f"Config error: {config.validateAndGetError()}")
    exit(1)

# Create metrics tracker
metrics = pynlm.ExperimentMetrics()

# Create experiment runner
runner = pynlm.createExperimentRunner(config)

# Setup (would need actual components)
# runner.setup(config, brain, agent, world, environment)

print("Experiment setup complete!")
```

### Example 2: Training Experiment

```python
import pynlm

# Create training environment
brain, world, agent = pynlm.createTrainingEnvironment()
brain.initialize()

# Create experiment configuration
config = pynlm.ExperimentConfig.createTrainingConfig()
config.setNeuronCount(500)
config.setMaxSteps(5000)
config.setName("training_experiment")

# Create experiment runner
runner = pynlm.createExperimentRunner(config)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run training experiment
runner.runExperiment(maxEpisodes=50)

# Get results
metrics = runner.getExperimentMetrics()
print(f"Training complete!")
print(f"Total reward: {metrics.getTotalReward()}")
print(f"Average reward per episode: {metrics.getAverageRewardPerStep()}")
print(f"Best reward: {metrics.getBestReward()}")
```

### Example 3: Comparative Experiment Analysis

```python
import pynlm

# Create multiple experiment configurations
configs = [
    pynlm.ExperimentConfig.createQuickTestConfig(),
    pynlm.ExperimentConfig.createTrainingConfig(),
    pynlm.ExperimentConfig.createBenchmarkConfig(),
]

# Set different parameters for each config
configs[0].setName("quick_test")
configs[0].setNeuronCount(100)
configs[0].setMaxSteps(500)

configs[1].setName("medium_training")
configs[1].setNeuronCount(500)
configs[1].setMaxSteps(5000)

configs[2].setName("large_benchmark")
configs[2].setNeuronCount(1000)
configs[2].setMaxSteps(10000)

# Create experiment runners
runners = []
for i, config in enumerate(configs):
    runner = pynlm.createExperimentRunner(config)
    runners.append(runner)
    print(f"Created runner {i+1}: {config.getName()}")

# Compare experiments (would need actual experiments)
# experiments = [runner1, runner2, runner3]
# comparisons = pynlm.compareExperiments(experiments, "total_reward")

# Print comparison results
# print(f"Comparison results: {comparisons}")
```

### Example 4: Phase 6 Integration Test

```python
import pynlm

# Create Phase 6 configuration
config = pynlm.Phase6Config()
config.neuronCount = 300
config.maxSteps = 3000

# Run Phase 6 experiment
print("Running Phase 6 integration test...")
success, result = pynlm.runPhase6Experiment(config)

# Check results
print(f"Integration successful: {success}")
print(f"Integration summary: {result.getIntegrationSummary()}")

# Check individual system integration
print(f"Memory system: {'Working' if result.memoryWorkingMemoryIntegrated else 'Failed'}")
print(f"Neuromodulation: {'Working' if result.neuromodulationWorking else 'Failed'}")
print(f"Development system: {'Working' if result.developmentSystemWorking else 'Failed'}")
print(f"Checkpointing: {'Working' if result.checkpointingWorking else 'Failed'}")
print(f"Replay system: {'Working' if result.replaySystemWorking else 'Failed'}")

# Print performance metrics
print(f"Total reward: {result.totalReward}")
print(f"Total steps: {result.totalSteps}")
print(f"Memory efficiency: {result.memoryEfficiency:.3f}")
print(f"Neuromodulation efficiency: {result.neuromodulationEfficiency:.3f}")
```

---

## Best Practices

### Configuration Management
```python
# Always validate configurations
config = pynlm.ExperimentConfig.createTrainingConfig()
if not config.validate():
    raise ValueError(config.validateAndGetError())

# Use descriptive names for experiments
config.setName("experiment_v1_training_1000_neurons")

# Set appropriate random seeds for reproducibility
config.setSeed(42)
```

### Metrics Collection
```python
# Record metrics at appropriate times
metrics.recordStep(step, reward, firing_rate, active_neurons, firing_neurons, total_spikes)

# Record episode summary at the end
metrics.recordEpisode(total_reward, avg_firing_rate, total_active_neurons, total_firing_neurons, total_spikes)

# Export metrics for long-running experiments
if step % 1000 == 0:
    metrics.exportToFile(f"metrics_step_{step}.json")
```

### Error Handling
```python
import pynlm

def safe_experiment_run(runner):
    """Safely run an experiment with error handling."""
    try:
        runner.runExperiment(maxEpisodes=100)
        return True
    except pynlm.NLMRuntimeError as e:
        print(f"NLM Error: {e}")
        return False
    except MemoryError:
        print("Out of memory - reduce experiment size")
        return False
    except KeyboardInterrupt:
        print("Experiment interrupted by user")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False
```

### Resource Management
```python
# Always cleanup resources
runner = pynlm.createExperimentRunner(config)
try:
    # Your experiment code here
    runner.runExperiment(maxEpisodes=50)
finally:
    # Cleanup
    runner.cleanupComponents()
```

---

## Integration with Other NLM Components

The experiment management framework integrates seamlessly with other NLM components:

### Integration with Brain
- Automatic metrics collection from brain performance
- Checkpoint saving/loading support
- Developmental stage tracking
- Neuromodulation system integration

### Integration with World
- Environment interaction metrics
- Reward calculation integration
- Sensory input processing
- Action application tracking

### Integration with Agent
- Learning subsystem tracking
- Neuromodulation level monitoring
- Development stage integration
- Curiosity and novelty tracking

### Integration with Training Systems
- Automatic training loop integration
- Reinforcement learning metrics
- Curriculum learning support
- Experience replay integration

---

## Performance Considerations

### Large-Scale Experiments
```python
# For large-scale experiments, use appropriate configurations
config = pynlm.ExperimentConfig.createBenchmarkConfig()
config.setNeuronCount(2000)  # Larger brain
config.setMaxSteps(20000)   # Longer training

# Enable checkpointing for long experiments
config.setEnableCheckpointing(True)
config.setCheckpointPath("checkpoints/")

# Use multiple experiment runners for parallel experiments
runners = [pynlm.createExperimentRunner(config) for _ in range(4)]
```

### Memory Management
```python
# Monitor memory usage during experiments
perf_metrics = pynlm.generatePerformanceReport(brain)

# Generate memory reports
memory_usage = perf_metrics.getMemoryUsage()
print(f"Memory usage: {memory_usage} MB")

# Export performance metrics
perf_metrics.exportToFile("performance_report.json")
```

---

## Conclusion

The NLM Experiment Management Framework provides a comprehensive and flexible system for running, managing, and analyzing neural learning machine experiments. It includes:

- **Complete experiment lifecycle management** - From setup to cleanup
- **Comprehensive metrics tracking** - Real-time and summary metrics
- **Integration with all NLM systems** - Brain, world, agent, and training systems
- **Error handling and recovery** - Robust error handling
- **Performance monitoring** - System and experiment performance tracking
- **Reproducibility support** - Checkpointing and configuration validation
- **Comparison capabilities** - Multiple experiment comparison
- **High-level convenience APIs** - Simplified experiment setup

This framework enables researchers and developers to:
- Design and run complex neural learning experiments
- Analyze experiment results effectively
- Compare different experimental conditions
- Reproduce and validate experiments
- Monitor system performance
- Optimize experiment parameters

The experiment management framework is an essential component of the NLM system, providing the infrastructure needed for scientific research and development with neural learning machines.

---

*Last updated: 2026-09-28*

This documentation provides comprehensive guidance for using the NLM Experiment Management Framework to design, run, and analyze neural learning machine experiments.