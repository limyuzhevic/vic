# NLM Advanced Usage Guide

This guide covers advanced usage patterns, best practices, and advanced features of the NLM (Neural Learning Machine) framework.

## Overview

This guide is intended for developers and researchers who want to use NLM at an advanced level. It covers:

- Advanced Python features and analysis tools
- Configuration management and optimization
- Experiment management and batch processing
- Network analysis techniques
- Learning curve analysis
- Performance optimization

## Quick Start

For beginners, start with the basic examples in the `examples/` directory:

- `examples/basic_usage.py` - Basic NLM usage
- `examples/advanced_python_usage.py` - Advanced Python features
- `examples/network_analysis.py` - Network analysis

Then move to these advanced examples:

- `examples/learning_curves.py` - Learning curve analysis
- `examples/experiment_management.py` - Experiment management
- `examples/configuration_examples.py` - Configuration examples

## Advanced Python Features

### 1. Network Analysis

NLM provides comprehensive network analysis tools:

```python
import pynlm

# Create brain
config = pynlm.create_advanced_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Analyze network connectivity
analysis = pynlm.analyze_network_connectivity(brain)
print(f"Total neurons: {analysis.totalNeurons}")
print(f"E/I ratio: {analysis.excitationInhibitionRatio:.3f}")
```

**Available Analysis Functions:**

- `analyze_network_connectivity(brain)` - Comprehensive network connectivity analysis
- `analyze_learning_progress(brain, window_size)` - Learning progress and pattern analysis
- `get_activity_statistics(brain)` - Detailed activity statistics

### 2. Configuration Management

NLM provides multiple configuration factory methods:

```python
import pynlm

# Advanced configuration for research
config = pynlm.create_advanced_config()

# Development-focused configuration
config = pynlm.create_development_config(plasticityRate=1.5, noveltyRate=1.2)

# Default configuration
config = pynlm.createDefaultConfig()
```

**Configuration Factory Methods:**

- `create_advanced_config()` - Advanced configuration for research
- `create_development_config()` - Development-focused configuration
- `createDefaultConfig()` - Basic configuration

### 3. Experiment Management

Set up and manage complex experiments easily:

```python
import pynlm

# Create experiment setup
experiment = pynlm.create_experiment_setup("advanced_learning")

# Run training loop
results = pynlm.run_training_loop(
    experiment.brain,
    experiment.world,
    experiment.agent,
    numEpisodes=1000,
    maxStepsPerEpisode=500
)
```

**Available Experiment Functions:**

- `create_experiment_setup(experimentType)` - Create complete experiment setup
- `run_training_loop()` - Run complete training loop with brain, world, and agent

### 4. Configuration File I/O

Save and load configurations easily:

```python
import pynlm

# Create configuration
config = pynlm.create_advanced_config()

# Save to file
pynlm.save_config_to_file(config, "my_config.json")

# Load from file
loaded_config = pynlm.load_config_from_file("my_config.json")
```

## Network Analysis Techniques

### 1. Connectivity Analysis

Analyze network connectivity and statistics:

```python
def analyze_network_properties(brain):
    # Basic connectivity
    analysis = pynlm.analyze_network_connectivity(brain)
    
    # Weight statistics
    print(f"Average weight: {analysis.averageWeight:.3f}")
    print(f"Weight std deviation: {analysis.weightStandardDeviation:.3f}")
    
    # Network properties
    ei_ratio = brain.getExcitationInhibitionRatio()
    print(f"E/I ratio: {ei_ratio:.3f}")
    
    # Network density
    total_possible_connections = analysis.totalNeurons * (analysis.totalNeurons - 1) / 2
    network_density = analysis.totalSynapses / total_possible_connections
    print(f"Network density: {network_density:.4f}")
```

### 2. Activity Pattern Analysis

Analyze neural activity patterns:

```python
def analyze_activity_patterns(brain):
    stats = pynlm.get_activity_statistics(brain)
    
    print(f"Average firing rate: {stats.averageFiringRate:.3f} Hz")
    print(f"Max firing neurons: {stats.maxFiringNeurons}")
    print(f"Activity variability: {stats.activityVariability:.3f}")
    
    # Analyze spike statistics
    spike_rate = stats.spikeCount / 100.0  # Example: spikes per 100 steps
    print(f"Spike rate: {spike_rate:.2f} spikes/step")
```

### 3. Learning Dynamics

Track learning progress and patterns:

```python
def analyze_learning_dynamics(brain, window_size=10):
    learning = pynlm.analyze_learning_progress(brain, window_size)
    
    print(f"Learning rate: {learning.totalLearning:.3f}")
    print(f"Learning stability: {learning.stability:.3f}")
    
    # Learning trend analysis
    if len(learning.learning_history) > 5:
        recent = learning.learning_history[-5:]
        trend = statistics.mean(recent) - statistics.mean(learning.learning_history[:-5])
        print(f"Learning trend: {'Increasing' if trend > 0 else 'Decreasing' if trend < 0 else 'Stable'}")
```

## Configuration Management

### 1. Configuration Templates

NLM provides several configuration templates for different use cases:

```python
# Research configuration (high plasticity, advanced features)
research_config = pynlm.create_advanced_config()

# Development configuration (age-dependent plasticity)
dev_config = pynlm.create_development_config(plasticityRate=1.2, noveltyRate=1.1)

# Basic configuration (simple setup)
basic_config = pynlm.createDefaultConfig()
```

### 2. Configuration Customization

Customize configurations for specific needs:

```python
# Start with advanced config
config = pynlm.create_advanced_config()

# Customize for specific use case
config.set("brain.neuron_count", 5000)
config.set("memory.working_capacity", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)
```

### 3. Configuration Validation

Validate configuration completeness:

```python
def validate_config(config):
    required_keys = [
        "brain.neuron_count",
        "brain.region_count",
        "brain.connection_probability",
        "plasticity.stdp.enable"
    ]
    
    missing_keys = [key for key in required_keys if not config.has(key)]
    if missing_keys:
        print(f"Missing required keys: {missing_keys}")
        return False
    return True
```

## Experiment Management

### 1. Single Experiment Management

Set up and run single experiments:

```python
import pynlm

# Create experiment setup
experiment = pynlm.create_experiment_setup("advanced_learning")

# Run experiment
results = pynlm.run_training_loop(
    experiment.brain,
    experiment.world,
    experiment.agent,
    numEpisodes=100,
    maxStepsPerEpisode=500
)
```

### 2. Batch Experiment Management

Run multiple experiments with different configurations:

```python
import pynlm

# Define experiment configurations
experiment_configs = [
    {
        'type': 'basic_learning',
        'parameters': {'neuron_count': 1000, 'enable_reward': True},
        'num_steps': 100
    },
    {
        'type': 'structural_plasticity',
        'parameters': {'neuron_count': 2000, 'enable_structural': True},
        'num_steps': 150
    },
    {
        'type': 'advanced_learning',
        'parameters': {'neuron_count': 3000, 'enable_all_features': True},
        'num_steps': 200
    }
]

# Run batch of experiments
results = pynlm.run_experiment_batch(experiment_configs)
```

### 3. Experiment Analysis

Analyze experiment results:

```python
def analyze_experiment_results(results):
    # Calculate statistics
    performance_scores = [r['performance_score'] for r in results]
    learning_rates = [r['learning_rate'] for r in results]
    
    # Find best and worst performers
    best_experiment = max(results, key=lambda x: x['performance_score'])
    worst_experiment = min(results, key=lambda x: x['performance_score'])
    
    print(f"Best performer: {best_experiment['experiment_type']}")
    print(f"Worst performer: {worst_experiment['experiment_type']}")
    
    # Performance ranking
    ranked = sorted(results, key=lambda x: x['performance_score'], reverse=True)
    
    print("Performance ranking:")
    for i, experiment in enumerate(ranked, 1):
        print(f"{i}. {experiment['experiment_type']}: {experiment['performance_score']:.3f}")
```

## Performance Optimization

### 1. Network Optimization

Optimize network parameters for better performance:

```python
def optimize_network_parameters(brain):
    # Analyze current network properties
    analysis = pynlm.analyze_network_connectivity(brain)
    
    # Calculate optimization targets
    target_ei_ratio = 1.0  # Balanced excitation/inhibition
    current_ei_ratio = analysis.excitationInhibitionRatio
    
    if current_ei_ratio > target_ei_ratio:
        print("Network has too much excitation - consider increasing inhibition")
    elif current_ei_ratio < target_ei_ratio:
        print("Network has too much inhibition - consider increasing excitation")
    
    # Optimize synaptic weights
    print(f"Current average weight: {analysis.averageWeight:.3f}")
    print("Target average weight: ~0.5 for balanced networks")
```

### 2. Learning Optimization

Optimize learning parameters:

```python
def optimize_learning_parameters(brain):
    learning = pynlm.analyze_learning_progress(brain, 20)
    
    # Check learning rate
    print(f"Current learning rate: {learning.totalLearning:.3f}")
    
    if learning.totalLearning < 0.1:
        print("Learning rate too low - consider increasing plasticity")
    elif learning.totalLearning > 1.0:
        print("Learning rate too high - consider decreasing plasticity")
    
    # Check learning stability
    stability = abs(learning.totalLearning - 0.5)
    print(f"Learning stability: {stability:.3f}")
```

## File Management

### 1. Experiment Data Storage

Store experiment results to files:

```python
import json
import time

def save_experiment_results(results, base_filename="experiment_results"):
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    filename = f"{base_filename}_{timestamp}.json"
    
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Results saved to: {filename}")
    return filename
```

### 2. Configuration Backup

Create backups of important configurations:

```python
def backup_config(config, backup_dir="backups"):
    import os
    
    # Create backup directory if it doesn't exist
    if not os.path.exists(backup_dir):
        os.makedirs(backup_dir)
    
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    filename = f"{backup_dir}/config_{timestamp}.json"
    
    # Save configuration
    pynlm.save_config_to_file(config, filename)
    print(f"Configuration backed up to: {filename}")
    
    return filename
```

## Troubleshooting

### 1. Common Issues and Solutions

**Issue: Network analysis returns incorrect values**
```python
# Solution: Check if brain is properly initialized
if not brain.getTotalNeuronCount():
    print("Brain not properly initialized")
    return False
```

**Issue: Configuration validation fails**
```python
# Solution: Check for required keys
required_keys = ["brain.neuron_count", "brain.region_count"]
for key in required_keys:
    if not config.has(key):
        print(f"Missing required key: {key}")
```

**Issue: Experiment fails to run**
```python
# Solution: Check experiment setup
if not setup or not setup.get('brain'):
    print("Experiment setup incomplete")
    return False
```

### 2. Debug Information

Generate debug information for troubleshooting:

```python
def debug_brain(brain):
    print("Brain Debug Information:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Analyze network
    analysis = pynlm.analyze_network_connectivity(brain)
    print(f"  Network analysis completed")
```

## Best Practices

### 1. Experiment Design

- **Reproducibility**: Use consistent random seeds for experiments
- **Parameter ranges**: Define reasonable parameter ranges for each experiment
- **Control groups**: Include control experiments for comparison
- **Replication**: Store experiment configurations and results

### 2. Performance Monitoring

- **Track performance**: Monitor key metrics (learning rate, firing rate, etc.)
- **Set thresholds**: Define performance thresholds for early detection of issues
- **Log experiments**: Log all experiment details for reproducibility
- **Visualize results**: Use plotting libraries for result visualization

### 3. Configuration Management

- **Document configurations**: Document configuration choices and their rationale
- **Use configuration templates**: Leverage predefined configuration templates
- **Validate configurations**: Validate configuration completeness before running experiments
- **Version control**: Store configurations in version control

### 4. Data Management

- **Backup data**: Regularly backup experiment data
- **Organize results**: Organize results by experiment type and parameters
- **Clean up temporary files**: Remove temporary files after analysis
- **Document analysis scripts**: Document analysis and visualization scripts

## Advanced Examples

The examples directory contains several advanced examples:

- `basic_usage.py` - Basic NLM usage
- `advanced_python_usage.py` - Advanced Python features
- `network_analysis.py` - Network analysis techniques
- `learning_curves.py` - Learning curve analysis
- `experiment_management.py` - Experiment management
- `configuration_examples.py` - Configuration examples

Each example demonstrates different aspects of advanced NLM usage and can be used as a starting point for more complex experiments.

## Further Reading

For more detailed information:

- **Source Code**: Refer to the source code for detailed API documentation
- **C++ Documentation**: Check C++ API documentation for advanced features
- **Scientific Papers**: Review NLM research papers for theoretical background
- **Documentation**: Explore other documentation files in the `docs/` directory

## Support

For support and questions:

- **Documentation**: Check the documentation in the `docs/` directory
- **Examples**: Use examples in the `examples/` directory
- **Community**: Join NLM community for support
- **Issues**: Report issues on the project issue tracker

---

This advanced usage guide provides comprehensive information for using NLM at an advanced level. Start with the basic examples and gradually move to advanced features as you become more familiar with the framework.
