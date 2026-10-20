# NLM Advanced Usage Guide

## Overview

This document covers advanced usage of the Neural Learning Machine (NLM) with all the enhancements we've implemented. It includes:

- Batch processing capabilities
- Learning workflow management
- Advanced configuration management
- Debugging and monitoring tools
- Experiment management
- Performance optimization
- Integration with existing NLM components

## Quick Start with Advanced Features

### 1. Setting Up Advanced Configuration

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Apply performance template for optimized simulation
from pynlm import ConfigurationTemplate
template = ConfigurationTemplate.createPerformanceTemplate()
config = template.apply_template(config)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Initialized brain with {brain.getTotalNeuronCount()} neurons")
```

### 2. Batch Processing Multiple Simulations

```python
import pynlm
from pynlm import BatchSimulationManager

# Create batch simulation manager
batch_manager = pynlm.create_batch_simulation_manager()
batch_manager.setMaxThreads(4)
batch_manager.setProgressReportingInterval(50)

# Define simulation tasks
tasks = [
    pynlm.BatchSimulationManager.SimulationTask(
        taskId="task1",
        config=pynlm.createDefaultConfig(),
        iterations=100
    ),
    pynlm.BatchSimulationManager.SimulationTask(
        taskId="task2", 
        config=pynlm.createDefaultConfig(),
        iterations=150
    )
]

# Run batch with progress reporting
def progress_callback(iteration, progress):
    print(f"Progress: {iteration}/100 ({progress*100:.1f}%)")

for task in tasks:
    task.progressCallback = progress_callback

result = batch_manager.run_batch(tasks, batchId="experiment_1", description="Multiple simulations")
print(result.getSummary())
```

### 3. Learning Workflow Management

```python
import pynlm
from pynlm import LearningWorkflowManager, ConfigurationTemplate

# Create learning workflow manager
workflow_manager = pynlm.create_learning_workflow_manager()

# Create learning scenario
scenario = pynlm.create_learning_scenario(
    name="Complex Learning",
    configTemplate=ConfigurationTemplate.createLearningTemplate(),
    episodes=20,
    stepsPerEpisode=200
)

# Add hyperparameters for learning
# (Note: This would use the actual hyperparameter mechanism)

# Run learning workflow
learning_result = workflow_manager.run_learning_workflow(
    scenario, 
    workflowId="learning_workflow_1",
    useBatchProcessing=True
)

print("Learning completed successfully!")
print(f"Success rate: {learning_result.getSuccessRate():.1f}%")
```

### 4. Advanced Configuration Validation

```python
import pynlm
from pynlm import ConfigurationSchema

# Create validation schema
schema = pynlm.create_configuration_schema()

# Add various validation rules
schema.add_range_rule("brain.neuron_count", 100, 10000, "Neuron count must be between 100 and 10000")
schema.add_pattern_rule("brain.name", "^[a-zA-Z0-9_]+$", "Name must contain only alphanumeric characters")
schema.add_choices_rule("learning.strategy", ["random", "structured", "curiosity"], "Invalid learning strategy")
schema.add_required_rule("plasticity.stdp.enable", "STDP must be enabled")

# Validate configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("learning.strategy", "random")

errors = pynlm.validate_config_with_schema(config, schema)
if errors:
    print("Validation errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### 5. Experiment Management

```python
import pynlm
from pynlm import ExperimentManager, LearningWorkflowManager

# Create experiment manager
experiment_manager = pynlm.create_experiment_manager()

# Create experiment
experiment_id = experiment_manager.create_experiment(
    name="Neural Development Study",
    description="Study of neural development with various learning strategies"
)

# Add learning scenarios to experiment
scenario1 = pynlm.create_learning_scenario(
    name="Baseline Learning",
    configTemplate=pynlm.ConfigurationTemplate.createDefaultTemplate(),
    episodes=10,
    stepsPerEpisode=100
)

experiment_manager.add_learning_scenario(experiment_id, scenario1)

# Add control task
from pynlm import BatchSimulationManager
control_task = pynlm.BatchSimulationManager.SimulationTask(
    taskId="control",
    config=pynlm.createDefaultConfig(),
    iterations=50
)

experiment_manager.add_control_task(experiment_id, control_task)

# Run experiment
result = experiment_manager.run_experiment(experiment_id, useBatchProcessing=True)

print("Experiment completed!")
print(f"Results saved to: experiment_results_{experiment_id}")

# Export results
experiment_manager.export_experiment_results(experiment_id, f"results/experiment_{experiment_id}")
```

### 6. Debugging and Monitoring

```python
import pynlm

# Create debug logger
logger = pynlm.create_debug_logger()
logger.setLogLevel(pynlm.AdvancedLogger.LogLevel.Debug)

# Enable file logging
logger.enableFileLogging("./logs/debug.log")

# Log some events
logger.log(pynlm.AdvancedLogger.LogLevel.Info, "Starting simulation", "main")
logger.log(pynlm.AdvancedLogger.LogLevel.Debug, "Initializing brain components", "brain")

# Create performance analyzer
performance_analyzer = pynlm.create_performance_analyzer()

# Start monitoring (in real usage, this would collect actual metrics)
performance_analyzer.startMonitoring(lambda: {
    "cpu_usage": 0.5,
    "memory_usage": 256.0,
    "simulation_time": 0.1,
    "firing_neurons": 150,
    "total_spikes": 5000
})

# Create simulation profiler
profiler = pynlm.create_simulation_profiler()
profiler.startRecording()

# Run simulation with profiling
for i in range(100):
    # Your simulation code here
    pass

profiler.endEvent()

# Get performance report
performance_report = performance_analyzer.generatePerformanceReport()
print(performance_report)

# Export performance data
performance_analyzer.exportPerformanceData("./performance_results.json")
```

## Advanced Features Reference

### 1. Configuration Templates

```python
from pynlm import ConfigurationTemplate

# Default template (comprehensive but slower)
default = ConfigurationTemplate.create_default_template()

# Performance template (optimized for speed)
performance = ConfigurationTemplate.create_performance_template()

# Learning template (optimized for learning)
learning = ConfigurationTemplate.create_learning_template()

# Apply template to configuration
config = pynlm.createDefaultConfig()
config = performance.apply_template(config)
```

### 2. Schema Validation

```python
from pynlm import ConfigurationSchema

# Create comprehensive schema
schema = pynlm.create_configuration_schema()

# Add various constraints
schema.add_range_rule("parameter.value", 0.0, 1.0, "Value must be between 0 and 1")
schema.add_choices_rule("mode", ["train", "test", "validate"], "Invalid mode")
schema.add_pattern_rule("name", "^[a-zA-Z][a-zA-Z0-9_]*$", "Invalid name format")
schema.add_required_rule("learning.rate", "Learning rate is required")

# Validate
errors = schema.validate(config)
```

### 3. Batch Processing Options

```python
from pynlm import BatchSimulationManager

# Configure batch processing
batch_manager = pynlm.create_batch_simulation_manager()
batch_manager.setMaxThreads(8)  # Use 8 threads
batch_manager.setProgressReportingInterval(25)  # Report every 25 iterations
batch_manager.enableAutoSave(True)  # Auto-save results

# Create tasks with specific configurations
task1 = BatchSimulationManager.SimulationTask(
    taskId="high_performance",
    config=performance_config,  # Pre-configured template
    iterations=200,
    parameters={
        "reward_modulation": True,
        "curiosity": True,
        "development": False
    }
)

# Run batch
result = batch_manager.run_batch([task1], batchId="perf_test", description="Performance testing")
```

### 4. Learning Workflow Options

```python
from pynlm import LearningWorkflowManager

# Create learning workflow
workflow_manager = pynlm.create_learning_workflow_manager()

# Get predefined scenarios
scenarios = workflow_manager.get_default_learning_scenarios()

# Run specific scenario
result = workflow_manager.run_learning_workflow(
    scenarios[1],  # Structured learning
    workflowId="learning_test_1",
    useBatchProcessing=True
)

# Check performance
print(f"Learning completed with {result.getSuccessRate():.1f}% success rate")
print(f"Average firing rate: {result.getAverageMetric('average_firing_rate'):.2f}")
```

### 5. Experiment Management Features

```python
from pynlm import ExperimentManager

# Create experiment
manager = pynlm.create_experiment_manager()
exp_id = manager.create_experiment(
    "Neural Plasticity Study",
    "Investigation of synaptic plasticity mechanisms"
)

# Add learning scenarios with different configurations
scenarios = manager.get_experiment(exp_id).learningScenarios

# Export results
manager.export_experiment_results(
    exp_id,
    f"experiments/{exp_id}_run_{pynlm.get_current_timestamp()}"
)
```

## Integration with Existing NLM Features

### 1. Using Advanced Features with Existing Code

```python
import pynlm

# Traditional NLM usage (unchanged)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced usage with new features
from pynlm import AdvancedLogger, PerformanceAnalyzer

# Setup advanced logging
logger = pynlm.create_debug_logger()
logger.setLogLevel(pynlm.AdvancedLogger.LogLevel.Info)

# Run simulation with monitoring
performance_analyzer = pynlm.create_performance_analyzer()

# Your existing simulation code here
for step in range(1000):
    # Traditional NLM operations
    # ...
    
    # Log progress
    logger.log(pynlm.AdvancedLogger.LogLevel.Info, 
              f"Step {step} completed", "simulation")

# Analyze performance
result = performance_analyzer.generatePerformanceReport()
```

### 2. Migration Examples

```python
# Old way (simple)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# New way (with templates)
from pynlm import ConfigurationTemplate
template = ConfigurationTemplate.create_default_template()
config = template.apply_template(pynlm.createDefaultConfig())
brain = pynlm.createBrain(config)
brain.initialize()

# Even newer way (with validation)
from pynlm import ConfigurationSchema
schema = ConfigurationTemplate.create_default_schema()
if not schema.validate(config).empty():
    print("Configuration validation failed!")
else:
    # Proceed with initialization
    pass
```

## Performance Optimization

### 1. Batch Processing Performance

```python
# Configure for maximum performance
batch_manager = pynlm.create_batch_simulation_manager()
batch_manager.setMaxThreads(std::thread::hardware_concurrency())
batch_manager.setProgressReportingInterval(1000)  # Less frequent reporting
batch_manager.enableAutoSave(False)  # Disable for fastest performance

# Create CPU-optimized tasks
tasks = []
for i in range(10):
    task = BatchSimulationManager.SimulationTask(
        taskId=f"perf_task_{i}",
        config=performance_template.apply_template(pynlm.createDefaultConfig()),
        iterations=500
    )
    tasks.push_back(task)

# Run high-performance batch
result = batch_manager.run_batch(tasks, parallel=True)
```

### 2. Memory Management

```python
# Enable memory monitoring
performance_analyzer = pynlm.create_performance_analyzer()
performance_analyzer.addMetric("memory_usage", getMemoryUsageFunction)

# Run simulation with memory monitoring
for episode in range(100):
    # Your simulation code
    # Memory will be tracked automatically
    pass

# Get memory statistics
metrics = performance_analyzer.generatePerformanceReport()
```

## Troubleshooting and Common Issues

### 1. Configuration Validation Issues

**Problem:** Configuration validation fails with unexpected errors

**Solution:**
```python
# Debug validation
from pynlm import ConfigurationSchema

schema = ConfigurationSchema()
schema.add_range_rule("param.value", 0, 10, "Custom error message")

config = pynlm.createDefaultConfig()
config.set("param.value", 15)

errors = schema.validate(config)
print("Validation errors:", errors)
```

### 2. Batch Processing Performance Issues

**Problem:** Batch processing is slower than expected

**Solutions:**
1. Reduce thread count if system is overloaded
2. Increase progress reporting interval to reduce overhead
3. Disable auto-save for faster execution
4. Use performance-optimized templates

### 3. Memory Issues

**Problem:** Memory usage grows during long simulations

**Solutions:**
1. Use performance templates that disable memory-intensive features
2. Implement checkpointing for long simulations
3. Monitor memory usage with performance analyzer
4. Use smaller simulation configurations

## Migration Guide

### From NLM v1.0 to v2.0 Advanced Features

#### Old API Usage:
```python
# Old way (limited features)
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)
```

#### New API Usage:
```python
# New way (comprehensive features)
import pynlm
from pynlm import ConfigurationTemplate, BatchSimulationManager

# Create optimized configuration
template = ConfigurationTemplate.create_default_template()
config = template.apply_template(pynlm.createDefaultConfig())

# Use batch processing for better performance
batch_manager = pynlm.create_batch_simulation_manager()
batch_manager.setMaxThreads(4)

result = batch_manager.run_batch([
    BatchSimulationManager.SimulationTask(
        taskId="main_simulation",
        config=config,
        iterations=100
    )
])
```

### Key Improvements:

1. **Configuration Templates**: Predefined configurations for common use cases
2. **Batch Processing**: Parallel execution of multiple simulations
3. **Advanced Validation**: Comprehensive configuration validation
4. **Performance Monitoring**: Built-in performance tracking and analysis
5. **Experiment Management**: Structured experiment tracking and management
6. **Enhanced Logging**: Advanced debugging and monitoring capabilities
7. **Error Handling**: Improved error reporting and recovery mechanisms

## Conclusion

The enhanced NLM v2.0 provides comprehensive tooling for advanced neural network simulation, including:

- **High-performance batch processing** for large-scale simulations
- **Learning workflow management** for structured experiments
- **Advanced configuration management** with validation and templates
- **Comprehensive debugging and monitoring** for performance analysis
- **Experiment management** for reproducible research
- **Backward compatibility** with existing NLM code

These enhancements make NLM suitable for both simple research prototypes and large-scale production simulations, while maintaining ease of use for beginners and comprehensive features for advanced users.

---

*Document Version: 2.0.0*
*Date: 2026-10-20*
*Last Updated: Advanced Features Documentation*