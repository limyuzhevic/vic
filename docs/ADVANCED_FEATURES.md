# NLM Advanced Features Documentation

This document provides comprehensive information about the advanced features added to the NLM (Neural Learning Machine) framework.

## Overview

The NLM project has been enhanced with advanced features for power users, researchers, and developers. These enhancements include:

- **Advanced Python bindings** with learning analysis, visualization, and experiment management
- **Command-line tools** for batch processing, analysis, and configuration management
- **Performance monitoring and profiling capabilities**
- **Comprehensive configuration management** with preset templates
- **Extended examples and tutorials** for advanced usage

## Table of Contents

1. [Advanced Python Bindings](#advanced-python-bindings)
   1.1 [Learning Analysis Functions](#learning-analysis-functions)
   1.2 [Visualization Data](#visualization-data)
   1.3 [Experiment Reporting](#experiment-reporting)
   1.4 [Advanced Configuration](#advanced-configuration)
   1.5 [Batch Processing API](#batch-processing-api)
   1.6 [Performance Monitoring](#performance-monitoring)

2. [Command-Line Tools](#command-line-tools)
   2.1 [Analysis Tool (nlm_analysis)](#analysis-tool-nlm_analysis)
   2.2 [Batch Processing Tool (nlm_batch)](#batch-processing-tool-nlm_batch)
   2.3 [Configuration Manager (nlm_config)](#configuration-manager-nlm_config)
   2.4 [Performance Profiler (nlm_profile)](#performance-profiler-nlm_profile)

3. [Configuration Management](#configuration-management)
   3.1 [Preset Configuration Templates](#preset-configuration-templates)
   3.2 [Experiment Configuration Management](#experiment-configuration-management)
   3.3 [Configuration Validation](#configuration-validation)
   3.4 [Configuration Export/Import](#configuration-export-import)

4. [Advanced Examples](#advanced-examples)
   4.1 [Learning Analysis Example](#learning-analysis-example)
   4.2 [Batch Processing Example](#batch-processing-example)
   4.3 [Configuration Management Example](#configuration-management-example)
   4.4 [Performance Monitoring Example](#performance-monitoring-example)

5. [Performance Optimization](#performance-optimization)
   6. [Troubleshooting](#troubleshooting)
   7. [API Reference](#api-reference)

## Advanced Python Bindings

The enhanced Python bindings provide advanced functionality for researchers and developers.

### Learning Analysis Functions

#### `analyze_learning_performance(brain, agent=None, rewards=[])`

Analyze learning performance with detailed metrics and recommendations.

**Parameters:**
- `brain`: Brain object to analyze
- `agent`: Optional AgentBrain object for additional metrics
- `rewards`: Optional list of reward signals for behavioral analysis

**Returns:**
- Dictionary with comprehensive analysis including:
  - Performance grade (A, B, C, D)
  - Improvement potential
  - Learning efficiency metrics
  - Recommendations for improvement

**Example:**
```python
import pynlm

# Create and initialize components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation...
for i in range(1000):
    brain.step(i)

# Analyze performance
metrics = pynlm.analyze_learning_performance(brain)
print(f"Learning Grade: {metrics['performance_grade']}")
print(f"Improvement Potential: {metrics['improvement_potential']:.1%}")
for recommendation in metrics['recommendations']:
    print(f"- {recommendation}")
```

#### `create_visualization_data(brain, agent=None, world=None, time_window=1.0)`

Create visualization data for neural activity patterns.

**Parameters:**
- `brain`: Brain object to analyze
- `agent`: Optional AgentBrain object
- `world`: Optional SimpleWorld object
- `time_window`: Time window for analysis in seconds

**Returns:**
- VisualizationData object containing:
  - Neural activity patterns
  - Synaptic weight distributions
  - Population activity data
  - Activity labels and timestamps

#### `generate_experiment_report(title, description, author, brain, agent=None, world=None, rewards=[])`

Generate a comprehensive experiment report with analysis.

**Parameters:**
- `title`: Experiment title
- `description`: Experiment description
- `author`: Experiment author
- `brain`: Brain object
- `agent`: Optional AgentBrain object
- `world`: Optional SimpleWorld object
- `rewards`: Optional list of reward signals

**Returns:**
- ExperimentReport object with:
  - Experiment metadata
  - Learning metrics
  - Performance statistics
  - Analysis results

### Advanced Configuration

#### `create_advanced_config(name, description, parameters, is_template=False, tags=[])`

Create an advanced configuration template for research experiments.

**Parameters:**
- `name`: Configuration name
- `description`: Configuration description
- `parameters`: Dictionary of configuration parameters
- `is_template`: Whether this is a template for others
- `tags`: List of tags for categorization

**Returns:**
- AdvancedConfig object with merging capabilities

**Example:**
```python
# Create a research-focused preset
research_config = pynlm.create_advanced_config(
    name="research_preset",
    description="Optimized for neuroscience research",
    parameters={
        "brain.neuron_count": "5000",
        "plasticity.stdp.learning_rate": "0.003",
        "neuromod.curiosity.enable": "true",
        "development.enabled": "true"
    },
    is_template=True,
    tags=["research", "neurology", "advanced"]
)

# Merge with base configuration
base_params = {"brain.synapse_density": "0.15"}
merged = research_config.get_merged_params(base_params)
```

### Batch Processing API

#### `run_batch_simulation(num_simulations, steps_per_simulation, output_dir="batch_results", save_results=True, parallel=False, timestep=0.001, config_template="")`

Run multiple simulations with varying configurations.

**Parameters:**
- `num_simulations`: Number of simulations to run
- `steps_per_simulation`: Steps per simulation
- `output_dir`: Output directory
- `save_results`: Whether to save individual results
- `parallel`: Whether to run in parallel
- `timestep`: Simulation timestep
- `config_template`: Template configuration to use

**Returns:**
- List of result dictionaries with metrics for each simulation

### Performance Monitoring

#### `get_performance_stats()`

Get current system performance statistics.

**Returns:**
- PerformanceStats object with:
  - CPU and memory usage
  - Simulation performance metrics
  - Learning efficiency measures
  - Stability indicators

## Command-Line Tools

The enhanced NLM distribution includes several command-line tools for advanced usage.

### Analysis Tool (nlm_analysis)

Usage:
```bash
nlm_analysis <command> [options]

Commands:
  analyze <brain_file>     Analyze a saved brain state
  compare <file1> <file2>   Compare two experiment results
  metrics [brain_file]     Show current or loaded metrics
```

**Example:**
```bash
# Analyze a saved brain
nlm_analysis analyze experiment_brain.state

# Compare two experiment results
nlm_analysis compare results/exp1.json results/exp2.json

# Show metrics for current brain
nlm_analysis metrics
```

### Batch Processing Tool (nlm_batch)

Usage:
```bash
nlm_batch <command> [options]

Commands:
  run <config_file> <name> <num_sim> <steps>  Run batch experiment
  quick <num_sim> <steps>                      Quick batch test
  parallel <config_file> <name> <num_sim> <steps> Run in parallel
```

**Example:**
```bash
# Run quick batch test
nlm_batch quick 10 1000

# Run batch experiment with custom config
nlm_batch run custom_config.cfg my_experiment 50 5000 --output results/ --save-each

# Run parallel batch
nlm_batch parallel default_config.cfg parallel_exp 20 2000
```

### Configuration Manager (nlm_config)

Usage:
```bash
nlm_config <command> [options]

Commands:
  list-presets              List all saved presets
  list-experiments          List all experiment configurations
  show-preset <name>        Show preset details
  show-experiment <name>    Show experiment details
  create-preset <name> [options]  Create a new preset
  create-experiment <name> [options] Create a new experiment config
  export-preset <name> <file>   Export preset to file
  import-preset <file>         Import preset from file
  export-experiment <name> <file> Export experiment to file
  import-experiment <file>      Import experiment from file
```

**Example:**
```bash
# List all presets
nlm_config list-presets

# Show details of a specific preset
nlm_config show-preset learning_experiment

# Create a new preset with parameters
nlm_config create-preset my_preset --description "Custom preset" --params "brain.neuron_count=1000,plasticity.rate=0.001"
```

### Performance Profiler (nlm_profile)

Usage:
```bash
nlm_profile <command> [options]

Commands:
  start [<duration>]    Start performance profiling
  stop                 Stop profiling and save results
  status               Show current profiling status
  results [<format>]    Show profiling results
  export <file> [<format>] Export profiling data
```

**Example:**
```bash
# Start profiling for 60 seconds
nlm_profile start 60

# Take a snapshot
nlm_profile status

# Show results
nlm_profile results json

# Export results
nlm_profile export profile_data.json
```

## Configuration Management

### Preset Configuration Templates

The configuration system supports predefined templates for common research scenarios:

#### 1. Learning Experiment Preset
```json
{
    "name": "learning_experiment",
    "description": "Optimized for neural learning and plasticity studies",
    "parameters": {
        "brain.neuron_count": "2000",
        "brain.synapse_density": "0.15",
        "plasticity.stdp.enable": "true",
        "plasticity.stdp.learning_rate": "0.002",
        "neuromod.curiosity.enable": "true",
        "neuromod.novelty.enable": "true"
    },
    "tags": ["learning", "plasticity", "neuromodulation"]
}
```

#### 2. Developmental Study Preset
```json
{
    "name": "developmental_study",
    "description": "Configuration for studying brain development",
    "parameters": {
        "brain.neuron_count": "1500",
        "development.enabled": "true",
        "development.max_age": "1000.0",
        "plasticity.structural.enable": "true"
    },
    "tags": ["development", "structural", "long_term"]
}
```

#### 3. Exploration Agent Preset
```json
{
    "name": "exploration_agent",
    "description": "Curiosity-driven exploration behavior",
    "parameters": {
        "brain.neuron_count": "1200",
        "neuromod.curiosity.enable": "true",
        "neuromod.novelty.enable": "true",
        "world.energy_decay": "0.95"
    },
    "tags": ["exploration", "curiosity", "environment"]
}
```

### Experiment Configuration Management

Experiment configurations include comprehensive metadata and parameter tracking:

```python
experiment_config = {
    "name": "synaptic_plasticity_study",
    "description": "Study of synaptic weight changes",
    "author": "Dr. Smith",
    "date_created": "2024-01-15T10:30:00",
    "version": "2.1",
    "tags": ["plasticity", "synaptic", "long_term"],
    "parameters": {
        "brain.neuron_count": "1500",
        "plasticity.stdp.learning_rate": "0.003",
        "simulation.duration": "5000.0"
    }
}
```

## Advanced Examples

### Learning Analysis Example

The `advanced_learning_analysis.py` example demonstrates:

1. Creating an advanced configuration with neuromodulation
2. Running a comprehensive learning experiment
3. Analyzing performance with advanced metrics
4. Generating experiment reports
5. Exporting results and visualizations

### Batch Processing Example

The `batch_processing_example.py` example demonstrates:

1. Running multiple simulations with different configurations
2. Monitoring batch performance
3. Analyzing comparative results
4. Exporting batch statistics

### Configuration Management Example

The `configuration_management.py` example demonstrates:

1. Creating preset configuration templates
2. Managing experiment configurations
3. Importing and exporting configurations
4. Validating configuration parameters

## Performance Optimization

### Tips for High-Performance NLM Usage

1. **Use Parallel Processing**: Enable parallel execution in batch processing tools
2. **Optimize Configuration**: Use appropriate neuron counts and synapse densities
3. **Monitor Memory Usage**: Track memory consumption during long simulations
4. **Use Checkpoints**: Save intermediate results to avoid recomputation
5. **Batch Analysis**: Process results in batches rather than individually

### Performance Profiling

The NLM performance profiler provides detailed insights into:

- CPU and memory usage patterns
- Simulation bottlenecks
- Learning efficiency metrics
- Resource allocation optimization

## Troubleshooting

### Common Issues and Solutions

#### Issue: "No learning detected"

**Solution:**
- Check if plasticity rules are enabled
- Verify neuromodulation settings
- Ensure sufficient simulation steps
- Check reward signals

#### Issue: "Memory usage too high"

**Solution:**
- Reduce neuron count in configuration
- Enable memory-efficient learning algorithms
- Use checkpointing for long simulations
- Monitor memory usage with profiling tools

#### Issue: "Configuration validation failed"

**Solution:**
- Check parameter syntax
- Validate parameter ranges
- Use preset configurations as templates
- Check for missing required parameters

## API Reference

This section provides detailed API documentation for the advanced features.

### Core Classes

#### LearningMetrics
```python
attributes:
    learning_rate (float)
    efficiency (float)
    novelty_processing (float)
    curiosity_driven (float)
    exploration_balance (float)
    stability (float)
    adaptability (float)
    resilience (float)
    synaptic_changes (List[float])
    behavioral_metrics (Dict[str, float])
```

#### VisualizationData
```python
attributes:
    neural_activity (List[float])
    synaptic_weights (List[float])
    population_activity (List[List[float]])
    activity_labels (List[str])
    timestamp (float)
```

#### ExperimentReport
```python
attributes:
    title (str)
    description (str)
    author (str)
    date (str)
    experiment_type (str)
    duration (float)
    success_rate (float)
    metrics (LearningMetrics)
```

### Factory Functions

#### `analyze_learning_performance(brain, agent=None, rewards=[])`
Analyzes learning performance and provides recommendations.

#### `create_visualization_data(brain, agent=None, world=None, time_window=1.0)`
Creates visualization data for neural activity.

#### `generate_experiment_report(title, description, author, brain, agent=None, world=None, rewards=[])`
Generates a comprehensive experiment report.

## Conclusion

The enhanced NLM framework provides comprehensive tools for advanced neural simulation research. The new features enable:

- Detailed learning analysis and performance monitoring
- Efficient batch processing and configuration management
- Comprehensive experiment tracking and reporting
- Advanced visualization and analysis capabilities

These tools make NLM suitable for:
- Neuroscience research
- Machine learning experiments
- Behavioral studies
- Performance optimization
- Educational demonstrations

The combination of enhanced Python bindings, command-line tools, and configuration management makes NLM a powerful platform for advanced neural simulation research.
