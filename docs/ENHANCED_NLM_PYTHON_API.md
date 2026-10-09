# NLM Python API - Enhanced Documentation

This document provides comprehensive documentation for the enhanced NLM Python API, including new convenience methods, advanced configuration options, error handling, logging, visualization, and examples.

## Overview

The NLM (Neural Learning Machine) Python API has been significantly enhanced to provide researchers and developers with a comprehensive set of tools for neural simulation research, machine learning integration, and advanced experimentation.

## Quick Start

### Basic Usage
```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### Enhanced Convenience Usage
```python
import pynlm

# Using convenience methods for common workflows
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with progress tracking
brain.run_simulation(1000)

# Get comprehensive brain summary
summary = brain.get_brain_summary()
print(summary)

# Configure brain for specific task
config.setup_development(start_age=0, duration=3600)
config.setup_learning(learning_rate=0.01, exploration_episodes=100)
```

## New Features

### 1. Convenience Methods

#### Brain Class Methods
- `run_simulation(steps)`: Run complete simulation with error handling
- `run_multiple_episodes(num_episodes, steps_per_episode)`: Run multiple episodes
- `run_parameter_sweep(param_key, values)`: Parameter sweep for experiments
- `get_brain_summary()`: Comprehensive brain statistics
- `export_network(filename)`: Export network connectivity
- `get_spiking_patterns()`: Extract spiking patterns
- `validate_brain()`: Validate brain configuration and state
- `check_memory_integrity()`: Check memory systems integrity
- `validate_agent()`: Validate agent configuration

#### Config Class Methods
- `configure_for_task(task_type, parameters)`: Configure for common tasks
- `setup_development(start_age, duration)`: Setup for developmental studies
- `setup_learning(learning_rate, exploration_episodes)`: Setup for learning experiments
- `getValidated(key, default)`: Get configuration with validation
- `set_from_json(json_string)`: Set from JSON string
- `to_map()`: Get configuration as Python-like dictionary
- `validate_configuration()`: Validate configuration against schema

#### AgentBrain Class Methods
- `create_default_agent_brain(brain)`: Create agent brain interface
- `process_sensory_input(percept)`: Process sensory input
- `decode_motor_command()`: Decode motor command
- `run_episode(steps)`: Run single episode
- `run_agent_simulation(steps)`: Run complete agent simulation
- `get_agent_metrics()`: Get agent performance metrics
- `validate_agent()`: Validate agent configuration
- `enable_all_features()`: Enable all agent features
- `disable_all_features()`: Disable all agent features
- `is_ready()`: Check if agent is ready for simulation
- `get_status()`: Get agent status string

#### SimpleWorld Class Methods
- `create_default()`: Create default world instance
- `create_empty(width, height)`: Create empty world with dimensions
- `create_with_objects(objects)`: Create world with initial objects

### 2. Error Handling and Validation

The enhanced NLM Python API includes comprehensive error handling:

#### Custom Exception Classes
- `NLMError`: Base exception for all NLM errors
- `ConfigError`: Configuration-related errors
- `ConfigValidationError`: Configuration validation errors
- `ConfigTypeError`: Configuration type errors
- `ConfigFileError`: Configuration file errors
- `BrainError`: Brain-related errors
- `BrainValidationError`: Brain validation errors
- `BrainStateError`: Brain state errors
- `NeuralNetworkError`: Neural network errors
- `SimulationError`: Simulation errors
- `SimulationParameterError`: Simulation parameter errors
- `SimulationStepError`: Simulation step errors
- `WorldError`: World-related errors
- `WorldValidationError`: World validation errors
- `MemoryError`: Memory-related errors
- `MemoryCapacityError`: Memory capacity errors
- `AgentError`: Agent-related errors
- `AgentValidationError`: Agent validation errors
- `PerformanceError`: Performance-related errors
- `MemoryLeakError`: Memory leak errors
- `PerformanceBottleneckError`: Performance bottleneck errors

#### Validation Functions
- `validate_config(config)`: Validate configuration
- `validate_brain(brain, operation)`: Validate brain for specific operation
- `validate_agent(agent)`: Validate agent configuration
- `check_memory_integrity(brain)`: Check memory systems integrity
- `generate_validation_summary(brain, config, world)`: Generate validation summary
- `check_common_issues(brain, config, world)`: Check common issues
- `monitor_system_health(brain, world)`: Monitor system health
- `get_error_report(exception)`: Get detailed error report

#### Error Recovery
- `safe_reset(brain, world)`: Safe reset of brain and world
- Automatic recovery from invalid states
- Configuration repair utilities
- Fallback behavior for critical errors

### 3. Logging and Debugging

#### Advanced Logger
- **Log Levels**: Trace, Debug, Info, Warning, Error, Critical, Off
- **Log Categories**: System, Neural, Simulation, Performance, Memory, Configuration, Validation, World, Agent, Development
- **Output Formats**: Plain text, JSON, CSV, XML, HTML
- **Sinks**: Console, File, Remote
- **Features**: Thread-safe, filtering, aggregation, rotation

#### DebugDiagnostics
- **Neural network debugging**: Connection visualization, validation
- **State inspection**: Brain, agent, world state inspection
- **Memory debugging**: Memory usage analysis, leak detection
- **Performance profiling**: Neural computation profiling, pattern analysis
- **Error traceback**: Detailed error reports and propagation tracing
- **Real-time monitoring**: System metrics, resource usage
- **Health checks**: System health validation with issue detection
- **Analysis tools**: Neural activity patterns, memory usage patterns

### 4. Visualization and Export

#### Visualization Methods
- **Neural Activity Visualization**:
  - `visualize_spiking_patterns()`: Spiking pattern visualization
  - `plot_firing_rates()`: Firing rate plots
  - `visualize_activity_heatmap()`: Activity heatmap
  - `visualize_time_series()`: Time series visualization
  - `visualize_network_topology()`: Network topology visualization
  - `visualize_memory_systems()`: Memory system visualization
  - `visualize_development_trajectory()`: Development trajectory visualization
  - `visualize_neuromodulation()`: Neuromodulation visualization

- **Advanced Analysis Visualization**:
  - `plot_network_analysis()`: Network analysis with community detection
  - `plot_connectivity_correlation()`: Connectivity correlation visualization
  - `plot_statistical_analysis()`: Statistical analysis visualization

- **Interactive Visualization**:
  - `generate_interactive_plot()`: Web-based interactive visualization
  - `generate_jupyter_notebook()`: Jupyter notebook integration
  - `create_3d_visualization()`: 3D neural network visualization
  - `create_animated_simulation()`: Animated simulations

#### Export Methods
- **Neural Data Export**:
  - `export_neural_data(filename)`: Export spiking data, connectivity
  - `export_configuration(filename)`: Export brain configurations
  - `export_results(filename)`: Export simulation results
  - `export_visualization(filename)`: Export plots and charts

- **Export Formats**: PNG, JPEG, SVG, PDF, CSV, JSON, TXT, Excel, RData, HTML, JavaScript, ZIP, TAR.GZ

### 5. Batch Processing and Parallel Execution

#### Batch Processing Utilities
- **BatchSimulationRunner**: Parallel/sequential batch execution with progress tracking
- **BatchConfigGenerator**: Parameter grid exploration with automatic iteration
- **CrossValidationUtilities**: Cross-validation for neural network validation

#### Parallel Execution Capabilities
- **SharedMemoryPool**: Thread-safe shared memory management
- **ThreadSafeConfigManager**: Thread-safe configuration handling
- **ParallelSimulationRunner**: Parallel simulation with work distribution
- **ParallelParameterSweep**: Dynamic parameter sweep across worker threads

#### Performance Monitoring
- **PerformanceMonitor**: Performance benchmarking with comprehensive metrics
- **ResourceMonitor**: System resource monitoring during parallel execution
- **ExecutionTracker**: Detailed timing analysis and optimization recommendations
- **MemoryMonitor**: Memory usage monitoring with leak detection

### 6. Advanced Features

#### Machine Learning Integration
- Neural network training with NLM brains
- Reinforcement learning with NLM brains
- Supervised learning with neural embeddings
- Transfer learning between different NLM configurations

#### Performance Optimization
- Performance monitoring and optimization
- Memory usage analysis
- Neuromodulation control and tuning
- Advanced plasticity management

#### Research and Analysis
- Large-scale neural network analysis
- Developmental trajectory studies
- Memory consolidation and replay experiments
- Cross-species neural system comparisons

## Examples

### 1. Advanced Simulation Management
```python
# Parameter sweep for learning rate optimization
results = example_advanced_simulation_management()

# Analyze and optimize parameters
best_config = max(results, key=lambda x: x['avg_firing_rate'])
print(f"Best configuration: LR={best_config['learning_rate']}, Neurons={best_config['neuron_count']}")
```

### 2. Machine Learning Integration
```python
# Reinforcement learning with NLM brain
rl_results = example_machine_learning_integration()

# Analyze learning performance
final_performance = rl_results[-1]
print(f"Final curiosity level: {final_performance['curiosity']:.3f}")
print(f"Final dopamine level: {final_performance['dopamine']:.3f}")
```

### 3. Performance Optimization
```python
# Performance monitoring and optimization
performance_results = example_performance_optimization()

# Compare performance across configurations
for config_name, metrics in performance_results.items():
    print(f"{config_name}: {metrics['steps_per_second']:.1f} steps/sec")
```

### 4. Research Workflow
```python
# Large-scale neural network analysis
analysis_results = example_research_workflow()

# Comparative analysis
best_by_firing_rate = max(analysis_results.values(),
                         key=lambda x: x['performance']['avg_firing_rate'])
print(f"Best firing rate: {best_by_firing_rate['name']} ({best_by_firing_rate['performance']['avg_firing_rate']:.2f})")
```

### 5. Reproducible Research
```python
# Configuration management for reproducible experiments
experiment_results = example_reproducible_research()

# Generate reproducibility report
print("Reproducibility Report:")
print("✓ All experiments used identical code structure")
print("✓ Each experiment had its own configuration file")
print("✓ Results are saved in JSON format for analysis")
print("✓ Configuration files can be shared for replication")
```

## Installation and Setup

### Prerequisites
- Python 3.8+
- numpy
- matplotlib (for visualization)
- plotly (for interactive visualization)
- jupyter (for notebook integration)

### Installation
```bash
pip install pynlm
pip install matplotlib plotly jupyter
```

### Building from Source
```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install .
```

## Usage Examples

### Complete Example: Advanced Research Simulation
```python
#!/usr/bin/env python3
"""
Complete example of advanced NLM research simulation.
"""

import pynlm
import numpy as np
import time
import json

def main():
    print("=== Advanced NLM Research Simulation ===\n")
    
    # 1. Setup configuration with validation
    print("1. Setting up configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("neuromod.dopamine.scale", 1.0)
    
    # Validate configuration
    validation_result = pynlm.validate_config(config)
    if not validation_result.isValid:
        print(f"   Configuration validation failed: {validation_result.message}")
        print(f"   Suggestions: {validation_result.suggestions}")
        return
    
    print("   ✓ Configuration validated successfully")
    
    # 2. Create and initialize components
    print("\n2. Creating simulation components...")
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure for research study
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    print("   ✓ Components initialized successfully")
    
    # 3. Run simulation with monitoring
    print("\n3. Running simulation with monitoring...")
    
    start_time = time.time()
    total_reward = 0.0
    
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get reward and apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        total_reward += reward
        
        # Progress monitoring
        if (step + 1) % 100 == 0:
            elapsed = time.time() - start_time
            print(f"   Step {step + 1}/500: Reward={reward:.3f}, "
                  f"Total={total_reward:.2f}, Time={elapsed:.2f}s")
    
    total_time = time.time() - start_time
    
    # 4. Analyze results
    print("\n4. Analyzing results...")
    
    # Performance metrics
    brain_metrics = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio()
    }
    
    agent_metrics = {
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel(),
        "dopamine": agent.getNeuromodulationLevel(),
        "prediction_error": agent.getPredictionError(),
        "development_stage": agent.getDevelopmentalStage().name
    }
    
    print("   Brain Performance:")
    for key, value in brain_metrics.items():
        print(f"     {key}: {value}")
    
    print("   Agent Performance:")
    for key, value in agent_metrics.items():
        print(f"     {key}: {value}")
    
    print(f"   Total Simulation Time: {total_time:.2f}s")
    print(f"   Steps per Second: {500/total_time:.1f}")
    
    # 5. Generate visualization
    print("\n5. Generating visualization...")
    
    try:
        # Create visualizations
        brain.visualize_spiking_patterns("spiking_patterns.png")
        brain.plot_firing_rates("firing_rates.png")
        agent.visualize_neuromodulation("neuromodulation.png")
        
        print("   ✓ Visualizations saved successfully")
    except Exception as e:
        print(f"   ⚠️  Visualization failed: {e}")
    
    # 6. Export results
    print("\n6. Exporting results...")
    
    # Export neural data
    brain.export_network("neural_network.csv")
    
    # Export configuration
    config.saveToFile("experiment_config.json")
    
    # Export results
    results = {
        "brain_metrics": brain_metrics,
        "agent_metrics": agent_metrics,
        "total_time": total_time,
        "total_reward": total_reward,
        "config": config.summary()
    }
    
    with open("experiment_results.json", 'w') as f:
        json.dump(results, f, indent=2)
    
    print("   ✓ Results exported successfully")
    
    # 7. Generate validation report
    print("\n7. Generating validation report...")
    
    validation_report = brain.generate_validation_summary(brain, config, world)
    
    # Check common issues
    issues = pynlm.check_common_issues(brain, config, world)
    
    print(f"   Validation report generated ({len(validation_report)} characters)")
    print(f"   Common issues found: {len(issues)}")
    
    for i, issue in enumerate(issues, 1):
        print(f"     {i}. {issue}")
    
    # 8. System health monitoring
    print("\n8. System health monitoring...")
    
    health_metrics = pynlm.monitor_system_health(brain, world)
    
    print("   System Health Metrics:")
    for key, value in health_metrics.items():
        print(f"     {key}: {value}")
    
    print("\n=== Simulation Complete ===")
    print(f"Performance: {brain.getAverageFiringRate():.2f} avg firing rate")
    print(f"Efficiency: {500/total_time:.1f} steps/second")
    print(f"Quality: {len(issues)} issues detected")

if __name__ == "__main__":
    main()
```

## Advanced Usage Tips

### 1. Performance Optimization
- Use `brain.run_simulation()` for better performance than manual loops
- Configure `config.setup_development()` for developmental studies
- Use `agent.run_agent_simulation()` for complete agent control

### 2. Debugging and Validation
- Always validate configuration before use: `pynlm.validate_config(config)`
- Use `brain.validate_brain()` to check brain state before operations
- Monitor system health regularly with `pynlm.monitor_system_health()`

### 3. Visualization Best Practices
- Use `brain.visualize_spiking_patterns()` for neural activity patterns
- Use `brain.plot_firing_rates()` for firing rate analysis
- Use `agent.visualize_neuromodulation()` for neuromodulation levels

### 4. Error Handling
- Catch custom NLM exceptions with `pynlm.NLMError` and its subclasses
- Use `pynlm.get_error_report()` for detailed error information
- Implement automatic recovery with `pynlm.safe_reset()`

### 5. Batch Processing
- Use `BatchSimulationRunner` for large-scale experiments
- Use `ParallelSimulationRunner` for parallel execution
- Use `BatchConfigGenerator` for parameter sweep generation

## Troubleshooting

### Common Issues

1. **Import Error**
   - Ensure pynlm is installed correctly
   - Check Python version compatibility
   - Verify build was successful

2. **Configuration Validation Errors**
   - Use `pynlm.validate_config()` to check configuration
   - Check error messages for specific validation failures
   - Use validation suggestions to fix configuration

3. **Simulation Errors**
   - Validate brain state before simulation: `brain.validate_brain("initialize")`
   - Check agent readiness: `agent.is_ready()`
   - Monitor system health: `pynlm.monitor_system_health(brain, world)`

4. **Visualization Errors**
   - Ensure visualization libraries are installed
   - Check available output formats
   - Use fallback visualization methods

5. **Performance Issues**
   - Monitor simulation performance with `brain.run_simulation()`
   - Optimize configuration with `config.setup_development()` and `config.setup_learning()`
   - Use parallel processing for large-scale simulations

### Debugging Commands
```python
# Check configuration
config = pynlm.createDefaultConfig()
validation = pynlm.validate_config(config)

# Check brain state
brain = pynlm.createBrain(config)
brain.validate_brain("initialize")

# Monitor system health
health = pynlm.monitor_system_health(brain, world)

# Check common issues
issues = pynlm.check_common_issues(brain, config, world)

# Generate error report
try:
    # Some operation that might fail
    pass
except pynlm.NLMError as e:
    report = pynlm.get_error_report(e)
```

## Advanced Features Reference

### Configuration Options
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

### API Methods
| Class | Method | Description |
|-------|--------|-------------|
| Brain | `run_simulation(steps)` | Run complete simulation |
| Brain | `get_brain_summary()` | Get comprehensive brain statistics |
| Config | `configure_for_task(task_type, parameters)` | Configure for common tasks |
| AgentBrain | `run_agent_simulation(steps)` | Run complete agent simulation |
| Logger | `set_log_level(level)` | Set log level |
| DebugDiagnostics | `generate_validation_summary()` | Generate validation summary |

## Conclusion

The enhanced NLM Python API provides a comprehensive set of tools for neural simulation research and machine learning applications. With over 100 new methods and features, including convenience methods, advanced configuration options, comprehensive error handling, logging and debugging capabilities, visualization and export features, and batch processing utilities, the API is now significantly more powerful and user-friendly.

These enhancements enable researchers and developers to:

1. **Easily implement common workflows** with convenience methods
2. **Perform advanced simulations** with parameter sweeps and batch processing
3. **Integrate neural networks** with machine learning applications
4. **Optimize performance** with comprehensive monitoring and analysis tools
5. **Conduct reproducible research** with configuration management and validation
6. **Visualize and export results** with publication-quality visualizations
7. **Debug and troubleshoot** issues with comprehensive error handling and logging
8. **Scale to large-scale experiments** with parallel processing capabilities

The API maintains full backward compatibility while providing extensive new functionality for advanced usage patterns, making it suitable for both beginners and expert users in neural simulation research.